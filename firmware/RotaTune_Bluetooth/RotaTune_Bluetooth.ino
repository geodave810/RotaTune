/*
  RotaTune_Bluetooth  v0.5.0
  ------------------------------------------------------------------------
  RotaTune Bluetooth link (formerly Classic_Link_A2DP_Receiver).
  Bluetooth transmitter for RotaTune (S3 player v0.16.9 and later). Runs on the
  ORIGINAL ESP32 (ESP32-U4WDH). Link protocol version 3.

  New in v0.5.0 (on top of v0.4.0):
    - Two saved speakers (NVS slots 0 and 1). The v0.4.0 saved speaker is
      moved to slot 0 on first boot. The "active" slot is the one the
      library reconnects to.
    - Commands from the player's Output page (Control.cmd, carried out once
      per cmdSeq; the last cmdSeq is stored in NVS before any restart, so a
      command is never repeated after the classic reboots):
        SCAN           restart into scan mode (library discovery)
        CONNECT_FOUND  connect a speaker found by the scan (like c <n>);
                       it is saved when connected (empty slot, otherwise
                       it replaces the slot that is not active)
        CONNECT_SAVED  make a saved slot active and restart to connect it
        FORGET_SAVED   clear a slot (disconnects it if it is the active one)
    - Device list packet to the player every 500 ms: saved speakers and the
      named devices found by the scan.
    - A scan starts Bluetooth even while the player is on wired output.

  New in v0.4.0 (on top of the hardware-tested Step 3 receiver v0.3.0):
    - Control packets from the S3 (every 100 ms and on change):
        wantConnected 1 (Bluetooth output selected on the player) or 0 (wired).
        volume 0..127 with a sequence number (rotary encoder on the player).
    - Bluetooth is started only when the player selects Bluetooth output.
      If no control packet arrives within 3 s of boot (S3 not running this
      protocol), it starts anyway, as v0.3.0 did.
    - Player switches to wired: disconnect and stop reconnecting (like d).
      Player switches to Bluetooth: reconnect to the saved speaker (like r).
    - Volume requests are applied with set_volume() (AVRCP absolute volume;
      mode 1, PCM never scaled). After the stream starts, the player's volume
      is applied once more (1.5 s later), so the speaker matches the knob.
    - The report now carries the speaker's volume, the last volume request
      applied, and the saved speaker's name.
    - Serial commands are unchanged and still work for testing.

  What it does
    - Keeps the PCM5100A unmuted (GPIO32 HIGH first, as always).
    - Receives 44.1 kHz 16-bit stereo PCM packets from the S3 over UART2
      (RX GPIO18, TX GPIO23, 3 Mbaud), checks each packet's CRC, and stores
      the audio in a 32 KB ring buffer (about 186 ms).
    - Plays the ring to the Bluetooth speaker through ESP32-A2DP, starting
      only when 24 KB (about 139 ms) is buffered, with a 10 ms fade-in.
      After an underrun it goes back to buffering.
    - Every ~5 ms sends the S3 a report: lastSeqProcessed and windowBytes,
      which is how the S3 knows how much more audio it may send.
    - Bluetooth handling is the settled v0.1.3 behaviour: library discovery,
      saved speaker, automatic reconnect, d / r, and volume-control mode 1
      (A2DPNoVolumeControl: the PCM is never scaled; get_volume() reports the
      speaker's volume; v sends AVRCP absolute volume).

  Flow-control rules implemented here (see README)
    - lastSeqProcessed advances when an audio packet of the current epoch
      is fully handled: written to the ring, or dropped as an overrun
      (overruns should never happen while the S3 obeys the window).
    - A packet with a bad CRC is dropped without using its seq (its header
      cannot be trusted). It is retired on the S3 side when a later packet
      is processed, or by the S3's 500 ms backstop.
    - windowBytes = min(ring free space, UART receive buffer - margin), taken
      in the same task, right after processing, so it is consistent with
      lastSeqProcessed. Free space grows only when Bluetooth removes real
      PCM from the ring; silence inserted during an underrun gives no credit.
    - Audio still in the UART driver buffer or the packet staging buffer
      belongs to packets with seq > lastSeqProcessed, so the S3 still counts
      it as outstanding.
    - The epoch (session id) changes at boot, when the A2DP stream starts,
      and when the speaker disconnects. The ring is flushed at each change
      and the S3 restarts at seq 1, so stale audio is never played.

  Reference environment: ESP32 board package 3.3.11, ESP32-A2DP 1.8.12.
  Board settings as for the v0.1.x tone tests (Huge APP partition).

  Changes from v0.2.0 (hardware-tested in Step 2): b command (reboot) only.

  Status: v0.2.0 passed the Step 2 hardware tests. The v0.3.0 change is
  syntax-checked only.
*/

// ------------------------------------------------------------------------
// PCM5100A DAC unmute. Identical in effect to Classic_ESP32_BasicSetting:
// first thing in setup(); nothing else touches GPIO32.
#define DAC_XSMT_PIN 32
// ------------------------------------------------------------------------

#include <Arduino.h>
#include <BluetoothA2DPSource.h>
#include <Preferences.h>
#include <esp_heap_caps.h>
#include <esp_random.h>
#include <ctype.h>
#include <string.h>
#include "link_protocol.h"

using namespace wslink;

static const char SKETCH_VERSION[] = "0.5.0";
static const char LOCAL_BT_NAME[]  = "Waveshare-Music-Player";

BluetoothA2DPSource a2dp_source;
Preferences prefs;
static A2DPNoVolumeControl noVolumeControl;   // mode 1; must outlive the source

// ------------------------------- Link UART -------------------------------
static HardwareSerial &Link = Serial2;
static constexpr int    LINK_RX_PIN = 18;      // from S3 GPIO38
static constexpr int    LINK_TX_PIN = 23;      // to S3 GPIO48
static constexpr size_t LINK_RX_BUFFER = 16384;
static constexpr size_t LINK_RX_MARGIN = 4096; // keep this much RX buffer spare
static constexpr uint8_t LINK_RX_FIFO_FULL = 64;   // UART FIFO is 128 bytes
static constexpr uint32_t REPORT_INTERVAL_MS = 5;

// ------------------------------- PCM ring --------------------------------
// Single writer (link task) and single reader (A2DP callback). Indices are
// free-running byte counters; ringMux protects index updates only.
static constexpr uint32_t RING_SIZE = 32768;                 // power of two
static constexpr uint32_t RING_MASK = RING_SIZE - 1;
static constexpr uint32_t START_THRESHOLD = 24576;           // ~139 ms
static constexpr uint32_t FADE_FRAMES = 441;                 // 10 ms
static uint8_t *ring = nullptr;
static volatile uint32_t ringHead = 0;     // written by link task
static volatile uint32_t ringTail = 0;     // written by A2DP callback (and flush)
static volatile uint32_t ringGen  = 0;     // incremented by flush
static portMUX_TYPE ringMux = portMUX_INITIALIZER_UNLOCKED;

static volatile uint8_t  playState = PLAY_BUFFERING;
static volatile uint32_t fadePos = FADE_FRAMES;

