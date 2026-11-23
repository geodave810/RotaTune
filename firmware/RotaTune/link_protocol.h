// link_protocol.h  -  S3 <-> classic ESP32 PCM link, protocol version 3
// ---------------------------------------------------------------------------
// THIS FILE MUST BE IDENTICAL in RotaTune (S3) and
// RotaTune_Bluetooth. Change both copies together.
//
// Version 2 (v0.16.8 / receiver v0.4.0) = version 1 (hardware-tested in
// Step 3) plus:
//   - PKT_CONTROL (S3 -> classic): output mode and speaker volume.
//   - Report extended with the speaker volume, the last volume request
//     applied, and the saved speaker's name.
// Version 3 (v0.16.9 / receiver v0.5.0) adds:
//   - Control.cmd / cmdArg / cmdSeq: scan, connect a found or saved speaker,
//     forget a saved speaker.
//   - Report: scan / connection state and the last command done.
//   - PKT_DEVICES (classic -> S3, every 500 ms): saved speakers (2) and the
//     speakers found by a scan.
// A peer with another version byte is rejected, so both chips must be
// updated together.
//
// Wire format (both directions, little-endian, both chips are little-endian):
//   Header (20 bytes) + payload (0..MAX_PAYLOAD bytes)
//   magic   4  'W','S','L','2'
//   type    1  PKT_AUDIO (S3 -> classic) or PKT_REPORT (classic -> S3)
//   version 1  LINK_VERSION
//   length  2  payload bytes
//   seq     4  audio: packet sequence number, starts at 1 in each epoch
//              report: report counter
//   epoch   4  receiver session id. Audio packets carry the epoch the S3
//              last saw; the receiver drops audio from any other epoch.
//   crc     4  CRC32 (esp_rom_crc32_le, seed 0) over the header with this
//              field set to 0, followed by the payload.
//
// Audio payload: 44.1 kHz, signed 16-bit, stereo, interleaved L,R.
// Flow control: the receiver reports (lastSeqProcessed, windowBytes). The S3
// may only have windowBytes of audio payload outstanding beyond
// lastSeqProcessed. See README files.
// ---------------------------------------------------------------------------
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_rom_crc.h"

namespace wslink {

static constexpr uint32_t LINK_BAUD    = 3000000;
static constexpr uint8_t  LINK_VERSION = 3;
static constexpr uint8_t  MAGIC_BYTES[4] = {'W', 'S', 'L', '2'};

enum : uint8_t {
  PKT_AUDIO   = 0x01,
  PKT_CONTROL = 0x02,   // S3 -> classic, sent every 100 ms and on change
  PKT_REPORT  = 0x10,
  PKT_DEVICES = 0x11,   // classic -> S3, every 500 ms
};

// 512 stereo frames = 11.6 ms of audio per packet.
static constexpr uint16_t AUDIO_PAYLOAD_BYTES = 2048;
static constexpr uint16_t MAX_PAYLOAD         = 2048;

struct __attribute__((packed)) Header {
  uint8_t  magic[4];
  uint8_t  type;
  uint8_t  version;
  uint16_t length;
  uint32_t seq;
  uint32_t epoch;
  uint32_t crc;
};
static_assert(sizeof(Header) == 20, "Header must be 20 bytes");

// Receiver play states (Report.playState).
enum : uint8_t { PLAY_BUFFERING = 0, PLAY_PLAYING = 1 };

struct __attribute__((packed)) Report {
  uint32_t lastSeqProcessed;  // highest audio seq of this epoch fully handled
  uint32_t windowBytes;       // audio payload the S3 may have beyond that seq
  uint32_t ringFillBytes;     // PCM waiting in the receiver ring
  uint32_t ringCapacity;
  uint8_t  btConnected;       // A2DP connected
  uint8_t  audioStarted;      // A2DP media started
  uint8_t  playState;         // PLAY_BUFFERING / PLAY_PLAYING
  uint8_t  reserved;
  uint32_t crcErrors;         // receiver counters since boot
  uint32_t resyncBytes;
  uint32_t seqGaps;
  uint32_t staleDrops;
  uint32_t overruns;
  uint32_t underruns;
  uint32_t pcmBytesPlayed;    // low 32 bits
  uint32_t silenceBytes;      // low 32 bits
  // ---- version 2 ----
  int16_t  speakerVolume;     // 0..127 (AVRCP) while connected, -1 otherwise
  uint16_t volSeqApplied;     // Control.volSeq of the last volume applied
  uint8_t  bluetoothStarted;  // A2DP stack started on the classic
  uint8_t  savedSpeaker;      // a speaker is saved for reconnection
  uint8_t  wantConnected;     // last Control.wantConnected received
  uint8_t  reserved2;
  char     speakerName[32];   // active saved speaker name ("" if none)
  // ---- version 3 ----
  uint8_t  scanning;          // this boot is a scan (devices are being listed)
  uint8_t  connState;         // CONN_* below
  uint8_t  activeSlot;        // saved speaker slot used for reconnection, 0xFF none
  uint8_t  reserved3;
  uint16_t cmdSeqDone;        // Control.cmdSeq of the last command carried out
  uint16_t reserved4;
};
static_assert(sizeof(Report) == 52 + 40 + 8, "Report v3 must be 100 bytes");

enum : uint8_t { CONN_DISCONNECTED = 0, CONN_CONNECTING = 1, CONN_CONNECTED = 2 };

// Control (S3 -> classic). Idempotent: the classic acts on changes only, so
// repeating it every 100 ms is harmless and recovers from a lost packet.
struct __attribute__((packed)) Control {
  uint8_t  wantConnected;     // 1 = Bluetooth output selected, 0 = wired
  uint8_t  volume;            // speaker volume 0..127 (AVRCP absolute)
  uint16_t volSeq;            // incremented on every volume change by the S3
  // ---- version 3 ----
  uint8_t  cmd;               // CMD_* below; carried out once per new cmdSeq
  uint8_t  cmdArg;            // found-device index or saved slot
  uint16_t cmdSeq;            // the S3 starts from a random value at each boot
};
static_assert(sizeof(Control) == 8, "Control must be 8 bytes");

enum : uint8_t {
  CMD_NONE          = 0,
  CMD_SCAN          = 1,      // restart the classic into scan mode
  CMD_CONNECT_FOUND = 2,      // cmdArg = index in DeviceList.found
  CMD_CONNECT_SAVED = 3,      // cmdArg = saved slot (0/1); restarts the classic
  CMD_FORGET_SAVED  = 4,      // cmdArg = saved slot (0/1)
};

static constexpr uint8_t MAX_SAVED_SPEAKERS = 2;
static constexpr uint8_t MAX_FOUND_SPEAKERS = 16;

struct __attribute__((packed)) SpeakerEntry {
  char    name[32];
  uint8_t mac[6];
  int8_t  rssi;               // found devices; 0 for saved
  uint8_t flags;              // bit0 valid, bit1 is a saved speaker, bit2 active saved
};
static_assert(sizeof(SpeakerEntry) == 40, "SpeakerEntry must be 40 bytes");

// Classic -> S3. Only devices that reported a name are listed.
struct __attribute__((packed)) DeviceList {
  uint8_t      foundCount;
  uint8_t      scanning;
  uint8_t      activeSlot;    // 0xFF none
  uint8_t      reserved;
  SpeakerEntry saved[MAX_SAVED_SPEAKERS];
  SpeakerEntry found[MAX_FOUND_SPEAKERS];
};
static_assert(sizeof(DeviceList) == 4 + 40 * (2 + 16), "DeviceList must be 724 bytes");

inline uint32_t packetCrc(const Header &h, const uint8_t *payload, uint16_t len) {
  Header tmp = h;
  tmp.crc = 0;
  uint32_t c = esp_rom_crc32_le(0, reinterpret_cast<const uint8_t *>(&tmp), sizeof(tmp));
  if (len) c = esp_rom_crc32_le(c, payload, len);
  return c;
}

inline void buildHeader(Header &h, uint8_t type, uint16_t len, uint32_t seq,
                        uint32_t epoch, const uint8_t *payload) {
  memcpy(h.magic, MAGIC_BYTES, 4);
  h.type = type;
  h.version = LINK_VERSION;
  h.length = len;
  h.seq = seq;
  h.epoch = epoch;
  h.crc = 0;
  h.crc = packetCrc(h, payload, len);
}

// Incremental packet parser. Feed it whatever bytes arrive; it calls
// onPacket(header, payload, length) for every packet whose CRC checks.
// A packet with a bad CRC is counted and dropped: its header cannot be
// trusted, so its seq is not used for anything.
class Parser {
 public:
  uint32_t crcErrors   = 0;   // complete packets with a bad CRC
  uint32_t resyncBytes = 0;   // bytes skipped while looking for a header

  template <typename OnPacket>
  void feed(const uint8_t *p, size_t n, OnPacket onPacket) {
    while (n > 0) {
      // 1. Hunt for the 4 magic bytes, one byte at a time.
      if (used_ < 4) {
        uint8_t b = *p++;
        --n;
        if (b == MAGIC_BYTES[used_]) {
          buf_[used_++] = b;
        } else {
          resyncBytes += used_ + 1;
          used_ = 0;
          if (b == MAGIC_BYTES[0]) { buf_[used_++] = b; resyncBytes -= 1; }
        }
        continue;
      }
      // 2. Collect the rest of the header, then the payload.
      size_t target = (used_ < sizeof(Header)) ? sizeof(Header) : sizeof(Header) + len_;
      size_t take = target - used_;
      if (take > n) take = n;
      memcpy(buf_ + used_, p, take);
      used_ += take; p += take; n -= take;

      if (used_ == sizeof(Header) && !haveHeader_) {
        Header h;
        memcpy(&h, buf_, sizeof(h));
        if (h.version != LINK_VERSION || h.length > MAX_PAYLOAD ||
            (h.type != PKT_AUDIO && h.type != PKT_REPORT && h.type != PKT_CONTROL &&
             h.type != PKT_DEVICES)) {
          resyncBytes += sizeof(Header);
          used_ = 0;
          continue;
        }
        len_ = h.length;
        haveHeader_ = true;
      }
      if (haveHeader_ && used_ == sizeof(Header) + len_) {
        Header h;
        memcpy(&h, buf_, sizeof(h));
        const uint8_t *payload = buf_ + sizeof(Header);
        if (packetCrc(h, payload, len_) == h.crc) onPacket(h, payload, len_);
        else ++crcErrors;
        used_ = 0;
        len_ = 0;
        haveHeader_ = false;
      }
    }
  }

 private:
  uint8_t  buf_[sizeof(Header) + MAX_PAYLOAD];
  size_t   used_ = 0;
  uint16_t len_ = 0;
  bool     haveHeader_ = false;
};

}  // namespace wslink