// ----------------------------- Link counters -----------------------------
static volatile uint32_t curEpoch = 1;
static volatile uint32_t lastSeqProcessed = 0;
static volatile uint32_t packetsOk = 0;
static volatile uint32_t seqGaps = 0;
static volatile uint32_t staleDrops = 0;
static volatile uint32_t overruns = 0;
static volatile uint32_t duplicates = 0;
static volatile uint32_t underruns = 0;
static volatile uint32_t reportsSent = 0;
static volatile uint64_t linkRxBytes = 0;
static volatile uint64_t pcmBytesPlayed = 0;
static volatile uint64_t silenceBytes = 0;
static volatile uint32_t epochChanges = 0;
static volatile bool     flushRequested = false;
static volatile uint32_t lastAudioPacketMs = 0;
static Parser linkParser;                 // used only by the link task
// Ring fill extremes over each status interval.
static volatile uint32_t fillMinPlaying = UINT32_MAX;
static volatile uint32_t fillMax = 0;

// --------------------------- A2DP request stats --------------------------
static portMUX_TYPE statsMux = portMUX_INITIALIZER_UNLOCKED;
static uint32_t statCallbacks = 0;
static uint64_t statBytes = 0;
static uint32_t statMaxGapUs = 0;
static uint32_t statLastCallUs = 0;
static uint32_t statLastReportMs = 0;

// ----------------------------- S3 control (v2) ---------------------------
static volatile bool     controlSeen = false;      // any control packet received
static volatile uint8_t  ctlWantConnected = 1;
static volatile uint8_t  ctlVolume = 0;
static volatile uint16_t ctlVolSeq = 0;
static volatile uint32_t controlsReceived = 0;
static volatile uint8_t  ctlCmd = CMD_NONE;
static volatile uint8_t  ctlCmdArg = 0;
static volatile uint16_t ctlCmdSeq = 0;
static volatile uint16_t cmdSeqDone = 0;            // stored in NVS before restarts
static volatile uint16_t volSeqApplied = 0;
static bool     volumeEverApplied = false;
static bool     bluetoothStarted = false;
static bool     appliedWantConnected = true;       // last wantConnected acted on
static uint32_t bootMs = 0;
static uint32_t pendingVolumeApplyMs = 0;          // re-apply after stream start
static constexpr uint32_t CONTROL_WAIT_MS = 3000;  // start BT anyway after this
static constexpr uint32_t VOLUME_REAPPLY_DELAY_MS = 1500;

// ------------------------------- Volume ---------------------------------
static constexpr int VOLUME_TEST_MAX = 50;            // of 127
static int lastVolumeSetBySketch = -1;
static int lastReportedLibraryVolume = 0;
static volatile int reportedSpeakerVolume = 0;   // copy of get_volume() for the link task

// ----------------------------- Device list ------------------------------
struct FoundDevice {
  char    name[64];
  uint8_t mac[6];
  int     rssi;
};
static constexpr size_t MAX_DEVICES = 20;   // index must fit 5 bits (DeviceList flags)
static FoundDevice devices[MAX_DEVICES];
static volatile size_t deviceCount = 0;
static portMUX_TYPE deviceMux = portMUX_INITIALIZER_UNLOCKED;

// Connection target. The library's discovery calls onDeviceFound() for each
// device; returning true for the target makes the library connect to it.
static uint8_t       targetMac[6] = {};
static char          targetName[64] = {};
static volatile bool targetArmed = false;

// Saved speaker (this sketch's own NVS record: MAC + name for display).
static bool    haveSavedSpeaker = false;   // the ACTIVE slot holds a speaker
static uint8_t savedMac[6] = {};           // copy of the active slot
static char    savedName[64] = {};
// v0.5.0: two saved speakers.
struct SavedSlot { bool valid; uint8_t mac[6]; char name[64]; };
static SavedSlot slots[MAX_SAVED_SPEAKERS] = {};
static int activeSlot = -1;                // -1: none
static volatile bool pendingSaveTarget = false;
static bool    scanModeBoot = false;      // this boot was started by the s command

// Reconnect policy.
// Library reconnect attempts after a drop. The library resets this count on
// every successful connection; 1000 is effectively "keep trying". Each
// attempt is driven by the library's 10 s heartbeat.
static constexpr int      LIBRARY_RECONNECT_RETRIES = 1000;
static constexpr uint32_t FALLBACK_CONNECT_INTERVAL_MS = 15000;
static constexpr uint32_t STATUS_INTERVAL_MS = 5000;
static bool     autoReconnectEnabled = true;     // cleared by the d command
static uint32_t lastFallbackAttemptMs = 0;
static volatile uint32_t lastConnActivityMs = 0;   // last connection/discovery activity
static constexpr uint32_t FALLBACK_QUIET_MS = 30000; // library must be quiet this long

// Connection / audio state from library callbacks.
static volatile int  connState  = ESP_A2D_CONNECTION_STATE_DISCONNECTED;
static volatile int  audioState = -1;
static volatile bool connStateChanged  = false;
static volatile bool audioStateChanged = false;
static volatile bool discoveryActive = false;
static volatile bool discoveryChanged = false;

// ------------------------------- Helpers --------------------------------
static void formatMac(const uint8_t *mac, char *out, size_t cap) {
  snprintf(out, cap, "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static const char *connStateName(int s) {
  switch (s) {
    case ESP_A2D_CONNECTION_STATE_DISCONNECTED:  return "DISCONNECTED";
    case ESP_A2D_CONNECTION_STATE_CONNECTING:    return "CONNECTING";
    case ESP_A2D_CONNECTION_STATE_CONNECTED:     return "CONNECTED";
    case ESP_A2D_CONNECTION_STATE_DISCONNECTING: return "DISCONNECTING";
    default: return "UNKNOWN";
  }
}

// ------------------------------ Ring buffer ------------------------------
static uint32_t ringFill() {
  portENTER_CRITICAL(&ringMux);
  uint32_t f = ringHead - ringTail;
  portEXIT_CRITICAL(&ringMux);
  return f;
}

// Writer side (link task only). All-or-nothing: returns false if it does
// not fit, so a packet is never split.
static bool ringWrite(const uint8_t *src, uint32_t n) {
  portENTER_CRITICAL(&ringMux);
  uint32_t h = ringHead, t = ringTail;
  portEXIT_CRITICAL(&ringMux);
  if (RING_SIZE - (h - t) < n) return false;
  uint32_t pos = h & RING_MASK;
  uint32_t first = RING_SIZE - pos;
  if (first > n) first = n;
  memcpy(ring + pos, src, first);
  if (n > first) memcpy(ring, src + first, n - first);
  portENTER_CRITICAL(&ringMux);
  ringHead = h + n;
  portEXIT_CRITICAL(&ringMux);
  return true;
}

// Reader side (A2DP callback only). Reads whole frames. If a flush happened
// while copying, the copy is discarded (zeroed) and 0 is returned.
static uint32_t ringRead(uint8_t *dst, uint32_t n) {
  portENTER_CRITICAL(&ringMux);
  uint32_t h = ringHead, t = ringTail, g = ringGen;
  portEXIT_CRITICAL(&ringMux);
  uint32_t avail = h - t;
  if (n > avail) n = avail;
  n &= ~3u;
  if (n == 0) return 0;
  uint32_t pos = t & RING_MASK;
  uint32_t first = RING_SIZE - pos;
  if (first > n) first = n;
  memcpy(dst, ring + pos, first);
  if (n > first) memcpy(dst + first, ring, n - first);
  bool ok;
  portENTER_CRITICAL(&ringMux);
  ok = (ringGen == g);
  if (ok) ringTail = t + n;
  portEXIT_CRITICAL(&ringMux);
  if (!ok) { memset(dst, 0, n); return 0; }
  return n;
}

// Link task only: drop everything buffered and start a new epoch.
static void flushAndNewEpoch() {
  portENTER_CRITICAL(&ringMux);
  ringTail = ringHead;
  ringGen = ringGen + 1;
  portEXIT_CRITICAL(&ringMux);
  playState = PLAY_BUFFERING;
  uint32_t e;
  do { e = esp_random(); } while (e == 0 || e == curEpoch);
  curEpoch = e;
  lastSeqProcessed = 0;
  epochChanges = epochChanges + 1;
}

// ------------------------- A2DP data callback ----------------------------
// Runs in the Bluetooth task. Output: 44.1 kHz 16-bit LE stereo.
int32_t provideLinkAudio(uint8_t *data, int32_t len) {
  if (data == nullptr || len <= 0) return 0;
  const uint32_t nowUs = micros();
  uint32_t want = (uint32_t)len & ~3u;

  uint32_t got = 0;
  if (playState == PLAY_BUFFERING) {
    if (ringFill() >= START_THRESHOLD) {
      playState = PLAY_PLAYING;
      fadePos = 0;
    }
  }
  if (playState == PLAY_PLAYING) {
    got = ringRead(data, want);
    // Fade in after (re)starting.
    for (uint32_t i = 0; i < got / 4 && fadePos < FADE_FRAMES; ++i, ++fadePos) {
      int16_t l, r;
      memcpy(&l, data + i * 4, 2);
      memcpy(&r, data + i * 4 + 2, 2);
      l = (int16_t)((int32_t)l * (int32_t)fadePos / (int32_t)FADE_FRAMES);
      r = (int16_t)((int32_t)r * (int32_t)fadePos / (int32_t)FADE_FRAMES);
      memcpy(data + i * 4, &l, 2);
      memcpy(data + i * 4 + 2, &r, 2);
    }
    if (got < want) {
      underruns = underruns + 1;
      playState = PLAY_BUFFERING;
    }
    uint32_t f = ringFill();
    if (f < fillMinPlaying) fillMinPlaying = f;
  }
  if (got < (uint32_t)len) memset(data + got, 0, (uint32_t)len - got);
  pcmBytesPlayed = pcmBytesPlayed + got;
  silenceBytes = silenceBytes + ((uint32_t)len - got);

  portENTER_CRITICAL(&statsMux);
  if (statLastCallUs != 0) {
    uint32_t gap = nowUs - statLastCallUs;
    if (gap > statMaxGapUs) statMaxGapUs = gap;
  }
  statLastCallUs = nowUs;
  ++statCallbacks;
  statBytes += (uint32_t)len;
  portEXIT_CRITICAL(&statsMux);
  return len;
}

// ------------------------------ Link task --------------------------------
static void handleAudioPacket(const Header &h, const uint8_t *payload, uint16_t len) {
  if (h.type != PKT_AUDIO) return;
  if (h.epoch != curEpoch) { staleDrops = staleDrops + 1; return; }
  if (h.seq <= lastSeqProcessed) { duplicates = duplicates + 1; return; }
  if (h.seq != lastSeqProcessed + 1) seqGaps = seqGaps + (h.seq - lastSeqProcessed - 1);
  if (len % 4 != 0 || !ringWrite(payload, len)) overruns = overruns + 1;
  lastSeqProcessed = h.seq;
  packetsOk = packetsOk + 1;
  lastAudioPacketMs = millis();
  uint32_t f = ringFill();
  if (f > fillMax) fillMax = f;
}

// Control from the S3: only stored here; loop() acts on changes (Bluetooth
// API calls are kept out of the link task).
static void handleControlPacket(const uint8_t *payload, uint16_t len) {
  if (len != sizeof(Control)) return;
  Control c;
  memcpy(&c, payload, sizeof(c));
  ctlWantConnected = c.wantConnected ? 1 : 0;
  ctlVolume = c.volume > 127 ? 127 : c.volume;
  ctlVolSeq = c.volSeq;
  ctlCmdArg = c.cmdArg;
  ctlCmdSeq = c.cmdSeq;
  ctlCmd = c.cmd;
  controlsReceived = controlsReceived + 1;
  controlSeen = true;
}

static void sendReport() {
  Report r = {};
  uint32_t fill = ringFill();
  uint32_t freeBytes = RING_SIZE - fill;
  uint32_t rxLimit = LINK_RX_BUFFER - LINK_RX_MARGIN;
  r.lastSeqProcessed = lastSeqProcessed;
  r.windowBytes = freeBytes < rxLimit ? freeBytes : rxLimit;
  r.ringFillBytes = fill;
  r.ringCapacity = RING_SIZE;
  r.btConnected = (connState == ESP_A2D_CONNECTION_STATE_CONNECTED);
  r.audioStarted = (r.btConnected && audioState == ESP_A2D_AUDIO_STATE_STARTED);
  r.playState = playState;
  r.crcErrors = linkParser.crcErrors;
  r.resyncBytes = linkParser.resyncBytes;
  r.seqGaps = seqGaps;
  r.staleDrops = staleDrops;
  r.overruns = overruns;
  r.underruns = underruns;
  r.pcmBytesPlayed = (uint32_t)pcmBytesPlayed;
  r.silenceBytes = (uint32_t)silenceBytes;
  r.speakerVolume = r.btConnected ? (int16_t)reportedSpeakerVolume : (int16_t)-1;
  r.volSeqApplied = volSeqApplied;
  r.bluetoothStarted = bluetoothStarted;
  r.savedSpeaker = haveSavedSpeaker;
  r.wantConnected = ctlWantConnected;
  strlcpy(r.speakerName, savedName, sizeof(r.speakerName));
  r.scanning = scanModeBoot;
  r.connState = connState == ESP_A2D_CONNECTION_STATE_CONNECTED ? CONN_CONNECTED :
                connState == ESP_A2D_CONNECTION_STATE_CONNECTING ? CONN_CONNECTING : CONN_DISCONNECTED;
  r.activeSlot = activeSlot < 0 ? 0xFF : (uint8_t)activeSlot;
  r.cmdSeqDone = cmdSeqDone;
  Header h;
  buildHeader(h, PKT_REPORT, sizeof(r), reportsSent + 1, curEpoch,
              reinterpret_cast<const uint8_t *>(&r));
  Link.write(reinterpret_cast<const uint8_t *>(&h), sizeof(h));
  Link.write(reinterpret_cast<const uint8_t *>(&r), sizeof(r));
  reportsSent = reportsSent + 1;
}

// v0.5.0: saved speakers + named devices found by the scan, every 500 ms.
static void sendDeviceList() {
  static DeviceList d;
  memset(&d, 0, sizeof(d));
  d.scanning = scanModeBoot;
  d.activeSlot = activeSlot < 0 ? 0xFF : (uint8_t)activeSlot;
  for (int k = 0; k < MAX_SAVED_SPEAKERS; ++k) {
    if (!slots[k].valid) continue;
    strlcpy(d.saved[k].name, slots[k].name, sizeof(d.saved[k].name));
    memcpy(d.saved[k].mac, slots[k].mac, 6);
    d.saved[k].flags = 0x01 | 0x02 | (k == activeSlot ? 0x04 : 0);
  }
  portENTER_CRITICAL(&deviceMux);
  size_t count = deviceCount;
  for (size_t i = 0; i < count && d.foundCount < MAX_FOUND_SPEAKERS; ++i) {
    if (!devices[i].name[0]) continue;               // named devices only
    SpeakerEntry &e = d.found[d.foundCount++];
    strlcpy(e.name, devices[i].name, sizeof(e.name));
    memcpy(e.mac, devices[i].mac, 6);
    e.rssi = (int8_t)devices[i].rssi;
    // bits 3..7: index in devices[] (stable; used by CONNECT_FOUND)
    e.flags = 0x01 | (uint8_t)(i << 3);
    for (int k = 0; k < MAX_SAVED_SPEAKERS; ++k)
      if (slots[k].valid && memcmp(slots[k].mac, devices[i].mac, 6) == 0) e.flags |= 0x02;
  }
  portEXIT_CRITICAL(&deviceMux);
  Header h;
  buildHeader(h, PKT_DEVICES, sizeof(d), 0, curEpoch, reinterpret_cast<const uint8_t *>(&d));
  Link.write(reinterpret_cast<const uint8_t *>(&h), sizeof(h));
  Link.write(reinterpret_cast<const uint8_t *>(&d), sizeof(d));
}

static void linkTask(void *) {
  static uint8_t chunk[1024];
  uint32_t lastReportMs = 0;
  for (;;) {
    if (flushRequested) {
      flushRequested = false;
      flushAndNewEpoch();
      sendReport();                  // tell the S3 about the new epoch at once
      lastReportMs = millis();
    }
    int avail = Link.available();
    if (avail > 0) {
      size_t n = Link.read(chunk, (size_t)min(avail, (int)sizeof(chunk)));
      linkRxBytes = linkRxBytes + n;
      linkParser.feed(chunk, n, [](const Header &h, const uint8_t *p, uint16_t len) {
        if (h.type == PKT_CONTROL) handleControlPacket(p, len);
        else handleAudioPacket(h, p, len);
      });
    }
    uint32_t now = millis();
    static uint32_t lastDevicesMs = 0;
    if (now - lastDevicesMs >= 500) {
      lastDevicesMs = now;
      sendDeviceList();
    }
    if (now - lastReportMs >= REPORT_INTERVAL_MS) {
      lastReportMs = now;
      sendReport();
    }
    if (avail <= 0) vTaskDelay(1);
  }
}

// --------------------- Discovery / device callbacks ---------------------
// Called by the library's own discovery for each device found. Every device
// is listed; true is returned only for the speaker selected with c or saved.
bool onDeviceFound(const char *name, esp_bd_addr_t address, int rssi) {
  bool isNew = false, isTarget = false;
  size_t index = 0;
  portENTER_CRITICAL(&deviceMux);
  size_t count = deviceCount;
  index = count;
  for (size_t i = 0; i < count; ++i)
    if (memcmp(devices[i].mac, address, 6) == 0) { index = i; break; }
  if (index == count && count < MAX_DEVICES) {
    memcpy(devices[count].mac, address, 6);
    devices[count].name[0] = '\0';
    deviceCount = count + 1;
    isNew = true;
  }
  if (index < deviceCount) {
    devices[index].rssi = rssi;
    if (name && name[0] && !devices[index].name[0])
      strlcpy(devices[index].name, name, sizeof(devices[index].name));
  }
  isTarget = targetArmed && memcmp(address, targetMac, 6) == 0;
  portEXIT_CRITICAL(&deviceMux);

  if (isNew) {
    char mac[18]; formatMac(address, mac, sizeof(mac));
    Serial.printf("[SCAN] %2u: %-32s %s  RSSI %d dBm\n", (unsigned)(index + 1),
                  (name && name[0]) ? name : "(name unavailable)", mac, rssi);
  }
  if (isTarget) Serial.println("[SCAN] Selected speaker found by the library scan; library will connect.");
  return isTarget;
}

void onDiscoveryState(esp_bt_gap_discovery_state_t state) {
  discoveryActive = (state == ESP_BT_GAP_DISCOVERY_STARTED);
  discoveryChanged = true;
  lastConnActivityMs = millis();   // the library may be about to connect
}

void onConnectionState(esp_a2d_connection_state_t state, void *) {
  lastConnActivityMs = millis();
  if ((int)state == connState) return;          // repeated event
  bool wasConnected = (connState == ESP_A2D_CONNECTION_STATE_CONNECTED);
  connState = (int)state;
  connStateChanged = true;
  if (state == ESP_A2D_CONNECTION_STATE_CONNECTED && targetArmed) pendingSaveTarget = true;
  if (state != ESP_A2D_CONNECTION_STATE_CONNECTED) {
    // No "audio stopped" event arrives on a deliberate disconnect, so the
    // audio state is cleared here; streaming = connected AND audio started.
    audioState = ESP_A2D_AUDIO_STATE_SUSPEND;
    if (wasConnected) flushRequested = true;     // new epoch, drop stale audio
  }
}

void onAudioState(esp_a2d_audio_state_t state, void *) {
  if (state == ESP_A2D_AUDIO_STATE_STARTED && audioState != ESP_A2D_AUDIO_STATE_STARTED) {
    flushRequested = true;                       // fresh audio for a new stream
    uint32_t t = millis() + VOLUME_REAPPLY_DELAY_MS;
    pendingVolumeApplyMs = t ? t : 1;            // apply the player's volume once AVRCP is up
  }
  audioState = (int)state;
  audioStateChanged = true;
}

// AVRCP passthrough commands received by the ESP32 acting as an AVRCP
// TARGET (library: ESP_AVRC_TG_PASSTHROUGH_CMD_EVT). A speaker sends these
// only if it acts as a remote controller (for example play/pause). Most
// speakers handle their own volume buttons internally and report volume
// through AVRCP volume-change notifications instead, which this callback does
// NOT receive. Logged only; nothing is changed. Registering this callback
// also makes the library initialise the AVRCP target role.
void onAvrcPassthroughCommand(uint8_t key, bool isReleased) {
  Serial.printf("[AVRC-TG] Passthrough command received: key=0x%02X %s%s\n", key,
                isReleased ? "released" : "pressed",
                key == 0x41 ? " (AVRCP VOLUME_UP code)" : key == 0x42 ? " (AVRCP VOLUME_DOWN code)" : "");
}

// ------------------------------ Speaker NVS -----------------------------
// v0.5.0: keys mac0/name0, mac1/name1, active (slot), lastcmd (cmdSeq done),
// scanboot. The v0.4.0 keys mac/name are moved into slot 0 once.
static void useActiveSlot() {
  if (activeSlot >= 0 && activeSlot < MAX_SAVED_SPEAKERS && slots[activeSlot].valid) {
    haveSavedSpeaker = true;
    memcpy(savedMac, slots[activeSlot].mac, 6);
    strlcpy(savedName, slots[activeSlot].name, sizeof(savedName));
  } else {
    activeSlot = -1;
    haveSavedSpeaker = false;
    memset(savedMac, 0, sizeof(savedMac));
    savedName[0] = 0;
  }
}

static void storeSlots() {
  prefs.begin("a2dptest", false);
  for (int k = 0; k < MAX_SAVED_SPEAKERS; ++k) {
    char km[8], kn[8];
    snprintf(km, sizeof(km), "mac%d", k);
    snprintf(kn, sizeof(kn), "name%d", k);
    if (slots[k].valid) { prefs.putBytes(km, slots[k].mac, 6); prefs.putString(kn, slots[k].name); }
    else { prefs.remove(km); prefs.remove(kn); }
  }
  prefs.putChar("active", (int8_t)activeSlot);
  prefs.end();
}

static void loadSettings() {
  prefs.begin("a2dptest", false);
  bool migrated = false;
  if (!prefs.isKey("mac0") && prefs.isKey("mac")) {          // v0.4.0 single speaker
    uint8_t mac[6];
    if (prefs.getBytes("mac", mac, 6) == 6) {
      prefs.putBytes("mac0", mac, 6);
      prefs.putString("name0", prefs.getString("name", ""));
      prefs.putChar("active", 0);
      migrated = true;
    }
    prefs.remove("mac");
    prefs.remove("name");
  }
  for (int k = 0; k < MAX_SAVED_SPEAKERS; ++k) {
    char km[8], kn[8];
    snprintf(km, sizeof(km), "mac%d", k);
    snprintf(kn, sizeof(kn), "name%d", k);
    slots[k].valid = prefs.isKey(km) && prefs.getBytes(km, slots[k].mac, 6) == 6;
    String n = prefs.getString(kn, "");
    strlcpy(slots[k].name, n.c_str(), sizeof(slots[k].name));
  }
  activeSlot = prefs.getChar("active", -1);
  cmdSeqDone = prefs.getUShort("lastcmd", 0);
  scanModeBoot = prefs.getBool("scanboot", false);
  if (scanModeBoot) prefs.putBool("scanboot", false);          // one-shot flag
  prefs.end();
  useActiveSlot();
  if (migrated) Serial.println("[NVS] v0.4.0 saved speaker moved to slot 1 of 2.");
  for (int k = 0; k < MAX_SAVED_SPEAKERS; ++k)
    if (slots[k].valid) {
      char m[18]; formatMac(slots[k].mac, m, sizeof(m));
      Serial.printf("[NVS] Saved speaker %d: %s  %s%s\n", k + 1,
                    slots[k].name[0] ? slots[k].name : "(no name)", m, k == activeSlot ? "  (active)" : "");
    }
}

// Save a speaker that just connected: its own slot if known, else an empty
// slot, else the slot that is not active. It becomes the active slot.
static void saveSpeaker(const uint8_t *mac, const char *name) {
  int k = -1;
  for (int i = 0; i < MAX_SAVED_SPEAKERS; ++i) if (slots[i].valid && memcmp(slots[i].mac, mac, 6) == 0) k = i;
  if (k < 0) for (int i = 0; i < MAX_SAVED_SPEAKERS; ++i) if (!slots[i].valid) { k = i; break; }
  if (k < 0) k = (activeSlot == 0) ? 1 : 0;
  slots[k].valid = true;
  memcpy(slots[k].mac, mac, 6);
  if (name && name[0]) strlcpy(slots[k].name, name, sizeof(slots[k].name));
  activeSlot = k;
  storeSlots();
  useActiveSlot();
  char m[18]; formatMac(mac, m, sizeof(m));
  Serial.printf("[NVS] Saved speaker %d: %s  %s (active)\n", k + 1, savedName[0] ? savedName : "(no name)", m);
}

static void forgetSlot(int k) {
  if (k < 0 || k >= MAX_SAVED_SPEAKERS || !slots[k].valid) return;
  bool wasActive = (k == activeSlot);
  Serial.printf("[NVS] Forgetting saved speaker %d: %s\n", k + 1, slots[k].name);
  slots[k] = SavedSlot{};
  if (wasActive) {
    int other = 1 - k;
    activeSlot = slots[other].valid ? other : -1;   // not connected until chosen
    targetArmed = false;
    autoReconnectEnabled = false;
    if (bluetoothStarted) {
      a2dp_source.set_auto_reconnect(false);
      a2dp_source.disconnect();
      a2dp_source.clean_last_connection();
    }
  }
  storeSlots();
  useActiveSlot();
}

static void forgetSpeaker() {                    // serial f: the active speaker
  if (activeSlot < 0) { Serial.println("[NVS] No active saved speaker."); return; }
  forgetSlot(activeSlot);
}

static void storeLastCommand(uint16_t seq) {
  prefs.begin("a2dptest", false);
  prefs.putUShort("lastcmd", seq);
  prefs.end();
}

// ------------------------------ Connecting ------------------------------
static void armTarget(const uint8_t *mac, const char *name) {
  portENTER_CRITICAL(&deviceMux);
  memcpy(targetMac, mac, 6);
  targetArmed = true;
  portEXIT_CRITICAL(&deviceMux);
  strlcpy(targetName, name ? name : "", sizeof(targetName));
  lastConnActivityMs = millis();   // give the library time before any fallback
}

// Direct connect is used only when the library is NOT discovering, so it
// never competes with the library's own discovery/connect state machine.
static void fallbackConnect(const uint8_t *mac, const char *reason) {
  if (discoveryActive || a2dp_source.is_discovery_active()) {
    Serial.printf("[BT] %s: library discovery is running; waiting for it to find the speaker.\n", reason);
    return;
  }
  char m[18]; formatMac(mac, m, sizeof(m));
  esp_bd_addr_t peer; memcpy(peer, mac, 6);
  bool ok = a2dp_source.connect_to(peer);
  Serial.printf("[BT] %s: connect_to(%s) returned %s\n", reason, m, ok ? "true" : "false");
  lastFallbackAttemptMs = millis();
  lastConnActivityMs = millis();
}

static void requestScanRestart() {
  if (a2dp_source.is_connected()) {
    Serial.println("[SCAN] Disconnect first (d). Scanning while streaming disturbs the audio.");
    return;
  }
  if (discoveryActive || a2dp_source.is_discovery_active()) {
    portENTER_CRITICAL(&deviceMux);
    deviceCount = 0;
    portEXIT_CRITICAL(&deviceMux);
    Serial.println("[SCAN] Library discovery is already running; list cleared, devices will be listed as found.");
    return;
  }
  // The library starts discovery from start(). The cleanest way to scan again
  // with only the library's mechanism is a restart into scan mode.
  Serial.println("[SCAN] Restarting into scan mode (auto-reconnect off for this boot)...");
  prefs.begin("a2dptest", false);
  prefs.putBool("scanboot", true);
  prefs.end();
  delay(200);
  ESP.restart();
}

static void listDevices() {
  size_t count = deviceCount;
  if (!count) { Serial.println("[SCAN] No devices found yet."); return; }
  Serial.println("[SCAN] Devices found:");
  for (size_t i = 0; i < count; ++i) {
    FoundDevice d;
    portENTER_CRITICAL(&deviceMux);
    d = devices[i];
    portEXIT_CRITICAL(&deviceMux);
    char m[18]; formatMac(d.mac, m, sizeof(m));
    Serial.printf("  %2u: %-32s %s  RSSI %d dBm\n", (unsigned)(i + 1),
                  d.name[0] ? d.name : "(name unavailable)", m, d.rssi);
  }
}

// ------------------------------- Status ---------------------------------
static void printStatus() {
  uint32_t callbacks, maxGap; uint64_t bytes;
  portENTER_CRITICAL(&statsMux);
  callbacks = statCallbacks; bytes = statBytes; maxGap = statMaxGapUs;
  statCallbacks = 0; statBytes = 0; statMaxGapUs = 0;
  portEXIT_CRITICAL(&statsMux);
  static uint64_t lastRx = 0, lastPlayed = 0, lastSilence = 0;
  uint64_t rx = linkRxBytes, played = pcmBytesPlayed, silence = silenceBytes;
  uint32_t now = millis();
  uint32_t elapsed = now - statLastReportMs;
  statLastReportMs = now;
  double secs = elapsed ? elapsed / 1000.0 : 1.0;
  uint32_t fMin = fillMinPlaying, fMax = fillMax;
  fillMinPlaying = UINT32_MAX; fillMax = 0;
  uint32_t fill = ringFill();

  Serial.printf("\n[STATUS] v%s | conn=%s | audio=%s | %s | epoch=%08lX\n",
                SKETCH_VERSION, connStateName(connState),
                audioState == ESP_A2D_AUDIO_STATE_STARTED ? "STARTED" : "not started",
                playState == PLAY_PLAYING ? "PLAYING" : "BUFFERING", (unsigned long)curEpoch);
  Serial.printf("  Link in: %.0f bytes/s (audio payload alone = 176400) | packets ok=%lu | last seq=%lu | reports sent=%lu\n",
                (rx - lastRx) / secs, (unsigned long)packetsOk,
                (unsigned long)lastSeqProcessed, (unsigned long)reportsSent);
  Serial.printf("  Link errors: CRC=%lu resync bytes=%lu seq gaps=%lu stale=%lu duplicates=%lu overruns=%lu\n",
                (unsigned long)linkParser.crcErrors, (unsigned long)linkParser.resyncBytes,
                (unsigned long)seqGaps, (unsigned long)staleDrops,
                (unsigned long)duplicates, (unsigned long)overruns);
  char minText[16];
  if (fMin == UINT32_MAX) strcpy(minText, "-");
  else snprintf(minText, sizeof(minText), "%lu", (unsigned long)fMin);
  Serial.printf("  Ring: now %lu bytes (%.0f ms) | min while playing %s | max %lu | underruns=%lu | epoch changes=%lu\n",
                (unsigned long)fill, fill / 176.4, minText,
                (unsigned long)fMax, (unsigned long)underruns, (unsigned long)epochChanges);
  Serial.printf("  A2DP: requests %.0f bytes/s, calls=%lu, longest gap %.1f ms | PCM %.0f bytes/s, silence %.0f bytes/s\n",
                bytes / secs, (unsigned long)callbacks, maxGap / 1000.0,
                (played - lastPlayed) / secs, (silence - lastSilence) / secs);
  Serial.printf("  Volume: get_volume()=%d (speaker), mode 1, PCM never scaled | last set by player/v: %d\n",
                a2dp_source.get_volume(), lastVolumeSetBySketch);
  Serial.printf("  Player control: %s | output=%s | volume %u/127 seq %u (applied %u) | Bluetooth %s\n",
                controlSeen ? "received" : "none yet", ctlWantConnected ? "BLUETOOTH" : "WIRED",
                (unsigned)ctlVolume, (unsigned)ctlVolSeq, (unsigned)volSeqApplied,
                bluetoothStarted ? "started" : "not started");
  Serial.printf("  Heap: free=%u  min-ever=%u  largest block=%u | GPIO32 (XSMT)=%d\n",
                (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMinFreeHeap(),
                (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT),
                digitalRead(DAC_XSMT_PIN));
  for (int k = 0; k < MAX_SAVED_SPEAKERS; ++k)
    if (slots[k].valid) Serial.printf("  Saved %d: %s%s\n", k + 1, slots[k].name, k == activeSlot ? " (active)" : "");
  if (haveSavedSpeaker) {
    char m[18]; formatMac(savedMac, m, sizeof(m));
    Serial.printf("  Saved speaker: %s  %s | auto-reconnect=%s\n",
                  savedName[0] ? savedName : "(no name)", m, autoReconnectEnabled ? "ON" : "OFF");
  } else {
    Serial.println("  Saved speaker: none (wait for the scan list, then c <number>)");
  }
  lastRx = rx; lastPlayed = played; lastSilence = silence;
}

static void printHelp() {
  Serial.println("\nCommands (type, then Enter; upper or lower case):");
  Serial.println("  s          scan (clears the list, or restarts into scan mode)");
  Serial.println("  l          list devices found");
  Serial.println("  c <n>      select device n; the library connects when its scan sees it");
  Serial.println("  r          reconnect to the saved speaker; resumes auto-reconnect");
  Serial.println("  d          disconnect; pauses sketch AND library auto-reconnect");
  Serial.println("  f          forget the active saved speaker");
  Serial.printf ("  v <0-%d>   speaker volume (AVRCP absolute volume; PCM not scaled)\n", VOLUME_TEST_MAX);
  Serial.println("  i          status now");
  Serial.println("  b          reboot the classic ESP32");
  Serial.println("  h          this help");
}

static void startBluetooth();   // defined below

static void handleCommand(char *line) {
  while (*line == ' ') ++line;
  char cmd = (char)tolower((unsigned char)line[0]);
  long arg = strtol(line + 1, nullptr, 10);
  bool hasArg = false;
  for (const char *p = line + 1; *p; ++p) if (*p >= '0' && *p <= '9') { hasArg = true; break; }

  // v0.4.0: Bluetooth is not started while the player uses wired output.
  if (!bluetoothStarted && (cmd == 'c' || cmd == 'r' || cmd == 'd' || cmd == 'v')) {
    Serial.println("[CMD] Bluetooth is not started (the player is on wired output). Use s to start it in scan mode.");
    return;
  }
  switch (cmd) {
    case 's':
      if (!bluetoothStarted) {
        Serial.println("[SCAN] Starting Bluetooth in scan mode...");
        scanModeBoot = true;
        startBluetooth();
        break;
      }
      requestScanRestart();
      break;
    case 'l': listDevices(); break;
    case 'c': {
      size_t count = deviceCount;
      if (!hasArg || arg < 1 || (size_t)arg > count) { Serial.println("[CMD] Use c <number> from the list (l)."); break; }
      if (a2dp_source.is_connected()) { Serial.println("[CMD] Disconnect first (d)."); break; }
      FoundDevice d;
      portENTER_CRITICAL(&deviceMux); d = devices[arg - 1]; portEXIT_CRITICAL(&deviceMux);
      autoReconnectEnabled = true;
      armTarget(d.mac, d.name);
      Serial.printf("[BT] Selected %s. The library connects the next time its scan sees it.\n",
                    d.name[0] ? d.name : "(no name)");
      fallbackConnect(d.mac, "selected");
      break;
    }
    case 'r': {
      if (!haveSavedSpeaker) { Serial.println("[CMD] No saved speaker."); break; }
      if (a2dp_source.is_connected()) { Serial.println("[CMD] Already connected."); break; }
      autoReconnectEnabled = true;
      esp_bd_addr_t peer; memcpy(peer, savedMac, 6);
      a2dp_source.set_auto_reconnect(peer, LIBRARY_RECONNECT_RETRIES);
      armTarget(savedMac, savedName);
      fallbackConnect(savedMac, "manual reconnect");
      break;
    }
    case 'd':
      autoReconnectEnabled = false;
      targetArmed = false;
      a2dp_source.set_auto_reconnect(false);
      a2dp_source.disconnect();
      Serial.println("[BT] Disconnect requested; sketch and library auto-reconnect paused (r to resume).");
      break;
    case 'f': forgetSpeaker(); break;
    case 'v': {
      if (!hasArg) { Serial.printf("[CMD] Use v <0-%d>.\n", VOLUME_TEST_MAX); break; }
      if (arg < 0 || arg > VOLUME_TEST_MAX) {
        Serial.printf("[VOL] Refused: %ld is above the test limit of %d (of 127).\n", arg, VOLUME_TEST_MAX);
        break;
      }
      bool connected = a2dp_source.is_connected();
      lastVolumeSetBySketch = (int)arg;
      a2dp_source.set_volume((uint8_t)arg);
      lastReportedLibraryVolume = a2dp_source.get_volume();
      Serial.printf("[VOL] set_volume(%ld)%s; PCM not scaled. get_volume()=%d\n", arg,
                    connected ? ": AVRCP absolute volume sent to speaker" : " (not connected: no AVRCP command)",
                    lastReportedLibraryVolume);
      break;
    }
    case 'i': printStatus(); break;
    case 'b': Serial.println("[CMD] Rebooting..."); delay(100); ESP.restart(); break;
    case 'h': case '?': printHelp(); break;
    case '\0': break;
    default: Serial.printf("[CMD] Unknown command '%c'. Type h for help.\n", cmd); break;
  }
}

static void serviceSerialCommands() {
  static char line[32];
  static size_t used = 0;
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\r') continue;
    if (c == '\n') { line[used] = '\0'; handleCommand(line); used = 0; continue; }
    if (used < sizeof(line) - 1) line[used++] = c;
  }
}

// v0.4.0: Bluetooth start, moved out of setup(). Called once.
static void startBluetooth() {
  if (bluetoothStarted) return;
  bluetoothStarted = true;
  if (haveSavedSpeaker && !scanModeBoot) {
    char m[18]; formatMac(savedMac, m, sizeof(m));
    Serial.printf("[BT] Saved speaker %s %s: library auto-reconnect, no scan.\n",
                  savedName[0] ? savedName : "(no name)", m);
    esp_bd_addr_t peer; memcpy(peer, savedMac, 6);
    a2dp_source.set_auto_reconnect(peer, LIBRARY_RECONNECT_RETRIES);
    armTarget(savedMac, savedName);
  } else {
    a2dp_source.set_auto_reconnect(false);
    Serial.println(scanModeBoot ? "[BT] Scan mode boot: library discovery starting."
                                : "[BT] No saved speaker: library discovery starting.");
    Serial.println("[BT] Put the speaker in pairing mode, then type c <number>.");
  }

  a2dp_source.start();

  Serial.printf("[HEAP] After start: free=%u largest block=%u\n",
                (unsigned)ESP.getFreeHeap(),
                (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  appliedWantConnected = true;
  lastFallbackAttemptMs = millis();
  lastConnActivityMs = millis();
}

// v0.4.0: act on the player's control messages (called from loop()).
static void serviceControl() {
  uint32_t now = millis();
  if (!bluetoothStarted && scanModeBoot) {
    Serial.println("[CTRL] Scan requested by the player: starting Bluetooth in scan mode.");
    startBluetooth();
  }
  if (!bluetoothStarted) {
    if (controlSeen) {
      if (ctlWantConnected) {
        Serial.println("[CTRL] Player selected Bluetooth output: starting Bluetooth.");
        startBluetooth();
      }
    } else if (now - bootMs >= CONTROL_WAIT_MS) {
      Serial.println("[CTRL] No control from the player within 3 s: starting Bluetooth anyway (v0.3.0 behaviour).");
      startBluetooth();
    }
    return;
  }
  // v0.5.0: player commands, once per cmdSeq (before the wired/Bluetooth
  // switch below, so a "connect" is not followed by a reconnect to the old one).
  if (controlSeen && ctlCmd != CMD_NONE && ctlCmdSeq != cmdSeqDone) {
    uint8_t cmd = ctlCmd, arg = ctlCmdArg;
    uint16_t seq = ctlCmdSeq;
    cmdSeqDone = seq;
    storeLastCommand(seq);                       // before any restart
    switch (cmd) {
      case CMD_SCAN:
        Serial.println("[CTRL] Player: find speakers -> restarting into scan mode.");
        prefs.begin("a2dptest", false); prefs.putBool("scanboot", true); prefs.end();
        delay(100);
        ESP.restart();
        break;
      case CMD_CONNECT_SAVED:
        if (arg < MAX_SAVED_SPEAKERS && slots[arg].valid) {
          Serial.printf("[CTRL] Player: connect saved speaker %u (%s) -> restarting to connect it.\n",
                        (unsigned)arg + 1, slots[arg].name);
          activeSlot = arg;
          storeSlots();
          delay(100);
          ESP.restart();
        }
        break;
      case CMD_CONNECT_FOUND: {
        size_t idx = arg;
        if (idx < deviceCount && bluetoothStarted) {
          FoundDevice d;
          portENTER_CRITICAL(&deviceMux); d = devices[idx]; portEXIT_CRITICAL(&deviceMux);
          autoReconnectEnabled = true;
          appliedWantConnected = true;           // the player selects Bluetooth with this
          armTarget(d.mac, d.name);
          Serial.printf("[CTRL] Player: connect %s. The library connects when its scan sees it.\n",
                        d.name[0] ? d.name : "(no name)");
          if (a2dp_source.is_connected()) a2dp_source.disconnect();
          fallbackConnect(d.mac, "selected on the player");
        }
        break;
      }
      case CMD_FORGET_SAVED:
        forgetSlot(arg);
        break;
      default:
        break;
    }
  }
  if (controlSeen) {
    bool want = ctlWantConnected != 0;
    if (want != appliedWantConnected) {
      appliedWantConnected = want;
      if (!want) {
        autoReconnectEnabled = false;
        targetArmed = false;
        a2dp_source.set_auto_reconnect(false);
        a2dp_source.disconnect();
        Serial.println("[CTRL] Player selected wired output: speaker disconnected, reconnect paused.");
      } else if (haveSavedSpeaker) {
        autoReconnectEnabled = true;
        esp_bd_addr_t peer; memcpy(peer, savedMac, 6);
        a2dp_source.set_auto_reconnect(peer, LIBRARY_RECONNECT_RETRIES);
        armTarget(savedMac, savedName);
        Serial.println("[CTRL] Player selected Bluetooth output: reconnecting to the saved speaker.");
        fallbackConnect(savedMac, "player selected Bluetooth");
      } else {
        Serial.println("[CTRL] Player selected Bluetooth output, but no speaker is saved (use s / c <n>).");
      }
    }
  }
  bool streaming = (connState == ESP_A2D_CONNECTION_STATE_CONNECTED &&
                    audioState == ESP_A2D_AUDIO_STATE_STARTED);
  uint16_t seq = ctlVolSeq;
  bool reapply = pendingVolumeApplyMs != 0 && (int32_t)(now - pendingVolumeApplyMs) >= 0;
  if (controlSeen && streaming && (seq != volSeqApplied || !volumeEverApplied || reapply)) {
    uint8_t v = ctlVolume;
    a2dp_source.set_volume(v);
    lastVolumeSetBySketch = v;
    volSeqApplied = seq;
    volumeEverApplied = true;
    pendingVolumeApplyMs = 0;
    Serial.printf("[CTRL] Speaker volume %u/127 from the player%s\n", (unsigned)v,
                  reapply ? " (re-applied after stream start)" : "");
  }
}

// -------------------------------- Setup ---------------------------------
void setup() {
  // PCM5100A unmute: first, exactly as in Classic_ESP32_BasicSetting.
  pinMode(DAC_XSMT_PIN, OUTPUT);
  digitalWrite(DAC_XSMT_PIN, HIGH);

  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("==============================================");
  Serial.printf(" RotaTune_Bluetooth v%s\n", SKETCH_VERSION);
  Serial.println(" S3 UART link -> ring buffer -> Bluetooth A2DP");
  Serial.println(" PCM5100A XSMT (GPIO32) set HIGH (unmuted)");
  Serial.println(" Volume-control mode 1 (A2DPNoVolumeControl: PCM never scaled)");
  Serial.println("==============================================");

  ring = (uint8_t *)heap_caps_malloc(RING_SIZE, MALLOC_CAP_8BIT);
  if (!ring) {
    Serial.println("[FATAL] Could not allocate the PCM ring buffer.");
    for (;;) delay(1000);
  }
  memset(ring, 0, RING_SIZE);
  uint32_t e;
  do { e = esp_random(); } while (e == 0);
  curEpoch = e;

  Link.setRxBufferSize(LINK_RX_BUFFER);
  Link.begin(LINK_BAUD, SERIAL_8N1, LINK_RX_PIN, LINK_TX_PIN);
  Link.setRxFIFOFull(LINK_RX_FIFO_FULL);
  Serial.printf("[LINK] UART2 %lu baud, RX GPIO%d, TX GPIO%d, RX buffer %u, ring %lu bytes\n",
                (unsigned long)LINK_BAUD, LINK_RX_PIN, LINK_TX_PIN,
                (unsigned)LINK_RX_BUFFER, (unsigned long)RING_SIZE);

  loadSettings();

  a2dp_source.set_local_name(LOCAL_BT_NAME);
  a2dp_source.set_ssid_callback(onDeviceFound);
  a2dp_source.set_discovery_mode_callback(onDiscoveryState);
  a2dp_source.set_on_connection_state_changed(onConnectionState);
  a2dp_source.set_on_audio_state_changed(onAudioState);
  a2dp_source.set_avrc_passthru_command_callback(onAvrcPassthroughCommand);
  a2dp_source.set_data_callback(provideLinkAudio);
  a2dp_source.set_volume_control(&noVolumeControl);   // mode 1, before start()

  // v0.4.0: the link task runs from boot; Bluetooth starts when the player
  // selects Bluetooth output (or after CONTROL_WAIT_MS without control).
  xTaskCreatePinnedToCore(linkTask, "link_rx", 6144, nullptr, 5, nullptr, 1);
  bootMs = millis();
  Serial.println("[BT] Waiting for the player's output selection (Bluetooth starts only if selected)...");

  printHelp();
  statLastReportMs = millis();
  lastFallbackAttemptMs = millis();
  lastConnActivityMs = millis();
}

// --------------------------------- Loop ---------------------------------
void loop() {
  serviceSerialCommands();
  serviceControl();

  if (connStateChanged) {
    connStateChanged = false;
    Serial.printf("[BT] Connection state: %s\n", connStateName(connState));
  }
  if (audioStateChanged) {
    audioStateChanged = false;
    Serial.printf("[BT] Audio state: %s\n",
                  audioState == ESP_A2D_AUDIO_STATE_STARTED ? "STARTED" : "not started");
  }
  if (discoveryChanged) {
    discoveryChanged = false;
    Serial.println(discoveryActive ? "[SCAN] Library discovery started."
                                   : "[SCAN] Library discovery stopped.");
  }
  if (pendingSaveTarget && connState == ESP_A2D_CONNECTION_STATE_CONNECTED) {
    pendingSaveTarget = false;
    const char *name = targetName[0] ? targetName : savedName;
    if (!haveSavedSpeaker || memcmp(savedMac, targetMac, 6) != 0 || strcmp(savedName, name) != 0) {
      saveSpeaker(targetMac, name);
      esp_bd_addr_t peer; memcpy(peer, targetMac, 6);
      a2dp_source.set_auto_reconnect(peer, LIBRARY_RECONNECT_RETRIES);   // reconnect to this one from now on
      autoReconnectEnabled = true;
    }
  }

  int libVol = a2dp_source.get_volume();
  reportedSpeakerVolume = libVol;
  if (libVol != lastReportedLibraryVolume) {
    Serial.printf("[VOL] Speaker volume now %d (reported by the speaker; PCM not scaled)\n", libVol);
    lastReportedLibraryVolume = libVol;
  }

  uint32_t now = millis();
  if (bluetoothStarted && autoReconnectEnabled && targetArmed &&
      connState == ESP_A2D_CONNECTION_STATE_DISCONNECTED &&
      !discoveryActive && !a2dp_source.is_discovery_active() &&
      now - lastConnActivityMs >= FALLBACK_QUIET_MS &&
      now - lastFallbackAttemptMs >= FALLBACK_CONNECT_INTERVAL_MS) {
    fallbackConnect(targetMac, "fallback reconnect");
  }

  if (now - statLastReportMs >= STATUS_INTERVAL_MS) printStatus();
  delay(5);
}
