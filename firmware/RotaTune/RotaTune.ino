/*
  RotaTune v0.18.3 - volume arc
  An MP3 music player for the Waveshare ESP32-S3-Knob-Touch-LCD-1.8

  v0.18.3 changes (from v0.18.2):
  - The volume line on the playing screen is replaced by an arc along the top
    edge of the round screen, filling left to right with the volume, and a
    larger number under it ("45%", "BT 45%", or "WAITING" / "NO LINK" in
    Bluetooth mode). The arc turns grey while Bluetooth is waiting or has no
    link. Tapping the number still opens the Output page.

  Earlier (v0.18.2): Browse Albums and Browse Songs (A-Z)

  v0.18.2 changes (from v0.18.0/v0.18.1):
  - The library menu has Browse Artists, Browse Albums, Browse Songs and
    Browse Folders. Albums and Songs use the same A-Z letter grid as Artists.
  - Albums: every album starting with the letter, shown as "Album  (Artist)";
    tap one to see its tracks (PLAY ALBUM / ADD ALBUM as usual).
  - Songs: every song title starting with the letter, shown as
    "Title  (Artist)"; tap one to play it and carry on through its album.
    In ADD TRACKS mode a tap adds the song to the playlist.
  - The lists have no length limit (only the 5 visible rows are built); the
    sorted match list is kept in PSRAM until the letter or library changes.
  - PLAYER_VERSION was still "0.17.6"; now matches the release.

  Earlier (v0.18.0): first public release

  v0.18.0 changes (from v0.17.7):
  - Libraries are discovered on the SD card: every folder directly inside
    /Music_Library becomes a library (except Playlists and names starting with
    '.'), sorted by name, up to 8. Index files keep the <folder>_Library.txt
    names, so existing indexes are reused. The folder list is read again on
    every RESCAN. Personal library names removed from the code.
  - SHUF/RPT labels set when the buttons are created, and again after
    showBrowser() at start-up.

  Earlier (v0.17.7): SHUF off at power-up, wider SHUF / RPT

  v0.17.7 changes (from v0.17.6):
  - Shuffle always starts OFF at power-up; the setting is no longer saved
    (any value saved by v0.17.5/v0.17.6 is erased the next time SHUF/RPT is tapped
    and ignored at boot). Repeat is still saved.
  - SHUF and RPT buttons widened from 80 to 98 px and use the same font as
    SETTINGS (Browser controls size).
  - The buttons are labelled at start-up (they showed "Text" until tapped).
  - OFF uses the Library choices colours instead of grey on dark grey.

  Earlier (v0.17.6):
  Board: Waveshare ESP32-S3-Knob-Touch-LCD-1.8

  v0.17.6 changes (on top of v0.17.5):
  - SHUF and RPT moved from the playing screen to the browser screens, left and
    right of SETTINGS. Their text always states the setting (SHUF ON / SHUF OFF,
    RPT OFF / RPT ALL / RPT 1): on = black text on bright green, off = grey
    text on dark grey. (In v0.17.5 "on" was only a brighter shade.)

  v0.17.5 changes (on top of v0.17.4):
  - SHUF and RPT buttons on the playing screen (centre of the time row).
    SHUF: plays the current queue (album, folder or playlist) in random
    order; the playing track continues and the rest follow shuffled. With
    shuffle on, PLAY ALBUM / PLAY FOLDER / PLAY ALL start on a random track.
    RPT cycles OFF -> ALL (queue wraps around) -> ONE (the current track
    repeats at its end; Next / Previous still move on).
    Both settings are saved.
  - Play order kept in playOrder[]; the AudioTools next/previous callbacks
    step through it.

  v0.17.4 changes (on top of v0.17.3):
  - Tap the spectrum on the playing screen to cycle its style; the name is
    shown for 1.5 s and the choice is saved:
      MIRROR (as before), BARS (from the bottom, falling peaks), RADIAL (ring),
      SCOPE (waveform), LEVELS (left/right meters with peak hold), OFF.
  - All styles are drawn by the same single object (no extra LVGL memory).
    The FFT is skipped while the spectrum is OFF.

  v0.17.3 changes (on top of v0.17.2):
  - Rotary knob on browser screens (libraries, folders, artists, albums,
    tracks, playlists, Output page): one detent = one page, like NEXT / PREV
    (clockwise = NEXT). On the playing screen and in Settings it still sets
    the volume. Selection is still by tapping a row.
  - RESCAN moved to the bottom centre, below the page counter; it now needs a
    second tap within 3 s ("CONFIRM?"). SETTINGS centred where both were.

  v0.17.2 changes (on top of v0.16.9; classic receiver v0.5.0 unchanged):
  - PLAYLISTS row in the library list (page 1; OUTPUT moved to page 2).
    PLAYLISTS: NEW PLAYLIST (on-screen keyboard, created only while typing)
    and the playlists by name. A playlist: PLAY ALL, ADD TRACKS, DELETE
    PLAYLIST (tap twice), then its tracks (tap one to play from there).
  - ADD TRACKS: browse as usual; tap tracks, ADD ALBUM or ADD FOLDER to append.
    The heading confirms each addition. DONE ADDING (library list), BACK at
    the library list, or PLAYING returns to the playlist.
  - Same files as v0.17.1: M3U in /sdcard/Music_Library/Playlists, paths
    written as ../Library/...; "..", "." and Windows backslashes are resolved
    by the player, so playlists made on a PC work too.
  - Playlist playback uses the normal queue and auto-advance (v0.17.1 used the
    library's own auto-next).
  - Tap the volume line at the top of the playing screen to open the Output page.
  - [PERF] reports LVGL memory use (v0.17.1 crashed at boot when LVGL's 48 KB
    pool ran out while building its extra playlist screen).

  v0.16.9 changes (on top of v0.16.8b; needs classic receiver v0.5.0,
  link protocol v3):
  - The OUTPUT row of the library list opens an Output page:
      USE / USING WIRED, USE / USING BLUETOOTH
      SPEAKER <name> for each saved speaker (up to 2; the active one shows
        connected / connecting / not connected). Tap to switch to it.
      FIND SPEAKERS: the classic restarts into scan mode (about 3 s); named
        devices found are listed as FOUND <name> <signal> and the list fills
        in over a few seconds. Tap one to connect it; it is then saved.
      FORGET <name> for each saved speaker.
    Choosing a speaker also selects Bluetooth output. The page refreshes by
    itself while it is open.

  v0.16.8b changes (on top of v0.16.8a):
  - Spectrum bars 75 % of their former maximum height (72 -> 54 px half-height),
    so they stay clear of the volume text above; less screen area to redraw.
  - Bluetooth volume: speaker volume reports are ignored for 5 s after the
    stream starts and 2.5 s after a knob change (the speaker rounds the values
    it is sent, and v0.16.8a mistook that echo for a button press). A volume
    changed on the speaker still updates the screen, but only knob changes are
    saved, so a wrong report can no longer become the next boot's volume.
  - Wired mode: a [WIRED] line every 10 s shows the bytes per second actually
    written to the headphone output (I2S), to diagnose any silent wired output.

  v0.16.8a fix: the 32 KB Bluetooth PCM buffer could not be allocated in
  internal RAM (v0.16.8 hardware test), so no audio reached the speaker and
  decoding ran unpaced. The buffer's storage is now in PSRAM. Bluetooth output
  cannot be selected if the buffer is missing.

  v0.16.8 changes (on top of v0.16.7d):
  - Output selection, WIRED or BLUETOOTH: last row of the library (root)
    list; saved, and restored at boot (first default: wired).
  - Bluetooth output: decoded PCM -> 44.1 kHz stereo (mono duplicated, other
    rates resampled) -> UART1 link (TX GPIO38, RX GPIO48, 3 Mbaud, protocol
    v2) -> classic ESP32 receiver v0.4.0 -> A2DP speaker. Uses the Step 3
    credit flow control and local clock: when the speaker is away, playback
    keeps advancing silently and resumes at the current position.
  - In Bluetooth mode the knob sets the speaker's own volume (AVRCP, volume
    mode 1, PCM never scaled). The first Bluetooth volume is the knob's
    wired percentage capped at 50 %; after that it is remembered separately.
    Volume changes made with the speaker's buttons are followed.
  - Top line of the playing screen: "BT VOLUME n%" while the speaker
    streams, "BT n%  WAITING" (no speaker) or "BT n%  NO LINK" (classic not
    answering).
  - Folder lists that contain MP3 files start with PLAY FOLDER (whole folder
    from the first track; picking a track still continues through the folder).
  - S3 GPIO0 (DAC enable) is unchanged. Wired output is unchanged; in
    Bluetooth mode it is fed silence.

  v0.16.7d changes (on top of v0.16.7c):
  - The selected library's index (path, title, artist, album, track, disc)
    is loaded once into PSRAM. Artist, album and track lists, paging, Back
    and starting an album search memory instead of re-reading the index
    file from the SD card each time.
  - The first load of a library shows "Loading library..." and reads the
    file through a 16 KB buffer. The copy is dropped after a rescan.
  - If PSRAM is unavailable the old SD-card code is used unchanged.

  v0.16.7c changes (on top of v0.16.7b):
  - The 32-band spectrum is drawn by ONE LVGL object in a single draw
    callback, instead of 64 separate objects (32 bars + 32 peak markers).
    Bands, colours, positions and peak behaviour are unchanged; bar corners
    are square instead of 1-pixel rounded.
  - Each update marks only the band of rows the bars actually use for
    redraw, so quiet passages redraw a smaller area.

  v0.16.7b changes (on top of v0.16.7):
  - Decoding works in smaller steps (DECODE_STEP_BYTES of MP3 data per
    player.copy() call instead of the library default of about 1 KB), so
    volume, pause and track buttons wait less for the player.
  - The [PERF] audio figures now measure the idle time between one decode
    step ending and the next starting (the real risk to the audio), not the
    start-to-start time, which mostly measured the step itself.
  - LVGL task pinned to core 0 (audio stays on core 1).
  - Touchscreen read every 15 ms instead of 30 ms.

  v0.16.7 changes (wired output only; no Bluetooth code):
  - MP3 decoding (player.copy()) runs in its own task ("audio", core 1,
    priority 3, above LVGL and loop()), so browsing, index reads and screen
    updates no longer stall the audio.
  - Every other AudioPlayer call (start, next, previous, pause, volume,
    seek) is made under playerMutex, so only one task uses the player at a
    time. Long browser index reads do not hold the mutex.
  - Playback buttons go into a queue (8 deep) instead of a single slot, so
    no press is lost; Next / Previous / Play-Pause are now logged.
  - Diagnostics: every button press logs how long it waited and how long it
    took to handle; a [PERF] line every 10 s reports the longest loop()
    pass, audio-task gaps, LVGL redraw time, touch-read gaps, LVGL lock
    waits and touch I2C errors.
  - Auto-advance: at the end of a track the next track in the current
    list starts; after the last one playback stops. (Done by the audio
    task; the library's own setAutoNext stays false.)
  Playback behaviour is otherwise unchanged from v0.16.6.

  Target processor: ESP32-S3

  Preserves v0.2 SDMMC and AudioTools/Helix playback.
  Adds the Waveshare SH8601 QSPI AMOLED display and CST816 touch initialization.
  Adds Xing/Info gapless-duration handling and sample-accurate decoded playback position, a
  progress bar, and a 32-band mirrored FFT spectrum analyzer.
  Album-art decoding is intentionally disabled in this release.
  Adds persistent tab-separated indexes, one per library folder.
  The original music folders and files are left in place.

  SDMMC: CLK=4, CMD=3, D0=5, D1=6, D2=42, D3=2
  I2S:   BCLK=39, WS=40, DOUT=41; DAC enable GPIO0 HIGH
  LCD:   SH8601 QSPI; pins are in lcd_config.h
  Touch: CST816 I2C; pins are in lcd_config.h
*/

#include <Arduino.h>
#include "player_type_fwd.h"  // Types used by Arduino-generated function prototypes
#include "player_ui_config.h" // User-adjustable display layout, fonts, and colors
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdlib.h>

#include "sd_card_bsp.h"
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
#include "driver/gpio.h"
#include "bidi_switch_knob.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_random.h"
#include "esp_heap_caps.h"
#include <Preferences.h>
#include "freertos/stream_buffer.h"
#include "link_protocol.h"   // v0.16.8: S3 <-> classic ESP32 link, protocol v2
#include <atomic>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lcd_bsp.h"
#include "player_perf.h"
#include "lcd_bl_pwm_bsp.h"
#include "cst816.h"

// ----------------------- Display color configuration --------------------
// All palette values use RGB888: 0xRRGGBB. lv_color_hex() converts them
// for the display's native color format. Change the palette here, then
// assign palette colors to UI roles in the section below.
#define C_BLACK        0x000000
#define C_WHITE        0xFFFFFF
#define C_RED          0xFF0000
#define C_GREEN        0x00FF00
#define C_BLUE         0x0000FF
#define C_YELLOW       0xFFFF00
#define C_CYAN         0x00FFFF
#define C_MAGENTA      0xFF00FF
#define C_GRAY         0x808080
#define C_DARK_GRAY    0x404040
#define C_LIGHT_GRAY   0xD3D3D3
#define C_LIGHT_BLUE   0xADD8E6
#define C_LIGHT_GREEN  0x90EE90
#define C_DARK_BLUE    0x173B55
#define C_PRESSED_BLUE 0x245675
#define C_DEEP_NAVY    0x080D13
#define C_DARK_GREEN   0x28533D
#define C_ACCENT_BLUE  0x075A9C
#define C_ACCENT_BLUE_PRESSED 0x1687D4
#define C_PLAY_GREEN   0x087A50
#define C_PLAY_GREEN_PRESSED 0x12A86F
#define C_MUTED_BLUE_GRAY 0x9FB3C8
#define C_PALE_BLUE_GRAY  0xDCE6F1
#define C_EMPTY_TEXT   0xAAB7C4
#define C_EQ_BASELINE  0x24313D
#define C_PROGRESS_TRACK 0x263342
#define C_PROGRESS_FILL  0x2997F0
#define C_PLAY_STATE  0x20D6B0
#define C_EQ_PURPLE_1 0xA66BDF
#define C_EQ_PURPLE_2 0x946DEB
#define C_EQ_PURPLE_3 0x806FF4
#define C_EQ_BLUE_1   0x687AF7
#define C_EQ_BLUE_2   0x4D91F4
#define C_EQ_BLUE_3   0x35A9E8
#define C_EQ_CYAN     0x25C4D2
#define C_EQ_GREEN    0x20D6A5

// UI role assignments: customize an element without changing the palette.
#define COLOR_BROWSER_BACKGROUND       C_DEEP_NAVY
#define COLOR_BROWSER_HEADER_BG        C_LIGHT_GREEN
#define COLOR_BROWSER_HEADER_TEXT      C_BLACK
#define COLOR_BROWSER_ENTRY_BG         C_DARK_BLUE
#define COLOR_BROWSER_ENTRY_PRESSED_BG C_PRESSED_BLUE
#define COLOR_BROWSER_ENTRY_TEXT       C_WHITE
#define COLOR_BROWSER_EMPTY_TEXT       C_EMPTY_TEXT
#define COLOR_BROWSER_NAV_BG           C_LIGHT_BLUE
#define COLOR_BROWSER_NAV_TEXT         C_BLACK
#define COLOR_BROWSER_PAGE_TEXT        C_WHITE
#define COLOR_BROWSER_CONTROL_BG       C_DARK_BLUE
#define COLOR_BROWSER_RESCAN_BG        C_DARK_GREEN
#define COLOR_BROWSER_CONTROL_TEXT    C_WHITE

#define COLOR_PLAYBACK_BACKGROUND      C_BLACK
#define COLOR_PLAYBACK_BROWSE_BG       C_ACCENT_BLUE
#define COLOR_PLAYBACK_VOLUME_TEXT     C_MUTED_BLUE_GRAY
#define COLOR_PLAYBACK_TITLE_TEXT      C_WHITE
#define COLOR_PLAYBACK_TRACK_TEXT      C_PALE_BLUE_GRAY
#define COLOR_PLAYBACK_TIME_TEXT       C_WHITE
#define COLOR_PLAYBACK_DURATION_TEXT   C_MUTED_BLUE_GRAY
#define COLOR_PLAYBACK_PROGRESS_BG     C_PROGRESS_TRACK
#define COLOR_PLAYBACK_PROGRESS_FILL   C_PROGRESS_FILL
#define COLOR_PLAYBACK_STATE_TEXT      C_PLAY_STATE
#define COLOR_PLAYBACK_BUTTON_TEXT     C_WHITE
#define COLOR_PLAYBACK_BUTTON_BG       C_ACCENT_BLUE
#define COLOR_PLAYBACK_BUTTON_PRESSED  C_ACCENT_BLUE_PRESSED
#define COLOR_PLAYBACK_TOGGLE_BG       C_PLAY_GREEN
#define COLOR_PLAYBACK_TOGGLE_PRESSED  C_PLAY_GREEN_PRESSED

// --------------------------- Board audio pins ---------------------------
static constexpr int BOARD_AUDIO_BCLK = 39;
static constexpr int BOARD_AUDIO_WS   = 40;
static constexpr int BOARD_AUDIO_DOUT = 41;
static constexpr int BOARD_DAC_ENABLE = 0;
static constexpr int BOARD_ENCODER_A = 8;
static constexpr int BOARD_ENCODER_B = 7;
static constexpr int VOLUME_STEP = 2;

static constexpr char PLAYER_VERSION[] = "0.18.3";

// ---------------------- User interface color settings ------------------
// Written by Waveshare_Color_Customizer_v0.1.2 to this SD-card file.
static constexpr char PLAYER_COLOR_FILE[] = "/sdcard/Music_Library/player_colors.txt";
static constexpr uint32_t DEFAULT_LIBRARY_HEADER_BG = PlayerUIConfig::DEFAULT_LIBRARY_HEADER_BG;
static constexpr uint32_t DEFAULT_LIBRARY_HEADER_TEXT = PlayerUIConfig::DEFAULT_LIBRARY_HEADER_TEXT;
static constexpr uint32_t DEFAULT_LIBRARY_ENTRY_BG = PlayerUIConfig::DEFAULT_LIBRARY_ENTRY_BG;
static constexpr uint32_t DEFAULT_LIBRARY_ENTRY_TEXT = PlayerUIConfig::DEFAULT_LIBRARY_ENTRY_TEXT;
static constexpr uint32_t DEFAULT_BROWSER_ENTRY_BG = PlayerUIConfig::DEFAULT_BROWSER_ENTRY_BG;
static constexpr uint32_t DEFAULT_BROWSER_ENTRY_TEXT = PlayerUIConfig::DEFAULT_BROWSER_ENTRY_TEXT;
static constexpr uint32_t DEFAULT_NAV_BUTTON_BG = PlayerUIConfig::DEFAULT_NAV_BUTTON_BG;
static constexpr uint32_t DEFAULT_NAV_BUTTON_TEXT = PlayerUIConfig::DEFAULT_NAV_BUTTON_TEXT;
static constexpr uint32_t DEFAULT_BROWSER_CONTROL_BG = PlayerUIConfig::DEFAULT_BROWSER_CONTROL_BG;
static constexpr uint32_t DEFAULT_BROWSER_CONTROL_TEXT = PlayerUIConfig::DEFAULT_BROWSER_CONTROL_TEXT;
static constexpr uint32_t DEFAULT_PLAYBACK_BACKGROUND = PlayerUIConfig::DEFAULT_PLAYBACK_BACKGROUND;
static constexpr uint32_t DEFAULT_PLAYBACK_TITLE_TEXT = PlayerUIConfig::DEFAULT_PLAYBACK_TITLE_TEXT;
static constexpr uint32_t DEFAULT_PLAYBACK_BUTTON_BG = PlayerUIConfig::DEFAULT_PLAYBACK_BUTTON_BG;
static constexpr uint32_t DEFAULT_PLAYBACK_BUTTON_TEXT = PlayerUIConfig::DEFAULT_PLAYBACK_BUTTON_TEXT;
static constexpr uint32_t DEFAULT_PLAYBACK_STATUS_BG = PlayerUIConfig::DEFAULT_PLAYBACK_STATUS_BG;
static constexpr uint32_t DEFAULT_PLAYBACK_STATE_TEXT = PlayerUIConfig::DEFAULT_PLAYBACK_STATE_TEXT;

static uint32_t uiLibraryHeaderBg = DEFAULT_LIBRARY_HEADER_BG;
static uint32_t uiLibraryHeaderText = DEFAULT_LIBRARY_HEADER_TEXT;
static uint32_t uiLibraryEntryBg = DEFAULT_LIBRARY_ENTRY_BG;
static uint32_t uiLibraryEntryText = DEFAULT_LIBRARY_ENTRY_TEXT;
static uint32_t uiBrowserEntryBg = DEFAULT_BROWSER_ENTRY_BG;
static uint32_t uiBrowserEntryText = DEFAULT_BROWSER_ENTRY_TEXT;
static uint32_t uiNavButtonBg = DEFAULT_NAV_BUTTON_BG;
static uint32_t uiNavButtonText = DEFAULT_NAV_BUTTON_TEXT;
static uint32_t uiNavDisabledBg = PlayerUIConfig::DEFAULT_NAV_DISABLED_BG;
static uint32_t uiNavDisabledText = PlayerUIConfig::DEFAULT_NAV_DISABLED_TEXT;
static uint32_t uiBrowserControlBg = DEFAULT_BROWSER_CONTROL_BG;
static uint32_t uiBrowserControlText = DEFAULT_BROWSER_CONTROL_TEXT;
static uint32_t uiPlaybackBackground = DEFAULT_PLAYBACK_BACKGROUND;
static uint32_t uiPlaybackTitleText = DEFAULT_PLAYBACK_TITLE_TEXT;
static uint32_t uiPlaybackButtonBg = DEFAULT_PLAYBACK_BUTTON_BG;
static bool uiPlaybackButtonBgWasLoaded = false;
static uint32_t uiPlaybackButtonText = DEFAULT_PLAYBACK_BUTTON_TEXT;
static uint32_t uiPlaybackStatusBg = DEFAULT_PLAYBACK_STATUS_BG;
static uint32_t uiPlaybackStateText = DEFAULT_PLAYBACK_STATE_TEXT;
static uint8_t uiFontLibraryHeader = PlayerUIConfig::FONT_LIBRARY_HEADER, uiFontLibraryEntry = PlayerUIConfig::FONT_LIBRARY_ENTRY;
static uint8_t uiFontBrowserEntry = PlayerUIConfig::FONT_BROWSER_ENTRY, uiFontNavButton = PlayerUIConfig::FONT_NAV_BUTTON;
static uint8_t uiFontBrowserControl = PlayerUIConfig::FONT_BROWSER_CONTROL, uiFontPlaybackTitle = PlayerUIConfig::FONT_PLAYBACK_TITLE;
static uint8_t uiFontPlaybackButton = PlayerUIConfig::FONT_PLAYBACK_BUTTON, uiFontPlaybackStatus = PlayerUIConfig::FONT_PLAYBACK_STATUS;

static const lv_font_t *playerFont(uint8_t size) {
  switch (size) {
    case 12: return &lv_font_montserrat_12;
    case 14: return &lv_font_montserrat_14;
    case 16: return &lv_font_montserrat_16;
    case 18: return &lv_font_montserrat_18;
    case 20: return &lv_font_montserrat_20;
    case 22: return &lv_font_montserrat_22;
    case 24: return &lv_font_montserrat_24;
    case 26: return &lv_font_montserrat_26;
    case 28: return &lv_font_montserrat_28;
    default: return &lv_font_montserrat_18;
  }
}

static bool validPlayerFontSize(unsigned long n) {
  return n == 12 || n == 14 || n == 16 || n == 18 || n == 20 ||
         n == 22 || n == 24 || n == 26 || n == 28;
}

static bool parsePlayerColor(uint32_t &target, const char *value) {
  if (!value || !*value) return false;
  char *end = nullptr;
  unsigned long n = strtoul(value, &end, 0);
  while (end && isspace((unsigned char)*end)) ++end;
  if (end == value || (end && *end) || n > 0xFFFFFFUL) return false;
  target = (uint32_t)n;
  return true;
}

static bool parsePlayerFont(uint8_t &target, const char *value) {
  if (!value || !*value) return false;
  char *end = nullptr;
  unsigned long n = strtoul(value, &end, 10);
  while (end && isspace((unsigned char)*end)) ++end;
  if (end == value || (end && *end) || !validPlayerFontSize(n)) return false;
  target = (uint8_t)n;
  return true;
}

static size_t loadPlayerColorSettings() {
  FILE *file = fopen(PLAYER_COLOR_FILE, "r");
  if (!file) {
    Serial.printf("[UI] No %s found; using built-in colors and font sizes.\n", PLAYER_COLOR_FILE);
    return 0;
  }
  struct Setting { const char *key; uint32_t *color; uint8_t *font; };
  Setting settings[] = {
    {"LIBRARY_HEADER_BG", &uiLibraryHeaderBg, nullptr},
    {"LIBRARY_HEADER_TEXT", &uiLibraryHeaderText, nullptr},
    {"FONT_LIBRARY_HEADER", nullptr, &uiFontLibraryHeader},
    {"LIBRARY_ENTRY_BG", &uiLibraryEntryBg, nullptr},
    {"LIBRARY_ENTRY_TEXT", &uiLibraryEntryText, nullptr},
    {"FONT_LIBRARY_ENTRY", nullptr, &uiFontLibraryEntry},
    {"BROWSER_ENTRY_BG", &uiBrowserEntryBg, nullptr},
    {"BROWSER_ENTRY_TEXT", &uiBrowserEntryText, nullptr},
    {"FONT_BROWSER_ENTRY", nullptr, &uiFontBrowserEntry},
    {"NAV_BUTTON_BG", &uiNavButtonBg, nullptr},
    {"NAV_BUTTON_TEXT", &uiNavButtonText, nullptr},
    {"FONT_NAV_BUTTON", nullptr, &uiFontNavButton},
    {"NAV_DISABLED_BG", &uiNavDisabledBg, nullptr},
    {"NAV_DISABLED_TEXT", &uiNavDisabledText, nullptr},
    {"BROWSER_CONTROL_BG", &uiBrowserControlBg, nullptr},
    {"BROWSER_CONTROL_TEXT", &uiBrowserControlText, nullptr},
    {"FONT_BROWSER_CONTROL", nullptr, &uiFontBrowserControl},
    {"PLAYBACK_BACKGROUND", &uiPlaybackBackground, nullptr},
    {"PLAYBACK_TITLE_TEXT", &uiPlaybackTitleText, nullptr},
    {"FONT_PLAYBACK_TITLE", nullptr, &uiFontPlaybackTitle},
    {"PLAYBACK_BUTTON_BG", &uiPlaybackButtonBg, nullptr},
    {"PLAYBACK_BUTTON_TEXT", &uiPlaybackButtonText, nullptr},
    {"FONT_PLAYBACK_BUTTON", nullptr, &uiFontPlaybackButton},
    {"PLAYBACK_STATUS_BG", &uiPlaybackStatusBg, nullptr},
    {"PLAYBACK_STATE_TEXT", &uiPlaybackStateText, nullptr},
    {"FONT_PLAYBACK_STATUS", nullptr, &uiFontPlaybackStatus}
  };
  char line[160];
  size_t loaded = 0;
  while (fgets(line, sizeof(line), file)) {
    char *p = line;
    while (isspace((unsigned char)*p)) ++p;
    if (!*p || *p == '#') continue;
    char *eq = strchr(p, '=');
    if (!eq) continue;
    *eq++ = '\0';
    char *keyEnd = p + strlen(p);
    while (keyEnd > p && isspace((unsigned char)keyEnd[-1])) *--keyEnd = '\0';
    while (isspace((unsigned char)*eq)) ++eq;
    char *valueEnd = eq + strlen(eq);
    while (valueEnd > eq && isspace((unsigned char)valueEnd[-1])) *--valueEnd = '\0';
    for (size_t i = 0; i < sizeof(settings) / sizeof(settings[0]); ++i) {
      if (strcmp(p, settings[i].key) != 0) continue;
      bool ok = settings[i].color ? parsePlayerColor(*settings[i].color, eq)
                                  : parsePlayerFont(*settings[i].font, eq);
      if (ok) {
        ++loaded;
        if (strcmp(p, "PLAYBACK_BUTTON_BG") == 0) uiPlaybackButtonBgWasLoaded = true;
      } else Serial.printf("[UI] Ignoring invalid setting: %s\n", p);
      break;
    }
  }
  fclose(file);
  Serial.printf("[UI] Loaded %u settings from player_colors.txt.\n", (unsigned)loaded);
  return loaded;
}


// ---------------------- Touchscreen appearance settings -----------------
// These nine groups mirror the player appearance settings. Values are
// aliases to the same variables used by the player UI and startup parser.
struct PlayerColorGroup {
  const char *name;
  const char *bgKey;
  const char *textKey;
  const char *fontKey;
  uint32_t *bg;
  uint32_t *text;
  uint8_t *font;
};
static PlayerColorGroup playerColorGroups[] = {
  {"Library header", "LIBRARY_HEADER_BG", "LIBRARY_HEADER_TEXT", "FONT_LIBRARY_HEADER",
   &uiLibraryHeaderBg, &uiLibraryHeaderText, &uiFontLibraryHeader},
  {"Library choices", "LIBRARY_ENTRY_BG", "LIBRARY_ENTRY_TEXT", "FONT_LIBRARY_ENTRY",
   &uiLibraryEntryBg, &uiLibraryEntryText, &uiFontLibraryEntry},
  {"Browser list", "BROWSER_ENTRY_BG", "BROWSER_ENTRY_TEXT", "FONT_BROWSER_ENTRY",
   &uiBrowserEntryBg, &uiBrowserEntryText, &uiFontBrowserEntry},
  {"PREV / NEXT", "NAV_BUTTON_BG", "NAV_BUTTON_TEXT", "FONT_NAV_BUTTON",
   &uiNavButtonBg, &uiNavButtonText, &uiFontNavButton},
  {"Page highlight", "NAV_DISABLED_BG", "NAV_DISABLED_TEXT", nullptr,
   &uiNavDisabledBg, &uiNavDisabledText, nullptr},
  {"Browser controls", "BROWSER_CONTROL_BG", "BROWSER_CONTROL_TEXT", "FONT_BROWSER_CONTROL",
   &uiBrowserControlBg, &uiBrowserControlText, &uiFontBrowserControl},
  {"Playback screen", "PLAYBACK_BACKGROUND", "PLAYBACK_TITLE_TEXT", "FONT_PLAYBACK_TITLE",
   &uiPlaybackBackground, &uiPlaybackTitleText, &uiFontPlaybackTitle},
  {"Playback buttons", "PLAYBACK_BUTTON_BG", "PLAYBACK_BUTTON_TEXT", "FONT_PLAYBACK_BUTTON",
   &uiPlaybackButtonBg, &uiPlaybackButtonText, &uiFontPlaybackButton},
  {"Playback status", "PLAYBACK_STATUS_BG", "PLAYBACK_STATE_TEXT", "FONT_PLAYBACK_STATUS",
   &uiPlaybackStatusBg, &uiPlaybackStateText, &uiFontPlaybackStatus}
};
static constexpr size_t PLAYER_COLOR_GROUP_COUNT =
  sizeof(playerColorGroups) / sizeof(playerColorGroups[0]);
static const uint8_t PLAYER_FONT_SIZES[] = {12,14,16,18,20,22,24,26,28};
static constexpr size_t PLAYER_FONT_SIZE_COUNT =
  sizeof(PLAYER_FONT_SIZES) / sizeof(PLAYER_FONT_SIZES[0]);

// Forward declarations for settings/browser objects used by the callbacks below.
// Their initialized definitions remain with the other LVGL globals later.
static lv_obj_t *browserScreen = nullptr;
static lv_obj_t *settingsScreen = nullptr;
static lv_obj_t *settingsPreviewPanel = nullptr;
static lv_obj_t *settingsPreviewLabel = nullptr;
static lv_obj_t *settingsElementLabel = nullptr;
static lv_obj_t *settingsModeLabel = nullptr;
static lv_obj_t *settingsHexLabel = nullptr;
static lv_obj_t *settingsFontLabel = nullptr;
static lv_obj_t *settingsStatusLabel = nullptr;
static lv_obj_t *settingsSliders[3] = {nullptr, nullptr, nullptr};
static lv_obj_t *settingsRgbLabels[3] = {nullptr, nullptr, nullptr};
static size_t settingsSelectedGroup = 0;
static bool settingsEditingText = false;

static uint32_t &settingsActiveColor() {
  return settingsEditingText ? *playerColorGroups[settingsSelectedGroup].text
                             : *playerColorGroups[settingsSelectedGroup].bg;
}
static size_t settingsFontIndex(uint8_t size) {
  for (size_t i=0; i<PLAYER_FONT_SIZE_COUNT; ++i)
    if (PLAYER_FONT_SIZES[i] == size) return i;
  return 3;
}
static void settingsUpdatePreview() {
  if (!settingsPreviewPanel || !settingsPreviewLabel) return;
  PlayerColorGroup &g = playerColorGroups[settingsSelectedGroup];
  lv_obj_set_style_bg_color(settingsPreviewPanel, lv_color_hex(*g.bg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(settingsPreviewPanel, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_text_color(settingsPreviewLabel, lv_color_hex(*g.text), LV_PART_MAIN);
  if (g.font) lv_obj_set_style_text_font(settingsPreviewLabel, playerFont(*g.font), LV_PART_MAIN);
  lv_label_set_text(settingsPreviewLabel, "Aa  BUTTON  123");
  char label[80];
  snprintf(label, sizeof(label), "%s  (%u/%u)", g.name,
           (unsigned)(settingsSelectedGroup+1), (unsigned)PLAYER_COLOR_GROUP_COUNT);
  lv_label_set_text(settingsElementLabel, label);
  lv_label_set_text(settingsModeLabel, settingsEditingText ? "EDITING TEXT COLOR" : "EDITING BACKGROUND COLOR");
  char hex[32];
  snprintf(hex, sizeof(hex), "%s  #%06lX", settingsEditingText ? "TEXT" : "BACKGROUND",
           (unsigned long)(settingsActiveColor() & 0xFFFFFF));
  lv_label_set_text(settingsHexLabel, hex);
  char fontText[20];
  if (g.font) snprintf(fontText, sizeof(fontText), "SIZE %u", *g.font);
  else snprintf(fontText, sizeof(fontText), "N/A");
  lv_label_set_text(settingsFontLabel, fontText);
}
static void settingsLoadSliders() {
  uint32_t c = settingsActiveColor();
  int v[3] = {(int)((c>>16)&255), (int)((c>>8)&255), (int)(c&255)};
  for (int i=0; i<3; ++i) {
    lv_slider_set_value(settingsSliders[i], v[i], LV_ANIM_OFF);
    char b[8]; snprintf(b,sizeof(b),"%02X",v[i]);
    lv_label_set_text(settingsRgbLabels[i], b);
  }
  settingsUpdatePreview();
}
static void settingsSliderEvent(lv_event_t *e) {
  if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;
  int ch = (int)(intptr_t)lv_event_get_user_data(e);
  uint32_t c = settingsActiveColor();
  uint32_t v = (uint32_t)lv_slider_get_value(settingsSliders[ch]);
  if (ch==0) c=(c&0x00FFFFUL)|(v<<16);
  else if (ch==1) c=(c&0xFF00FFUL)|(v<<8);
  else c=(c&0xFFFF00UL)|v;
  settingsActiveColor()=c;
  char b[8]; snprintf(b,sizeof(b),"%02X",(unsigned)v);
  lv_label_set_text(settingsRgbLabels[ch],b);
  settingsUpdatePreview();
}
static void settingsPrevEvent(lv_event_t *e) {
  if (lv_event_get_code(e)!=LV_EVENT_CLICKED) return;
  settingsSelectedGroup=(settingsSelectedGroup+PLAYER_COLOR_GROUP_COUNT-1)%PLAYER_COLOR_GROUP_COUNT;
  settingsLoadSliders();
}
static void settingsNextEvent(lv_event_t *e) {
  if (lv_event_get_code(e)!=LV_EVENT_CLICKED) return;
  settingsSelectedGroup=(settingsSelectedGroup+1)%PLAYER_COLOR_GROUP_COUNT;
  settingsLoadSliders();
}
static void settingsBgEvent(lv_event_t *e) {
  if (lv_event_get_code(e)!=LV_EVENT_CLICKED) return;
  settingsEditingText=false; settingsLoadSliders();
}
static void settingsTextEvent(lv_event_t *e) {
  if (lv_event_get_code(e)!=LV_EVENT_CLICKED) return;
  settingsEditingText=true; settingsLoadSliders();
}
static void settingsFontDownEvent(lv_event_t *e) {
  if (lv_event_get_code(e)!=LV_EVENT_CLICKED) return;
  PlayerColorGroup &g=playerColorGroups[settingsSelectedGroup];
  if (!g.font) return;
  size_t i=settingsFontIndex(*g.font);
  if(i>0) *g.font=PLAYER_FONT_SIZES[i-1];
  settingsUpdatePreview();
}
static void settingsFontUpEvent(lv_event_t *e) {
  if (lv_event_get_code(e)!=LV_EVENT_CLICKED) return;
  PlayerColorGroup &g=playerColorGroups[settingsSelectedGroup];
  if (!g.font) return;
  size_t i=settingsFontIndex(*g.font);
  if(i+1<PLAYER_FONT_SIZE_COUNT) *g.font=PLAYER_FONT_SIZES[i+1];
  settingsUpdatePreview();
}
static void settingsSaveEvent(lv_event_t *e) {
  if (lv_event_get_code(e)!=LV_EVENT_CLICKED) return;
  FILE *f=fopen(PLAYER_COLOR_FILE,"w");
  if(!f) {
    lv_label_set_text(settingsStatusLabel,"SAVE FAILED - CHECK SD CARD");
    return;
  }
  fprintf(f,"# RotaTune appearance settings\n# RGB888 colors\n");
  for(size_t i=0;i<PLAYER_COLOR_GROUP_COUNT;++i) {
    PlayerColorGroup &g=playerColorGroups[i];
    fprintf(f,"%s=0x%06lX\n%s=0x%06lX\n",
      g.bgKey,(unsigned long)(*g.bg&0xFFFFFF),
      g.textKey,(unsigned long)(*g.text&0xFFFFFF));
    if (g.fontKey && g.font) fprintf(f,"%s=%u\n",g.fontKey,(unsigned)*g.font);
  }
  fclose(f);
  uiPlaybackButtonBgWasLoaded=true;
  lv_label_set_text(settingsStatusLabel,"SAVED - RESTART PLAYER TO APPLY");
  Serial.println("[UI] Appearance settings saved to player_colors.txt.");
}
static void settingsDefaultsEvent(lv_event_t *e) {
  if(lv_event_get_code(e)!=LV_EVENT_CLICKED) return;
  const uint32_t bg[]={PlayerUIConfig::DEFAULT_LIBRARY_HEADER_BG,PlayerUIConfig::DEFAULT_LIBRARY_ENTRY_BG,PlayerUIConfig::DEFAULT_BROWSER_ENTRY_BG,PlayerUIConfig::DEFAULT_NAV_BUTTON_BG,PlayerUIConfig::DEFAULT_NAV_DISABLED_BG,PlayerUIConfig::DEFAULT_BROWSER_CONTROL_BG,PlayerUIConfig::DEFAULT_PLAYBACK_BACKGROUND,PlayerUIConfig::DEFAULT_PLAYBACK_BUTTON_BG,PlayerUIConfig::DEFAULT_PLAYBACK_STATUS_BG};
  const uint32_t tx[]={PlayerUIConfig::DEFAULT_LIBRARY_HEADER_TEXT,PlayerUIConfig::DEFAULT_LIBRARY_ENTRY_TEXT,PlayerUIConfig::DEFAULT_BROWSER_ENTRY_TEXT,PlayerUIConfig::DEFAULT_NAV_BUTTON_TEXT,PlayerUIConfig::DEFAULT_NAV_DISABLED_TEXT,PlayerUIConfig::DEFAULT_BROWSER_CONTROL_TEXT,PlayerUIConfig::DEFAULT_PLAYBACK_TITLE_TEXT,PlayerUIConfig::DEFAULT_PLAYBACK_BUTTON_TEXT,PlayerUIConfig::DEFAULT_PLAYBACK_STATE_TEXT};
  const uint8_t fs[]={PlayerUIConfig::FONT_LIBRARY_HEADER,PlayerUIConfig::FONT_LIBRARY_ENTRY,PlayerUIConfig::FONT_BROWSER_ENTRY,PlayerUIConfig::FONT_NAV_BUTTON,0,PlayerUIConfig::FONT_BROWSER_CONTROL,PlayerUIConfig::FONT_PLAYBACK_TITLE,PlayerUIConfig::FONT_PLAYBACK_BUTTON,PlayerUIConfig::FONT_PLAYBACK_STATUS};
  for(size_t i=0;i<PLAYER_COLOR_GROUP_COUNT;++i) {
    *playerColorGroups[i].bg=bg[i]; *playerColorGroups[i].text=tx[i];
    if (playerColorGroups[i].font) *playerColorGroups[i].font=fs[i];
  }
  uiPlaybackButtonBgWasLoaded=false;
  settingsLoadSliders();
  lv_label_set_text(settingsStatusLabel,"DEFAULTS RESTORED - PRESS SAVE");
}
static volatile bool settingsOpen = false;   // v0.17.3: knob sets volume while Settings is shown
static void settingsBackEvent(lv_event_t *e) {
  if(lv_event_get_code(e)==LV_EVENT_CLICKED && browserScreen) { lv_scr_load(browserScreen); settingsOpen = false; }
}
static void settingsOpenEvent(lv_event_t *e) {
  if(lv_event_get_code(e)==LV_EVENT_CLICKED && settingsScreen) {
    settingsLoadSliders();
    lv_scr_load(settingsScreen);
    settingsOpen = true;
  }
}
static lv_obj_t *settingsMakeButton(lv_obj_t *parent,const char *caption,int w,int h,
                                   lv_align_t align,int x,int y,lv_event_cb_t cb) {
  lv_obj_t *b=lv_btn_create(parent);
  lv_obj_set_size(b,w,h); lv_obj_align(b,align,x,y);
  lv_obj_set_style_radius(b,7,LV_PART_MAIN);
  lv_obj_set_style_bg_color(b,lv_color_hex(0x24465F),LV_PART_MAIN);
  lv_obj_set_style_bg_color(b,lv_color_hex(0x36769A),LV_PART_MAIN|LV_STATE_PRESSED);
  lv_obj_set_style_text_color(b,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
  lv_obj_set_style_text_font(b,&lv_font_montserrat_14,LV_PART_MAIN);
  lv_obj_add_event_cb(b,cb,LV_EVENT_CLICKED,nullptr);
  lv_obj_t *l=lv_label_create(b); lv_label_set_text(l,caption); lv_obj_center(l);
  return b;
}
static void createPlayerSettingsScreen() {
  settingsScreen=lv_obj_create(nullptr);
  lv_obj_clear_flag(settingsScreen,LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(settingsScreen,lv_color_hex(0x080D13),LV_PART_MAIN);
  lv_obj_set_style_bg_opa(settingsScreen,LV_OPA_COVER,LV_PART_MAIN);
  lv_obj_t *title=lv_label_create(settingsScreen);
  lv_label_set_text(title,"PLAYER APPEARANCE");
  lv_obj_set_style_text_color(title,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
  lv_obj_set_style_text_font(title,&lv_font_montserrat_14,LV_PART_MAIN);
  lv_obj_align(title,LV_ALIGN_TOP_MID,0,PlayerUIConfig::SETTINGS_TITLE_Y);
  settingsElementLabel=lv_label_create(settingsScreen);
  lv_obj_set_style_text_color(settingsElementLabel,lv_color_hex(0xDCE6F1),LV_PART_MAIN);
  lv_obj_set_style_text_font(settingsElementLabel,&lv_font_montserrat_14,LV_PART_MAIN);
  lv_obj_align(settingsElementLabel,LV_ALIGN_TOP_MID,0,PlayerUIConfig::SETTINGS_ELEMENT_Y);
  settingsMakeButton(settingsScreen,"< PREV",PlayerUIConfig::SETTINGS_PREV_NEXT_WIDTH,PlayerUIConfig::SETTINGS_PREV_NEXT_HEIGHT,LV_ALIGN_TOP_MID,PlayerUIConfig::SETTINGS_PREV_X,PlayerUIConfig::SETTINGS_PREV_NEXT_Y,settingsPrevEvent);
  settingsMakeButton(settingsScreen,"NEXT >",PlayerUIConfig::SETTINGS_PREV_NEXT_WIDTH,PlayerUIConfig::SETTINGS_PREV_NEXT_HEIGHT,LV_ALIGN_TOP_MID,PlayerUIConfig::SETTINGS_NEXT_X,PlayerUIConfig::SETTINGS_PREV_NEXT_Y,settingsNextEvent);
  settingsPreviewPanel=lv_obj_create(settingsScreen);
  lv_obj_set_size(settingsPreviewPanel,PlayerUIConfig::SETTINGS_PREVIEW_WIDTH,PlayerUIConfig::SETTINGS_PREVIEW_HEIGHT);
  lv_obj_align(settingsPreviewPanel,LV_ALIGN_TOP_MID,0,PlayerUIConfig::SETTINGS_PREVIEW_Y);
  lv_obj_set_style_radius(settingsPreviewPanel,8,LV_PART_MAIN);
  lv_obj_set_style_border_width(settingsPreviewPanel,2,LV_PART_MAIN);
  lv_obj_set_style_border_color(settingsPreviewPanel,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
  settingsPreviewLabel=lv_label_create(settingsPreviewPanel);
  lv_obj_center(settingsPreviewLabel);
  settingsModeLabel=lv_label_create(settingsScreen);
  lv_obj_set_style_text_color(settingsModeLabel,lv_color_hex(0xDCE6F1),LV_PART_MAIN);
  lv_obj_set_style_text_font(settingsModeLabel,&lv_font_montserrat_12,LV_PART_MAIN);
  lv_obj_align(settingsModeLabel,LV_ALIGN_TOP_MID,0,PlayerUIConfig::SETTINGS_MODE_Y);
  settingsMakeButton(settingsScreen,"BACKGROUND",PlayerUIConfig::SETTINGS_BACKGROUND_WIDTH,PlayerUIConfig::SETTINGS_MODE_BUTTON_HEIGHT,LV_ALIGN_TOP_MID,PlayerUIConfig::SETTINGS_BACKGROUND_X,PlayerUIConfig::SETTINGS_MODE_BUTTON_Y,settingsBgEvent);
  settingsMakeButton(settingsScreen,"TEXT",PlayerUIConfig::SETTINGS_TEXT_WIDTH,PlayerUIConfig::SETTINGS_MODE_BUTTON_HEIGHT,LV_ALIGN_TOP_MID,PlayerUIConfig::SETTINGS_TEXT_X,PlayerUIConfig::SETTINGS_MODE_BUTTON_Y,settingsTextEvent);
  settingsMakeButton(settingsScreen,"A-",PlayerUIConfig::SETTINGS_FONT_BUTTON_WIDTH,PlayerUIConfig::SETTINGS_FONT_BUTTON_HEIGHT,LV_ALIGN_TOP_MID,PlayerUIConfig::SETTINGS_FONT_DOWN_X,PlayerUIConfig::SETTINGS_FONT_BUTTON_Y,settingsFontDownEvent);
  settingsFontLabel=lv_label_create(settingsScreen);
  lv_obj_set_style_text_color(settingsFontLabel,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
  lv_obj_set_style_text_font(settingsFontLabel,&lv_font_montserrat_14,LV_PART_MAIN);
  lv_obj_align(settingsFontLabel,LV_ALIGN_TOP_MID,0,PlayerUIConfig::SETTINGS_FONT_LABEL_Y);
  settingsMakeButton(settingsScreen,"A+",PlayerUIConfig::SETTINGS_FONT_BUTTON_WIDTH,PlayerUIConfig::SETTINGS_FONT_BUTTON_HEIGHT,LV_ALIGN_TOP_MID,PlayerUIConfig::SETTINGS_FONT_UP_X,PlayerUIConfig::SETTINGS_FONT_BUTTON_Y,settingsFontUpEvent);
  const char *names[]={"R","G","B"};
  const int *ys=PlayerUIConfig::SETTINGS_RGB_Y;
  for(int i=0;i<3;++i) {
    lv_obj_t *n=lv_label_create(settingsScreen);
    lv_label_set_text(n,names[i]);
    lv_obj_set_style_text_color(n,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
    lv_obj_set_style_text_font(n,&lv_font_montserrat_14,LV_PART_MAIN);
    lv_obj_align(n,LV_ALIGN_TOP_LEFT,PlayerUIConfig::SETTINGS_RGB_LABEL_X,ys[i]-7);
    settingsSliders[i]=lv_slider_create(settingsScreen);
    lv_obj_set_size(settingsSliders[i],PlayerUIConfig::SETTINGS_RGB_SLIDER_WIDTH,10);
    lv_slider_set_range(settingsSliders[i],0,255);
    lv_obj_align(settingsSliders[i],LV_ALIGN_TOP_LEFT,PlayerUIConfig::SETTINGS_RGB_SLIDER_X,ys[i]);
    lv_obj_add_event_cb(settingsSliders[i],settingsSliderEvent,LV_EVENT_VALUE_CHANGED,
                        (void *)(intptr_t)i);
    settingsRgbLabels[i]=lv_label_create(settingsScreen);
    lv_obj_set_style_text_color(settingsRgbLabels[i],lv_color_hex(0xFFFFFF),LV_PART_MAIN);
    lv_obj_set_style_text_font(settingsRgbLabels[i],&lv_font_montserrat_12,LV_PART_MAIN);
    lv_obj_align(settingsRgbLabels[i],LV_ALIGN_TOP_LEFT,PlayerUIConfig::SETTINGS_RGB_VALUE_X,ys[i]-7);
  }
  settingsHexLabel=lv_label_create(settingsScreen);
  lv_obj_set_style_text_color(settingsHexLabel,lv_color_hex(0xFFFFFF),LV_PART_MAIN);
  lv_obj_set_style_text_font(settingsHexLabel,&lv_font_montserrat_14,LV_PART_MAIN);
  lv_obj_align(settingsHexLabel,LV_ALIGN_TOP_MID,0,PlayerUIConfig::SETTINGS_HEX_LABEL_Y);
  settingsStatusLabel=lv_label_create(settingsScreen);
  lv_obj_set_width(settingsStatusLabel,260);
  lv_label_set_long_mode(settingsStatusLabel,LV_LABEL_LONG_DOT);
  lv_obj_set_style_text_align(settingsStatusLabel,LV_TEXT_ALIGN_CENTER,LV_PART_MAIN);
  lv_obj_set_style_text_color(settingsStatusLabel,lv_color_hex(0xDCE6F1),LV_PART_MAIN);
  lv_obj_set_style_text_font(settingsStatusLabel,&lv_font_montserrat_12,LV_PART_MAIN);
  lv_label_set_text(settingsStatusLabel,"ADJUST COLORS AND SAVE");
  lv_obj_align(settingsStatusLabel,LV_ALIGN_TOP_MID,0,PlayerUIConfig::SETTINGS_STATUS_LABEL_Y);
  settingsMakeButton(settingsScreen,"SAVE",PlayerUIConfig::SETTINGS_SAVE_BACK_WIDTH,PlayerUIConfig::SETTINGS_SAVE_BACK_HEIGHT,LV_ALIGN_TOP_MID,PlayerUIConfig::SETTINGS_SAVE_X,PlayerUIConfig::SETTINGS_SAVE_BACK_Y,settingsSaveEvent);
  settingsMakeButton(settingsScreen,"BACK",PlayerUIConfig::SETTINGS_SAVE_BACK_WIDTH,PlayerUIConfig::SETTINGS_SAVE_BACK_HEIGHT,LV_ALIGN_TOP_MID,PlayerUIConfig::SETTINGS_BACK_X,PlayerUIConfig::SETTINGS_SAVE_BACK_Y,settingsBackEvent);
  settingsMakeButton(settingsScreen,"DEFAULT",PlayerUIConfig::SETTINGS_DEFAULT_BUTTON_WIDTH,PlayerUIConfig::SETTINGS_DEFAULT_BUTTON_HEIGHT,LV_ALIGN_TOP_MID,0,PlayerUIConfig::SETTINGS_DEFAULT_BUTTON_Y,settingsDefaultsEvent);
  settingsLoadSliders();
}

// Only the selected album is loaded into this queue; the full library is never
// copied into RAM. 256 tracks is a generous per-folder queue limit.
static constexpr size_t MAX_TRACKS = 256;
static constexpr size_t PATH_LENGTH = 256;

// --------------------------- Track index -------------------------------
static char trackPaths[MAX_TRACKS][PATH_LENGTH];
struct TrackMetadata {
  char title[128];
  char artist[96];
  char album[128];
  char albumArtist[96];
  char track[16];
  char disc[16];
  char genre[48];
  char year[12];
  char composer[96];
  char comment[96];
  char bpm[12];
};
static TrackMetadata currentTrackMetadata;
// Reused ID3 frame buffer lives in static RAM, not the relatively small loopTask stack.
static uint8_t id3FrameBuffer[4096];
// Scratch metadata used only while building library indexes.
static TrackMetadata libraryMetadataScratch;
static size_t trackCount = 0;
static uint16_t trackDiscNumbers[MAX_TRACKS] = {};
static uint16_t trackNumbers[MAX_TRACKS] = {};
static size_t currentTrack = 0;
static volatile bool trackChanged = false;
static volatile bool playerStarted = false;
static volatile bool pausedByUser = false;
static size_t scanFoundCount = 0;

// ------------------------- Persistent music library ---------------------
// The four source folders remain untouched. Text indexes are stored separately
// under /sdcard/Music_Library/.player_index/ and can be rebuilt with RESCAN.
static constexpr char MUSIC_ROOT[] = "/sdcard/Music_Library";
static constexpr char INDEX_DIR[] = "/sdcard/Music_Library/.player_index";
static constexpr size_t MAX_SCAN_DEPTH = 20;
// v0.18.0: libraries are discovered on the SD card. Every folder directly
// inside Music_Library becomes a library, except Playlists and names that
// start with '.'. Index file = <folder>_Library.txt (same names as before, so
// existing indexes are reused). Folders are sorted by name; at most
// MAX_MUSIC_LIBRARIES are used.
static constexpr size_t MAX_MUSIC_LIBRARIES = 8;
struct MusicLibraryDefinition { char folder[48]; char indexFile[64]; };
static MusicLibraryDefinition MUSIC_LIBRARIES[MAX_MUSIC_LIBRARIES];
static size_t MUSIC_LIBRARY_COUNT = 0;
static volatile bool pendingLibraryRescan = false;
// v0.17.3: RESCAN bottom centre and SETTINGS centred. Kept here (not in
// player_ui_config.h) so an existing/customised player_ui_config.h still compiles.
static constexpr int RESCAN_BOTTOM_WIDTH  = 84;
static constexpr int RESCAN_BOTTOM_HEIGHT = 24;
static constexpr int RESCAN_BOTTOM_Y      = 327;   // bottom edge 351: inside the round screen
static constexpr int SETTINGS_CENTRE_X    = 0;
static constexpr int SHUF_RPT_WIDTH       = 98;    // v0.17.7 (was 80)
static constexpr int SHUF_RPT_CENTRE_X    = 104;   // v0.17.7 (was 110)
static lv_obj_t *browserRescanButton = nullptr;
static lv_obj_t *browserRescanLabel = nullptr;     // v0.17.3: shows CONFIRM? after the first tap
static uint32_t rescanArmedUntilMs = 0;

// Folder browser. Entries are read from the SD card only for the folder being
// viewed; the library is not globally indexed at startup.
static constexpr size_t MAX_BROWSER_ENTRIES = 128;
static constexpr size_t BROWSER_VISIBLE_ROWS = 5;
static constexpr size_t BROWSER_PAGE_SIZE = BROWSER_VISIBLE_ROWS;
static size_t browserPage = 0;
// Preserve the parent folder and its page while drilling down through the library.
// This lets BACK from an album restore the exact artist-list page the user left.
static constexpr size_t MAX_BROWSER_DEPTH = 16;
struct BrowserLocation { char path[PATH_LENGTH]; size_t page; };
static BrowserLocation browserHistory[MAX_BROWSER_DEPTH];
static size_t browserHistoryDepth = 0;
struct BrowserEntry {
  char name[128];
  char path[PATH_LENGTH];
  bool isDirectory;
  bool isAction;
  uint16_t discNumber;
  uint16_t trackNumber;
};
enum BrowserView : uint8_t { BROWSER_ROOT, BROWSER_COLLECTION_MENU, BROWSER_FOLDERS,
                              BROWSER_LETTERS, BROWSER_ARTISTS, BROWSER_ALBUMS, BROWSER_TRACKS,
                              BROWSER_OUTPUT /* v0.16.9 */,
                              BROWSER_PLAYLISTS, BROWSER_PLAYLIST /* v0.17.2 */,
                              BROWSER_ALBUM_INDEX, BROWSER_SONG_INDEX /* v0.18.2 */ };
static BrowserView browserView = BROWSER_ROOT;
static size_t selectedLibraryIndex = 0;
static char selectedArtist[96] = {};
static char selectedAlbum[128] = {};
static char selectedLetter = 'A';
static volatile int pendingBrowserLetter = 0;
// v0.18.2: what the A-Z grid is for, and where the track list was opened from.
enum LetterMode : uint8_t { LETTERS_ARTISTS, LETTERS_ALBUMS, LETTERS_SONGS };
static LetterMode letterMode = LETTERS_ARTISTS;
static bool tracksFromAlbumIndex = false;
static size_t albumIndexPage = 0;
static inline bool browserViewIsPaged(BrowserView v) {   // only the visible page is in browserEntries
  return v == BROWSER_FOLDERS || v == BROWSER_ALBUM_INDEX || v == BROWSER_SONG_INDEX;
}

// v0.17.2: playlist state (used by the library list, so declared here).
static constexpr char PLAYLIST_DIR[] = "/sdcard/Music_Library/Playlists";
static void startSelectedPlayback();
static void showBrowser();
static void refreshBrowserScreen();
static char openPlaylistPath[PATH_LENGTH] = {};   // playlist shown in BROWSER_PLAYLIST
static char openPlaylistName[64] = {};
static bool playlistAddMode = false;              // browsing to add tracks to openPlaylistPath
static bool playlistDeleteArmed = false;          // DELETE needs a second tap
static volatile int pendingPlaylistNameReady = 0; // 1 = name entered, -1 = cancelled
static char pendingPlaylistName[48] = {};
static lv_obj_t *playlistNameTextArea = nullptr;
static lv_obj_t *playlistNameKeyboard = nullptr;

enum PlaylistRowKind : uint16_t { PR_NEW = 1, PR_OPEN, PR_PLAY, PR_ADD, PR_DELETE, PR_TRACK };
enum RootRowKind : uint16_t { RR_OUTPUT = 1, RR_PLAYLISTS };

static lv_obj_t *browserLetterButtons[27] = {};
static BrowserEntry browserEntries[MAX_BROWSER_ENTRIES];
static size_t browserEntryCount = 0;
// Total entries in the current view; folder views page directly from SD.
static size_t browserTotalEntries = 0;
static char browsePath[PATH_LENGTH] = "/sdcard/Music_Library";
static volatile int pendingBrowserSelection = -1;
static bool browserVisible = true;
static lv_obj_t *playbackScreen = nullptr;
static lv_obj_t *browserList = nullptr;
static lv_obj_t *browserHeading = nullptr;
// Keep only five reusable visible rows in LVGL. Larger folders are paged.
static lv_obj_t *browserRowButtons[BROWSER_VISIBLE_ROWS] = {};
static lv_obj_t *browserRowLabels[BROWSER_VISIBLE_ROWS] = {};
static lv_obj_t *browserEmptyLabel = nullptr;
static lv_obj_t *browserPageLabel = nullptr;
static lv_obj_t *browserPagePrevButton = nullptr;
static lv_obj_t *browserPageNextButton = nullptr;
static lv_obj_t *browserLetterPanel = nullptr;
static lv_obj_t *browserStatusLabel = nullptr;
static lv_obj_t *browserProgressBar = nullptr;

// --------------------------- LVGL objects ------------------------------
static lv_obj_t *trackTitleLabel = nullptr;
static lv_obj_t *trackNumberLabel = nullptr;
static lv_obj_t *playStateLabel = nullptr;
static lv_obj_t *elapsedLabel = nullptr;
static lv_obj_t *durationLabel = nullptr;
static lv_obj_t *progressBar = nullptr;
static lv_obj_t *playPauseButtonLabel = nullptr;
static lv_obj_t *volumeLabel = nullptr;
static lv_obj_t *volumeArc = nullptr;   // v0.18.3
// v0.18.3 volume arc layout (kept here, not in player_ui_config.h).
static constexpr int VOLUME_ARC_DIAMETER = 352;     // outer edge 4 px inside the round screen
static constexpr int VOLUME_ARC_WIDTH    = 6;
static constexpr int VOLUME_ARC_START    = 238;     // LVGL degrees: 270 = top centre, clockwise
static constexpr int VOLUME_ARC_END      = 302;
static constexpr int VOLUME_LABEL_Y      = 12;
static constexpr uint32_t VOLUME_ARC_TRACK   = 0x263342;
static constexpr uint32_t VOLUME_ARC_FILL    = 0x2997F0;
static constexpr uint32_t VOLUME_ARC_INACTIVE = 0x5A6470;
static int volumePercent = 45;
static knob_handle_t volumeEncoder = nullptr;
static portMUX_TYPE encoderEventMux = portMUX_INITIALIZER_UNLOCKED;
static volatile int32_t pendingEncoderSteps = 0;
enum PlaybackAction : uint8_t { ACTION_NONE, ACTION_PREVIOUS, ACTION_TOGGLE, ACTION_NEXT,
                                ACTION_SHUFFLE, ACTION_REPEAT /* v0.17.5 */ };

// ---------------- v0.17.5: play order, shuffle and repeat ----------------
enum RepeatMode : uint8_t { REPEAT_OFF = 0, REPEAT_ALL, REPEAT_ONE };
static uint16_t playOrder[MAX_TRACKS];
static size_t queuePos = 0;                  // position of currentTrack in playOrder
static bool shuffleOn = false;
static uint8_t repeatMode = REPEAT_OFF;
static bool queueStartRandom = false;        // PLAY ALBUM / FOLDER / ALL: random first track when shuffling
static lv_obj_t *shuffleButton = nullptr, *shuffleLabel = nullptr;
static lv_obj_t *repeatButton = nullptr, *repeatLabel = nullptr;
static void updateShuffleRepeatButtons();   // defined with the playback actions
static void applyShuffleRepeatButtonsLocked();   // v0.17.7: caller holds the LVGL lock

// Rebuild the order for the current queue. With shuffle, the current track is
// first and the rest are in random order; otherwise the normal order.
static void rebuildPlayOrder() {
  for (size_t i = 0; i < trackCount; ++i) playOrder[i] = (uint16_t)i;
  if (shuffleOn && trackCount > 1) {
    for (size_t i = trackCount - 1; i > 0; --i) {
      size_t j = esp_random() % (i + 1);
      uint16_t t = playOrder[i]; playOrder[i] = playOrder[j]; playOrder[j] = t;
    }
    for (size_t i = 0; i < trackCount; ++i)
      if (playOrder[i] == currentTrack) { playOrder[i] = playOrder[0]; playOrder[0] = (uint16_t)currentTrack; break; }
    queuePos = 0;
  } else {
    queuePos = currentTrack < trackCount ? currentTrack : 0;
  }
}
// v0.16.7c: the spectrum is one object; these arrays hold what it draws.
static lv_obj_t *spectrumObj = nullptr;
static int16_t spectrumBarHalf[32] = {};   // half-height of each bar (px)
static int16_t spectrumPeakHalf[32] = {};  // height of each peak marker above the baseline (px)
static lv_color_t spectrumColors[32];
static constexpr int SPECTRUM_BAR_WIDTH = 5;
static constexpr int SPECTRUM_BAR_GAP = 2;
static constexpr int SPECTRUM_MAX_HALF = 54;              // tallest half-bar (v0.16.8b: 75 % of 72)
static constexpr int SPECTRUM_SPAN = SPECTRUM_MAX_HALF + 2; // rows above/below the baseline

// v0.17.4: spectrum styles.
enum SpectrumStyle : uint8_t { SPEC_MIRROR = 0, SPEC_BARS, SPEC_RADIAL, SPEC_SCOPE, SPEC_LEVELS, SPEC_OFF, SPEC_COUNT };
static const char *const SPECTRUM_STYLE_NAMES[SPEC_COUNT] = { "MIRROR", "BARS", "RADIAL", "SCOPE", "LEVELS", "SPECTRUM OFF" };
static volatile uint8_t spectrumStyle = SPEC_MIRROR;
static volatile uint32_t spectrumNameUntilMs = 0;    // style name shown until then
static volatile bool spectrumStyleSaveDue = false;
static int8_t spectrumScope[64] = {};                // copies drawn by the callback
static int16_t spectrumLevelPx[2] = {}, spectrumLevelPeakPx[2] = {};
static float radialCos[32], radialSin[32];

static void spectrumTapEvent(lv_event_t *e) {
  if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
  uint8_t next = (uint8_t)((spectrumStyle + 1) % SPEC_COUNT);
  spectrumStyle = next;
  uint32_t t = millis() + 1500;
  spectrumNameUntilMs = t ? t : 1;
  spectrumStyleSaveDue = true;
  lv_obj_invalidate(lv_event_get_target(e));
}

static void drawSpectrumStyle(lv_draw_ctx_t *drawCtx, const lv_area_t &coords) {
  const int baseY = coords.y1 + SPECTRUM_SPAN;
  const int width = coords.x2 - coords.x1 + 1;
  uint8_t style = spectrumStyle;
  lv_draw_rect_dsc_t dsc;
  lv_draw_rect_dsc_init(&dsc);
  dsc.bg_opa = LV_OPA_COVER;
  dsc.radius = 0;
  dsc.border_width = 0;

  if (style == SPEC_BARS) {
    for (int i = 0; i < 32; ++i) {
      const int x1 = coords.x1 + i * (SPECTRUM_BAR_WIDTH + SPECTRUM_BAR_GAP);
      dsc.bg_color = spectrumColors[i];
      const int h = spectrumBarHalf[i] * 2, p = spectrumPeakHalf[i] * 2;
      lv_area_t bar = { (lv_coord_t)x1, (lv_coord_t)(coords.y2 - h + 1), (lv_coord_t)(x1 + SPECTRUM_BAR_WIDTH - 1), coords.y2 };
      lv_draw_rect(drawCtx, &dsc, &bar);
      lv_area_t peak = { (lv_coord_t)x1, (lv_coord_t)(coords.y2 - p - 1), (lv_coord_t)(x1 + SPECTRUM_BAR_WIDTH - 1), (lv_coord_t)(coords.y2 - p) };
      lv_draw_rect(drawCtx, &dsc, &peak);
    }
  } else if (style == SPEC_RADIAL) {
    const int cx = (coords.x1 + coords.x2) / 2, cy = baseY;
    const int r0 = 14, rMax = SPECTRUM_SPAN - 3;
    lv_draw_line_dsc_t line;
    lv_draw_line_dsc_init(&line);
    line.width = 3;
    line.round_start = 0; line.round_end = 0;
    for (int i = 0; i < 32; ++i) {
      int len = (spectrumBarHalf[i] * (rMax - r0)) / SPECTRUM_MAX_HALF;
      int plen = (spectrumPeakHalf[i] * (rMax - r0)) / SPECTRUM_MAX_HALF;
      line.color = spectrumColors[i];
      lv_point_t a = { (lv_coord_t)(cx + radialCos[i] * r0), (lv_coord_t)(cy + radialSin[i] * r0) };
      lv_point_t b = { (lv_coord_t)(cx + radialCos[i] * (r0 + len)), (lv_coord_t)(cy + radialSin[i] * (r0 + len)) };
      lv_draw_line(drawCtx, &line, &a, &b);
      dsc.bg_color = spectrumColors[i];
      int px = cx + (int)(radialCos[i] * (r0 + plen + 2)), py = cy + (int)(radialSin[i] * (r0 + plen + 2));
      lv_area_t dot = { (lv_coord_t)(px - 1), (lv_coord_t)(py - 1), (lv_coord_t)px, (lv_coord_t)py };
      lv_draw_rect(drawCtx, &dsc, &dot);
    }
  } else if (style == SPEC_SCOPE) {
    lv_draw_line_dsc_t line;
    lv_draw_line_dsc_init(&line);
    line.width = 2;
    line.color = spectrumColors[12];
    lv_point_t prev = { coords.x1, (lv_coord_t)(baseY - (spectrumScope[0] * (SPECTRUM_SPAN - 2)) / 127) };
    for (int i = 1; i < 64; ++i) {
      lv_point_t cur = { (lv_coord_t)(coords.x1 + (i * (width - 1)) / 63),
                         (lv_coord_t)(baseY - (spectrumScope[i] * (SPECTRUM_SPAN - 2)) / 127) };
      lv_draw_line(drawCtx, &line, &prev, &cur);
      prev = cur;
    }
  } else if (style == SPEC_LEVELS) {
    // Two segmented meters (L above, R below the centre line), green -> yellow -> red.
    const int segW = 8, segGap = 1, segs = width / (segW + segGap);
    for (int ch = 0; ch < 2; ++ch) {
      int y1 = ch == 0 ? baseY - 22 : baseY + 6;
      int lit = spectrumLevelPx[ch], peakSeg = spectrumLevelPeakPx[ch];
      for (int k = 0; k < segs; ++k) {
        bool on = k < lit || k == peakSeg;
        uint32_t col = k < segs * 7 / 10 ? 0x22CC55 : (k < segs * 9 / 10 ? 0xE8C020 : 0xE03030);
        dsc.bg_color = lv_color_hex(col);
        dsc.bg_opa = on ? LV_OPA_COVER : LV_OPA_20;
        int x = coords.x1 + k * (segW + segGap);
        lv_area_t seg = { (lv_coord_t)x, (lv_coord_t)y1, (lv_coord_t)(x + segW - 1), (lv_coord_t)(y1 + 15) };
        lv_draw_rect(drawCtx, &dsc, &seg);
      }
    }
  }
}

// Draws all 32 bars and peak markers in one pass. LVGL clips each rectangle
// to the area being redrawn, so only the invalidated rows cost anything.
static void spectrumDrawEvent(lv_event_t *e) {
  lv_draw_ctx_t *drawCtx = lv_event_get_draw_ctx(e);
  lv_obj_t *obj = lv_event_get_target(e);
  lv_area_t coords;
  lv_obj_get_coords(obj, &coords);
  const int baseY = coords.y1 + SPECTRUM_SPAN;   // screen row of the baseline

  // v0.17.4: style name for 1.5 s after a tap.
  if (spectrumNameUntilMs && (int32_t)(millis() - spectrumNameUntilMs) < 0) {
    lv_draw_label_dsc_t label;
    lv_draw_label_dsc_init(&label);
    label.color = lv_color_hex(0xFFFFFF);
    label.font = &lv_font_montserrat_14;
    label.align = LV_TEXT_ALIGN_CENTER;
    lv_area_t area = { coords.x1, coords.y1, coords.x2, (lv_coord_t)(coords.y1 + 18) };
    lv_draw_label(drawCtx, &label, &area, SPECTRUM_STYLE_NAMES[spectrumStyle], nullptr);
  }
  if (spectrumStyle != SPEC_MIRROR) { drawSpectrumStyle(drawCtx, coords); return; }

  lv_draw_rect_dsc_t dsc;
  lv_draw_rect_dsc_init(&dsc);
  dsc.bg_opa = LV_OPA_COVER;
  dsc.radius = 0;
  dsc.border_width = 0;

  for (int i = 0; i < 32; ++i) {
    const int x1 = coords.x1 + i * (SPECTRUM_BAR_WIDTH + SPECTRUM_BAR_GAP);
    dsc.bg_color = spectrumColors[i];
    const int h = spectrumBarHalf[i];
    lv_area_t bar = { (lv_coord_t)x1, (lv_coord_t)(baseY - h),
                      (lv_coord_t)(x1 + SPECTRUM_BAR_WIDTH - 1), (lv_coord_t)(baseY + h - 1) };
    lv_draw_rect(drawCtx, &dsc, &bar);
    const int p = spectrumPeakHalf[i];
    lv_area_t peak = { (lv_coord_t)x1, (lv_coord_t)(baseY - p - 1),
                       (lv_coord_t)(x1 + SPECTRUM_BAR_WIDTH - 1), (lv_coord_t)(baseY - p) };
    lv_draw_rect(drawCtx, &dsc, &peak);
  }
}

// Playback position is accumulated from decoded PCM, not wall-clock time.
static portMUX_TYPE audioStatsMux = portMUX_INITIALIZER_UNLOCKED;
static volatile uint64_t decodedSampleValues = 0;
static volatile uint64_t decodedTimeUs = 0;
static volatile float eqLevels[32] = {};
// v0.17.4: waveform snapshot (64 points, -127..127) and left/right levels (0..100).
static volatile int8_t scopeSnapshot[64] = {};
static volatile float levelLeft = 0.0f, levelRight = 0.0f;
static double levelSumSqL = 0.0, levelSumSqR = 0.0;
static uint32_t levelSumCount = 0;
static volatile float eqPeakLevels[32] = {};
static uint32_t eqPeakTimes[32] = {};
static constexpr int EQ_BANDS = 32;
static constexpr int FFT_SIZE = 512;
static float fftSamples[FFT_SIZE] = {};
static int fftSampleCount = 0;
// FFT display sensitivity: increase to make quieter frequency components taller.
// FFT spectrum has a different scale than the previous 8-bar meter.
// Start at 1.5; increase gradually if the mirrored bars still look too short.
static constexpr float EQ_DISPLAY_GAIN = 1.5f;
static constexpr uint32_t EQ_PEAK_HOLD_MS = 420;
static constexpr float EQ_PEAK_FALL_PER_UPDATE = 2.8f;
static uint64_t trackDurationMs = 0;
static uint32_t lastUiUpdateMs = 0;
// Seeking is requested by LVGL but performed by the main loop, never in the LVGL task.
static volatile bool pendingSeek = false;
static volatile uint64_t pendingSeekMs = 0;
static bool seekPreviewActive = false;
static uint64_t seekPreviewMs = 0;
// PCM decoded before the requested point is discarded after a seek. This gives
// the MP3 decoder preroll for bit-reservoir dependencies without audible old audio.
static uint64_t seekDiscardUntilUs = 0;

class VfsFileStream : public Stream {
public:
  VfsFileStream() : file(nullptr) {}

  bool isOpen() const { return file != nullptr; }
  FILE *getFile() const { return file; }

  bool open(const char *path) {
    close();
    file = fopen(path, "rb");
    if (!file) {
      Serial.printf("[FILE] Could not open: %s\n", path);
      return false;
    }
    Serial.printf("[FILE] Opened: %s\n", path);
    return true;
  }

  void close() {
    if (file) {
      fclose(file);
      file = nullptr;
    }
  }

  ~VfsFileStream() override { close(); }

  int available() override {
    if (!file) return 0;
    long here = ftell(file);
    if (here < 0) return 0;
    if (fseek(file, 0, SEEK_END) != 0) return 0;
    long end = ftell(file);
    fseek(file, here, SEEK_SET);
    if (end < here) return 0;
    long remaining = end - here;
    return remaining > 32767 ? 32767 : static_cast<int>(remaining);
  }

  int read() override {
    if (!file) return -1;
    int value = fgetc(file);
    return value == EOF ? -1 : value;
  }

  int peek() override {
    if (!file) return -1;
    int value = fgetc(file);
    if (value == EOF) return -1;
    ungetc(value, file);
    return value;
  }

  void flush() override {
    if (file) fflush(file);
  }

  size_t write(uint8_t) override { return 0; }
  using Print::write;

private:
  FILE *file;
};

static VfsFileStream audioFileStream;
static AudioSourceCallback source;
static I2SStream i2s;
static MP3DecoderHelix decoder;
static AudioPlayer player(source, i2s, decoder);

// ---------------- v0.16.7: audio task, player mutex, action queue -------
// The audio task is the only caller of player.copy(). Every other
// AudioPlayer call (and performSeek(), which repositions the same FILE)
// takes playerMutex first. The mutex is recursive because selectTrack()
// runs inside player.begin()/next()/previous().
static SemaphoreHandle_t playerMutex = nullptr;
static QueueHandle_t playbackActionQueue = nullptr;
static volatile bool playerActiveFlag = false;     // published by the audio task
static TaskHandle_t audioTaskHandle = nullptr;

// MP3 bytes read per decode step. Smaller steps release the player mutex
// more often (128 kbps: 256 bytes is about 16 ms of audio).
static constexpr size_t DECODE_STEP_BYTES = 256;

// Diagnostics, reported by printPerfStats() every 10 s.
static volatile uint32_t perfMaxCopyUs = 0;         // longest single player.copy()
static volatile uint32_t perfMaxCopyGapUs = 0;      // longest idle time between two decode steps
static volatile uint32_t perfIdleOver50ms = 0;      // idle times over 50 ms
static volatile uint32_t perfMaxPlayerWaitUs = 0;   // longest wait for playerMutex in loop()
static uint32_t perfMaxLoopUs = 0;                  // longest loop() pass
static uint32_t perfLoopsOver100ms = 0;

// Last UI event, for the press-to-handled log.
static volatile uint32_t uiEventMs = 0;
static const char *volatile uiEventName = nullptr;
static void noteUiEvent(const char *name) {
  uiEventName = name;
  uiEventMs = millis();
}

// Number of tasks waiting for playerMutex. The audio task has the higher
// priority and would otherwise re-take the mutex immediately after giving
// it, starving loop(); when someone is waiting it sleeps for one tick.
static std::atomic<int> playerLockWaiters{0};

static void lockPlayer() {
  int64_t t0 = esp_timer_get_time();
  playerLockWaiters++;
  xSemaphoreTakeRecursive(playerMutex, portMAX_DELAY);
  playerLockWaiters--;
  uint32_t waited = (uint32_t)(esp_timer_get_time() - t0);
  if (waited > perfMaxPlayerWaitUs) perfMaxPlayerWaitUs = waited;
}
static void unlockPlayer() {
  playerActiveFlag = player.isActive();
  xSemaphoreGiveRecursive(playerMutex);
}
struct PlayerLock {
  PlayerLock() { lockPlayer(); }
  ~PlayerLock() { unlockPlayer(); }
};

// ======================= v0.16.8: Bluetooth output ========================
// Wired (I2S -> PCM5100A, unchanged) or Bluetooth: decoded PCM is converted to
// 44.1 kHz stereo and sent over UART1 (TX GPIO38 -> classic GPIO18, RX GPIO48
// <- classic GPIO23) to the classic ESP32, which plays it to the speaker.
// Link protocol v2 (link_protocol.h, identical in the classic receiver).
// The link task, resampler and local-clock logic are the hardware-tested
// Step 3 code; the Step 3 digital gain is removed (volume mode 1: full-scale
// PCM, the rotary encoder sets the speaker's own volume over AVRCP).
enum OutputMode : uint8_t { OUTPUT_WIRED = 0, OUTPUT_BLUETOOTH = 1 };
static volatile uint8_t outputMode = OUTPUT_WIRED;
static Preferences playerPrefs;                         // namespace "wsplayer"

static int btVolumePercent = -1;                        // -1: not set yet
static constexpr int BT_FIRST_VOLUME_CAP = 50;          // first Bluetooth volume <= 50 %
static volatile uint16_t btVolSeq = 1;
static volatile bool btControlDirty = true;
static uint32_t btLastLocalVolumeMs = 0;
static uint32_t btVolumeSaveDueMs = 0;                  // debounced NVS save
static int btSavedVolumePercent = -1;                   // v0.16.8b: last knob-chosen value (what is saved)

static HardwareSerial &Link = Serial1;
static constexpr int LINK_RX_PIN = 48;
static constexpr int LINK_TX_PIN = 38;
static constexpr size_t LINK_TX_BUFFER = 8192;
static constexpr size_t LINK_RX_BUFFER = 2048;
static constexpr uint32_t LINK_TIMEOUT_MS = 500;
static constexpr uint32_t LINK_STALL_RESET_MS = 500;
static constexpr uint32_t LINK_CONTROL_INTERVAL_MS = 100;
static constexpr uint32_t OUT_BYTES_PER_SEC = 176400;   // 44.1 kHz stereo 16-bit

static constexpr size_t PCM_STREAM_BYTES = 32768;       // ~186 ms
static StreamBufferHandle_t pcmStream = nullptr;
static StaticStreamBuffer_t pcmStreamControl;            // small control block, internal RAM

// v0.16.8a: storage in PSRAM (internal RAM had no 32 KB block free).
static bool createPcmStream() {
  uint8_t *storage = (uint8_t *)heap_caps_malloc(PCM_STREAM_BYTES + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  const char *where = "PSRAM";
  if (!storage) {
    storage = (uint8_t *)heap_caps_malloc(PCM_STREAM_BYTES + 1, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    where = "internal RAM";
  }
  if (!storage) {
    Serial.printf("[ERROR] Bluetooth PCM buffer: no memory (internal free %u, largest %u). Bluetooth output disabled.\n",
                  (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
                  (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
    return false;
  }
  pcmStream = xStreamBufferCreateStatic(PCM_STREAM_BYTES, 1, storage, &pcmStreamControl);
  Serial.printf("[OUTPUT] Bluetooth PCM buffer: %u bytes in %s%s\n", (unsigned)PCM_STREAM_BYTES, where,
                pcmStream ? "" : " (create FAILED)");
  return pcmStream != nullptr;
}

// Link state (link task writes; loop() reads for display/status).
static volatile bool     linkUp = false;
static volatile bool     linkStreaming = false;         // speaker connected and audio started
static volatile bool     linkLocalClock = true;
static volatile uint32_t linkEpoch = 0;
static volatile uint32_t linkNextSeq = 1;
static volatile uint32_t linkLastSeqProcessed = 0;
static volatile uint32_t linkWindowBytes = 0;
static volatile uint32_t linkReportsReceived = 0;
static volatile uint32_t linkEpochChanges = 0;
static volatile uint32_t linkCreditResets = 0;
static volatile uint32_t linkLocalClockResets = 0;
static volatile uint32_t linkModeSwitches = 0;
static volatile uint64_t linkBytesSent = 0;
static volatile uint64_t linkBytesDiscarded = 0;
static volatile uint32_t linkReportCrcErrors = 0;
static wslink::Report linkLastReport = {};
static portMUX_TYPE linkReportMux = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t btStreamingSinceMs = 0;

// v0.16.9: commands to the classic (scan / connect / forget) and its device list.
static volatile uint8_t  btCmd = wslink::CMD_NONE;
static volatile uint8_t  btCmdArg = 0;
static volatile uint16_t btCmdSeq = 0;                  // random start at boot (see setup)
static wslink::DeviceList linkDevices = {};
static volatile uint32_t linkDevicesVersion = 0;        // incremented when the list changes

// Linear resampler (Q32.32 input-frame position), from Step 3.
static uint64_t btRsStepQ32 = 1ULL << 32;
static uint64_t btRsPosQ32 = 0;
static int16_t  btRsPrevL = 0, btRsPrevR = 0;
static bool     btRsPrimed = false;
static uint32_t btRsRate = 0;
static constexpr size_t BT_CONVERT_MAX_FRAMES = 2560;   // 1152 @ 22.05 kHz -> 2304
static uint8_t btConvertBuf[BT_CONVERT_MAX_FRAMES * 4];

static void updateVolumeDisplay();   // defined with the encoder code
static volatile uint64_t wiredBytesWritten = 0;   // v0.16.8b: bytes accepted by the wired output chain

static int btVolumeToAvrcp(int percent) {
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  return (percent * 127 + 50) / 100;
}

static void btResetResampler() {
  btRsPosQ32 = 0;
  btRsPrimed = false;
  btRsRate = 0;
}

// Blocks while the stream is full: that paces the decoder in Bluetooth mode
// (the link task drains it in credit mode or at real time in local-clock mode).
static void btQueuePcm(const uint8_t *data, size_t bytes) {
  size_t done = 0;
  while (done < bytes)
    done += xStreamBufferSend(pcmStream, data + done, bytes - done, portMAX_DELAY);
}

static inline void btPutFrame(uint8_t *p, int16_t l, int16_t r) {
  p[0] = (uint8_t)(l & 0xFF); p[1] = (uint8_t)(((uint16_t)l) >> 8);
  p[2] = (uint8_t)(r & 0xFF); p[3] = (uint8_t)(((uint16_t)r) >> 8);
}

// Called from mp3PcmCallback (audio task) in Bluetooth mode.
// pcm: interleaved int16, ch channels (1 or 2), frames frames at rate Hz.
static void btConvertAndQueue(const short *pcm, size_t frames, int ch, uint32_t rate) {
  if (!pcm || frames == 0 || ch < 1 || ch > 2 || rate == 0 || !pcmStream) return;
  if (rate != btRsRate) {
    btRsRate = rate;
    btRsStepQ32 = ((uint64_t)rate << 32) / 44100u;
    btRsPosQ32 = 0;
    btRsPrimed = false;
  }
  size_t outFrames = 0;
  if (rate == 44100) {
    for (size_t i = 0; i < frames; ++i) {
      int16_t l = pcm[i * ch];
      int16_t r = (ch == 2) ? pcm[i * ch + 1] : l;
      btPutFrame(btConvertBuf + outFrames * 4, l, r);
      if (++outFrames == BT_CONVERT_MAX_FRAMES) { btQueuePcm(btConvertBuf, outFrames * 4); outFrames = 0; }
    }
  } else {
    // Linear interpolation. S(0) = previous frame, S(k) = input frame k-1.
    if (!btRsPrimed) {
      btRsPrevL = pcm[0];
      btRsPrevR = (ch == 2) ? pcm[1] : pcm[0];
      btRsPosQ32 = 1ULL << 32;               // start exactly on input frame 0
      btRsPrimed = true;
    }
    const uint64_t endQ32 = (uint64_t)frames << 32;
    while (btRsPosQ32 < endQ32) {
      uint32_t k = (uint32_t)(btRsPosQ32 >> 32);
      int32_t frac = (int32_t)((btRsPosQ32 >> 16) & 0xFFFF);
      int32_t aL, aR;
      if (k == 0) { aL = btRsPrevL; aR = btRsPrevR; }
      else { aL = pcm[(k - 1) * ch]; aR = (ch == 2) ? pcm[(k - 1) * ch + 1] : aL; }
      int32_t bL = pcm[k * ch];
      int32_t bR = (ch == 2) ? pcm[k * ch + 1] : bL;
      int32_t l = aL + (((bL - aL) * frac) >> 16);
      int32_t r = aR + (((bR - aR) * frac) >> 16);
      btPutFrame(btConvertBuf + outFrames * 4, (int16_t)l, (int16_t)r);
      if (++outFrames == BT_CONVERT_MAX_FRAMES) { btQueuePcm(btConvertBuf, outFrames * 4); outFrames = 0; }
      btRsPosQ32 += btRsStepQ32;
    }
    btRsPosQ32 -= endQ32;
    btRsPrevL = pcm[(frames - 1) * ch];
    btRsPrevR = (ch == 2) ? pcm[(frames - 1) * ch + 1] : btRsPrevL;
  }
  if (outFrames) btQueuePcm(btConvertBuf, outFrames * 4);
}

static void linkSendControl() {
  wslink::Control c = {};
  c.wantConnected = (outputMode == OUTPUT_BLUETOOTH) ? 1 : 0;
  c.volume = (uint8_t)btVolumeToAvrcp(btVolumePercent < 0 ? 0 : btVolumePercent);
  c.volSeq = btVolSeq;
  c.cmd = btCmd;
  c.cmdArg = btCmdArg;
  c.cmdSeq = btCmdSeq;
  wslink::Header h;
  wslink::buildHeader(h, wslink::PKT_CONTROL, sizeof(c), 0, linkEpoch,
                      reinterpret_cast<const uint8_t *>(&c));
  Link.write(reinterpret_cast<const uint8_t *>(&h), sizeof(h));
  Link.write(reinterpret_cast<const uint8_t *>(&c), sizeof(c));
}

static void linkHandleReport(const wslink::Header &h, const uint8_t *payload, uint16_t len,
                             uint32_t &lastReportMs, uint32_t &lastProgressMs) {
  if (h.type == wslink::PKT_DEVICES && len == sizeof(wslink::DeviceList)) {
    bool changed;
    portENTER_CRITICAL(&linkReportMux);
    changed = memcmp(&linkDevices, payload, sizeof(wslink::DeviceList)) != 0;
    if (changed) memcpy(&linkDevices, payload, sizeof(wslink::DeviceList));
    portEXIT_CRITICAL(&linkReportMux);
    if (changed) linkDevicesVersion = linkDevicesVersion + 1;
    return;
  }
  if (h.type != wslink::PKT_REPORT || len != sizeof(wslink::Report)) return;
  wslink::Report r;
  memcpy(&r, payload, sizeof(r));
  uint32_t now = millis();
  if (h.epoch != linkEpoch) {
    // Receiver rebooted, stream (re)started or speaker dropped: adopt its
    // position so we never resend into a fresh ring with stale numbering.
    linkEpoch = h.epoch;
    linkLastSeqProcessed = r.lastSeqProcessed;
    linkNextSeq = r.lastSeqProcessed + 1;
    linkEpochChanges = linkEpochChanges + 1;
    lastProgressMs = now;
  }
  if (r.lastSeqProcessed <= linkNextSeq - 1 && r.lastSeqProcessed >= linkLastSeqProcessed) {
    if (r.lastSeqProcessed != linkLastSeqProcessed) lastProgressMs = now;
    linkLastSeqProcessed = r.lastSeqProcessed;
  }
  linkWindowBytes = r.windowBytes;
  if (btCmd != wslink::CMD_NONE && r.cmdSeqDone == btCmdSeq) btCmd = wslink::CMD_NONE;   // done
  bool streaming = r.btConnected && r.audioStarted;
  if (streaming && !linkStreaming) btStreamingSinceMs = now;
  linkStreaming = streaming;
  lastReportMs = now;
  linkReportsReceived = linkReportsReceived + 1;
  portENTER_CRITICAL(&linkReportMux);
  linkLastReport = r;
  portEXIT_CRITICAL(&linkReportMux);
}

// Core 0, priority 3. Reads reports, sends control every 100 ms, and in
// Bluetooth mode moves PCM from pcmStream to the UART (credit mode, speaker
// streaming) or discards it at real time (local clock, speaker not streaming),
// so playback keeps advancing while the speaker is away.
static void linkTask(void *) {
  static wslink::Parser parser;
  static uint8_t rxChunk[256];
  static uint8_t pkt[wslink::AUDIO_PAYLOAD_BYTES];
  uint32_t lastReportMs = 0;
  uint32_t lastProgressMs = millis();
  uint32_t lastControlMs = 0;
  int64_t localStartUs = 0;
  uint64_t localBytes = 0;

  for (;;) {
    int avail = Link.available();
    if (avail > 0) {
      size_t n = Link.read(rxChunk, (size_t)min(avail, (int)sizeof(rxChunk)));
      parser.feed(rxChunk, n, [&](const wslink::Header &h, const uint8_t *p, uint16_t len) {
        linkHandleReport(h, p, len, lastReportMs, lastProgressMs);
      });
      linkReportCrcErrors = parser.crcErrors;
    }
    uint32_t now = millis();
    linkUp = (linkReportsReceived > 0) && (now - lastReportMs < LINK_TIMEOUT_MS);
    if (!linkUp) linkStreaming = false;

    if (btControlDirty || now - lastControlMs >= LINK_CONTROL_INTERVAL_MS) {
      btControlDirty = false;
      lastControlMs = now;
      linkSendControl();
    }

    if (outputMode != OUTPUT_BLUETOOTH || !pcmStream) {
      linkLocalClock = true;
      localStartUs = 0;
      vTaskDelay(pdMS_TO_TICKS(5));
      continue;
    }

    bool credit = linkUp && linkStreaming;
    if (credit) {
      if (linkLocalClock) { linkLocalClock = false; linkModeSwitches = linkModeSwitches + 1; }
      uint32_t outstanding = linkNextSeq - 1 - linkLastSeqProcessed;
      uint32_t unprocessed = outstanding * wslink::AUDIO_PAYLOAD_BYTES;
      if (outstanding > 0 && now - lastProgressMs > LINK_STALL_RESET_MS) {
        linkLastSeqProcessed = linkNextSeq - 1;           // backstop for lost packets
        linkCreditResets = linkCreditResets + 1;
        lastProgressMs = now;
        unprocessed = 0;
        outstanding = 0;
      }
      if (unprocessed + wslink::AUDIO_PAYLOAD_BYTES <= linkWindowBytes &&
          xStreamBufferBytesAvailable(pcmStream) >= wslink::AUDIO_PAYLOAD_BYTES) {
        if (outstanding == 0) lastProgressMs = now;
        xStreamBufferReceive(pcmStream, pkt, wslink::AUDIO_PAYLOAD_BYTES, 0);
        wslink::Header h;
        wslink::buildHeader(h, wslink::PKT_AUDIO, wslink::AUDIO_PAYLOAD_BYTES, linkNextSeq, linkEpoch, pkt);
        Link.write(reinterpret_cast<const uint8_t *>(&h), sizeof(h));
        Link.write(pkt, wslink::AUDIO_PAYLOAD_BYTES);
        linkNextSeq = linkNextSeq + 1;
        linkBytesSent = linkBytesSent + wslink::AUDIO_PAYLOAD_BYTES;
        continue;
      }
    } else {
      int64_t t = esp_timer_get_time();
      if (!linkLocalClock || localStartUs == 0) {
        if (!linkLocalClock) linkModeSwitches = linkModeSwitches + 1;
        linkLocalClock = true;
        localStartUs = t;
        localBytes = 0;
      }
      uint64_t due = (uint64_t)(t - localStartUs) * OUT_BYTES_PER_SEC / 1000000ULL;
      if (due > localBytes + OUT_BYTES_PER_SEC) {            // fell >1 s behind (e.g. paused)
        localStartUs = t;
        localBytes = 0;
        linkLocalClockResets = linkLocalClockResets + 1;
        due = 0;
      }
      bool moved = false;
      while (localBytes + wslink::AUDIO_PAYLOAD_BYTES <= due &&
             xStreamBufferBytesAvailable(pcmStream) >= wslink::AUDIO_PAYLOAD_BYTES) {
        xStreamBufferReceive(pcmStream, pkt, wslink::AUDIO_PAYLOAD_BYTES, 0);
        localBytes += wslink::AUDIO_PAYLOAD_BYTES;
        linkBytesDiscarded = linkBytesDiscarded + wslink::AUDIO_PAYLOAD_BYTES;
        moved = true;
      }
      if (moved) continue;
    }
    vTaskDelay(1);
  }
}

// Drop PCM queued for the link (about 0.2 s) when the user skips or seeks,
// so the old track is not heard after the new one is chosen. Caller holds the
// player lock: the audio task is then not inside copy(), and the link task
// never blocks on the stream, so the reset cannot fail. Not used for the
// natural advance at the end of a track (that would clip its ending).
static void btFlushQueuedAudio() {
  if (outputMode == OUTPUT_BLUETOOTH && pcmStream) xStreamBufferReset(pcmStream);
}

// Fill the I2S DMA ring with silence, so the wired output does not keep
// repeating the last buffer while Bluetooth is selected. Caller holds the
// player lock (nothing else writes to I2S at the same time).
static void writeI2sSilence() {
  static const uint8_t zeros[1024] = {};
  for (int i = 0; i < 8; ++i) i2s.write(zeros, sizeof(zeros));   // 8 KB > DMA ring (8 x 512)
}

static void saveOutputSettings() {
  playerPrefs.begin("wsplayer", false);
  playerPrefs.putUChar("outmode", outputMode);
  if (btSavedVolumePercent >= 0) playerPrefs.putUChar("btvol", (uint8_t)btSavedVolumePercent);
  playerPrefs.end();
}

static void loadOutputSettings() {
  playerPrefs.begin("wsplayer", false);   // read-write: avoids a "not found" error on first boot
  uint8_t mode = playerPrefs.getUChar("outmode", OUTPUT_WIRED);
  int vol = playerPrefs.isKey("btvol") ? (int)playerPrefs.getUChar("btvol", 0) : -1;
  uint8_t spec = playerPrefs.getUChar("specstyle", SPEC_MIRROR);   // v0.17.4
  shuffleOn = false;   // v0.17.7: shuffle always starts OFF at power-up (not restored)
  repeatMode = playerPrefs.getUChar("repeat", REPEAT_OFF);
  if (repeatMode > REPEAT_ONE) repeatMode = REPEAT_OFF;
  spectrumStyle = spec < SPEC_COUNT ? spec : SPEC_MIRROR;
  playerPrefs.end();
  outputMode = (mode == OUTPUT_BLUETOOTH) ? OUTPUT_BLUETOOTH : OUTPUT_WIRED;
  // v0.16.8b: a saved value below 5 % is treated as unset (v0.16.8a could save 0 %
  // from a misread speaker report, which would start the speaker silent).
  btVolumePercent = (vol >= 5 && vol <= 100) ? vol : -1;
  btSavedVolumePercent = btVolumePercent;
  if (outputMode == OUTPUT_BLUETOOTH && btVolumePercent < 0) {
    btVolumePercent = volumePercent < BT_FIRST_VOLUME_CAP ? volumePercent : BT_FIRST_VOLUME_CAP;
    btSavedVolumePercent = btVolumePercent;
  }
}

// Switch output. Called from loop() (root list OUTPUT entry).
static void setOutputMode(uint8_t mode) {
  if (mode != OUTPUT_BLUETOOTH) mode = OUTPUT_WIRED;
  if (mode == outputMode) return;
  if (mode == OUTPUT_BLUETOOTH && !pcmStream) {
    Serial.println("[OUTPUT] Bluetooth output unavailable: the PCM buffer could not be allocated.");
    return;
  }
  {
    PlayerLock lock;   // the audio task is not inside copy() while we hold this
    if (mode == OUTPUT_BLUETOOTH) {
      writeI2sSilence();
      btResetResampler();
      if (pcmStream) xStreamBufferReset(pcmStream);
      if (btVolumePercent < 0) {
        btVolumePercent = volumePercent < BT_FIRST_VOLUME_CAP ? volumePercent : BT_FIRST_VOLUME_CAP;
        btSavedVolumePercent = btVolumePercent;
      }
      outputMode = OUTPUT_BLUETOOTH;
    } else {
      outputMode = OUTPUT_WIRED;
      if (pcmStream) xStreamBufferReset(pcmStream);
    }
  }
  btVolSeq = btVolSeq + 1;      // make the classic apply the Bluetooth volume afresh
  btControlDirty = true;
  saveOutputSettings();
  Serial.printf("[OUTPUT] %s selected%s\n", mode == OUTPUT_BLUETOOTH ? "Bluetooth" : "Wired",
                mode == OUTPUT_BLUETOOTH ? " (the classic ESP32 connects the saved speaker)" : "");
  updateVolumeDisplay();
}

// Encoder in Bluetooth mode: speaker volume (sent to the classic over the link).
static void btChangeVolume(int32_t steps) {
  int v = (btVolumePercent < 0 ? 0 : btVolumePercent) + (int)steps * VOLUME_STEP;
  if (v < 0) v = 0;
  if (v > 100) v = 100;
  if (v == btVolumePercent) return;
  btVolumePercent = v;
  btSavedVolumePercent = v;
  btVolSeq = btVolSeq + 1;
  btControlDirty = true;
  btLastLocalVolumeMs = millis();
  btVolumeSaveDueMs = millis() + 3000;
  Serial.printf("[VOLUME] Bluetooth speaker %d%% (AVRCP %d/127, encoder steps: %+ld)\n",
                v, btVolumeToAvrcp(v), (long)steps);
  updateVolumeDisplay();
}

// v0.16.9: send a command to the classic (carried by the next control packet).
static void btSendCommand(uint8_t cmd, uint8_t arg) {
  btCmdArg = arg;
  uint16_t seq = btCmdSeq + 1;
  if (seq == 0) seq = 1;
  btCmdSeq = seq;
  btCmd = cmd;
  btControlDirty = true;
  Serial.printf("[OUTPUT] Command to the classic ESP32: %s %u (seq %u)\n",
                cmd == wslink::CMD_SCAN ? "find speakers" : cmd == wslink::CMD_CONNECT_FOUND ? "connect found" :
                cmd == wslink::CMD_CONNECT_SAVED ? "connect saved" : cmd == wslink::CMD_FORGET_SAVED ? "forget saved" : "?",
                (unsigned)arg, (unsigned)seq);
}

// Display state of the Bluetooth path: 0 streaming, 1 waiting for speaker, 2 no link.
static int btDisplayState() {
  if (!linkUp) return 2;
  return linkStreaming ? 0 : 1;
}

// loop(): follow volume changes made on the speaker, refresh the label when the
// link state changes, save the Bluetooth volume a few seconds after changes.
static void btServiceLoop() {
  static int shownState = -1;
  static int lastSeenSpeakerVolume = -1;
  uint32_t now = millis();
  if (btVolumeSaveDueMs && (int32_t)(now - btVolumeSaveDueMs) >= 0) {
    btVolumeSaveDueMs = 0;
    saveOutputSettings();
  }
  if (outputMode != OUTPUT_BLUETOOTH) { shownState = -1; lastSeenSpeakerVolume = -1; return; }
  int state = btDisplayState();
  if (state != shownState) {
    shownState = state;
    Serial.printf("[BT] %s\n", state == 0 ? "Speaker streaming." :
                               state == 1 ? "Link up; waiting for the speaker (playback continues silently)." :
                                            "No link to the classic ESP32 (playback continues silently).");
    updateVolumeDisplay();
  }
  if (state != 0) { lastSeenSpeakerVolume = -1; return; }
  wslink::Report r;
  portENTER_CRITICAL(&linkReportMux);
  r = linkLastReport;
  portEXIT_CRITICAL(&linkReportMux);
  // Changes made with the speaker's own buttons. Ignored until the player's
  // volume has been applied after the stream started (5 s), and for 2.5 s
  // after a knob change (the speaker echoes, and rounds, our own change).
  if (r.speakerVolume < 0 || r.volSeqApplied != btVolSeq) return;
  if (now - btStreamingSinceMs < 5000) { lastSeenSpeakerVolume = r.speakerVolume; return; }
  if (r.speakerVolume == lastSeenSpeakerVolume) return;
  bool first = (lastSeenSpeakerVolume < 0);
  lastSeenSpeakerVolume = r.speakerVolume;
  if (first || now - btLastLocalVolumeMs < 2500) return;
  int p = (r.speakerVolume * 100 + 63) / 127;
  if (p != btVolumePercent) {
    btVolumePercent = p;               // shown and used, but not saved (v0.16.8b)
    Serial.printf("[VOLUME] Changed on the speaker: %d%% (AVRCP %d/127)\n", p, (int)r.speakerVolume);
    updateVolumeDisplay();
  }
}

static void printBtStatus() {
  static uint64_t lastSent = 0, lastDiscarded = 0;
  static uint32_t lastMs = 0;
  uint32_t now = millis();
  double secs = lastMs ? (now - lastMs) / 1000.0 : 10.0;
  uint64_t sent = linkBytesSent, disc = linkBytesDiscarded;
  wslink::Report r;
  portENTER_CRITICAL(&linkReportMux);
  r = linkLastReport;
  portEXIT_CRITICAL(&linkReportMux);
  Serial.printf("[BT] link %s, %s | sent %.0f B/s, discarded %.0f B/s (real time 176400) | queue %u B | "
                "credit resets %lu, epoch changes %lu, report CRC errors %lu\n",
                linkUp ? "UP" : "DOWN", linkStreaming ? "speaker streaming" : "local clock",
                (sent - lastSent) / secs, (disc - lastDiscarded) / secs,
                pcmStream ? (unsigned)xStreamBufferBytesAvailable(pcmStream) : 0u,
                (unsigned long)linkCreditResets, (unsigned long)linkEpochChanges,
                (unsigned long)linkReportCrcErrors);
  if (linkReportsReceived)
    Serial.printf("[BT] receiver: %s, ring %.0f ms, underruns %lu, CRC %lu, gaps %lu, overruns %lu | "
                  "speaker \"%s\" volume %d/127 | knob %d%% (seq %u, applied %u)\n",
                  r.playState == wslink::PLAY_PLAYING ? "PLAYING" : "BUFFERING", r.ringFillBytes / 176.4,
                  (unsigned long)r.underruns, (unsigned long)r.crcErrors, (unsigned long)r.seqGaps,
                  (unsigned long)r.overruns, r.speakerName, (int)r.speakerVolume, btVolumePercent,
                  (unsigned)btVolSeq, (unsigned)r.volSeqApplied);
  lastSent = sent; lastDiscarded = disc; lastMs = now;
}
// ===================== end of v0.16.8 Bluetooth output =====================

// Auto-advance: the library's own auto-next is left off (setAutoNext(false)).
// The audio task detects the end of the file itself and moves to the next
// track in the current list; after the last track, playback stops.
static void advanceAtEndOfTrack() {
  if (repeatMode == REPEAT_ONE) {                          // v0.17.5
    Serial.println("[PLAY] End of track; repeating it (RPT 1).");
    if (player.next(0)) { pausedByUser = false; trackChanged = true; }
    else player.setActive(false);
    return;
  }
  Serial.println("[PLAY] End of track; advancing to the next track.");
  if (player.next(1)) {
    pausedByUser = false;
    trackChanged = true;
  } else {
    Serial.println("[PLAY] End of the track list; playback stopped.");
    player.setActive(false);
  }
}

static void audioTask(void *) {
  int64_t lastStepEndUs = 0;   // end of the previous decode step (0 = none)
  uint8_t emptyCopies = 0;
  for (;;) {
    bool worked = false;
    if (playerStarted) {
      xSemaphoreTakeRecursive(playerMutex, portMAX_DELAY);
      bool active = player.isActive();
      if (active) {
        int64_t start = esp_timer_get_time();
        if (lastStepEndUs != 0) {
          uint32_t gap = (uint32_t)(start - lastStepEndUs);
          if (gap > perfMaxCopyGapUs) perfMaxCopyGapUs = gap;
          if (gap > 50000) perfIdleOver50ms = perfIdleOver50ms + 1;
        }
        size_t copied = player.copy(DECODE_STEP_BYTES);
        int64_t end = esp_timer_get_time();
        lastStepEndUs = end;
        uint32_t took = (uint32_t)(end - start);
        if (took > perfMaxCopyUs) perfMaxCopyUs = took;
        worked = copied > 0;
        // End of file: nothing copied and nothing left to read, three passes in a row.
        if (copied == 0 && audioFileStream.isOpen() && audioFileStream.available() == 0) {
          if (++emptyCopies >= 3) {
            emptyCopies = 0;
            lastStepEndUs = 0;
            advanceAtEndOfTrack();
          }
        } else {
          emptyCopies = 0;
        }
        // An empty step means a 2 ms sleep follows; don't count that as idle time.
        if (!copied) lastStepEndUs = 0;
      } else {
        lastStepEndUs = 0;
        emptyCopies = 0;
      }
      playerActiveFlag = player.isActive();
      xSemaphoreGiveRecursive(playerMutex);
    } else {
      lastStepEndUs = 0;
    }
    // copy() blocks on the I2S DMA while playing, which paces this task.
    // If it did no work, sleep briefly so lower-priority tasks can run.
    if (!worked) vTaskDelay(pdMS_TO_TICKS(2));
    else if (playerLockWaiters.load() > 0) vTaskDelay(1);
  }
}

// --------------------------- File helpers ------------------------------
static bool isMp3(const char *name) {
  size_t len = strlen(name);
  if (len < 4) return false;
  const char *ext = name + len - 4;
  return ext[0] == '.' &&
         (ext[1] == 'm' || ext[1] == 'M') &&
         (ext[2] == 'p' || ext[2] == 'P') &&
         (ext[3] == '3');
}

// ---------------------- ID3 metadata ------------------------------------
static void cleanField(char *s) {
  for (size_t i = 0; s[i]; ++i) if (s[i] == '|' || s[i] == '\r' || s[i] == '\n' || s[i] == '\t') s[i] = ' ';
}

static void appendUtf8(char *out, size_t cap, size_t &used, uint32_t cp) {
  if (cp == 0 || used + 4 >= cap) return;
  if (cp < 0x80) out[used++] = (char)cp;
  else if (cp < 0x800) { out[used++] = 0xC0 | (cp >> 6); out[used++] = 0x80 | (cp & 0x3F); }
  else if (cp < 0x10000) { out[used++] = 0xE0 | (cp >> 12); out[used++] = 0x80 | ((cp >> 6) & 0x3F); out[used++] = 0x80 | (cp & 0x3F); }
  else { out[used++] = 0xF0 | (cp >> 18); out[used++] = 0x80 | ((cp >> 12) & 0x3F); out[used++] = 0x80 | ((cp >> 6) & 0x3F); out[used++] = 0x80 | (cp & 0x3F); }
  out[used] = 0;
}

static void decodeId3Text(const uint8_t *data, size_t len, char *out, size_t cap) {
  out[0] = 0; if (!len || !cap) return;
  uint8_t enc = data[0]; size_t i = 1, used = 0;
  if (enc == 1 && len >= 3) {
    bool little = data[1] == 0xFF && data[2] == 0xFE;
    if ((data[1] == 0xFF && data[2] == 0xFE) || (data[1] == 0xFE && data[2] == 0xFF)) i = 3;
    while (i + 1 < len && used + 4 < cap) {
      uint16_t u = little ? (uint16_t)data[i] | ((uint16_t)data[i+1] << 8) : ((uint16_t)data[i] << 8) | data[i+1]; i += 2;
      if (!u) break;
      uint32_t cp = u;
      if (u >= 0xD800 && u <= 0xDBFF && i + 1 < len) {
        uint16_t lo = little ? (uint16_t)data[i] | ((uint16_t)data[i+1] << 8) : ((uint16_t)data[i] << 8) | data[i+1];
        if (lo >= 0xDC00 && lo <= 0xDFFF) { cp = 0x10000 + (((uint32_t)u - 0xD800) << 10) + (lo - 0xDC00); i += 2; }
        else cp = ' ';
      }
      appendUtf8(out, cap, used, cp);
    }
  } else if (enc == 2) {
    while (i + 1 < len && used + 4 < cap) { uint16_t u = ((uint16_t)data[i] << 8) | data[i+1]; i += 2; if (!u) break; appendUtf8(out, cap, used, u); }
  } else {
    while (i < len && used + 2 < cap && data[i]) {
      uint8_t c = data[i++];
      if (enc == 0 && c >= 0x80) { appendUtf8(out, cap, used, 0x00A0 + (c - 0xA0)); }
      else out[used++] = (char)c;
    }
    out[used] = 0;
  }
  while (used && (out[used-1] == ' ' || out[used-1] == '\0')) --used;
  out[used] = 0;
}

static uint32_t synchsafe32(const uint8_t *b) { return ((uint32_t)(b[0]&0x7F)<<21)|((uint32_t)(b[1]&0x7F)<<14)|((uint32_t)(b[2]&0x7F)<<7)|(b[3]&0x7F); }
static void assignId3Field(TrackMetadata &m, const char *id, const uint8_t *data, size_t len) {
  char value[128]; decodeId3Text(data, len, value, sizeof(value)); if (!value[0]) return;
  if (!strcmp(id,"TIT2")) strlcpy(m.title,value,sizeof(m.title));
  else if (!strcmp(id,"TPE1")) strlcpy(m.artist,value,sizeof(m.artist));
  else if (!strcmp(id,"TALB")) strlcpy(m.album,value,sizeof(m.album));
  else if (!strcmp(id,"TPE2")) strlcpy(m.albumArtist,value,sizeof(m.albumArtist));
  else if (!strcmp(id,"TRCK")) strlcpy(m.track,value,sizeof(m.track));
  else if (!strcmp(id,"TPOS")) strlcpy(m.disc,value,sizeof(m.disc));
  else if (!strcmp(id,"TCON")) strlcpy(m.genre,value,sizeof(m.genre));
  else if (!strcmp(id,"TYER") || !strcmp(id,"TDRC")) strlcpy(m.year,value,sizeof(m.year));
  else if (!strcmp(id,"TCOM")) strlcpy(m.composer,value,sizeof(m.composer));
  else if (!strcmp(id,"TBPM")) strlcpy(m.bpm,value,sizeof(m.bpm));
  else if (!strcmp(id,"COMM") && len > 4) decodeId3Text(data + 4, len - 4, m.comment, sizeof(m.comment));
}

static void readId3Metadata(const char *path, TrackMetadata &m) {
  memset(&m, 0, sizeof(m));
  FILE *f = fopen(path,"rb"); if (!f) return;
  uint8_t h[10];
  if (fread(h,1,10,f)==10 && !memcmp(h,"ID3",3)) {
    uint32_t tagSize = synchsafe32(h+6); long pos=10; long end=10+(long)tagSize;
    while (pos + 10 <= end) {
      uint8_t fh[10]; if (fseek(f,pos,SEEK_SET)!=0 || fread(fh,1,10,f)!=10) break;
      if (!fh[0] || fh[0]<'A' || fh[0]>'Z') break;
      uint32_t frameSize = (h[3]==4) ? synchsafe32(fh+4) : ((uint32_t)fh[4]<<24)|((uint32_t)fh[5]<<16)|((uint32_t)fh[6]<<8)|fh[7];
      if (frameSize==0 || pos+10+(long)frameSize>end) break;
      char id[5]; memcpy(id,fh,4); id[4]=0;
      if ((id[0]=='T' || !strcmp(id,"COMM")) && frameSize<=4096) {
        if (fread(id3FrameBuffer,1,frameSize,f)==frameSize)
          assignId3Field(m,id,id3FrameBuffer,frameSize);
      }
      // Large APIC artwork and other binary frames are skipped, not treated as a malformed tag.
      pos += 10 + frameSize;
    }
  }
  // ID3v1 fallback for common fields not found in ID3v2.
  if (!m.title[0] || !m.artist[0] || !m.album[0] || !m.year[0] || !m.genre[0]) {
    if (fseek(f,-128,SEEK_END)==0) {
      uint8_t v1[128]; if (fread(v1,1,128,f)==128 && !memcmp(v1,"TAG",3)) {
        char tmp[32];
        if (!m.title[0]) { memcpy(tmp,v1+3,30); tmp[30]=0; strlcpy(m.title,tmp,sizeof(m.title)); }
        if (!m.artist[0]) { memcpy(tmp,v1+33,30); tmp[30]=0; strlcpy(m.artist,tmp,sizeof(m.artist)); }
        if (!m.album[0]) { memcpy(tmp,v1+63,30); tmp[30]=0; strlcpy(m.album,tmp,sizeof(m.album)); }
        if (!m.year[0]) { memcpy(tmp,v1+93,4); tmp[4]=0; strlcpy(m.year,tmp,sizeof(m.year)); }
        if (!m.genre[0] && v1[127]!=255) snprintf(m.genre,sizeof(m.genre),"ID3v1 genre %u",v1[127]);
      }
    }
  }
  fclose(f);
  if (!m.title[0]) { const char *b=strrchr(path,'/'); b=b?b+1:path; strlcpy(m.title,b,sizeof(m.title)); size_t n=strlen(m.title); if(n>4) m.title[n-4]=0; }
  if (!m.artist[0]) strlcpy(m.artist,"Unknown artist",sizeof(m.artist));
  if (!m.album[0]) strlcpy(m.album,"Unknown album",sizeof(m.album));
  if (!m.albumArtist[0]) strlcpy(m.albumArtist,m.artist,sizeof(m.albumArtist));
  cleanField(m.title); cleanField(m.artist); cleanField(m.album); cleanField(m.albumArtist); cleanField(m.track); cleanField(m.disc); cleanField(m.genre); cleanField(m.year); cleanField(m.composer); cleanField(m.comment); cleanField(m.bpm);
}

static void printTrackMetadata() {
  const TrackMetadata &m = currentTrackMetadata;
  Serial.printf("[META] Title: %s | Artist: %s | Album: %s\n", m.title, m.artist, m.album);
  Serial.printf("[META] Album Artist: %s | Track: %s | Disc: %s | Genre: %s | Year: %s\n", m.albumArtist, m.track, m.disc, m.genre, m.year);
  Serial.printf("[META] Composer: %s | Comment: %s | BPM: %s\n", m.composer, m.comment, m.bpm);
}

// Touch callbacks queue commands for the main loop; the audio player is not
// manipulated from the LVGL task.
static void playbackButtonEvent(lv_event_t *event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  uint8_t action = (uint8_t)(uintptr_t)lv_event_get_user_data(event);
  noteUiEvent(action == ACTION_NEXT ? "Next" : action == ACTION_PREVIOUS ? "Previous" :
              action == ACTION_SHUFFLE ? "Shuffle" : action == ACTION_REPEAT ? "Repeat" : "Play/Pause");
  if (playbackActionQueue && xQueueSend(playbackActionQueue, &action, 0) != pdTRUE)
    Serial.println("[UI] Playback button queue full; press dropped.");
}

static lv_obj_t *createPlaybackButton(lv_obj_t *parent, const char *caption,
                                      int x, PlaybackAction action) {
  lv_obj_t *button = lv_btn_create(parent);
  lv_obj_set_size(button, PlayerUIConfig::PLAYBACK_BUTTON_WIDTH, PlayerUIConfig::PLAYBACK_BUTTON_HEIGHT);
  lv_obj_align(button, LV_ALIGN_TOP_MID, x, PlayerUIConfig::PLAYBACK_BUTTON_Y);
  uint32_t normalColor = uiPlaybackButtonBgWasLoaded ? uiPlaybackButtonBg :
                         ((action == ACTION_TOGGLE) ? COLOR_PLAYBACK_TOGGLE_BG : COLOR_PLAYBACK_BUTTON_BG);
  uint32_t pressedColor = (action == ACTION_TOGGLE) ? COLOR_PLAYBACK_TOGGLE_PRESSED : COLOR_PLAYBACK_BUTTON_PRESSED;
  lv_obj_set_style_bg_color(button, lv_color_hex(normalColor), LV_PART_MAIN);
  lv_obj_set_style_bg_color(button, lv_color_hex(pressedColor), LV_PART_MAIN | LV_STATE_PRESSED);
  lv_obj_set_style_radius(button, 12, LV_PART_MAIN);
  lv_obj_add_event_cb(button, playbackButtonEvent, LV_EVENT_CLICKED,
                      (void *)(uintptr_t)action);
  lv_obj_t *label = lv_label_create(button);
  lv_label_set_text(label, caption);
  lv_obj_center(label);
  lv_obj_set_style_text_color(label, lv_color_hex(uiPlaybackButtonText), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, playerFont(uiFontPlaybackButton), LV_PART_MAIN);
  return button;
}

static void formatTime(uint32_t seconds, char *out, size_t capacity);

// Progress-bar touch handling. The LVGL task only queues the request; file and
// decoder operations are carried out later by loop().
static uint64_t progressTouchToMs(lv_event_t *event) {
  lv_obj_t *obj = lv_event_get_target(event);
  lv_area_t area;
  lv_obj_get_coords(obj, &area);
  lv_indev_t *indev = lv_indev_get_act();
  if (!indev || trackDurationMs == 0) return 0;
  lv_point_t point;
  lv_indev_get_point(indev, &point);
  int32_t width = area.x2 - area.x1;
  if (width <= 0) return 0;
  int32_t x = point.x - area.x1;
  if (x < 0) x = 0;
  if (x > width) x = width;
  return (trackDurationMs * (uint64_t)x) / (uint32_t)width;
}

static void progressBarEvent(lv_event_t *event) {
  lv_event_code_t code = lv_event_get_code(event);
  if (code == LV_EVENT_PRESSED || code == LV_EVENT_PRESSING) {
    seekPreviewMs = progressTouchToMs(event);
    seekPreviewActive = true;
    if (elapsedLabel) {
      char text[16];
      formatTime((uint32_t)(seekPreviewMs / 1000ULL), text, sizeof(text));
      lv_label_set_text(elapsedLabel, text);
    }
    if (progressBar && trackDurationMs) {
      int value = (int)((seekPreviewMs * 1000ULL) / trackDurationMs);
      if (value > 1000) value = 1000;
      lv_bar_set_value(progressBar, value, LV_ANIM_OFF);
    }
  } else if (code == LV_EVENT_RELEASED) {
    pendingSeekMs = progressTouchToMs(event);
    pendingSeek = true;
    seekPreviewActive = false;
  }
}

// Functions implemented later in the sketch but used by the browser/index UI.
static void makeDisplayTitle(const char *source, char *destination, size_t capacity);
static void makeLibraryIndexPath(size_t libraryIndex, char *out, size_t capacity);
static void updateMusicScreen();

// --------------------------- Folder browser -----------------------------
static void browserEntryEvent(lv_event_t *event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  intptr_t encodedRow = (intptr_t)lv_event_get_user_data(event);
  if (encodedRow > 0) {
    int entryIndex = (int)(browserPage * BROWSER_PAGE_SIZE + (size_t)(encodedRow - 1));
    pendingBrowserSelection = entryIndex;
    noteUiEvent("Browser entry");
    Serial.printf("[BROWSE] Entry tapped: %d\n", entryIndex);
  }
}

static void browseButtonEvent(lv_event_t *event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  pendingBrowserSelection = -2; // switch to browser root/current folder
  noteUiEvent("Browse");
}

static void browserBackEvent(lv_event_t *event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  pendingBrowserSelection = -3; // parent folder
  noteUiEvent("Back");
}

static void volumeLineEvent(lv_event_t *event) {   // v0.17.2
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  pendingBrowserSelection = -7;
  noteUiEvent("Volume line (Output page)");
}

static void browserNowPlayingEvent(lv_event_t *event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  pendingBrowserSelection = -4; // return to playback screen
  noteUiEvent("Now playing");
}

static void browserPagePrevEvent(lv_event_t *event) {
  if (lv_event_get_code(event) == LV_EVENT_CLICKED) { pendingBrowserSelection = -6; noteUiEvent("Page previous"); }
}
static void browserPageNextEvent(lv_event_t *event) {
  if (lv_event_get_code(event) == LV_EVENT_CLICKED) { pendingBrowserSelection = -5; noteUiEvent("Page next"); }
}
static void browserLetterEvent(lv_event_t *event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
  pendingBrowserLetter = (int)(intptr_t)lv_event_get_user_data(event);
  noteUiEvent("Letter");
}
static void browserRescanEvent(lv_event_t *event) {
  if (lv_event_get_code(event) == LV_EVENT_CLICKED) pendingLibraryRescan = true;
}

static bool isLibraryRoot(const char *path) {
  return strcmp(path, "/sdcard/Music_Library") == 0;
}

static void sortBrowserEntries() {
  // Directories first, then files; alphabetical within each group.
  for (size_t i = 0; i < browserEntryCount; ++i) {
    for (size_t j = i + 1; j < browserEntryCount; ++j) {
      bool swap = (browserEntries[i].isDirectory < browserEntries[j].isDirectory) ||
        (browserEntries[i].isDirectory == browserEntries[j].isDirectory &&
         strcasecmp(browserEntries[i].name, browserEntries[j].name) > 0);
      if (swap) { BrowserEntry t = browserEntries[i]; browserEntries[i] = browserEntries[j]; browserEntries[j] = t; }
    }
  }
}

static void addBrowserEntry(const char *name, const char *path, bool isDirectory) {
  if (browserEntryCount >= MAX_BROWSER_ENTRIES) return;
  BrowserEntry &e = browserEntries[browserEntryCount++];
  strlcpy(e.name, name, sizeof(e.name));
  strlcpy(e.path, path ? path : "", sizeof(e.path));
  e.isDirectory = isDirectory;
  e.isAction = false;
  e.discNumber = 0;
  e.trackNumber = 0;
}

static char *indexField(char *line, unsigned wanted) {
  char *field = line;
  for (unsigned i = 0; i < wanted; ++i) {
    char *tab = strchr(field, '\t');
    if (!tab) return (char *)"";
    *tab = 0;
    field = tab + 1;
  }
  char *tab = strchr(field, '\t');
  if (tab) *tab = 0;
  return field;
}

static bool artistMatchesSelected(const char *artist) {
  return artist && artist[0] && strcasecmp(artist, selectedArtist) == 0;
}

static const char *indexArtistField(char *line) {
  char *fields[12] = {};
  char *cursor = line;
  for (unsigned i = 0; i < 12; ++i) {
    fields[i] = cursor;
    char *tab = strchr(cursor, '\t');
    if (!tab) break;
    *tab = 0;
    cursor = tab + 1;
  }
  // Group by Artist; the ID3 reader writes "Unknown artist" when Artist is
  // missing, so treat that placeholder like an empty field for fallback.
  return (fields[2][0] && strcasecmp(fields[2], "Unknown artist") != 0)
           ? fields[2] : fields[4];
}

static void sortTrackBrowserEntries() {
  // Entry zero is the PLAY ALBUM action. Sort the remaining tracks by disc/track.
  for (size_t i = 2; i < browserEntryCount; ++i) {
    size_t j = i;
    while (j > 1) {
      BrowserEntry &a = browserEntries[j - 1];
      BrowserEntry &b = browserEntries[j];
      bool swap = (a.discNumber > b.discNumber) ||
                  (a.discNumber == b.discNumber && a.trackNumber > b.trackNumber) ||
                  (a.discNumber == b.discNumber && a.trackNumber == b.trackNumber &&
                   strcasecmp(a.name, b.name) > 0);
      if (!swap) break;
      BrowserEntry temp = a; a = b; b = temp;
      --j;
    }
  }
}

// ---------------- v0.16.7d: library index kept in PSRAM -----------------
// One library at a time. Strings live in one pool; records hold offsets.
struct IndexRecord {
  uint32_t path, title, artist, album;   // offsets into indexPool (album: NO_ALBUM if empty)
  uint16_t track, disc;
};
static constexpr uint32_t NO_ALBUM = 0xFFFFFFFFu;
static char *indexPool = nullptr;
static size_t indexPoolUsed = 0, indexPoolCap = 0;
static IndexRecord *indexRecords = nullptr;
static size_t indexRecordCount = 0, indexRecordCap = 0;
static int cachedLibrary = -1;          // library held in PSRAM, -1 = none
static int cacheFailedLibrary = -1;     // don't retry a failed load on every tap

static void freeLetterMatches();   // v0.18.2
static void freeLibraryCache() {
  freeLetterMatches();
  if (indexPool) heap_caps_free(indexPool);
  if (indexRecords) heap_caps_free(indexRecords);
  indexPool = nullptr; indexRecords = nullptr;
  indexPoolUsed = indexPoolCap = indexRecordCount = indexRecordCap = 0;
  cachedLibrary = -1;
  cacheFailedLibrary = -1;
}

static inline const char *poolStr(uint32_t offset) { return indexPool + offset; }
static inline const char *recordAlbum(const IndexRecord &r) {
  return r.album == NO_ALBUM ? "Unknown album" : indexPool + r.album;
}

static bool poolAdd(const char *text, uint32_t &offset) {
  size_t len = strlen(text) + 1;
  if (indexPoolUsed + len > indexPoolCap) return false;
  memcpy(indexPool + indexPoolUsed, text, len);
  offset = (uint32_t)indexPoolUsed;
  indexPoolUsed += len;
  return true;
}

static void showBrowserLoading(const char *message) {
  if (!browserScreen || !browserStatusLabel || !lcd_lvgl_lock(100)) return;
  if (browserList) lv_obj_add_flag(browserList, LV_OBJ_FLAG_HIDDEN);
  if (browserEmptyLabel) lv_obj_add_flag(browserEmptyLabel, LV_OBJ_FLAG_HIDDEN);
  lv_label_set_text(browserStatusLabel, message);
  lv_obj_clear_flag(browserStatusLabel, LV_OBJ_FLAG_HIDDEN);
  lcd_lvgl_unlock();
  // The LVGL task (core 0) draws this while loop() reads the SD card.
  delay(40);
}

static bool loadLibraryCache(size_t libraryIndex) {
  freeLibraryCache();
  uint32_t t0 = millis();
  char indexPath[PATH_LENGTH];
  makeLibraryIndexPath(libraryIndex, indexPath, sizeof(indexPath));
  FILE *f = fopen(indexPath, "rb");
  if (!f) { Serial.printf("[LIBRARY] Cannot open index for PSRAM copy: %s\n", indexPath); return false; }
  fseek(f, 0, SEEK_END);
  long fileSize = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (fileSize <= 0) { fclose(f); return false; }

  showBrowserLoading("Loading library...");

  // Each record's strings are a subset of its line, so the file size bounds the pool.
  indexPoolCap = (size_t)fileSize + 1;
  indexPool = (char *)heap_caps_malloc(indexPoolCap, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  indexRecordCap = 2048;
  indexRecords = (IndexRecord *)heap_caps_malloc(indexRecordCap * sizeof(IndexRecord),
                                                 MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!indexPool || !indexRecords) {
    Serial.println("[LIBRARY] Not enough PSRAM for the index copy; using the SD card.");
    fclose(f); freeLibraryCache(); return false;
  }
  // The default stdio buffer is small; a 16 KB buffer makes the one-time read much faster.
  char *ioBuf = (char *)heap_caps_malloc(16384, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (ioBuf) setvbuf(f, ioBuf, _IOFBF, 16384);

  static char line[1024];
  bool ok = true;
  while (fgets(line, sizeof(line), f)) {
    if (line[0] == '#') continue;
    char *nl = strpbrk(line, "\r\n");
    if (nl) *nl = 0;
    char *fields[15] = {};
    char *cursor = line;
    for (size_t i = 0; i < 15; ++i) {
      fields[i] = cursor;
      char *tab = strchr(cursor, '\t');
      if (!tab) { for (size_t k = i + 1; k < 15; ++k) fields[k] = (char *)""; break; }
      *tab = 0;
      cursor = tab + 1;
    }
    if (!fields[0][0]) continue;
    // Same artist/album rules as the file-based code.
    const char *artist = (fields[2][0] && strcasecmp(fields[2], "Unknown artist") != 0)
                           ? fields[2] : fields[4];
    if (indexRecordCount == indexRecordCap) {
      size_t newCap = indexRecordCap * 2;
      IndexRecord *grown = (IndexRecord *)heap_caps_realloc(indexRecords, newCap * sizeof(IndexRecord),
                                                            MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
      if (!grown) { ok = false; break; }
      indexRecords = grown; indexRecordCap = newCap;
    }
    IndexRecord &r = indexRecords[indexRecordCount];
    if (!poolAdd(fields[0], r.path) || !poolAdd(fields[1], r.title) || !poolAdd(artist, r.artist)) { ok = false; break; }
    if (fields[3][0]) { if (!poolAdd(fields[3], r.album)) { ok = false; break; } }
    else r.album = NO_ALBUM;
    r.track = (uint16_t)strtoul(fields[5], nullptr, 10);
    r.disc = (uint16_t)strtoul(fields[6], nullptr, 10);
    ++indexRecordCount;
  }
  fclose(f);
  if (ioBuf) heap_caps_free(ioBuf);
  if (!ok) {
    Serial.println("[LIBRARY] PSRAM index copy failed; using the SD card.");
    freeLibraryCache(); return false;
  }
  cachedLibrary = (int)libraryIndex;
  Serial.printf("[LIBRARY] %s index loaded into PSRAM: %u tracks, %u KB, %lu ms (PSRAM free %u KB)\n",
                MUSIC_LIBRARIES[libraryIndex].folder, (unsigned)indexRecordCount,
                (unsigned)((indexPoolUsed + indexRecordCount * sizeof(IndexRecord)) / 1024),
                (unsigned long)(millis() - t0),
                (unsigned)(heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024));
  return true;
}

// True when the selected library is available in PSRAM (loading it if needed).
static bool ensureLibraryCache() {
  if (cachedLibrary == (int)selectedLibraryIndex && indexPool) return true;
  if (cacheFailedLibrary == (int)selectedLibraryIndex) return false;
  if (loadLibraryCache(selectedLibraryIndex)) return true;
  cacheFailedLibrary = (int)selectedLibraryIndex;
  return false;
}

static char artistInitial(const char *artist) {
  unsigned char first = (unsigned char)artist[0];
  return (first < 0x80 && isalpha(first)) ? (char)toupper(first) : '#';
}

static bool addUniqueBrowserName(const char *name) {
  for (size_t i = 0; i < browserEntryCount; ++i)
    if (strcasecmp(browserEntries[i].name, name) == 0) return false;
  addBrowserEntry(name, "", false);
  return true;
}

static void populateArtistEntriesFromCache() {
  browserEntryCount = 0;
  for (size_t n = 0; n < indexRecordCount && browserEntryCount < MAX_BROWSER_ENTRIES; ++n) {
    const char *artist = poolStr(indexRecords[n].artist);
    if (!artist[0] || artistInitial(artist) != selectedLetter) continue;
    addUniqueBrowserName(artist);
  }
  sortBrowserEntries();
  Serial.printf("[BROWSE] %u artist(s) beginning with %c\n", (unsigned)browserEntryCount, selectedLetter);
}

static void populateAlbumEntriesFromCache() {
  browserEntryCount = 0;
  for (size_t n = 0; n < indexRecordCount && browserEntryCount < MAX_BROWSER_ENTRIES; ++n) {
    if (!artistMatchesSelected(poolStr(indexRecords[n].artist))) continue;
    addUniqueBrowserName(recordAlbum(indexRecords[n]));
  }
  sortBrowserEntries();
  Serial.printf("[BROWSE] %u album(s) for %s\n", (unsigned)browserEntryCount, selectedArtist);
}

static void populateTrackEntriesFromCache() {
  browserEntryCount = 0;
  addBrowserEntry("ALBUM", "", false);
  browserEntries[0].isAction = true;
  for (size_t n = 0; n < indexRecordCount; ++n) {
    const IndexRecord &r = indexRecords[n];
    if (!artistMatchesSelected(poolStr(r.artist))) continue;
    if (strcasecmp(recordAlbum(r), selectedAlbum) != 0) continue;
    const char *path = poolStr(r.path);
    const char *title = poolStr(r.title);
    if (!title[0]) { const char *base = strrchr(path, '/'); title = base ? base + 1 : path; }
    if (browserEntryCount < MAX_BROWSER_ENTRIES) {
      addBrowserEntry(title, path, false);
      BrowserEntry &entry = browserEntries[browserEntryCount - 1];
      entry.trackNumber = r.track;
      entry.discNumber = r.disc;
    }
  }
  if (browserEntryCount == 1) browserEntryCount = 0;
  else sortTrackBrowserEntries();
  Serial.printf("[BROWSE] %u visible track(s) for %s / %s\n",
                (unsigned)(browserEntryCount ? browserEntryCount - 1 : 0), selectedArtist, selectedAlbum);
}

// ===================== v0.18.2: Browse Albums / Browse Songs =================
// The records whose album (or song title) starts with the selected letter are
// collected as record numbers in PSRAM and sorted once; each page then builds
// only its five rows. Albums are one row per album + artist.
static uint32_t *letterMatches = nullptr;
static size_t letterMatchCount = 0;
static bool letterMatchesValid = false;
static LetterMode letterMatchMode = LETTERS_ARTISTS;
static char letterMatchLetter = 0;
static int letterMatchLibrary = -1;

static void noteAdded(size_t added, const char *what);          // defined with the playlists
static bool appendPathToOpenPlaylist(const char *absolutePath);
static void loadIndexedAlbumQueue(const char *selectedPath);

static void freeLetterMatches() {
  if (letterMatches) heap_caps_free(letterMatches);
  letterMatches = nullptr;
  letterMatchCount = 0;
  letterMatchesValid = false;
}

static const char *recordTitle(const IndexRecord &r) {
  const char *title = poolStr(r.title);
  if (title[0]) return title;
  const char *path = poolStr(r.path);
  const char *base = strrchr(path, '/');
  return base ? base + 1 : path;
}

static int compareSongRecords(const void *a, const void *b) {
  const IndexRecord &ra = indexRecords[*(const uint32_t *)a], &rb = indexRecords[*(const uint32_t *)b];
  int c = strcasecmp(recordTitle(ra), recordTitle(rb));
  return c ? c : strcasecmp(poolStr(ra.artist), poolStr(rb.artist));
}

static int compareAlbumRecords(const void *a, const void *b) {
  const IndexRecord &ra = indexRecords[*(const uint32_t *)a], &rb = indexRecords[*(const uint32_t *)b];
  int c = strcasecmp(recordAlbum(ra), recordAlbum(rb));
  return c ? c : strcasecmp(poolStr(ra.artist), poolStr(rb.artist));
}

static bool buildLetterMatches(LetterMode mode) {
  if (letterMatchesValid && letterMatchMode == mode && letterMatchLetter == selectedLetter &&
      letterMatchLibrary == (int)selectedLibraryIndex && cachedLibrary == (int)selectedLibraryIndex)
    return true;
  freeLetterMatches();
  if (!ensureLibraryCache() || indexRecordCount == 0) return false;
  letterMatches = (uint32_t *)heap_caps_malloc(indexRecordCount * sizeof(uint32_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!letterMatches) { Serial.println("[BROWSE] No PSRAM for the letter list."); return false; }
  uint32_t startMs = millis();
  for (size_t n = 0; n < indexRecordCount; ++n) {
    const IndexRecord &r = indexRecords[n];
    if (mode == LETTERS_ALBUMS) {
      if (r.album == NO_ALBUM || !poolStr(r.artist)[0]) continue;   // the album's tracks are found by artist
      if (artistInitial(recordAlbum(r)) != selectedLetter) continue;
    } else if (artistInitial(recordTitle(r)) != selectedLetter) continue;
    letterMatches[letterMatchCount++] = (uint32_t)n;
  }
  qsort(letterMatches, letterMatchCount, sizeof(uint32_t),
        mode == LETTERS_ALBUMS ? compareAlbumRecords : compareSongRecords);
  if (mode == LETTERS_ALBUMS && letterMatchCount > 1) {   // one row per album + artist
    size_t kept = 1;
    for (size_t i = 1; i < letterMatchCount; ++i)
      if (compareAlbumRecords(&letterMatches[i], &letterMatches[kept - 1]) != 0)
        letterMatches[kept++] = letterMatches[i];
    letterMatchCount = kept;
  }
  letterMatchMode = mode; letterMatchLetter = selectedLetter;
  letterMatchLibrary = (int)selectedLibraryIndex; letterMatchesValid = true;
  Serial.printf("[BROWSE] %u %s beginning with %c (%lu ms)\n", (unsigned)letterMatchCount,
                mode == LETTERS_ALBUMS ? "album(s)" : "song(s)", selectedLetter,
                (unsigned long)(millis() - startMs));
  return true;
}

static void populateLetterIndexEntries(LetterMode mode) {
  browserEntryCount = 0;
  browserTotalEntries = 0;
  if (!buildLetterMatches(mode)) return;
  browserTotalEntries = letterMatchCount;
  size_t first = browserPage * BROWSER_PAGE_SIZE;
  for (size_t i = first; i < letterMatchCount && i < first + BROWSER_PAGE_SIZE; ++i) {
    const IndexRecord &r = indexRecords[letterMatches[i]];
    const char *artist = poolStr(r.artist);
    char label[sizeof(((BrowserEntry *)0)->name)];
    const char *name = mode == LETTERS_ALBUMS ? recordAlbum(r) : recordTitle(r);
    if (artist[0]) snprintf(label, sizeof(label), "%.80s  (%.40s)", name, artist);
    else strlcpy(label, name, sizeof(label));
    addBrowserEntry(label, poolStr(r.path), false);
  }
}

// Tap on a row of Browse Albums / Browse Songs (selection = position in the list).
static void handleLetterIndexSelection(size_t selection) {
  if (!letterMatches || selection >= letterMatchCount) return;
  const IndexRecord &r = indexRecords[letterMatches[selection]];
  strlcpy(selectedArtist, poolStr(r.artist), sizeof(selectedArtist));
  strlcpy(selectedAlbum, recordAlbum(r), sizeof(selectedAlbum));
  if (browserView == BROWSER_ALBUM_INDEX) {
    tracksFromAlbumIndex = true;
    albumIndexPage = browserPage;
    browserView = BROWSER_TRACKS; browserPage = 0; showBrowser();
    return;
  }
  const char *path = poolStr(r.path);
  if (playlistAddMode) { noteAdded(appendPathToOpenPlaylist(path) ? 1 : 0, recordTitle(r)); return; }
  if (selectedArtist[0]) {
    loadIndexedAlbumQueue(path);                 // play the song, then carry on through its album
  } else {                                       // no artist tag: play just this song
    strlcpy(trackPaths[0], path, PATH_LENGTH);
    trackNumbers[0] = r.track; trackDiscNumbers[0] = r.disc;
    trackCount = 1; currentTrack = 0;
  }
  startSelectedPlayback();
}

static void populateArtistEntries() {
  if (ensureLibraryCache()) { populateArtistEntriesFromCache(); return; }
  browserEntryCount = 0;
  char indexPath[PATH_LENGTH];
  makeLibraryIndexPath(selectedLibraryIndex, indexPath, sizeof(indexPath));
  FILE *f = fopen(indexPath, "rb");
  if (!f) { Serial.printf("[BROWSE] Cannot open artist index: %s\n", indexPath); return; }
  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    if (line[0] == '#') continue;
    char artistLine[1024]; strlcpy(artistLine, line, sizeof(artistLine));
    const char *artist = indexArtistField(artistLine);
    if (!artist[0]) continue;
    unsigned char first = (unsigned char)artist[0];
    char initial = (first < 0x80 && isalpha(first)) ? (char)toupper(first) : '#';
    if (initial != selectedLetter) continue;
    bool duplicate = false;
    for (size_t i = 0; i < browserEntryCount; ++i)
      if (strcasecmp(browserEntries[i].name, artist) == 0) { duplicate = true; break; }
    if (!duplicate) addBrowserEntry(artist, "", false);
    if (browserEntryCount >= MAX_BROWSER_ENTRIES) break;
  }
  fclose(f);
  sortBrowserEntries();
  Serial.printf("[BROWSE] %u artist(s) beginning with %c\n", (unsigned)browserEntryCount, selectedLetter);
}

static void populateAlbumEntries() {
  if (ensureLibraryCache()) { populateAlbumEntriesFromCache(); return; }
  browserEntryCount = 0;
  char indexPath[PATH_LENGTH];
  makeLibraryIndexPath(selectedLibraryIndex, indexPath, sizeof(indexPath));
  FILE *f = fopen(indexPath, "rb");
  if (!f) return;
  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    if (line[0] == '#') continue;
    char copy[1024]; strlcpy(copy, line, sizeof(copy));
    const char *artist = indexArtistField(copy);
    if (!artistMatchesSelected(artist)) continue;
    char albumLine[1024]; strlcpy(albumLine, line, sizeof(albumLine));
    char *album = indexField(albumLine, 3);
    if (!album[0]) album = (char *)"Unknown album";
    bool duplicate = false;
    for (size_t i = 0; i < browserEntryCount; ++i)
      if (strcasecmp(browserEntries[i].name, album) == 0) { duplicate = true; break; }
    if (!duplicate) addBrowserEntry(album, "", false);
    if (browserEntryCount >= MAX_BROWSER_ENTRIES) break;
  }
  fclose(f);
  sortBrowserEntries();
  Serial.printf("[BROWSE] %u album(s) for %s\n", (unsigned)browserEntryCount, selectedArtist);
}

static void populateTrackEntries() {
  if (ensureLibraryCache()) { populateTrackEntriesFromCache(); return; }
  browserEntryCount = 0;
  addBrowserEntry("ALBUM", "", false);
  browserEntries[0].isAction = true;

  char indexPath[PATH_LENGTH];
  makeLibraryIndexPath(selectedLibraryIndex, indexPath, sizeof(indexPath));
  FILE *f = fopen(indexPath, "rb");
  if (!f) return;

  // One line buffer plus pointers keeps this off the ESP32 task's limited stack.
  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    if (line[0] == '#') continue;
    char *fields[15] = {};
    char *cursor = line;
    for (size_t i = 0; i < 15; ++i) {
      fields[i] = cursor;
      char *tab = strchr(cursor, '\t');
      if (!tab) break;
      *tab = 0;
      cursor = tab + 1;
    }

    const char *artist = (fields[2][0] && strcasecmp(fields[2], "Unknown artist") != 0)
                           ? fields[2] : fields[4];
    if (!artistMatchesSelected(artist)) continue;
    const char *album = fields[3][0] ? fields[3] : "Unknown album";
    if (strcasecmp(album, selectedAlbum) != 0) continue;

    const char *path = fields[0];
    const char *title = fields[1];
    if (!title[0]) {
      const char *base = strrchr(path, '/');
      title = base ? base + 1 : path;
    }
    if (browserEntryCount < MAX_BROWSER_ENTRIES) {
      addBrowserEntry(title, path, false);
      BrowserEntry &entry = browserEntries[browserEntryCount - 1];
      entry.trackNumber = (uint16_t)strtoul(fields[5], nullptr, 10);
      entry.discNumber = (uint16_t)strtoul(fields[6], nullptr, 10);
    }
  }
  fclose(f);
  if (browserEntryCount == 1) browserEntryCount = 0;
  else sortTrackBrowserEntries();
  Serial.printf("[BROWSE] %u visible track(s) for %s / %s\n",
                (unsigned)(browserEntryCount ? browserEntryCount - 1 : 0), selectedArtist, selectedAlbum);
}

static bool readFolderEntryAt(const char *folder, size_t wantedIndex, BrowserEntry &out) {
  DIR *dir = opendir(folder);
  if (!dir) return false;
  size_t index = 0;
  bool found = false;
  struct dirent *entry;
  while ((entry = readdir(dir)) != nullptr) {
    const char *name = entry->d_name;
    if (!strcmp(name, ".") || !strcmp(name, "..") || name[0] == '.') continue;
    char path[PATH_LENGTH];
    int n = snprintf(path, sizeof(path), "%s/%s", folder, name);
    if (n < 0 || (size_t)n >= sizeof(path)) continue;
    struct stat st;
    if (stat(path, &st) != 0) continue;
    bool isDirectory = S_ISDIR(st.st_mode);
    if (!isDirectory && !(S_ISREG(st.st_mode) && isMp3(name))) continue;
    if (index++ == wantedIndex) {
      strlcpy(out.name, name, sizeof(out.name));
      strlcpy(out.path, path, sizeof(out.path));
      out.isDirectory = isDirectory;
      out.isAction = false;
      out.discNumber = 0;
      out.trackNumber = 0;
      found = true;
      break;
    }
  }
  closedir(dir);
  return found;
}

static bool folderHasPlayAll = false;   // v0.16.8: current folder list starts with PLAY FOLDER

static void populateFolderEntries() {
  browserEntryCount = 0;
  browserTotalEntries = 0;
  if (isLibraryRoot(browsePath)) {
    struct stat st;
    for (size_t i = 0; i < MUSIC_LIBRARY_COUNT; ++i) {
      char path[PATH_LENGTH];
      int n = snprintf(path, sizeof(path), "%s/%s", browsePath, MUSIC_LIBRARIES[i].folder);
      if (n > 0 && (size_t)n < sizeof(path) && stat(path, &st) == 0 && S_ISDIR(st.st_mode))
        addBrowserEntry(MUSIC_LIBRARIES[i].folder, path, true);
    }
    sortBrowserEntries();
    // v0.17.2: PLAYLISTS (DONE ADDING while adding), then OUTPUT (not while adding).
    addBrowserEntry(playlistAddMode ? "DONE ADDING" : "PLAYLISTS", "", false);
    browserEntries[browserEntryCount - 1].isAction = true;
    browserEntries[browserEntryCount - 1].trackNumber = RR_PLAYLISTS;
    if (!playlistAddMode) {
      addBrowserEntry(outputMode == OUTPUT_BLUETOOTH ? "BLUETOOTH" : "WIRED", "", false);
      browserEntries[browserEntryCount - 1].isAction = true;
      browserEntries[browserEntryCount - 1].trackNumber = RR_OUTPUT;
    }
    browserTotalEntries = browserEntryCount;
    return;
  }

  // Pass 1: count the entire directory without retaining its paths in RAM.
  DIR *dir = opendir(browsePath);
  if (!dir) {
    Serial.printf("[BROWSE] Cannot open %s\n", browsePath);
    return;
  }
  struct dirent *entry;
  size_t mp3Count = 0;
  while ((entry = readdir(dir)) != nullptr) {
    const char *name = entry->d_name;
    if (!strcmp(name, ".") || !strcmp(name, "..") || name[0] == '.') continue;
    char path[PATH_LENGTH];
    int n = snprintf(path, sizeof(path), "%s/%s", browsePath, name);
    if (n < 0 || (size_t)n >= sizeof(path)) continue;
    struct stat st;
    if (stat(path, &st) != 0) continue;
    if (S_ISDIR(st.st_mode)) ++browserTotalEntries;
    else if (S_ISREG(st.st_mode) && isMp3(name)) { ++browserTotalEntries; ++mp3Count; }
  }
  closedir(dir);

  // v0.16.8: a folder holding MP3 files gets a PLAY FOLDER entry at index 0;
  // directory entries then start at list index 1.
  folderHasPlayAll = mp3Count > 0;
  const size_t offset = folderHasPlayAll ? 1 : 0;
  browserTotalEntries += offset;

  // Pass 2: load only the five items belonging to the selected page.
  size_t first = browserPage * BROWSER_PAGE_SIZE;
  size_t last = first + BROWSER_PAGE_SIZE;
  if (folderHasPlayAll && first == 0) {
    addBrowserEntry("FOLDER", browsePath, false);
    browserEntries[0].isAction = true;
  }
  first = first >= offset ? first - offset : 0;     // directory-entry numbering
  last -= offset;
  size_t index = 0;
  dir = opendir(browsePath);
  if (!dir) return;
  while ((entry = readdir(dir)) != nullptr && browserEntryCount < BROWSER_PAGE_SIZE) {
    const char *name = entry->d_name;
    if (!strcmp(name, ".") || !strcmp(name, "..") || name[0] == '.') continue;
    char path[PATH_LENGTH];
    int n = snprintf(path, sizeof(path), "%s/%s", browsePath, name);
    if (n < 0 || (size_t)n >= sizeof(path)) continue;
    struct stat st;
    if (stat(path, &st) != 0) continue;
    bool isDirectory = S_ISDIR(st.st_mode);
    if (!isDirectory && !(S_ISREG(st.st_mode) && isMp3(name))) continue;
    if (index >= first && index < last) addBrowserEntry(name, path, isDirectory);
    ++index;
    if (index >= last) break;
  }
  closedir(dir);
  // Preserve filesystem order. Sorting only a five-item page would be misleading.
}

// ======================= v0.17.2: playlists ===============================
// M3U files in /sdcard/Music_Library/Playlists (same place and format as
// v0.17.1). Entries are written relative to that folder ("../Rock/...");
// on reading, "..", "." and Windows backslashes are resolved here, so the
// files work on a PC too and do not depend on the SD driver supporting "..".
static bool ensurePlaylistDirectory() {
  struct stat st;
  if (stat(PLAYLIST_DIR, &st) == 0 && S_ISDIR(st.st_mode)) return true;
  if (mkdir(PLAYLIST_DIR, 0775) == 0) return true;
  Serial.printf("[PLAYLIST] Could not create %s\n", PLAYLIST_DIR);
  return false;
}

static bool hasM3uExtension(const char *name) {
  size_t n = strlen(name);
  return n > 4 && strcasecmp(name + n - 4, ".m3u") == 0;
}

// Turn one M3U line into an absolute /sdcard path. Returns false for comments
// and blank lines.
static bool playlistLineToPath(const char *lineIn, char *out, size_t cap) {
  char line[PATH_LENGTH + 32];
  strlcpy(line, lineIn, sizeof(line));
  size_t n = strlen(line);
  while (n && (line[n - 1] == '\r' || line[n - 1] == '\n' || line[n - 1] == ' ' || line[n - 1] == '\t')) line[--n] = 0;
  char *p = line;
  if ((unsigned char)p[0] == 0xEF && (unsigned char)p[1] == 0xBB && (unsigned char)p[2] == 0xBF) p += 3;   // UTF-8 BOM
  while (*p == ' ' || *p == '\t') ++p;
  if (!*p || *p == '#') return false;
  for (char *c = p; *c; ++c) if (*c == '\\') *c = '/';
  char joined[PATH_LENGTH * 2];
  const char *ml = strstr(p, "Music_Library/");
  if (p[0] == '/' && strncmp(p, "/sdcard/", 8) == 0) strlcpy(joined, p, sizeof(joined));
  else if (ml) snprintf(joined, sizeof(joined), "/sdcard/%s", ml);        // e.g. E:/Music_Library/... from a PC
  else if (p[0] == '/') snprintf(joined, sizeof(joined), "/sdcard%s", p);
  else snprintf(joined, sizeof(joined), "%s/%s", PLAYLIST_DIR, p);
  // Resolve "." and ".." components.
  char *parts[64]; int count = 0;
  char *save = nullptr;
  for (char *tok = strtok_r(joined, "/", &save); tok && count < 64; tok = strtok_r(nullptr, "/", &save)) {
    if (!strcmp(tok, ".")) continue;
    if (!strcmp(tok, "..")) { if (count) --count; continue; }
    parts[count++] = tok;
  }
  size_t used = 0;
  out[0] = 0;
  for (int i = 0; i < count; ++i) {
    int w = snprintf(out + used, cap - used, "/%s", parts[i]);
    if (w < 0 || (size_t)w >= cap - used) return false;
    used += (size_t)w;
  }
  return used > 0;
}

static bool appendPathToOpenPlaylist(const char *absolutePath) {
  if (!absolutePath || !*absolutePath || !openPlaylistPath[0]) return false;
  FILE *f = fopen(openPlaylistPath, "ab");
  if (!f) { Serial.printf("[PLAYLIST] Cannot append to %s\n", openPlaylistPath); return false; }
  const char *relative = absolutePath;
  const size_t rootLen = strlen(MUSIC_ROOT);
  if (strncmp(absolutePath, MUSIC_ROOT, rootLen) == 0 && absolutePath[rootLen] == '/') {
    relative = absolutePath + rootLen + 1;
    fprintf(f, "../%s\n", relative);
  } else {
    fprintf(f, "%s\n", absolutePath);
  }
  fclose(f);
  return true;
}

// Brief confirmation in the browser heading (restored by the next refresh).
static void flashBrowserHeading(const char *text) {
  if (!browserHeading || !lcd_lvgl_lock(50)) return;
  lv_label_set_text(browserHeading, text);
  lcd_lvgl_unlock();
}

static void noteAdded(size_t added, const char *what) {
  char msg[96];
  snprintf(msg, sizeof(msg), "ADDED %u: %.60s", (unsigned)added, what);
  flashBrowserHeading(msg);
  Serial.printf("[PLAYLIST] Added %u track(s) (%s) to %s\n", (unsigned)added, what, openPlaylistName);
}

static size_t appendSelectedAlbumToPlaylist() {
  // Same matching as loadIndexedAlbumQueue; uses the PSRAM index when present.
  size_t added = 0;
  if (ensureLibraryCache()) {
    // Collect, sort by disc/track like the album queue, then append.
    struct Pick { uint16_t disc, track; uint32_t path; };
    static Pick picks[MAX_TRACKS];
    size_t n = 0;
    for (size_t i = 0; i < indexRecordCount && n < MAX_TRACKS; ++i) {
      const IndexRecord &r = indexRecords[i];
      if (!artistMatchesSelected(poolStr(r.artist))) continue;
      if (strcasecmp(recordAlbum(r), selectedAlbum) != 0) continue;
      picks[n++] = {r.disc, r.track, r.path};
    }
    for (size_t i = 1; i < n; ++i) {
      Pick t = picks[i]; size_t j = i;
      while (j > 0 && (picks[j - 1].disc > t.disc || (picks[j - 1].disc == t.disc && picks[j - 1].track > t.track))) {
        picks[j] = picks[j - 1]; --j;
      }
      picks[j] = t;
    }
    for (size_t i = 0; i < n; ++i) if (appendPathToOpenPlaylist(poolStr(picks[i].path))) ++added;
  }
  return added;
}

static int comparePathNames(const void *a, const void *b) {
  const char *pa = *(const char * const *)a, *pb = *(const char * const *)b;
  const char *na = strrchr(pa, '/'), *nb = strrchr(pb, '/');
  return strcasecmp(na ? na + 1 : pa, nb ? nb + 1 : pb);
}

static size_t appendFolderToPlaylist(const char *folder) {
  // Same order as the folder queue (file name, case-insensitive). Paths are
  // gathered in PSRAM so the play queue (trackPaths) is not disturbed.
  char *pool = (char *)heap_caps_malloc(MAX_TRACKS * PATH_LENGTH, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  const char **list = (const char **)heap_caps_malloc(MAX_TRACKS * sizeof(char *), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!pool || !list) { if (pool) heap_caps_free(pool); if (list) heap_caps_free(list); return 0; }
  size_t n = 0;
  DIR *dir = opendir(folder);
  if (dir) {
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr && n < MAX_TRACKS) {
      if (entry->d_name[0] == '.' || !isMp3(entry->d_name)) continue;
      char *slot = pool + n * PATH_LENGTH;
      int w = snprintf(slot, PATH_LENGTH, "%s/%s", folder, entry->d_name);
      if (w < 0 || w >= (int)PATH_LENGTH) continue;
      list[n] = slot;
      ++n;
    }
    closedir(dir);
  }
  qsort(list, n, sizeof(char *), comparePathNames);
  size_t added = 0;
  for (size_t i = 0; i < n; ++i) if (appendPathToOpenPlaylist(list[i])) ++added;
  heap_caps_free(pool);
  heap_caps_free(list);
  return added;
}

static void displayNameFromPath(const char *path, char *out, size_t cap) {
  const char *base = strrchr(path, '/');
  strlcpy(out, base ? base + 1 : path, cap);
  size_t n = strlen(out);
  if (n > 4 && (hasM3uExtension(out) || isMp3(out))) out[n - 4] = 0;
}

static void populatePlaylistsEntries() {
  browserEntryCount = 0;
  addBrowserEntry("NEW PLAYLIST", "", false);
  browserEntries[0].isAction = true;
  browserEntries[0].trackNumber = PR_NEW;
  if (!ensurePlaylistDirectory()) return;
  DIR *dir = opendir(PLAYLIST_DIR);
  if (!dir) return;
  struct dirent *entry;
  size_t first = browserEntryCount;
  while ((entry = readdir(dir)) != nullptr && browserEntryCount < MAX_BROWSER_ENTRIES) {
    if (entry->d_name[0] == '.' || !hasM3uExtension(entry->d_name)) continue;
    char path[PATH_LENGTH], name[128];
    int w = snprintf(path, sizeof(path), "%s/%s", PLAYLIST_DIR, entry->d_name);
    if (w < 0 || w >= (int)sizeof(path)) continue;
    displayNameFromPath(path, name, sizeof(name));
    addBrowserEntry(name, path, false);
    browserEntries[browserEntryCount - 1].isAction = true;
    browserEntries[browserEntryCount - 1].trackNumber = PR_OPEN;
  }
  closedir(dir);
  // Sort the playlists by name (row 0, NEW PLAYLIST, stays first).
  for (size_t i = first + 1; i < browserEntryCount; ++i) {
    BrowserEntry t = browserEntries[i]; size_t j = i;
    while (j > first && strcasecmp(browserEntries[j - 1].name, t.name) > 0) { browserEntries[j] = browserEntries[j - 1]; --j; }
    browserEntries[j] = t;
  }
}

static void populatePlaylistEntries() {
  browserEntryCount = 0;
  addBrowserEntry("PLAY  ALL", "", false);
  browserEntries[0].isAction = true; browserEntries[0].trackNumber = PR_PLAY;
  addBrowserEntry("ADD TRACKS", "", false);
  browserEntries[1].isAction = true; browserEntries[1].trackNumber = PR_ADD;
  addBrowserEntry(playlistDeleteArmed ? "TAP AGAIN TO DELETE PLAYLIST" : "DELETE PLAYLIST", "", false);
  browserEntries[2].isAction = true; browserEntries[2].trackNumber = PR_DELETE;
  FILE *f = fopen(openPlaylistPath, "rb");
  if (!f) return;
  char line[PATH_LENGTH + 32], path[PATH_LENGTH];
  uint16_t index = 0;
  while (fgets(line, sizeof(line), f) && browserEntryCount < MAX_BROWSER_ENTRIES) {
    if (!playlistLineToPath(line, path, sizeof(path))) continue;
    char name[128];
    displayNameFromPath(path, name, sizeof(name));
    addBrowserEntry(name, path, false);
    BrowserEntry &e = browserEntries[browserEntryCount - 1];
    e.isAction = true; e.trackNumber = PR_TRACK; e.discNumber = index++;
  }
  fclose(f);
}

// Play queue from a playlist; startIndex counts playlist entries (missing
// files are skipped and the start position follows the same entry).
static bool loadPlaylistQueue(const char *path, size_t startIndex) {
  trackCount = 0;
  currentTrack = 0;
  FILE *f = fopen(path, "rb");
  if (!f) { Serial.printf("[PLAYLIST] Cannot open %s\n", path); return false; }
  char line[PATH_LENGTH + 32];
  size_t entry = 0, missing = 0;
  while (fgets(line, sizeof(line), f) && trackCount < MAX_TRACKS) {
    char abs[PATH_LENGTH];
    if (!playlistLineToPath(line, abs, sizeof(abs))) continue;
    struct stat st;
    if (stat(abs, &st) == 0 && S_ISREG(st.st_mode)) {
      if (entry == startIndex) currentTrack = trackCount;
      strlcpy(trackPaths[trackCount], abs, PATH_LENGTH);
      trackNumbers[trackCount] = 0;
      trackDiscNumbers[trackCount] = 0;
      ++trackCount;
    } else {
      ++missing;
      Serial.printf("[PLAYLIST] Missing file skipped: %s\n", abs);
    }
    ++entry;
  }
  fclose(f);
  Serial.printf("[PLAYLIST] Loaded %u track(s) from %s%s\n", (unsigned)trackCount, path,
                missing ? " (some files missing)" : "");
  return trackCount > 0;
}

static bool createNewPlaylist(const char *requestedName) {
  if (!ensurePlaylistDirectory() || !requestedName) return false;
  char safeName[48] = {};
  size_t out = 0;
  for (size_t i = 0; requestedName[i] && out < sizeof(safeName) - 1; ++i) {
    unsigned char c = (unsigned char)requestedName[i];
    if (isalnum(c) || c == ' ' || c == '_' || c == '-') safeName[out++] = (char)c;
  }
  while (out && safeName[out - 1] == ' ') safeName[--out] = 0;
  if (out > 4 && strcasecmp(safeName + out - 4, ".m3u") == 0) safeName[out - 4] = 0;
  if (!safeName[0]) { Serial.println("[PLAYLIST] Name empty after removing unsupported characters."); return false; }
  char path[PATH_LENGTH];
  int n = snprintf(path, sizeof(path), "%s/%s.m3u", PLAYLIST_DIR, safeName);
  if (n < 0 || (size_t)n >= sizeof(path)) return false;
  struct stat st;
  if (stat(path, &st) == 0) { Serial.printf("[PLAYLIST] '%s' already exists; opening it.\n", safeName); }
  else {
    FILE *f = fopen(path, "wb");
    if (!f) { Serial.printf("[PLAYLIST] Cannot create %s\n", path); return false; }
    fprintf(f, "#EXTM3U\n");
    fclose(f);
    Serial.printf("[PLAYLIST] Created %s\n", path);
  }
  strlcpy(openPlaylistPath, path, sizeof(openPlaylistPath));
  strlcpy(openPlaylistName, safeName, sizeof(openPlaylistName));
  return true;
}

// ---- on-screen keyboard for NEW PLAYLIST (created only while in use) ----
static void playlistKeyboardEvent(lv_event_t *event) {
  lv_event_code_t code = lv_event_get_code(event);
  if (code == LV_EVENT_READY) {
    const char *name = playlistNameTextArea ? lv_textarea_get_text(playlistNameTextArea) : "";
    strlcpy(pendingPlaylistName, name, sizeof(pendingPlaylistName));
    pendingPlaylistNameReady = 1;
  } else if (code == LV_EVENT_CANCEL) {
    pendingPlaylistNameReady = -1;
  }
}

static void closePlaylistKeyboard() {
  if (!lcd_lvgl_lock(300)) return;
  if (playlistNameKeyboard) { lv_obj_del(playlistNameKeyboard); playlistNameKeyboard = nullptr; }
  if (playlistNameTextArea) { lv_obj_del(playlistNameTextArea); playlistNameTextArea = nullptr; }
  lcd_lvgl_unlock();
}

static void openPlaylistKeyboard() {
  if (!browserScreen || playlistNameKeyboard || !lcd_lvgl_lock(300)) return;
  playlistNameTextArea = lv_textarea_create(browserScreen);
  if (playlistNameTextArea) {
    lv_obj_set_size(playlistNameTextArea, 260, 42);
    lv_obj_align(playlistNameTextArea, LV_ALIGN_TOP_MID, 0, 40);
    lv_textarea_set_one_line(playlistNameTextArea, true);
    lv_textarea_set_max_length(playlistNameTextArea, 40);
    lv_textarea_set_placeholder_text(playlistNameTextArea, "Playlist name");
    playlistNameKeyboard = lv_keyboard_create(browserScreen);
  }
  if (!playlistNameTextArea || !playlistNameKeyboard) {
    // LVGL memory full: undo and report instead of crashing (see v0.17.1).
    if (playlistNameTextArea) { lv_obj_del(playlistNameTextArea); playlistNameTextArea = nullptr; }
    lcd_lvgl_unlock();
    Serial.println("[PLAYLIST] Not enough LVGL memory for the keyboard.");
    return;
  }
  lv_obj_set_size(playlistNameKeyboard, 330, 220);
  lv_obj_align(playlistNameKeyboard, LV_ALIGN_TOP_MID, 0, 92);
  lv_keyboard_set_textarea(playlistNameKeyboard, playlistNameTextArea);
  lv_obj_add_event_cb(playlistNameKeyboard, playlistKeyboardEvent, LV_EVENT_READY, nullptr);
  lv_obj_add_event_cb(playlistNameKeyboard, playlistKeyboardEvent, LV_EVENT_CANCEL, nullptr);
  lcd_lvgl_unlock();
}

// loop(): keyboard finished (objects are deleted here, outside their own event).
static void servicePlaylistKeyboard() {
  int ready = pendingPlaylistNameReady;
  if (!ready) return;
  pendingPlaylistNameReady = 0;
  closePlaylistKeyboard();
  if (ready > 0 && createNewPlaylist(pendingPlaylistName)) {
    playlistDeleteArmed = false;
    browserView = BROWSER_PLAYLIST;
    browserPage = 0;
  }
  pendingPlaylistName[0] = 0;
  showBrowser();
}

static void startPlaylistPlayback(size_t startIndex) {
  if (loadPlaylistQueue(openPlaylistPath, startIndex)) startSelectedPlayback();
  else flashBrowserHeading("NO PLAYABLE TRACKS");
}

static void handlePlaylistRow(const BrowserEntry &chosen) {
  if (browserView == BROWSER_PLAYLISTS) {
    if (chosen.trackNumber == PR_NEW) { openPlaylistKeyboard(); return; }
    strlcpy(openPlaylistPath, chosen.path, sizeof(openPlaylistPath));
    strlcpy(openPlaylistName, chosen.name, sizeof(openPlaylistName));
    playlistDeleteArmed = false;
    browserView = BROWSER_PLAYLIST;
    browserPage = 0;
    showBrowser();
    return;
  }
  // BROWSER_PLAYLIST
  if (chosen.trackNumber != PR_DELETE) playlistDeleteArmed = false;
  switch (chosen.trackNumber) {
    case PR_PLAY: queueStartRandom = true; startPlaylistPlayback(0); return;
    case PR_TRACK: startPlaylistPlayback(chosen.discNumber); return;
    case PR_ADD:
      playlistAddMode = true;
      browserView = BROWSER_ROOT; browserPage = 0; browserHistoryDepth = 0;
      strlcpy(browsePath, MUSIC_ROOT, sizeof(browsePath));
      Serial.printf("[PLAYLIST] Add mode for %s: browse and tap tracks, PLAY ALBUM or PLAY FOLDER.\n", openPlaylistName);
      showBrowser();
      return;
    case PR_DELETE:
      if (!playlistDeleteArmed) { playlistDeleteArmed = true; refreshBrowserScreen(); return; }
      playlistDeleteArmed = false;
      if (remove(openPlaylistPath) == 0) Serial.printf("[PLAYLIST] Deleted %s\n", openPlaylistPath);
      else Serial.printf("[PLAYLIST] Could not delete %s\n", openPlaylistPath);
      openPlaylistPath[0] = 0;
      browserView = BROWSER_PLAYLISTS; browserPage = 0;
      showBrowser();
      return;
    default: return;
  }
}

static void endPlaylistAddMode() {
  playlistAddMode = false;
  browserView = BROWSER_PLAYLIST;
  browserPage = 0;
  showBrowser();
}
// ======================= end of v0.17.2 playlists =========================

// ----------------------- v0.16.9: Output page ---------------------------
// Rows are action entries; trackNumber holds the row kind, discNumber its
// argument (saved slot or index of the found device on the classic).
enum OutputRowKind : uint16_t { OR_WIRED = 1, OR_BLUETOOTH, OR_SAVED, OR_FIND, OR_FOUND, OR_FORGET };

static void addOutputRow(const char *label, uint16_t kind, uint16_t arg) {
  addBrowserEntry(label, "", false);
  BrowserEntry &e = browserEntries[browserEntryCount - 1];
  e.isAction = true;
  e.trackNumber = kind;
  e.discNumber = arg;
}

static void populateOutputEntries() {
  browserEntryCount = 0;
  wslink::DeviceList d;
  wslink::Report r;
  portENTER_CRITICAL(&linkReportMux);
  d = linkDevices;
  r = linkLastReport;
  portEXIT_CRITICAL(&linkReportMux);
  bool bt = (outputMode == OUTPUT_BLUETOOTH);
  addOutputRow(bt ? "USE  WIRED" : "USING  WIRED", OR_WIRED, 0);
  addOutputRow(bt ? "USING  BLUETOOTH" : "USE  BLUETOOTH", OR_BLUETOOTH, 0);
  char label[96];
  for (int k = 0; k < wslink::MAX_SAVED_SPEAKERS; ++k) {
    const wslink::SpeakerEntry &e = d.saved[k];
    if (!(e.flags & 0x01)) continue;
    const char *state = "";
    if (e.flags & 0x04) {
      if (!linkUp) state = "  - no link";
      else if (r.connState == wslink::CONN_CONNECTED) state = linkStreaming ? "  - playing" : "  - connected";
      else if (r.connState == wslink::CONN_CONNECTING) state = "  - connecting";
      else state = "  - not connected";
    }
    snprintf(label, sizeof(label), "SPEAKER  %.32s%s", e.name[0] ? e.name : "(no name)", state);
    addOutputRow(label, OR_SAVED, (uint16_t)k);
  }
  addOutputRow((d.scanning && r.connState != wslink::CONN_CONNECTED) ? "FIND SPEAKERS  (scanning)" : "FIND SPEAKERS", OR_FIND, 0);
  for (int i = 0; i < d.foundCount && i < wslink::MAX_FOUND_SPEAKERS; ++i) {
    const wslink::SpeakerEntry &e = d.found[i];
    if (!(e.flags & 0x01)) continue;
    snprintf(label, sizeof(label), "FOUND  %.32s  %d dB%s", e.name, (int)e.rssi, (e.flags & 0x02) ? "  (saved)" : "");
    addOutputRow(label, OR_FOUND, (uint16_t)(e.flags >> 3));
  }
  for (int k = 0; k < wslink::MAX_SAVED_SPEAKERS; ++k) {
    const wslink::SpeakerEntry &e = d.saved[k];
    if (!(e.flags & 0x01)) continue;
    snprintf(label, sizeof(label), "FORGET  %.32s", e.name[0] ? e.name : "(no name)");
    addOutputRow(label, OR_FORGET, (uint16_t)k);
  }
}

static void handleOutputRow(const BrowserEntry &chosen) {
  switch (chosen.trackNumber) {
    case OR_WIRED: setOutputMode(OUTPUT_WIRED); break;
    case OR_BLUETOOTH: setOutputMode(OUTPUT_BLUETOOTH); break;
    case OR_SAVED: {
      wslink::Report r;
      portENTER_CRITICAL(&linkReportMux);
      r = linkLastReport;
      portEXIT_CRITICAL(&linkReportMux);
      setOutputMode(OUTPUT_BLUETOOTH);
      bool alreadyActive = linkUp && r.activeSlot == chosen.discNumber && r.connState != wslink::CONN_DISCONNECTED;
      if (alreadyActive) Serial.println("[OUTPUT] That speaker is already the active one.");
      else btSendCommand(wslink::CMD_CONNECT_SAVED, (uint8_t)chosen.discNumber);
      break;
    }
    case OR_FIND: btSendCommand(wslink::CMD_SCAN, 0); break;
    case OR_FOUND:
      setOutputMode(OUTPUT_BLUETOOTH);
      btSendCommand(wslink::CMD_CONNECT_FOUND, (uint8_t)chosen.discNumber);
      break;
    case OR_FORGET: btSendCommand(wslink::CMD_FORGET_SAVED, (uint8_t)chosen.discNumber); break;
    default: break;
  }
}

// loop(): keep the Output page current (device list, connection state).
static void serviceOutputPage() {
  static uint32_t lastSig = 0, lastRefreshMs = 0;
  if (!browserVisible || browserView != BROWSER_OUTPUT) { lastSig = 0; return; }
  wslink::Report r;
  portENTER_CRITICAL(&linkReportMux);
  r = linkLastReport;
  portEXIT_CRITICAL(&linkReportMux);
  uint32_t sig = linkDevicesVersion * 131u + (uint32_t)outputMode * 7u + (linkUp ? 3u : 0u) +
                 (uint32_t)r.connState * 17u + (linkStreaming ? 29u : 0u) + (uint32_t)r.activeSlot * 37u + 1u;
  uint32_t now = millis();
  if (sig != lastSig && now - lastRefreshMs >= 700) {
    lastSig = sig;
    lastRefreshMs = now;
    refreshBrowserScreen();
  }
}

static void populateBrowserEntries() {
  browserEntryCount = 0;
  browserTotalEntries = 0;
  switch (browserView) {
    case BROWSER_ROOT:
    case BROWSER_FOLDERS:
      populateFolderEntries();
      break;
    case BROWSER_COLLECTION_MENU:
      addBrowserEntry("Browse Artists", "", false);
      addBrowserEntry("Browse Albums", "", false);    // v0.18.2
      addBrowserEntry("Browse Songs", "", false);     // v0.18.2
      addBrowserEntry("Browse Folders", "", false);
      break;
    case BROWSER_ALBUM_INDEX: populateLetterIndexEntries(LETTERS_ALBUMS); return;   // v0.18.2
    case BROWSER_SONG_INDEX:  populateLetterIndexEntries(LETTERS_SONGS);  return;   // v0.18.2
    case BROWSER_ARTISTS:
      populateArtistEntries();
      break;
    case BROWSER_ALBUMS:
      populateAlbumEntries();
      break;
    case BROWSER_TRACKS:
      populateTrackEntries();
      break;
    case BROWSER_OUTPUT:
      populateOutputEntries();
      break;
    case BROWSER_PLAYLISTS:
      populatePlaylistsEntries();
      break;
    case BROWSER_PLAYLIST:
      populatePlaylistEntries();
      break;
    default:
      break;
  }
  if (browserView != BROWSER_FOLDERS && browserView != BROWSER_ROOT)
    browserTotalEntries = browserEntryCount;
  else if (browserView == BROWSER_ROOT)
    browserTotalEntries = browserEntryCount;
}

static void refreshBrowserScreen() {
  Serial.printf("[BROWSE] Refresh requested: view=%u path=%s\n", (unsigned)browserView, browsePath);
  populateBrowserEntries();
  if (!browserScreen || !browserList) {
    Serial.println("[BROWSE] ERROR: browser screen objects are not ready.");
    return;
  }
  size_t pageCount = (browserTotalEntries + BROWSER_PAGE_SIZE - 1) / BROWSER_PAGE_SIZE;
  if (pageCount == 0) pageCount = 1;
  if (browserPage >= pageCount) browserPage = pageCount - 1;
  size_t firstEntry = browserPage * BROWSER_PAGE_SIZE;
  if (!lcd_lvgl_lock(500)) {
    Serial.println("[BROWSE] Could not acquire LVGL lock to refresh list.");
    return;
  }
  const char *heading = "SELECT MUSIC";
  switch (browserView) {
    case BROWSER_ROOT:
      if (playlistAddMode) { static char h[80]; snprintf(h, sizeof(h), "ADD TO %.60s", openPlaylistName); heading = h; }
      else heading = "SELECT MUSIC";
      break;
    case BROWSER_PLAYLISTS: heading = "PLAYLISTS"; break;
    case BROWSER_PLAYLIST: heading = openPlaylistName; break;
    case BROWSER_COLLECTION_MENU: heading = MUSIC_LIBRARIES[selectedLibraryIndex].folder; break;
    case BROWSER_FOLDERS: { const char *slash = strrchr(browsePath, '/'); heading = slash ? slash + 1 : browsePath; break; }
    case BROWSER_LETTERS:
      heading = letterMode == LETTERS_ALBUMS ? "BROWSE ALBUMS" : letterMode == LETTERS_SONGS ? "BROWSE SONGS" : "BROWSE ARTISTS";
      break;
    case BROWSER_ALBUM_INDEX: { static char h[32]; snprintf(h, sizeof(h), "ALBUMS  %c", selectedLetter); heading = h; break; }
    case BROWSER_SONG_INDEX:  { static char h[32]; snprintf(h, sizeof(h), "SONGS  %c", selectedLetter); heading = h; break; }
    case BROWSER_ARTISTS: { static char h[32]; snprintf(h, sizeof(h), "ARTISTS  %c", selectedLetter); heading = h; break; }
    case BROWSER_ALBUMS: heading = selectedArtist; break;
    case BROWSER_TRACKS: heading = selectedAlbum; break;
    case BROWSER_OUTPUT: heading = "OUTPUT"; break;
  }
  if (browserHeading) lv_label_set_text(browserHeading, heading);
  if (browserStatusLabel) lv_obj_add_flag(browserStatusLabel, LV_OBJ_FLAG_HIDDEN);
  if (browserProgressBar) lv_obj_add_flag(browserProgressBar, LV_OBJ_FLAG_HIDDEN);
  if (browserRescanButton) {
    if (browserView == BROWSER_ROOT) lv_obj_clear_flag(browserRescanButton, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(browserRescanButton, LV_OBJ_FLAG_HIDDEN);
  }
  bool letters = (browserView == BROWSER_LETTERS);
  if (browserLetterPanel) {
    if (letters) lv_obj_clear_flag(browserLetterPanel, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(browserLetterPanel, LV_OBJ_FLAG_HIDDEN);
  }
  if (letters) {
    lv_obj_add_flag(browserList, LV_OBJ_FLAG_HIDDEN);
    for (size_t i = 0; i < 27; ++i) if (browserLetterButtons[i]) lv_obj_clear_flag(browserLetterButtons[i], LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_clear_flag(browserList, LV_OBJ_FLAG_HIDDEN);
    for (size_t i = 0; i < 27; ++i) if (browserLetterButtons[i]) lv_obj_add_flag(browserLetterButtons[i], LV_OBJ_FLAG_HIDDEN);
  }
  for (size_t row = 0; row < BROWSER_VISIBLE_ROWS; ++row) {
    if (!browserRowButtons[row] || !browserRowLabels[row]) continue;
    size_t entryIndex = browserViewIsPaged(browserView) ? row : (firstEntry + row);
    if (!letters && entryIndex < browserEntryCount) {
      char label[160]; char safeName[128];
      makeDisplayTitle(browserEntries[entryIndex].name, safeName, sizeof(safeName));
      const char *prefix = "";
      if (browserView == BROWSER_ROOT)
        prefix = !browserEntries[entryIndex].isAction ? "LIBRARY  " :
                 browserEntries[entryIndex].trackNumber == RR_OUTPUT ? "OUTPUT  " : "";
      else if (browserView == BROWSER_FOLDERS)
        prefix = browserEntries[entryIndex].isAction ? (playlistAddMode ? "ADD  " : "PLAY  ") :
                 browserEntries[entryIndex].isDirectory ? "FOLDER  " : "TRACK  ";
      else if (browserView == BROWSER_COLLECTION_MENU) prefix = entryIndex < 3 ? "MUSIC  " : "FILES  ";
      else if (browserView == BROWSER_TRACKS && browserEntries[entryIndex].isAction) prefix = playlistAddMode ? "ADD  " : "PLAY  ";
      snprintf(label, sizeof(label), "%s%s", prefix, safeName);
      lv_label_set_text(browserRowLabels[row], label);
      // Library entries retain white text; only the heading uses black text.
      bool libraryChoice = (browserView == BROWSER_ROOT || browserView == BROWSER_COLLECTION_MENU);
      lv_obj_set_style_text_color(browserRowLabels[row],
                                  lv_color_hex(libraryChoice ? uiLibraryEntryText : uiBrowserEntryText), LV_PART_MAIN);
      lv_obj_set_style_text_font(browserRowLabels[row], playerFont(libraryChoice ? uiFontLibraryEntry : uiFontBrowserEntry), LV_PART_MAIN);
      lv_obj_set_style_bg_color(browserRowButtons[row], lv_color_hex(libraryChoice ? uiLibraryEntryBg : uiBrowserEntryBg), LV_PART_MAIN);
      lv_obj_clear_flag(browserRowButtons[row], LV_OBJ_FLAG_HIDDEN);
    } else lv_obj_add_flag(browserRowButtons[row], LV_OBJ_FLAG_HIDDEN);
  }
  if (browserEmptyLabel) {
    const char *emptyText = browserView == BROWSER_ARTISTS ? "No artists for this letter" :
                            browserView == BROWSER_ALBUM_INDEX ? "No albums for this letter" :
                            browserView == BROWSER_SONG_INDEX ? "No songs for this letter" :
                            browserView == BROWSER_ALBUMS ? "No albums found" :
                            browserView == BROWSER_TRACKS ? "No tracks found" : "No folders or MP3 files here";
    lv_label_set_text(browserEmptyLabel, emptyText);
    if (!letters && browserTotalEntries == 0) lv_obj_clear_flag(browserEmptyLabel, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(browserEmptyLabel, LV_OBJ_FLAG_HIDDEN);
  }
  bool showPaging = !letters;
  if (browserPagePrevButton) {
    if (!showPaging) lv_obj_add_flag(browserPagePrevButton, LV_OBJ_FLAG_HIDDEN);
    else { lv_obj_clear_flag(browserPagePrevButton, LV_OBJ_FLAG_HIDDEN); if (browserPage == 0) lv_obj_add_state(browserPagePrevButton, LV_STATE_DISABLED); else lv_obj_clear_state(browserPagePrevButton, LV_STATE_DISABLED); }
  }
  if (browserPageNextButton) {
    if (!showPaging) lv_obj_add_flag(browserPageNextButton, LV_OBJ_FLAG_HIDDEN);
    else { lv_obj_clear_flag(browserPageNextButton, LV_OBJ_FLAG_HIDDEN); if (browserPage + 1 >= pageCount) lv_obj_add_state(browserPageNextButton, LV_STATE_DISABLED); else lv_obj_clear_state(browserPageNextButton, LV_STATE_DISABLED); }
  }
  if (browserPageLabel) {
    if (letters) lv_obj_add_flag(browserPageLabel, LV_OBJ_FLAG_HIDDEN);
    else { char pageText[24]; snprintf(pageText, sizeof(pageText), "%u / %u", (unsigned)(browserPage + 1), (unsigned)pageCount); lv_label_set_text(browserPageLabel, pageText); lv_obj_clear_flag(browserPageLabel, LV_OBJ_FLAG_HIDDEN); }
  }
  lcd_lvgl_unlock();
  Serial.printf("[BROWSE] Page %u/%u, %u entries\n", (unsigned)(browserPage + 1), (unsigned)pageCount, (unsigned)browserEntryCount);
}

static void createBrowserScreen() {
  browserScreen = lv_obj_create(NULL);
  if (!browserScreen) {
    Serial.println("[BROWSE] ERROR: could not create browser screen.");
    return;
  }
  lv_obj_set_style_bg_color(browserScreen, lv_color_hex(COLOR_BROWSER_BACKGROUND), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(browserScreen, LV_OPA_COVER, LV_PART_MAIN);

  // Shared circular-safe header used by the owner, artist, and album screens.
  lv_obj_t *back = lv_btn_create(browserScreen);
  if (back) {
    lv_obj_set_size(back, PlayerUIConfig::BROWSER_BACK_WIDTH, PlayerUIConfig::BROWSER_BACK_HEIGHT);
    lv_obj_align(back, LV_ALIGN_TOP_MID, PlayerUIConfig::BROWSER_BACK_X, PlayerUIConfig::BROWSER_BACK_Y);
    lv_obj_set_style_bg_color(back, lv_color_hex(uiBrowserControlBg), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(back, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(back, 9, LV_PART_MAIN);
    lv_obj_add_event_cb(back, browserBackEvent, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *backText = lv_label_create(back);
    if (backText) { lv_label_set_text(backText, "BACK"); lv_obj_center(backText); lv_obj_set_style_text_color(backText, lv_color_hex(uiBrowserControlText), LV_PART_MAIN); lv_obj_set_style_text_font(backText, playerFont(uiFontBrowserControl), LV_PART_MAIN); }
  }

  lv_obj_t *now = lv_btn_create(browserScreen);
  if (now) {
    lv_obj_set_size(now, PlayerUIConfig::BROWSER_PLAYING_WIDTH, PlayerUIConfig::BROWSER_PLAYING_HEIGHT);
    lv_obj_align(now, LV_ALIGN_TOP_MID, PlayerUIConfig::BROWSER_PLAYING_X, PlayerUIConfig::BROWSER_PLAYING_Y);
    lv_obj_set_style_bg_color(now, lv_color_hex(uiBrowserControlBg), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(now, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(now, 9, LV_PART_MAIN);
    lv_obj_add_event_cb(now, browserNowPlayingEvent, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *nowText = lv_label_create(now);
    if (nowText) { lv_label_set_text(nowText, "PLAYING"); lv_obj_center(nowText); lv_obj_set_style_text_color(nowText, lv_color_hex(uiBrowserControlText), LV_PART_MAIN); lv_obj_set_style_text_font(nowText, playerFont(uiFontBrowserControl), LV_PART_MAIN); }
  }

  browserRescanButton = lv_btn_create(browserScreen);
  if (browserRescanButton) {
    // v0.17.3: bottom centre, below the page counter.
    lv_obj_set_size(browserRescanButton, RESCAN_BOTTOM_WIDTH, RESCAN_BOTTOM_HEIGHT);
    lv_obj_align(browserRescanButton, LV_ALIGN_TOP_MID, 0, RESCAN_BOTTOM_Y);
    lv_obj_set_style_bg_color(browserRescanButton, lv_color_hex(uiBrowserControlBg), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(browserRescanButton, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(browserRescanButton, 8, LV_PART_MAIN);
    lv_obj_add_event_cb(browserRescanButton, browserRescanEvent, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *rescanText = lv_label_create(browserRescanButton);
    browserRescanLabel = rescanText;
    if (rescanText) { lv_label_set_text(rescanText, "RESCAN"); lv_obj_center(rescanText); lv_obj_set_style_text_color(rescanText, lv_color_hex(uiBrowserControlText), LV_PART_MAIN); lv_obj_set_style_text_font(rescanText, playerFont(uiFontBrowserControl), LV_PART_MAIN); }
  }

  lv_obj_t *settingsButton = lv_btn_create(browserScreen);
  if (settingsButton) {
    lv_obj_set_size(settingsButton, PlayerUIConfig::RESCAN_SETTINGS_WIDTH, PlayerUIConfig::RESCAN_SETTINGS_HEIGHT);
    lv_obj_align(settingsButton, LV_ALIGN_TOP_MID, SETTINGS_CENTRE_X, PlayerUIConfig::RESCAN_SETTINGS_Y);
    lv_obj_set_style_bg_color(settingsButton, lv_color_hex(uiBrowserControlBg), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(settingsButton, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(settingsButton, 8, LV_PART_MAIN);
    lv_obj_add_event_cb(settingsButton, settingsOpenEvent, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *settingsText = lv_label_create(settingsButton);
    if (settingsText) {
      lv_label_set_text(settingsText, "SETTINGS");
      lv_obj_center(settingsText);
      lv_obj_set_style_text_color(settingsText, lv_color_hex(uiBrowserControlText), LV_PART_MAIN);
      lv_obj_set_style_text_font(settingsText, playerFont(uiFontBrowserControl), LV_PART_MAIN);
    }
  }

  // v0.17.6: SHUF (left) and RPT (right) beside SETTINGS.
  // v0.17.7: wider (98 px) and the same font as SETTINGS. Centre x = +/-104 leaves a
  // 4 px gap to SETTINGS (102 px) and keeps the outer top corner (x = 153, y = 88)
  // inside the 360 px round screen (edge at x = 154 for that row).
  for (int k = 0; k < 2; ++k) {
    lv_obj_t *b = lv_btn_create(browserScreen);
    if (!b) continue;
    lv_obj_set_size(b, SHUF_RPT_WIDTH, PlayerUIConfig::RESCAN_SETTINGS_HEIGHT);
    lv_obj_align(b, LV_ALIGN_TOP_MID, k == 0 ? -SHUF_RPT_CENTRE_X : SHUF_RPT_CENTRE_X, PlayerUIConfig::RESCAN_SETTINGS_Y);
    lv_obj_set_style_radius(b, 8, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(b, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(b, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(b, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(b, playbackButtonEvent, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)(k == 0 ? ACTION_SHUFFLE : ACTION_REPEAT));
    lv_obj_t *l = lv_label_create(b);
    if (l) {
      lv_obj_center(l);
      lv_obj_set_style_text_font(l, playerFont(uiFontBrowserControl), LV_PART_MAIN);
    }
    if (k == 0) { shuffleButton = b; shuffleLabel = l; } else { repeatButton = b; repeatLabel = l; }
  }
  applyShuffleRepeatButtonsLocked();   // v0.17.7: correct text from the start (lock already held)

  browserHeading = lv_label_create(browserScreen);
  if (browserHeading) {
    lv_label_set_text(browserHeading, "SELECT MUSIC");
    lv_obj_set_width(browserHeading, 180);
    lv_label_set_long_mode(browserHeading, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(browserHeading, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(browserHeading, lv_color_hex(uiLibraryHeaderText), LV_PART_MAIN);
    lv_obj_set_style_bg_color(browserHeading, lv_color_hex(uiLibraryHeaderBg), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(browserHeading, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_pad_hor(browserHeading, 6, LV_PART_MAIN);
    lv_obj_set_style_pad_ver(browserHeading, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(browserHeading, 6, LV_PART_MAIN);
    lv_obj_set_style_text_font(browserHeading, playerFont(uiFontLibraryHeader), LV_PART_MAIN);
    lv_obj_align(browserHeading, LV_ALIGN_TOP_MID, 0, 20);
  }

  browserList = lv_obj_create(browserScreen);
  if (!browserList) {
    Serial.println("[BROWSE] ERROR: could not create browser list.");
    return;
  }
  lv_obj_set_size(browserList, PlayerUIConfig::LIBRARY_LIST_WIDTH, PlayerUIConfig::LIBRARY_LIST_HEIGHT);
  lv_obj_align(browserList, LV_ALIGN_TOP_MID, 0, 116);
  lv_obj_set_style_bg_opa(browserList, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(browserList, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(browserList, 4, LV_PART_MAIN);
  lv_obj_clear_flag(browserList, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(browserList, LV_SCROLLBAR_MODE_OFF);

  // Five reusable rows keep LVGL memory use small; PREV/NEXT pages the entries.
  for (size_t i = 0; i < BROWSER_VISIBLE_ROWS; ++i) {
    lv_obj_t *button = lv_btn_create(browserList);
    if (!button) {
      Serial.printf("[BROWSE] Could not allocate row button %u; stopping row pool at this size.\n",
                    (unsigned)i);
      break;
    }
    browserRowButtons[i] = button;
    lv_obj_set_size(button, PlayerUIConfig::LIBRARY_BUTTON_WIDTH, PlayerUIConfig::LIBRARY_BUTTON_HEIGHT);
    lv_obj_set_pos(button, PlayerUIConfig::LIBRARY_BUTTON_X, (lv_coord_t)(i * PlayerUIConfig::LIBRARY_BUTTON_ROW_STEP));
    lv_obj_set_style_bg_color(button, lv_color_hex(uiBrowserControlBg), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(button, lv_color_hex(COLOR_BROWSER_ENTRY_PRESSED_BG), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_radius(button, 8, LV_PART_MAIN);
    lv_obj_add_flag(button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(button, browserEntryEvent, LV_EVENT_CLICKED, (void *)(intptr_t)(i + 1));
    lv_obj_t *text = lv_label_create(button);
    browserRowLabels[i] = text;
    if (text) {
      lv_obj_set_width(text, PlayerUIConfig::LIBRARY_BUTTON_LABEL_WIDTH);
      lv_label_set_long_mode(text, LV_LABEL_LONG_DOT);
      lv_obj_set_style_text_color(text, lv_color_hex(uiBrowserControlText), LV_PART_MAIN);
      lv_obj_set_style_text_font(text, playerFont(uiFontBrowserControl), LV_PART_MAIN);
      lv_obj_align(text, LV_ALIGN_LEFT_MID, 8, 0);
    }
  }

  browserEmptyLabel = lv_label_create(browserList);
  if (browserEmptyLabel) {
    lv_label_set_text(browserEmptyLabel, "No folders or MP3 files here");
    lv_obj_set_style_text_color(browserEmptyLabel, lv_color_hex(COLOR_BROWSER_EMPTY_TEXT), LV_PART_MAIN);
    lv_obj_set_pos(browserEmptyLabel, 8, 12);
    lv_obj_add_flag(browserEmptyLabel, LV_OBJ_FLAG_HIDDEN);
  }
  browserPagePrevButton = lv_btn_create(browserScreen);
  if (browserPagePrevButton) {
    lv_obj_set_size(browserPagePrevButton, PlayerUIConfig::BROWSER_PAGE_BUTTON_WIDTH, PlayerUIConfig::BROWSER_PAGE_BUTTON_HEIGHT); lv_obj_set_pos(browserPagePrevButton, PlayerUIConfig::BROWSER_PAGE_PREV_X, PlayerUIConfig::BROWSER_PAGE_BUTTON_Y);
    lv_obj_set_style_bg_color(browserPagePrevButton, lv_color_hex(uiNavButtonBg), LV_PART_MAIN);
    lv_obj_set_style_bg_color(browserPagePrevButton, lv_color_hex(uiNavDisabledBg), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_bg_opa(browserPagePrevButton, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_bg_opa(browserPagePrevButton, LV_OPA_COVER, LV_PART_MAIN); lv_obj_set_style_radius(browserPagePrevButton, 8, LV_PART_MAIN);
    lv_obj_add_event_cb(browserPagePrevButton, browserPagePrevEvent, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *t = lv_label_create(browserPagePrevButton); if (t) { lv_label_set_text(t, "PREV"); lv_obj_center(t); lv_obj_set_style_text_color(t, lv_color_hex(uiNavButtonText), LV_PART_MAIN); lv_obj_set_style_text_color(t, lv_color_hex(uiNavDisabledText), LV_PART_MAIN | LV_STATE_DISABLED); lv_obj_set_style_text_font(t, playerFont(uiFontNavButton), LV_PART_MAIN); }
  }
  browserPageLabel = lv_label_create(browserScreen);
  if (browserPageLabel) {
    lv_obj_set_style_text_color(browserPageLabel, lv_color_hex(COLOR_BROWSER_PAGE_TEXT), LV_PART_MAIN);
    lv_obj_set_width(browserPageLabel, 80);
    lv_obj_set_style_text_align(browserPageLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(browserPageLabel, LV_ALIGN_TOP_MID, 0, 301);
  }
  browserPageNextButton = lv_btn_create(browserScreen);
  if (browserPageNextButton) {
    lv_obj_set_size(browserPageNextButton, PlayerUIConfig::BROWSER_PAGE_BUTTON_WIDTH, PlayerUIConfig::BROWSER_PAGE_BUTTON_HEIGHT); lv_obj_set_pos(browserPageNextButton, PlayerUIConfig::BROWSER_PAGE_NEXT_X, PlayerUIConfig::BROWSER_PAGE_BUTTON_Y);
    lv_obj_set_style_bg_color(browserPageNextButton, lv_color_hex(uiNavButtonBg), LV_PART_MAIN);
    lv_obj_set_style_bg_color(browserPageNextButton, lv_color_hex(uiNavDisabledBg), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_bg_opa(browserPageNextButton, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_bg_opa(browserPageNextButton, LV_OPA_COVER, LV_PART_MAIN); lv_obj_set_style_radius(browserPageNextButton, 8, LV_PART_MAIN);
    lv_obj_add_event_cb(browserPageNextButton, browserPageNextEvent, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *t = lv_label_create(browserPageNextButton); if (t) { lv_label_set_text(t, "NEXT"); lv_obj_center(t); lv_obj_set_style_text_color(t, lv_color_hex(uiNavButtonText), LV_PART_MAIN); lv_obj_set_style_text_color(t, lv_color_hex(uiNavDisabledText), LV_PART_MAIN | LV_STATE_DISABLED); lv_obj_set_style_text_font(t, playerFont(uiFontNavButton), LV_PART_MAIN); }
  }

  browserLetterPanel = lv_obj_create(browserScreen);
  if (browserLetterPanel) {
    lv_obj_set_size(browserLetterPanel, PlayerUIConfig::ALPHABET_PANEL_WIDTH, PlayerUIConfig::ALPHABET_PANEL_HEIGHT);
    lv_obj_align(browserLetterPanel, LV_ALIGN_TOP_MID, 0, PlayerUIConfig::ALPHABET_PANEL_Y);
    lv_obj_set_style_bg_opa(browserLetterPanel, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(browserLetterPanel, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(browserLetterPanel, 0, LV_PART_MAIN);
    lv_obj_clear_flag(browserLetterPanel, LV_OBJ_FLAG_SCROLLABLE);
    for (int i = 0; i < 27; ++i) {
      char letter[2] = { i < 26 ? (char)('A' + i) : '#', 0 };
      lv_obj_t *b = lv_btn_create(browserLetterPanel);
      browserLetterButtons[i] = b;
      if (!b) continue;
      int col = i % 6, row = i / 6;
      lv_obj_set_size(b, PlayerUIConfig::ALPHABET_BUTTON_WIDTH, PlayerUIConfig::ALPHABET_BUTTON_HEIGHT);
      lv_obj_set_pos(b, PlayerUIConfig::ALPHABET_BUTTON_X + col * PlayerUIConfig::ALPHABET_COLUMN_STEP, PlayerUIConfig::ALPHABET_BUTTON_Y + row * PlayerUIConfig::ALPHABET_ROW_STEP);
      lv_obj_set_style_bg_color(b, lv_color_hex(uiBrowserControlBg), LV_PART_MAIN);
      lv_obj_set_style_bg_opa(b, LV_OPA_COVER, LV_PART_MAIN);
      lv_obj_set_style_radius(b, 6, LV_PART_MAIN);
      lv_obj_add_event_cb(b, browserLetterEvent, LV_EVENT_CLICKED, (void *)(intptr_t)(i < 26 ? 'A' + i : '#'));
      lv_obj_t *t = lv_label_create(b);
      if (t) { lv_label_set_text(t, letter); lv_obj_center(t); lv_obj_set_style_text_color(t, lv_color_hex(uiBrowserControlText), LV_PART_MAIN); lv_obj_set_style_text_font(t, playerFont(uiFontBrowserControl), LV_PART_MAIN); }
    }
    lv_obj_add_flag(browserLetterPanel, LV_OBJ_FLAG_HIDDEN);
  }

  browserStatusLabel = lv_label_create(browserScreen);
  if (browserStatusLabel) {
    lv_obj_set_width(browserStatusLabel, 260);
    lv_obj_set_style_text_align(browserStatusLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(browserStatusLabel, lv_color_hex(uiBrowserControlText), LV_PART_MAIN);
    lv_obj_set_style_text_font(browserStatusLabel, playerFont(uiFontBrowserControl), LV_PART_MAIN);
    lv_obj_align(browserStatusLabel, LV_ALIGN_TOP_MID, 0, 132);
    lv_label_set_text(browserStatusLabel, "Preparing library scan...");
    lv_obj_add_flag(browserStatusLabel, LV_OBJ_FLAG_HIDDEN);
  }
  browserProgressBar = lv_bar_create(browserScreen);
  if (browserProgressBar) {
    lv_obj_set_size(browserProgressBar, 240, 12);
    lv_obj_align(browserProgressBar, LV_ALIGN_TOP_MID, 0, 190);
    lv_bar_set_range(browserProgressBar, 0, 100);
    lv_bar_set_value(browserProgressBar, 0, LV_ANIM_OFF);
    lv_obj_add_flag(browserProgressBar, LV_OBJ_FLAG_HIDDEN);
  }
}

static void showBrowser() {
  browserVisible = true;
  settingsOpen = false;
  refreshBrowserScreen();
  if (browserScreen && lcd_lvgl_lock(100)) { lv_scr_load(browserScreen); lcd_lvgl_unlock(); }
}

static void showPlayback() {
  browserVisible = false;
  settingsOpen = false;
  if (playbackScreen && lcd_lvgl_lock(100)) { lv_scr_load(playbackScreen); lcd_lvgl_unlock(); }
}

static void loadAlbumQueue(const char *folder, const char *selectedPath) {
  trackCount = 0;
  DIR *dir = opendir(folder);
  if (!dir) { Serial.printf("[BROWSE] Cannot open album folder: %s\n", folder); return; }
  struct dirent *entry;
  while ((entry = readdir(dir)) != nullptr && trackCount < MAX_TRACKS) {
    if (entry->d_name[0] == '.' || !isMp3(entry->d_name)) continue;
    char path[PATH_LENGTH];
    int n = snprintf(path, sizeof(path), "%s/%s", folder, entry->d_name);
    if (n < 0 || (size_t)n >= sizeof(path)) continue;
    strlcpy(trackPaths[trackCount], path, PATH_LENGTH);
    ++trackCount;
  }
  closedir(dir);
  for (size_t i = 0; i < trackCount; ++i) for (size_t j = i + 1; j < trackCount; ++j)
    if (strcasecmp(strrchr(trackPaths[i], '/') + 1, strrchr(trackPaths[j], '/') + 1) > 0) {
      char temp[PATH_LENGTH]; strlcpy(temp, trackPaths[i], sizeof(temp));
      strlcpy(trackPaths[i], trackPaths[j], PATH_LENGTH); strlcpy(trackPaths[j], temp, PATH_LENGTH);
    }
  currentTrack = 0;
  for (size_t i = 0; i < trackCount; ++i) if (!strcmp(trackPaths[i], selectedPath)) { currentTrack = i; break; }
  Serial.printf("[BROWSE] Loaded %u MP3 track(s) from %s\n", (unsigned)trackCount, folder);
}

static void loadIndexedAlbumQueue(const char *selectedPath) {
  trackCount = 0;
  if (ensureLibraryCache()) {
    for (size_t n = 0; n < indexRecordCount && trackCount < MAX_TRACKS; ++n) {
      const IndexRecord &r = indexRecords[n];
      if (!artistMatchesSelected(poolStr(r.artist))) continue;
      if (strcasecmp(recordAlbum(r), selectedAlbum) != 0) continue;
      strlcpy(trackPaths[trackCount], poolStr(r.path), PATH_LENGTH);
      trackNumbers[trackCount] = r.track;
      trackDiscNumbers[trackCount] = r.disc;
      ++trackCount;
    }
  } else {
  char indexPath[PATH_LENGTH];
  makeLibraryIndexPath(selectedLibraryIndex, indexPath, sizeof(indexPath));
  FILE *f = fopen(indexPath, "rb");
  if (!f) { Serial.printf("[PLAY] Cannot open index: %s\n", indexPath); return; }

  char line[1024];
  while (fgets(line, sizeof(line), f) && trackCount < MAX_TRACKS) {
    if (line[0] == '#') continue;
    char *fields[15] = {};
    char *cursor = line;
    for (size_t i = 0; i < 15; ++i) {
      fields[i] = cursor;
      char *tab = strchr(cursor, '\t');
      if (!tab) break;
      *tab = 0;
      cursor = tab + 1;
    }

    const char *artist = (fields[2][0] && strcasecmp(fields[2], "Unknown artist") != 0)
                           ? fields[2] : fields[4];
    if (!artistMatchesSelected(artist)) continue;
    const char *album = fields[3][0] ? fields[3] : "Unknown album";
    if (strcasecmp(album, selectedAlbum) != 0) continue;

    strlcpy(trackPaths[trackCount], fields[0], PATH_LENGTH);
    trackNumbers[trackCount] = (uint16_t)strtoul(fields[5], nullptr, 10);
    trackDiscNumbers[trackCount] = (uint16_t)strtoul(fields[6], nullptr, 10);
    ++trackCount;
  }
  fclose(f);
  }  // end of SD-card fallback

  for (size_t i = 0; i < trackCount; ++i) {
    for (size_t j = i + 1; j < trackCount; ++j) {
      bool swap = trackDiscNumbers[i] > trackDiscNumbers[j] ||
        (trackDiscNumbers[i] == trackDiscNumbers[j] && trackNumbers[i] > trackNumbers[j]);
      if (swap) {
        char temp[PATH_LENGTH]; strlcpy(temp, trackPaths[i], sizeof(temp));
        strlcpy(trackPaths[i], trackPaths[j], PATH_LENGTH); strlcpy(trackPaths[j], temp, PATH_LENGTH);
        uint16_t n = trackNumbers[i]; trackNumbers[i] = trackNumbers[j]; trackNumbers[j] = n;
        n = trackDiscNumbers[i]; trackDiscNumbers[i] = trackDiscNumbers[j]; trackDiscNumbers[j] = n;
      }
    }
  }
  currentTrack = 0;
  if (selectedPath && selectedPath[0])
    for (size_t i = 0; i < trackCount; ++i)
      if (!strcmp(trackPaths[i], selectedPath)) { currentTrack = i; break; }
  Serial.printf("[PLAY] Loaded %u indexed track(s) for %s / %s\n",
                (unsigned)trackCount, selectedArtist, selectedAlbum);
}

static void startSelectedPlaybackLocked() {
  if (!trackCount) return;
  PlayerLock lock;
  btFlushQueuedAudio();
  if (shuffleOn && queueStartRandom && trackCount > 1) currentTrack = esp_random() % trackCount;   // v0.17.5
  queueStartRandom = false;
  rebuildPlayOrder();
  if (!playerStarted) {
    if (player.begin((int)currentTrack)) {
      playerStarted = true; pausedByUser = false; trackChanged = true;
      Serial.println("[PLAY] Playback started from browser selection.");
    }
  } else {
    Stream *stream = selectTrack((int)currentTrack);
    if (stream && player.setStream(stream)) { player.setActive(true); pausedByUser = false; playerStarted = true; trackChanged = true; }
  }
}

static void startSelectedPlayback() {
  startSelectedPlaybackLocked();   // player calls under playerMutex
  showPlayback();                  // UI work without holding the mutex
  updateMusicScreen();
}

static bool pathIsSelectedLibrary(const char *path, size_t &libraryIndex) {
  for (size_t i = 0; i < MUSIC_LIBRARY_COUNT; ++i) {
    char expected[PATH_LENGTH];
    snprintf(expected, sizeof(expected), "%s/%s", MUSIC_ROOT, MUSIC_LIBRARIES[i].folder);
    if (strcmp(path, expected) == 0) { libraryIndex = i; return true; }
  }
  return false;
}

static void handleBrowserSelection() {
  int letter = pendingBrowserLetter;
  if (letter) {
    pendingBrowserLetter = 0;
    selectedLetter = (char)letter;
    browserView = letterMode == LETTERS_ALBUMS ? BROWSER_ALBUM_INDEX :      // v0.18.2
                  letterMode == LETTERS_SONGS ? BROWSER_SONG_INDEX : BROWSER_ARTISTS;
    browserPage = 0;
    showBrowser();
    return;
  }
  int selection = pendingBrowserSelection;
  if (selection == -1) return;
  pendingBrowserSelection = -1;
  if (playlistNameKeyboard) {                 // v0.17.2: any other tap closes the keyboard
    closePlaylistKeyboard();
    pendingPlaylistNameReady = 0;
  }
  if (selection == -4) {
    if (playlistAddMode) { endPlaylistAddMode(); return; }      // v0.17.2
    if (playerStarted) showPlayback();
    return;
  }
  if (selection == -7) {                                        // v0.17.2: volume line tapped
    playlistAddMode = false;
    browserView = BROWSER_OUTPUT; browserPage = 0;
    showBrowser();
    return;
  }
  if (selection == -2) { showBrowser(); return; }
  if (selection == -3) {
    if (playlistAddMode && browserView == BROWSER_ROOT) { endPlaylistAddMode(); return; }   // v0.17.2
    browserPage = 0;
    if (browserView == BROWSER_PLAYLIST) { playlistDeleteArmed = false; browserView = BROWSER_PLAYLISTS; showBrowser(); return; }
    if (browserView == BROWSER_PLAYLISTS) { browserView = BROWSER_ROOT; showBrowser(); return; }
    if (browserView == BROWSER_COLLECTION_MENU) {
      browserView = BROWSER_ROOT; browsePath[0] = 0; strlcpy(browsePath, MUSIC_ROOT, sizeof(browsePath)); browserHistoryDepth = 0;
    } else if (browserView == BROWSER_LETTERS) browserView = BROWSER_COLLECTION_MENU;
    else if (browserView == BROWSER_ARTISTS) browserView = BROWSER_LETTERS;
    else if (browserView == BROWSER_ALBUM_INDEX || browserView == BROWSER_SONG_INDEX) browserView = BROWSER_LETTERS;   // v0.18.2
    else if (browserView == BROWSER_ALBUMS) browserView = BROWSER_ARTISTS;
    else if (browserView == BROWSER_TRACKS) {
      if (tracksFromAlbumIndex) { browserView = BROWSER_ALBUM_INDEX; browserPage = albumIndexPage; }   // v0.18.2
      else browserView = BROWSER_ALBUMS;
    }
    else if (browserView == BROWSER_OUTPUT) browserView = BROWSER_ROOT;
    else if (browserView == BROWSER_FOLDERS) {
      char libraryRoot[PATH_LENGTH];
      snprintf(libraryRoot, sizeof(libraryRoot), "%s/%s", MUSIC_ROOT, MUSIC_LIBRARIES[selectedLibraryIndex].folder);
      if (strcmp(browsePath, libraryRoot) == 0) browserView = BROWSER_COLLECTION_MENU;
      else if (browserHistoryDepth > 0) {
        --browserHistoryDepth;
        strlcpy(browsePath, browserHistory[browserHistoryDepth].path, sizeof(browsePath));
        browserPage = browserHistory[browserHistoryDepth].page;
      } else {
        char *slash = strrchr(browsePath, '/');
        if (slash) *slash = 0;
      }
    }
    showBrowser(); return;
  }
  if (selection == -5 || selection == -6) {
    size_t pages = (browserTotalEntries + BROWSER_PAGE_SIZE - 1) / BROWSER_PAGE_SIZE;
    if (selection == -5 && browserPage + 1 < pages) ++browserPage;
    if (selection == -6 && browserPage > 0) --browserPage;
    refreshBrowserScreen(); return;
  }
  if (selection < 0) return;
  size_t availableEntries = browserViewIsPaged(browserView) ? browserTotalEntries : browserEntryCount;
  if ((size_t)selection >= availableEntries) return;
  if (browserView == BROWSER_ALBUM_INDEX || browserView == BROWSER_SONG_INDEX) {   // v0.18.2
    handleLetterIndexSelection((size_t)selection);
    return;
  }
  BrowserEntry chosen;
  if (browserView == BROWSER_FOLDERS && !isLibraryRoot(browsePath) && folderHasPlayAll && selection == 0) {
    memset(&chosen, 0, sizeof(chosen));            // v0.16.8: PLAY FOLDER
    strlcpy(chosen.name, "FOLDER", sizeof(chosen.name));
    strlcpy(chosen.path, browsePath, sizeof(chosen.path));
    chosen.isAction = true;
  } else if (browserView == BROWSER_FOLDERS && !isLibraryRoot(browsePath)) {
    if (!readFolderEntryAt(browsePath, (size_t)selection - (folderHasPlayAll ? 1 : 0), chosen)) {
      Serial.printf("[BROWSE] Could not retrieve folder entry %d from SD card.\n", selection);
      return;
    }
  } else {
    if ((size_t)selection >= browserEntryCount) return;
    chosen = browserEntries[selection];
  }
  if (browserView == BROWSER_ROOT) {
    if (chosen.isAction && chosen.trackNumber == RR_PLAYLISTS) {   // v0.17.2
      if (playlistAddMode) { endPlaylistAddMode(); return; }
      browserView = BROWSER_PLAYLISTS;
      browserPage = 0;
      showBrowser();
      return;
    }
    if (chosen.isAction) {                          // v0.16.9: OUTPUT row opens the Output page
      browserView = BROWSER_OUTPUT;
      browserPage = 0;
      showBrowser();
      return;
    }
    size_t idx;
    if (pathIsSelectedLibrary(chosen.path, idx)) {
      selectedLibraryIndex = idx;
      browserView = BROWSER_COLLECTION_MENU;
      browserPage = 0;
      browserHistoryDepth = 0;
      showBrowser();
    }
    return;
  }
  if (browserView == BROWSER_COLLECTION_MENU) {
    if (selection <= 2) {                                       // v0.18.2: Artists / Albums / Songs
      letterMode = selection == 1 ? LETTERS_ALBUMS : selection == 2 ? LETTERS_SONGS : LETTERS_ARTISTS;
      browserView = BROWSER_LETTERS;
    } else {
      browserView = BROWSER_FOLDERS;
      snprintf(browsePath, sizeof(browsePath), "%s/%s", MUSIC_ROOT, MUSIC_LIBRARIES[selectedLibraryIndex].folder);
      browserHistoryDepth = 0;
    }
    browserPage = 0; showBrowser(); return;
  }
  if (browserView == BROWSER_LETTERS) return;
  if (browserView == BROWSER_OUTPUT) {               // v0.16.9
    handleOutputRow(chosen);
    refreshBrowserScreen();
    return;
  }
  if (browserView == BROWSER_PLAYLISTS || browserView == BROWSER_PLAYLIST) {   // v0.17.2
    handlePlaylistRow(chosen);
    return;
  }
  if (browserView == BROWSER_ARTISTS) {
    strlcpy(selectedArtist, chosen.name, sizeof(selectedArtist));
    browserView = BROWSER_ALBUMS; browserPage = 0; showBrowser(); return;
  }
  if (browserView == BROWSER_ALBUMS) {
    strlcpy(selectedAlbum, chosen.name, sizeof(selectedAlbum));
    tracksFromAlbumIndex = false;                               // v0.18.2
    browserView = BROWSER_TRACKS; browserPage = 0; showBrowser(); return;
  }
  if (browserView == BROWSER_TRACKS) {
    if (playlistAddMode) {                                       // v0.17.2
      if (chosen.isAction) noteAdded(appendSelectedAlbumToPlaylist(), selectedAlbum);
      else noteAdded(appendPathToOpenPlaylist(chosen.path) ? 1 : 0, chosen.name);
      return;
    }
    if (chosen.isAction) { loadIndexedAlbumQueue(nullptr); queueStartRandom = true; }
    else loadIndexedAlbumQueue(chosen.path);
    startSelectedPlayback();
    return;
  }
  if (playlistAddMode && browserView == BROWSER_FOLDERS && !chosen.isDirectory) {   // v0.17.2
    if (chosen.isAction) {
      const char *slash = strrchr(browsePath, '/');
      noteAdded(appendFolderToPlaylist(browsePath), slash ? slash + 1 : browsePath);
    } else {
      noteAdded(appendPathToOpenPlaylist(chosen.path) ? 1 : 0, chosen.name);
    }
    return;
  }
  if (browserView == BROWSER_FOLDERS && chosen.isAction) {   // v0.16.8: PLAY FOLDER
    loadAlbumQueue(browsePath, "");
    queueStartRandom = true;
    startSelectedPlayback();
    return;
  }
  if (browserView == BROWSER_FOLDERS && chosen.isDirectory) {
    if (browserHistoryDepth < MAX_BROWSER_DEPTH) {
      strlcpy(browserHistory[browserHistoryDepth].path, browsePath, sizeof(browserHistory[browserHistoryDepth].path));
      browserHistory[browserHistoryDepth].page = browserPage;
      ++browserHistoryDepth;
    }
    strlcpy(browsePath, chosen.path, sizeof(browsePath));
    browserPage = 0; showBrowser(); return;
  }
  if (browserView == BROWSER_FOLDERS && !chosen.isDirectory) {
    char folder[PATH_LENGTH]; strlcpy(folder, chosen.path, sizeof(folder));
    char *slash = strrchr(folder, '/'); if (!slash) return; *slash = 0;
    loadAlbumQueue(folder, chosen.path);
    startSelectedPlayback();
  }
}

// --------------------------- LVGL screen -------------------------------
static void createMusicScreen() {
  playbackScreen = lv_obj_create(NULL);
  lv_obj_t *screen = playbackScreen;
  lv_obj_set_style_bg_color(screen, lv_color_hex(uiPlaybackBackground), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

  constexpr int EQ_BAR_WIDTH = 5;
  constexpr int EQ_BAR_GAP = 2;
  constexpr int EQ_TOTAL_WIDTH = 32 * EQ_BAR_WIDTH + 31 * EQ_BAR_GAP;
  constexpr int EQ_LEFT = (360 - EQ_TOTAL_WIDTH) / 2;
  constexpr int EQ_BASELINE = 96;

  lv_obj_t *browseButton = lv_btn_create(screen);
  lv_obj_set_size(browseButton, 100, 34);
  lv_obj_align(browseButton, LV_ALIGN_TOP_RIGHT, -42, 194);
  lv_obj_set_style_bg_color(browseButton, lv_color_hex(COLOR_PLAYBACK_BROWSE_BG), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(browseButton, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_radius(browseButton, 9, LV_PART_MAIN);
  lv_obj_add_event_cb(browseButton, browseButtonEvent, LV_EVENT_CLICKED, nullptr);
  lv_obj_t *browseText = lv_label_create(browseButton); lv_label_set_text(browseText, "BROWSE"); lv_obj_center(browseText);

  // v0.18.3: volume arc along the top edge, display only (not touchable).
  volumeArc = lv_arc_create(screen);
  lv_obj_set_size(volumeArc, VOLUME_ARC_DIAMETER, VOLUME_ARC_DIAMETER);
  lv_obj_center(volumeArc);
  lv_arc_set_mode(volumeArc, LV_ARC_MODE_NORMAL);
  lv_arc_set_bg_angles(volumeArc, VOLUME_ARC_START, VOLUME_ARC_END);
  lv_arc_set_rotation(volumeArc, 0);
  lv_arc_set_range(volumeArc, 0, 100);
  lv_arc_set_value(volumeArc, 45);
  lv_obj_clear_flag(volumeArc, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(volumeArc, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_opa(volumeArc, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(volumeArc, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(volumeArc, 0, LV_PART_MAIN);
  lv_obj_set_style_arc_width(volumeArc, VOLUME_ARC_WIDTH, LV_PART_MAIN);
  lv_obj_set_style_arc_color(volumeArc, lv_color_hex(VOLUME_ARC_TRACK), LV_PART_MAIN);
  lv_obj_set_style_arc_rounded(volumeArc, true, LV_PART_MAIN);
  lv_obj_set_style_arc_width(volumeArc, VOLUME_ARC_WIDTH, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(volumeArc, lv_color_hex(VOLUME_ARC_FILL), LV_PART_INDICATOR);
  lv_obj_set_style_arc_rounded(volumeArc, true, LV_PART_INDICATOR);
  lv_obj_set_style_bg_opa(volumeArc, LV_OPA_TRANSP, LV_PART_KNOB);   // no knob
  lv_obj_set_style_pad_all(volumeArc, 0, LV_PART_KNOB);

  volumeLabel = lv_label_create(screen);
  lv_label_set_text(volumeLabel, "45%");
  lv_obj_set_style_text_color(volumeLabel, lv_color_hex(COLOR_PLAYBACK_VOLUME_TEXT), LV_PART_MAIN);
  lv_obj_set_style_text_font(volumeLabel, &lv_font_montserrat_24, LV_PART_MAIN);   // v0.18.3: was 18
  lv_obj_align(volumeLabel, LV_ALIGN_TOP_MID, 0, VOLUME_LABEL_Y);
  // v0.17.2: tap the volume line to open the Output page.
  lv_obj_add_flag(volumeLabel, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_ext_click_area(volumeLabel, 16);
  lv_obj_add_event_cb(volumeLabel, volumeLineEvent, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *baseline = lv_obj_create(screen);
  lv_obj_remove_style_all(baseline);
  lv_obj_set_size(baseline, EQ_TOTAL_WIDTH + 8, 2);
  lv_obj_set_pos(baseline, EQ_LEFT - 4, EQ_BASELINE - 1);
  lv_obj_set_style_bg_color(baseline, lv_color_hex(C_EQ_BASELINE), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(baseline, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(baseline, LV_OBJ_FLAG_SCROLLABLE);

  static const uint32_t eqPalette[8] = {
    C_EQ_PURPLE_1, C_EQ_PURPLE_2, C_EQ_PURPLE_3, C_EQ_BLUE_1,
    C_EQ_BLUE_2, C_EQ_BLUE_3, C_EQ_CYAN, C_EQ_GREEN
  };

  for (int i = 0; i < 32; ++i) {
    spectrumColors[i] = lv_color_hex(eqPalette[(i * 8) / 32]);
    spectrumBarHalf[i] = 1;    // same resting look as v0.16.7b (2 px tall bar)
    spectrumPeakHalf[i] = 1;
  }
  spectrumObj = lv_obj_create(screen);
  lv_obj_remove_style_all(spectrumObj);
  lv_obj_set_size(spectrumObj, EQ_TOTAL_WIDTH, SPECTRUM_SPAN * 2);
  lv_obj_set_pos(spectrumObj, EQ_LEFT, EQ_BASELINE - SPECTRUM_SPAN);
  lv_obj_clear_flag(spectrumObj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(spectrumObj, LV_OBJ_FLAG_CLICKABLE);        // v0.17.4: tap = next style
  lv_obj_add_event_cb(spectrumObj, spectrumDrawEvent, LV_EVENT_DRAW_MAIN, nullptr);
  lv_obj_add_event_cb(spectrumObj, spectrumTapEvent, LV_EVENT_CLICKED, nullptr);
  for (int i = 0; i < 32; ++i) {                              // RADIAL: band 0 at the top, clockwise
    float a = -1.5707963f + i * (6.2831853f / 32.0f);
    radialCos[i] = cosf(a);
    radialSin[i] = sinf(a);
  }

  trackTitleLabel = lv_label_create(screen);
  lv_label_set_text(trackTitleLabel, "Preparing music...");
  // Keep long titles within a fixed three-line area above the track counter.
  // LONG_DOT wraps text and adds an ellipsis if it exceeds the available height.
  lv_label_set_long_mode(trackTitleLabel, LV_LABEL_LONG_DOT);
  lv_obj_set_size(trackTitleLabel, 330, 72);
  lv_obj_set_style_text_align(trackTitleLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_color(trackTitleLabel, lv_color_hex(uiPlaybackTitleText), LV_PART_MAIN);
  lv_obj_set_style_text_font(trackTitleLabel, playerFont(uiFontPlaybackTitle), LV_PART_MAIN);
  lv_obj_align(trackTitleLabel, LV_ALIGN_TOP_MID, 0, 125);

  trackNumberLabel = lv_label_create(screen);
  lv_label_set_text(trackNumberLabel, "Scanning music...");
  lv_obj_set_style_text_color(trackNumberLabel, lv_color_hex(COLOR_PLAYBACK_TRACK_TEXT), LV_PART_MAIN);
  lv_obj_set_style_text_font(trackNumberLabel, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_obj_set_width(trackNumberLabel, 150);
  lv_obj_set_style_text_align(trackNumberLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
  lv_obj_align(trackNumberLabel, LV_ALIGN_TOP_LEFT, 42, 198);

  elapsedLabel = lv_label_create(screen);
  lv_label_set_text(elapsedLabel, "0:00");
  lv_obj_set_style_text_color(elapsedLabel, lv_color_hex(COLOR_PLAYBACK_TIME_TEXT), LV_PART_MAIN);
  lv_obj_set_style_text_font(elapsedLabel, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_obj_set_width(elapsedLabel, 58);
  lv_obj_align(elapsedLabel, LV_ALIGN_TOP_LEFT, 42, 280);

  durationLabel = lv_label_create(screen);
  lv_label_set_text(durationLabel, "0:00");
  lv_obj_set_style_text_color(durationLabel, lv_color_hex(COLOR_PLAYBACK_VOLUME_TEXT), LV_PART_MAIN);
  lv_obj_set_style_text_font(durationLabel, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_obj_set_width(durationLabel, 58);
  lv_obj_set_style_text_align(durationLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
  lv_obj_align(durationLabel, LV_ALIGN_TOP_RIGHT, -42, 280);

  progressBar = lv_bar_create(screen);
  lv_obj_set_size(progressBar, 250, 14);
  lv_obj_align(progressBar, LV_ALIGN_TOP_MID, 0, 305);
  lv_bar_set_range(progressBar, 0, 1000);
  lv_bar_set_value(progressBar, 0, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(progressBar, lv_color_hex(COLOR_PLAYBACK_PROGRESS_BG), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(progressBar, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_radius(progressBar, 4, LV_PART_MAIN);
  lv_obj_set_style_bg_color(progressBar, lv_color_hex(COLOR_PLAYBACK_PROGRESS_FILL), LV_PART_INDICATOR);
  lv_obj_set_style_bg_opa(progressBar, LV_OPA_COVER, LV_PART_INDICATOR);
  lv_obj_set_style_radius(progressBar, 4, LV_PART_INDICATOR);
  lv_obj_add_flag(progressBar, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(progressBar, progressBarEvent, LV_EVENT_ALL, nullptr);

  playStateLabel = lv_label_create(screen);
  lv_label_set_text(playStateLabel, "PLAYING");
  lv_obj_set_style_text_color(playStateLabel, lv_color_hex(uiPlaybackStateText), LV_PART_MAIN);
  lv_obj_set_style_text_font(playStateLabel, playerFont(uiFontPlaybackStatus), LV_PART_MAIN);
  lv_obj_set_style_bg_color(playStateLabel, lv_color_hex(uiPlaybackStatusBg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(playStateLabel, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_text_letter_space(playStateLabel, 2, LV_PART_MAIN);
  lv_obj_align(playStateLabel, LV_ALIGN_TOP_MID, 0, 330);

  createPlaybackButton(screen, "|<", -92, ACTION_PREVIOUS);
  lv_obj_t *playPauseButton = createPlaybackButton(screen, "PAUSE", 0, ACTION_TOGGLE);
  createPlaybackButton(screen, ">|", 92, ACTION_NEXT);
  playPauseButtonLabel = lv_obj_get_child(playPauseButton, 0);
}

// Decode one UTF-8 code point. Returns bytes consumed (1..4).
static size_t readUtf8CodePoint(const unsigned char *s, uint32_t *cp) {
  if (s[0] < 0x80) { *cp = s[0]; return 1; }
  if ((s[0] & 0xE0) == 0xC0 && s[1]) {
    *cp = ((uint32_t)(s[0] & 0x1F) << 6) | (s[1] & 0x3F); return 2;
  }
  if ((s[0] & 0xF0) == 0xE0 && s[1] && s[2]) {
    *cp = ((uint32_t)(s[0] & 0x0F) << 12) |
          ((uint32_t)(s[1] & 0x3F) << 6) | (s[2] & 0x3F); return 3;
  }
  if ((s[0] & 0xF8) == 0xF0 && s[1] && s[2] && s[3]) {
    *cp = ((uint32_t)(s[0] & 0x07) << 18) |
          ((uint32_t)(s[1] & 0x3F) << 12) |
          ((uint32_t)(s[2] & 0x3F) << 6) | (s[3] & 0x3F); return 4;
  }
  *cp = ' ';
  return 1;
}

static bool isCjkCodePoint(uint32_t cp) {
  return (cp >= 0x2E80 && cp <= 0x9FFF) ||
         (cp >= 0xF900 && cp <= 0xFAFF) ||
         (cp >= 0x20000 && cp <= 0x3134F);
}

static bool isOpenParen(uint32_t cp) { return cp == '(' || cp == 0xFF08; }
static bool isCloseParen(uint32_t cp) { return cp == ')' || cp == 0xFF09; }

// Convert common Windows/Unicode punctuation to ASCII equivalents.
// CJK text in parentheses (often language/edition tags in downloaded music
// names) is omitted because the built-in LVGL font does not contain those glyphs.
static void makeDisplayTitle(const char *source, char *destination, size_t capacity) {
  if (!destination || capacity == 0) return;
  size_t out = 0;
  bool previousWasSpace = true;
  const unsigned char *p = (const unsigned char *)source;

  while (p && *p && out + 1 < capacity) {
    uint32_t cp = 0;
    size_t consumed = readUtf8CodePoint(p, &cp);

    // If a parenthetical group contains CJK characters, omit the whole group.
    if (isOpenParen(cp)) {
      const unsigned char *q = p + consumed;
      bool hasCjk = false;
      bool foundClose = false;
      while (*q) {
        uint32_t look = 0;
        size_t n = readUtf8CodePoint(q, &look);
        if (isCjkCodePoint(look)) hasCjk = true;
        if (isCloseParen(look)) { foundClose = true; q += n; break; }
        q += n;
      }
      if (hasCjk && foundClose) {
        p = q;
        cp = ' ';
        consumed = 0;
      }
    }

    p += consumed;

    // Common Unicode punctuation and spacing used in Windows filenames.
    switch (cp) {
      case 0x3001: case 0xFF0C: cp = ','; break;       // 、 ，
      case 0x3002: case 0xFF0E: cp = '.'; break;       // 。 ．
      case 0x2018: case 0x2019: case 0xFF07: cp = '\''; break;
      case 0x201C: case 0x201D: case 0xFF02: cp = '"'; break;
      case 0x2013: case 0x2014: case 0x2212: cp = '-'; break;
      case 0x2026: cp = '.'; break;                    // …
      case 0x00A0: case 0x3000: cp = ' '; break;       // no-break/ideographic space
      case 0xFF08: cp = '('; break;
      case 0xFF09: cp = ')'; break;
      case 0xFF01: cp = '!'; break;
      case 0xFF1F: cp = '?'; break;
      case 0xFF1A: cp = ':'; break;
      case 0xFF1B: cp = ';'; break;
      case 0xFF0B: cp = '+'; break;
      case 0xFF06: cp = '&'; break;
      case 0xFF05: cp = '%'; break;
      case 0xFF03: cp = '#'; break;
      case 0xFF20: cp = '@'; break;
      case 0xFF5E: cp = '~'; break;
      case 0xFF3F: cp = '_'; break;
      default: break;
    }

    if (cp >= 0xFF01 && cp <= 0xFF5E) cp -= 0xFEE0; // remaining full-width ASCII

    char ch = (cp >= 32 && cp <= 126) ? (char)cp : ' ';
    if (ch == '_' || ch == '\\') ch = ' ';

    if (ch == ' ') {
      if (!previousWasSpace && out + 1 < capacity) {
        destination[out++] = ' ';
        previousWasSpace = true;
      }
    } else {
      destination[out++] = ch;
      previousWasSpace = false;
    }
  }

  while (out > 0 && destination[out - 1] == ' ') --out;
  destination[out] = '\0';
  if (out == 0) strlcpy(destination, "Unknown track", capacity);
}

static void updateMusicScreen() {
  if (!trackTitleLabel || !trackNumberLabel || !playStateLabel) return;
  if (!lcd_lvgl_lock(100)) return;

  if (trackCount == 0) {
    lv_label_set_text(trackTitleLabel, "No MP3 files found");
    lv_label_set_text(trackNumberLabel, "Check Music_Library");
    lv_label_set_text(playStateLabel, "READY");
  } else {
    const char *path = trackPaths[currentTrack];
    const char *filename = strrchr(path, '/');
    filename = filename ? filename + 1 : path;

    char rawName[PATH_LENGTH];
    strlcpy(rawName, filename, sizeof(rawName));
    size_t len = strlen(rawName);
    if (len > 4 && rawName[len - 4] == '.') {
      rawName[len - 4] = '\0';
    }

    char displayName[PATH_LENGTH];
    if (currentTrackMetadata.title[0]) makeDisplayTitle(currentTrackMetadata.title, displayName, sizeof(displayName));
    else makeDisplayTitle(rawName, displayName, sizeof(displayName));

    char trackInfo[48];
    snprintf(trackInfo, sizeof(trackInfo), "TRACK %u OF %u",
             static_cast<unsigned>(currentTrack + 1),
             static_cast<unsigned>(trackCount));

    lv_label_set_text(trackTitleLabel, displayName);

    // Measure the wrapped title and vertically center one- and two-line titles
    // within the same 72px area used by three-line titles.
    lv_point_t titleTextSize;
    lv_txt_get_size(&titleTextSize, displayName, &lv_font_montserrat_20,
                    0, 0, 330, LV_TEXT_FLAG_NONE);
    int titlePadTop = (72 - titleTextSize.y) / 2;
    if (titlePadTop < 0) titlePadTop = 0;
    lv_obj_set_style_pad_top(trackTitleLabel, titlePadTop, LV_PART_MAIN);

    lv_label_set_text(trackNumberLabel, trackInfo);
    lv_label_set_text(playStateLabel, playerStarted ? (pausedByUser ? "PAUSED" : (playerActiveFlag ? "PLAYING" : "STOPPED")) : "READY");
    if (playPauseButtonLabel) lv_label_set_text(playPauseButtonLabel, (playerStarted && !pausedByUser && playerActiveFlag) ? "PAUSE" : "PLAY");
  }

  lcd_lvgl_unlock();
  trackChanged = false;
}

// --------------------------- MP3 duration and PCM analysis --------------
// Fast MP3 duration estimate: inspect the first audio frame for Xing/Info/VBRI.
// If no frame-count header exists, estimate duration from file size and first
// frame bitrate (appropriate for CBR). Never scan the entire audio file here.
static uint64_t scanMp3DurationMs(const char *path, bool logResult = true) {
  FILE *f = fopen(path, "rb");
  if (!f) return 0;

  if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return 0; }
  long fileSize = ftell(f);
  if (fileSize <= 0) { fclose(f); return 0; }

  long audioStart = 0;
  uint8_t id3[10];
  rewind(f);
  if (fread(id3, 1, sizeof(id3), f) == sizeof(id3) &&
      id3[0] == 'I' && id3[1] == 'D' && id3[2] == '3') {
    uint32_t tagSize = ((uint32_t)(id3[6] & 0x7F) << 21) |
                       ((uint32_t)(id3[7] & 0x7F) << 14) |
                       ((uint32_t)(id3[8] & 0x7F) << 7) |
                       (uint32_t)(id3[9] & 0x7F);
    audioStart = 10L + tagSize + ((id3[5] & 0x10) ? 10L : 0L);
  }
  if (audioStart < 0 || audioStart + 4 >= fileSize ||
      fseek(f, audioStart, SEEK_SET) != 0) {
    fclose(f); return 0;
  }

  // Locate the first MPEG Layer III frame after any ID3v2 tag.
  long framePos = -1;
  uint32_t header = 0;
  for (int scanned = 0; scanned < 4096 && audioStart + scanned + 4 <= fileSize; ++scanned) {
    uint8_t h[4];
    if (fseek(f, audioStart + scanned, SEEK_SET) != 0 || fread(h, 1, 4, f) != 4) break;
    uint32_t v = ((uint32_t)h[0] << 24) | ((uint32_t)h[1] << 16) |
                 ((uint32_t)h[2] << 8) | h[3];
    int ver = (v >> 19) & 3, layer = (v >> 17) & 3;
    int bi = (v >> 12) & 15, si = (v >> 10) & 3;
    if ((v & 0xFFE00000UL) == 0xFFE00000UL && ver != 1 && layer == 1 &&
        bi > 0 && bi < 15 && si < 3) {
      framePos = audioStart + scanned; header = v; break;
    }
  }
  if (framePos < 0) { fclose(f); return 0; }

  static const int rates[3] = {44100, 48000, 32000};
  static const int mpeg1L3Kbps[16] = {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0};
  static const int mpeg2L3Kbps[16] = {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0};
  int version = (header >> 19) & 3;
  int bitrateIndex = (header >> 12) & 15;
  int sampleIndex = (header >> 10) & 3;
  int channelMode = (header >> 6) & 3;
  bool hasCrc = ((header >> 16) & 1) == 0;
  int sampleRate = rates[sampleIndex];
  if (version == 2) sampleRate /= 2;
  else if (version == 0) sampleRate /= 4;
  int bitrate = (version == 3 ? mpeg1L3Kbps[bitrateIndex] : mpeg2L3Kbps[bitrateIndex]) * 1000;
  if (sampleRate <= 0 || bitrate <= 0) { fclose(f); return 0; }

  bool mpeg1 = version == 3;
  bool mono = channelMode == 3;
  int sideInfoBytes = mpeg1 ? (mono ? 17 : 32) : (mono ? 9 : 17);
  long xingOffset = framePos + 4 + (hasCrc ? 2 : 0) + sideInfoBytes;
  uint8_t marker[4] = {};
  uint32_t frameCount = 0;
  uint32_t encoderDelay = 0, encoderPadding = 0;
  bool haveXing = false;

  if (fseek(f, xingOffset, SEEK_SET) == 0 && fread(marker, 1, 4, f) == 4 &&
      (!memcmp(marker, "Xing", 4) || !memcmp(marker, "Info", 4))) {
    haveXing = true;
    uint8_t flagBytes[4];
    if (fread(flagBytes, 1, 4, f) == 4) {
      uint32_t flags = ((uint32_t)flagBytes[0] << 24) | ((uint32_t)flagBytes[1] << 16) |
                       ((uint32_t)flagBytes[2] << 8) | flagBytes[3];
      if (flags & 0x1) {
        uint8_t b[4];
        if (fread(b, 1, 4, f) == 4)
          frameCount = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | b[3];
      }
      if (flags & 0x2) fseek(f, 4, SEEK_CUR);       // byte count
      if (flags & 0x4) fseek(f, 100, SEEK_CUR);     // TOC
      if (flags & 0x8) fseek(f, 4, SEEK_CUR);       // quality
      // LAME-compatible encoder tag stores 12-bit delay and padding at bytes 21..23.
      uint8_t lameTag[24];
      if (fread(lameTag, 1, sizeof(lameTag), f) == sizeof(lameTag) &&
          (!memcmp(lameTag, "LAME", 4) || !memcmp(lameTag, "Lavf", 4) ||
           !memcmp(lameTag, "Lavc", 4) || !memcmp(lameTag, "GOGO", 4))) {
        encoderDelay = ((uint32_t)lameTag[21] << 4) | (lameTag[22] >> 4);
        encoderPadding = (((uint32_t)lameTag[22] & 0x0F) << 8) | lameTag[23];
      }
    }
  }

  bool haveVbri = false;
  if (frameCount == 0 && fseek(f, framePos + 36, SEEK_SET) == 0 &&
      fread(marker, 1, 4, f) == 4 && memcmp(marker, "VBRI", 4) == 0) {
    uint8_t vbri[14];
    if (fread(vbri, 1, sizeof(vbri), f) == sizeof(vbri)) {
      frameCount = ((uint32_t)vbri[10] << 24) | ((uint32_t)vbri[11] << 16) |
                   ((uint32_t)vbri[12] << 8) | vbri[13];
      haveVbri = frameCount > 0;
    }
  }

  uint64_t durationMs = 0;
  const uint32_t samplesPerFrame = mpeg1 ? 1152 : 576;
  if (frameCount > 0) {
    uint64_t totalSamples = (uint64_t)frameCount * samplesPerFrame;
    uint64_t trimSamples = (uint64_t)encoderDelay + encoderPadding;
    if (haveXing && trimSamples < totalSamples) totalSamples -= trimSamples;
    durationMs = (totalSamples * 1000ULL + sampleRate / 2) / sampleRate;
    if (logResult) {
      Serial.printf("[DURATION] %s: %lu frames, delay %lu, padding %lu, %llu ms\n",
                    haveXing ? "Xing/Info" : (haveVbri ? "VBRI" : "frame-count"),
                    (unsigned long)frameCount, (unsigned long)encoderDelay,
                    (unsigned long)encoderPadding, (unsigned long long)durationMs);
    }
  } else {
    // Fallback estimate for files without a usable frame-count header.
    long audioEnd = fileSize;
    uint8_t tail[128];
    if (fileSize >= 128 && fseek(f, fileSize - 128, SEEK_SET) == 0 &&
        fread(tail, 1, 128, f) == 128 && !memcmp(tail, "TAG", 3)) audioEnd -= 128;
    long audioBytes = audioEnd - audioStart;
    if (audioBytes > 0) {
      durationMs = ((uint64_t)audioBytes * 8ULL * 1000ULL + bitrate / 2) / bitrate;
      if (logResult) {
        Serial.printf("[DURATION] Bitrate estimate: %d kbps, %llu ms (may be approximate for VBR)\n",
                      bitrate / 1000, (unsigned long long)durationMs);
      }
    }
  }
  fclose(f);
  return durationMs;
}

// Update the browser screen during an on-device rebuild. This is called only
// from the main loop's synchronous scan path, with LVGL protected by its lock.
static void setLibraryScanUi(const char *message, uint32_t done, uint32_t total) {
  if (!browserScreen || !lcd_lvgl_lock(100)) return;
  if (browserHeading) lv_label_set_text(browserHeading, "LIBRARY INDEX");
  if (browserList) lv_obj_add_flag(browserList, LV_OBJ_FLAG_HIDDEN);
  if (browserLetterPanel) lv_obj_add_flag(browserLetterPanel, LV_OBJ_FLAG_HIDDEN);
  for (size_t i = 0; i < 27; ++i) if (browserLetterButtons[i]) lv_obj_add_flag(browserLetterButtons[i], LV_OBJ_FLAG_HIDDEN);
  if (browserPagePrevButton) lv_obj_add_flag(browserPagePrevButton, LV_OBJ_FLAG_HIDDEN);
  if (browserPageNextButton) lv_obj_add_flag(browserPageNextButton, LV_OBJ_FLAG_HIDDEN);
  if (browserPageLabel) lv_obj_add_flag(browserPageLabel, LV_OBJ_FLAG_HIDDEN);
  if (browserRescanButton) lv_obj_add_flag(browserRescanButton, LV_OBJ_FLAG_HIDDEN);
  if (browserEmptyLabel) lv_obj_add_flag(browserEmptyLabel, LV_OBJ_FLAG_HIDDEN);
  uint32_t percent = total ? (uint32_t)(((uint64_t)done * 100ULL) / total) : 0;
  if (percent > 100) percent = 100;
  if (browserStatusLabel) {
    char status[112];
    if (total) snprintf(status, sizeof(status), "%s\n%lu / %lu files  (%lu%%)", message,
                        (unsigned long)done, (unsigned long)total, (unsigned long)percent);
    else snprintf(status, sizeof(status), "%s", message);
    lv_label_set_text(browserStatusLabel, status);
    lv_obj_clear_flag(browserStatusLabel, LV_OBJ_FLAG_HIDDEN);
  }
  if (browserProgressBar) {
    lv_bar_set_value(browserProgressBar, (int)percent, LV_ANIM_OFF);
    lv_obj_clear_flag(browserProgressBar, LV_OBJ_FLAG_HIDDEN);
  }
  lcd_lvgl_unlock();
}

// ----------------------- Library index scanner --------------------------
// Index format is UTF-8, tab-separated text. Metadata fields are sanitized by
// readId3Metadata(); the original MP3s are opened read-only and never modified.
static bool ensureIndexDirectory() {
  struct stat st;
  if (stat(INDEX_DIR, &st) == 0) return S_ISDIR(st.st_mode);
  if (mkdir(INDEX_DIR, 0775) == 0) return true;
  Serial.printf("[LIBRARY] Could not create index directory %s\n", INDEX_DIR);
  return false;
}

static void makeLibraryIndexPath(size_t libraryIndex, char *out, size_t capacity) {
  snprintf(out, capacity, "%s/%s", INDEX_DIR, MUSIC_LIBRARIES[libraryIndex].indexFile);
}

static bool libraryIndexIsValid(size_t libraryIndex) {
  char path[PATH_LENGTH];
  makeLibraryIndexPath(libraryIndex, path, sizeof(path));
  FILE *f = fopen(path, "rb");
  if (!f) return false;
  char line[128];
  bool valid = fgets(line, sizeof(line), f) != nullptr;
  fclose(f);
  if (!valid) return false;
  char expected[96];
  snprintf(expected, sizeof(expected), "#MUSIC_LIBRARY_INDEX\t1\t%s", MUSIC_LIBRARIES[libraryIndex].folder);
  return strncmp(line, expected, strlen(expected)) == 0;
}

static bool writeLibraryIndexEntry(FILE *out, const char *path, uint32_t durationMs) {
  TrackMetadata &metadata = libraryMetadataScratch;
  readId3Metadata(path, metadata);
  char safePath[PATH_LENGTH];
  strlcpy(safePath, path, sizeof(safePath));
  cleanField(safePath);
  // All text fields are sanitized to remove tabs/newlines before TSV output.
  int written = fprintf(out,
      "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%lu\n",
      safePath, metadata.title, metadata.artist, metadata.album,
      metadata.albumArtist, metadata.track, metadata.disc, metadata.genre,
      metadata.year, metadata.composer, metadata.bpm,
      (unsigned long)durationMs);
  return written > 0;
}

// Iterative depth-first scanner. The previous recursive implementation consumed
// loopTask stack at every nested folder; each metadata read also had a 4 KB
// temporary ID3 frame buffer. Directory traversal now uses a heap-allocated
// stack of open directory handles and paths, avoiding recursion entirely.
struct LibraryScanFrame {
  DIR *dir;
  char path[PATH_LENGTH];
};

static bool countLibraryMp3Files(const char *rootDirectory, uint32_t &totalCount) {
  totalCount = 0;
  LibraryScanFrame *frames = static_cast<LibraryScanFrame *>(
      calloc(MAX_SCAN_DEPTH + 1, sizeof(LibraryScanFrame)));
  if (!frames) return false;
  size_t depth = 0;
  frames[0].dir = opendir(rootDirectory);
  if (!frames[0].dir) { free(frames); return false; }
  strlcpy(frames[0].path, rootDirectory, sizeof(frames[0].path));
  bool ok = true;
  while (true) {
    struct dirent *entry = readdir(frames[depth].dir);
    if (!entry) {
      closedir(frames[depth].dir); frames[depth].dir = nullptr;
      if (depth == 0) break;
      --depth; continue;
    }
    const char *name = entry->d_name;
    if (!strcmp(name, ".") || !strcmp(name, "..") || name[0] == '.') continue;
    char fullPath[PATH_LENGTH];
    int n = snprintf(fullPath, sizeof(fullPath), "%s/%s", frames[depth].path, name);
    if (n <= 0 || (size_t)n >= sizeof(fullPath)) continue;
    struct stat st;
    if (stat(fullPath, &st) != 0) continue;
    if (S_ISDIR(st.st_mode)) {
      if (depth >= MAX_SCAN_DEPTH) continue;
      DIR *child = opendir(fullPath);
      if (!child) { ok = false; continue; }
      ++depth; frames[depth].dir = child;
      strlcpy(frames[depth].path, fullPath, sizeof(frames[depth].path));
    } else if (S_ISREG(st.st_mode) && isMp3(name)) ++totalCount;
  }
  while (depth > 0) { if (frames[depth].dir) closedir(frames[depth].dir); --depth; }
  if (frames[0].dir) closedir(frames[0].dir);
  free(frames);
  return ok;
}

static bool scanLibraryDirectory(const char *rootDirectory, FILE *out,
                                 uint32_t &songCount, uint32_t totalCount,
                                 const char *libraryName) {
  LibraryScanFrame *frames = static_cast<LibraryScanFrame *>(
      calloc(MAX_SCAN_DEPTH + 1, sizeof(LibraryScanFrame)));
  if (!frames) {
    Serial.println("[LIBRARY] Not enough heap for directory scan stack.");
    return false;
  }

  size_t depth = 0;
  frames[0].dir = opendir(rootDirectory);
  if (!frames[0].dir) {
    Serial.printf("[LIBRARY] Cannot open folder: %s\n", rootDirectory);
    free(frames);
    return false;
  }
  strlcpy(frames[0].path, rootDirectory, sizeof(frames[0].path));

  bool ok = true;
  while (true) {
    struct dirent *entry = readdir(frames[depth].dir);
    if (!entry) {
      closedir(frames[depth].dir);
      frames[depth].dir = nullptr;
      if (depth == 0) break;
      --depth;
      continue;
    }

    const char *name = entry->d_name;
    if (!strcmp(name, ".") || !strcmp(name, "..") || name[0] == '.') continue;

    char fullPath[PATH_LENGTH];
    int n = snprintf(fullPath, sizeof(fullPath), "%s/%s", frames[depth].path, name);
    if (n <= 0 || (size_t)n >= sizeof(fullPath)) {
      Serial.printf("[LIBRARY] Skipping overlong path under %s\n", frames[depth].path);
      continue;
    }

    struct stat st;
    if (stat(fullPath, &st) != 0) continue;

    if (S_ISDIR(st.st_mode)) {
      if (depth >= MAX_SCAN_DEPTH) {
        Serial.printf("[LIBRARY] Maximum folder depth reached; skipping %s\n", fullPath);
        continue;
      }
      DIR *child = opendir(fullPath);
      if (!child) {
        Serial.printf("[LIBRARY] Cannot open folder: %s\n", fullPath);
        ok = false;
        continue;
      }
      ++depth;
      frames[depth].dir = child;
      strlcpy(frames[depth].path, fullPath, sizeof(frames[depth].path));
    } else if (S_ISREG(st.st_mode) && isMp3(name)) {
      uint64_t duration = scanMp3DurationMs(fullPath, false);
      if (!writeLibraryIndexEntry(out, fullPath, (uint32_t)duration)) {
        Serial.printf("[LIBRARY] Failed writing index entry for %s\n", fullPath);
        ok = false;
        break;
      }
      ++songCount;
      if ((songCount % 25U) == 0U) {
        Serial.printf("[LIBRARY] %s: indexed %lu / %lu songs...\n",
                      libraryName, (unsigned long)songCount, (unsigned long)totalCount);
        char progressMessage[64];
        snprintf(progressMessage, sizeof(progressMessage), "SCANNING %s", libraryName);
        setLibraryScanUi(progressMessage, songCount, totalCount);
      }
    }
  }

  while (depth > 0) {
    if (frames[depth].dir) closedir(frames[depth].dir);
    --depth;
  }
  if (frames[0].dir) closedir(frames[0].dir);
  free(frames);
  return ok;
}

static bool scanOneMusicLibrary(size_t libraryIndex) {
  if (libraryIndex >= MUSIC_LIBRARY_COUNT || !ensureIndexDirectory()) return false;
  char sourcePath[PATH_LENGTH], finalPath[PATH_LENGTH];
  snprintf(sourcePath, sizeof(sourcePath), "%s/%s", MUSIC_ROOT,
           MUSIC_LIBRARIES[libraryIndex].folder);
  makeLibraryIndexPath(libraryIndex, finalPath, sizeof(finalPath));
  struct stat sourceStat;
  if (stat(sourcePath, &sourceStat) != 0 || !S_ISDIR(sourceStat.st_mode)) {
    Serial.printf("[LIBRARY] Folder not found; no index created: %s\n", sourcePath);
    return false;
  }

  char tempPath[PATH_LENGTH], backupPath[PATH_LENGTH];
  snprintf(tempPath, sizeof(tempPath), "%s.tmp", finalPath);
  snprintf(backupPath, sizeof(backupPath), "%s.bak", finalPath);
  remove(tempPath);
  FILE *out = fopen(tempPath, "wb");
  if (!out) {
    Serial.printf("[LIBRARY] Cannot create temporary index: %s\n", tempPath);
    return false;
  }
  fprintf(out, "#MUSIC_LIBRARY_INDEX\t1\t%s\tpath\ttitle\tartist\talbum\talbum_artist\ttrack\tdisc\tgenre\tyear\tcomposer\tbpm\tduration_ms\n",
          MUSIC_LIBRARIES[libraryIndex].folder);
  uint32_t songCount = 0;
  uint32_t totalCount = 0;
  char statusMessage[64];
  snprintf(statusMessage, sizeof(statusMessage), "COUNTING %s FILES...", MUSIC_LIBRARIES[libraryIndex].folder);
  setLibraryScanUi(statusMessage, 0, 0);
  bool countOk = countLibraryMp3Files(sourcePath, totalCount);
  if (!countOk) Serial.printf("[LIBRARY] File count pass had inaccessible folders for %s.\n", MUSIC_LIBRARIES[libraryIndex].folder);
  char scanningMessage[64];
  snprintf(scanningMessage, sizeof(scanningMessage), "SCANNING %s", MUSIC_LIBRARIES[libraryIndex].folder);
  setLibraryScanUi(scanningMessage, 0, totalCount);
  bool ok = scanLibraryDirectory(sourcePath, out, songCount, totalCount, MUSIC_LIBRARIES[libraryIndex].folder);
  setLibraryScanUi(scanningMessage, songCount, totalCount);
  if (fflush(out) != 0) ok = false;
  fclose(out);
  if (!ok) {
    remove(tempPath);
    Serial.printf("[LIBRARY] Scan failed; previous index retained for %s\n",
                  MUSIC_LIBRARIES[libraryIndex].folder);
    return false;
  }

  // Replace the old index only after the complete temporary file is closed.
  remove(backupPath);
  bool hadOld = (rename(finalPath, backupPath) == 0);
  if (rename(tempPath, finalPath) != 0) {
    Serial.printf("[LIBRARY] Could not install new index for %s\n",
                  MUSIC_LIBRARIES[libraryIndex].folder);
    if (hadOld) rename(backupPath, finalPath);
    remove(tempPath);
    return false;
  }
  if (hadOld) remove(backupPath);
  Serial.printf("[LIBRARY] %s: complete, %lu MP3 file(s) indexed -> %s\n",
                MUSIC_LIBRARIES[libraryIndex].folder,
                (unsigned long)songCount, finalPath);
  return true;
}

// v0.18.0: fill MUSIC_LIBRARIES from the folders in Music_Library.
static void discoverMusicLibraries() {
  MUSIC_LIBRARY_COUNT = 0;
  DIR *dir = opendir(MUSIC_ROOT);
  if (!dir) { Serial.printf("[LIBRARY] Cannot open %s\n", MUSIC_ROOT); return; }
  struct dirent *entry;
  size_t skipped = 0;
  while ((entry = readdir(dir)) != nullptr) {
    const char *name = entry->d_name;
    if (name[0] == '.' || strcasecmp(name, "Playlists") == 0) continue;
    char path[PATH_LENGTH];
    struct stat st;
    int n = snprintf(path, sizeof(path), "%s/%s", MUSIC_ROOT, name);
    if (n < 0 || (size_t)n >= sizeof(path) || stat(path, &st) != 0 || !S_ISDIR(st.st_mode)) continue;
    if (strlen(name) >= sizeof(MUSIC_LIBRARIES[0].folder)) {
      Serial.printf("[LIBRARY] Folder name too long, skipped: %s\n", name);
      continue;
    }
    if (MUSIC_LIBRARY_COUNT >= MAX_MUSIC_LIBRARIES) { ++skipped; continue; }
    MusicLibraryDefinition &lib = MUSIC_LIBRARIES[MUSIC_LIBRARY_COUNT++];
    strlcpy(lib.folder, name, sizeof(lib.folder));
    snprintf(lib.indexFile, sizeof(lib.indexFile), "%.47s_Library.txt", name);
  }
  closedir(dir);
  for (size_t i = 1; i < MUSIC_LIBRARY_COUNT; ++i) {          // sort by name
    MusicLibraryDefinition t = MUSIC_LIBRARIES[i];
    size_t j = i;
    while (j > 0 && strcasecmp(MUSIC_LIBRARIES[j - 1].folder, t.folder) > 0) { MUSIC_LIBRARIES[j] = MUSIC_LIBRARIES[j - 1]; --j; }
    MUSIC_LIBRARIES[j] = t;
  }
  if (skipped) Serial.printf("[LIBRARY] %u extra folder(s) ignored (limit %u libraries).\n",
                             (unsigned)skipped, (unsigned)MAX_MUSIC_LIBRARIES);
  for (size_t i = 0; i < MUSIC_LIBRARY_COUNT; ++i)
    Serial.printf("[LIBRARY] Library %u: %s\n", (unsigned)(i + 1), MUSIC_LIBRARIES[i].folder);
  if (selectedLibraryIndex >= MUSIC_LIBRARY_COUNT) selectedLibraryIndex = 0;
}

static void ensureMusicLibraryIndexes(bool forceRescan) {
  freeLibraryCache();   // v0.16.7d: the PSRAM copy is reloaded from the new index on next use
  discoverMusicLibraries();   // v0.18.0
  if (!ensureIndexDirectory()) return;
  Serial.println(forceRescan ? "[LIBRARY] Starting full library rescan..." :
                               "[LIBRARY] Checking saved library indexes...");
  for (size_t i = 0; i < MUSIC_LIBRARY_COUNT; ++i) {
    if (!forceRescan && libraryIndexIsValid(i)) {
      Serial.printf("[LIBRARY] Existing index found: %s\n", MUSIC_LIBRARIES[i].indexFile);
      continue;
    }
    Serial.printf("[LIBRARY] Scanning %s...\n", MUSIC_LIBRARIES[i].folder);
    scanOneMusicLibrary(i);
  }
  Serial.println("[LIBRARY] Index check/rescan finished.");
}

static void setRescanLabel(const char *text) {
  if (browserRescanLabel && lcd_lvgl_lock(50)) { lv_label_set_text(browserRescanLabel, text); lcd_lvgl_unlock(); }
}

static void handleLibraryRescan() {
  // v0.17.3: the first tap only arms RESCAN for 3 s ("CONFIRM?").
  if (rescanArmedUntilMs && (int32_t)(millis() - rescanArmedUntilMs) >= 0) {
    rescanArmedUntilMs = 0;
    setRescanLabel("RESCAN");
  }
  if (!pendingLibraryRescan) return;
  pendingLibraryRescan = false;
  if (!isLibraryRoot(browsePath)) return;
  if (!rescanArmedUntilMs) {
    uint32_t t = millis() + 3000;
    rescanArmedUntilMs = t ? t : 1;
    setRescanLabel("CONFIRM?");
    Serial.println("[LIBRARY] RESCAN tapped: tap again within 3 s to confirm.");
    return;
  }
  rescanArmedUntilMs = 0;
  setRescanLabel("RESCAN");
  if (playerStarted && playerActiveFlag && !pausedByUser) {
    Serial.println("[LIBRARY] Pause playback before starting a library rescan.");
    return;
  }
  setLibraryScanUi("STARTING LIBRARY RESCAN...", 0, 0);
  ensureMusicLibraryIndexes(true);
  setLibraryScanUi("INDEXING COMPLETE", 1, 1);
  delay(1200);
  browserPage = 0;
  showBrowser();
}

// In-place radix-2 complex FFT. The input is a 512-sample mono window.
static void runSpectrumFft(const float *input, int sampleRate) {
  static float real[FFT_SIZE];
  static float imag[FFT_SIZE];

  for (int i = 0; i < FFT_SIZE; ++i) {
    float window = 0.5f - 0.5f * cosf(2.0f * 3.14159265f * i /
                                      (float)(FFT_SIZE - 1));
    real[i] = input[i] * window;
    imag[i] = 0.0f;
  }

  int j = 0;
  for (int i = 1; i < FFT_SIZE; ++i) {
    int bit = FFT_SIZE >> 1;
    while (j & bit) { j ^= bit; bit >>= 1; }
    j ^= bit;
    if (i < j) {
      float tr = real[i]; real[i] = real[j]; real[j] = tr;
      float ti = imag[i]; imag[i] = imag[j]; imag[j] = ti;
    }
  }

  for (int len = 2; len <= FFT_SIZE; len <<= 1) {
    float angle = -2.0f * 3.14159265f / (float)len;
    float wLenR = cosf(angle), wLenI = sinf(angle);
    for (int i = 0; i < FFT_SIZE; i += len) {
      float wr = 1.0f, wi = 0.0f;
      for (int k = 0; k < len / 2; ++k) {
        int u = i + k, v = u + len / 2;
        float vr = real[v] * wr - imag[v] * wi;
        float vi = real[v] * wi + imag[v] * wr;
        real[v] = real[u] - vr;
        imag[v] = imag[u] - vi;
        real[u] += vr;
        imag[u] += vi;
        float nextWr = wr * wLenR - wi * wLenI;
        wi = wr * wLenI + wi * wLenR;
        wr = nextWr;
      }
    }
  }

  float measured[EQ_BANDS] = {};
  const float minHz = 86.0f;
  const float maxHz = fminf(11000.0f, sampleRate * 0.48f);
  for (int band = 0; band < EQ_BANDS; ++band) {
    float lowHz = minHz * powf(maxHz / minHz, (float)band / EQ_BANDS);
    float highHz = minHz * powf(maxHz / minHz, (float)(band + 1) / EQ_BANDS);
    int firstBin = (int)ceilf(lowHz * FFT_SIZE / sampleRate);
    int lastBin = (int)floorf(highHz * FFT_SIZE / sampleRate);
    if (lastBin < firstBin) lastBin = firstBin;
    firstBin = constrain(firstBin, 1, FFT_SIZE / 2 - 1);
    lastBin = constrain(lastBin, firstBin, FFT_SIZE / 2 - 1);

    float sum = 0.0f;
    int count = 0;
    for (int bin = firstBin; bin <= lastBin; ++bin) {
      float magnitude = sqrtf(real[bin] * real[bin] + imag[bin] * imag[bin]) *
                        (4.0f / FFT_SIZE);
      sum += magnitude;
      ++count;
    }
    float amplitude = count ? sum / count : 0.0f;
    float db = 20.0f * log10f(amplitude + 0.000001f);
    measured[band] = constrain((db + 58.0f) * (100.0f / 58.0f) *
                               EQ_DISPLAY_GAIN, 0.0f, 100.0f);
  }

  // v0.17.4: waveform snapshot (every 8th sample, 2x gain) and L/R levels.
  int8_t snap[64];
  for (int i = 0; i < 64; ++i) {
    float v = input[i * 8] * 2.0f * 127.0f;
    snap[i] = (int8_t)constrain(v, -127.0f, 127.0f);
  }
  float lvL = 0, lvR = 0;
  if (levelSumCount) {
    float rmsL = sqrtf((float)(levelSumSqL / levelSumCount)) / 32768.0f;
    float rmsR = sqrtf((float)(levelSumSqR / levelSumCount)) / 32768.0f;
    lvL = constrain((20.0f * log10f(rmsL + 0.000001f) + 48.0f) * (100.0f / 48.0f), 0.0f, 100.0f);
    lvR = constrain((20.0f * log10f(rmsR + 0.000001f) + 48.0f) * (100.0f / 48.0f), 0.0f, 100.0f);
  }
  levelSumSqL = levelSumSqR = 0.0; levelSumCount = 0;

  uint32_t now = millis();
  portENTER_CRITICAL(&audioStatsMux);
  for (int i = 0; i < 64; ++i) scopeSnapshot[i] = snap[i];
  levelLeft = lvL; levelRight = lvR;
  for (int i = 0; i < EQ_BANDS; ++i) {
    float smoothed = eqLevels[i] * 0.48f + measured[i] * 0.52f;
    eqLevels[i] = smoothed;
    if (smoothed >= eqPeakLevels[i]) {
      eqPeakLevels[i] = smoothed;
      eqPeakTimes[i] = now;
    }
  }
  portEXIT_CRITICAL(&audioStatsMux);
}

static void mp3PcmCallback(MP3FrameInfo &info, short *pcm, size_t sampleValues, void *ref) {
  (void)ref;
  if (!pcm || info.nChans <= 0 || info.samprate <= 0) return;

  size_t frames = sampleValues / (size_t)info.nChans;
  size_t firstOutputFrame = 0;
  uint64_t frameDurationUs = ((uint64_t)frames * 1000000ULL +
                              (uint32_t)info.samprate / 2) / (uint32_t)info.samprate;
  portENTER_CRITICAL(&audioStatsMux);
  uint64_t frameStartUs = decodedTimeUs;
  decodedSampleValues += sampleValues;
  if (seekDiscardUntilUs > frameStartUs) {
    uint64_t deltaUs = seekDiscardUntilUs - frameStartUs;
    firstOutputFrame = (size_t)((deltaUs * (uint32_t)info.samprate + 999999ULL) / 1000000ULL);
    if (firstOutputFrame > frames) firstOutputFrame = frames;
  }
  decodedTimeUs += frameDurationUs;
  if (decodedTimeUs >= seekDiscardUntilUs) seekDiscardUntilUs = 0;
  portEXIT_CRITICAL(&audioStatsMux);

  if (firstOutputFrame < frames) {
    const short *outputPcm = pcm + firstOutputFrame * (size_t)info.nChans;
    if (outputMode == OUTPUT_BLUETOOTH) {
      // v0.16.8: 44.1 kHz stereo to the link (blocks while its buffer is full).
      btConvertAndQueue(outputPcm, frames - firstOutputFrame, info.nChans, (uint32_t)info.samprate);
    } else {
      Print *out = decoder.getOutput();
      size_t outputValues = (frames - firstOutputFrame) * (size_t)info.nChans;
      if (out) wiredBytesWritten = wiredBytesWritten +
                 out->write((const uint8_t *)outputPcm, outputValues * sizeof(short));
    }
  }

  if (spectrumStyle == SPEC_OFF) return;      // v0.17.4: no FFT while the spectrum is off
  for (size_t i = firstOutputFrame; i < frames; ++i) {
    int32_t sample = 0;
    for (int ch = 0; ch < info.nChans; ++ch)
      sample += pcm[i * info.nChans + ch];
    {                                          // v0.17.4: L/R energy for LEVELS
      int32_t l = pcm[i * info.nChans];
      int32_t r = info.nChans > 1 ? pcm[i * info.nChans + 1] : l;
      levelSumSqL += (double)(l * l);
      levelSumSqR += (double)(r * r);
      ++levelSumCount;
    }
    fftSamples[fftSampleCount++] =
        (float)sample / (float)info.nChans / 32768.0f;
    if (fftSampleCount >= FFT_SIZE) {
      runSpectrumFft(fftSamples, info.samprate);
      fftSampleCount = 0;
    }
  }
}

static void resetTrackStats() {
  portENTER_CRITICAL(&audioStatsMux);
  decodedSampleValues = 0;
  decodedTimeUs = 0;
  seekDiscardUntilUs = 0;
  for (int i = 0; i < EQ_BANDS; ++i) {
    eqLevels[i] = 0.0f;
    eqPeakLevels[i] = 0.0f;
    eqPeakTimes[i] = 0;
  }
  fftSampleCount = 0;
  portEXIT_CRITICAL(&audioStatsMux);
}

static void updateVolumeDisplay() {
  if (!volumeLabel || !lcd_lvgl_lock(20)) return;
  char text[32];
  int arcValue = volumePercent;
  bool active = true;
  if (outputMode == OUTPUT_BLUETOOTH) {
    int state = btDisplayState();
    int v = btVolumePercent < 0 ? 0 : btVolumePercent;
    arcValue = v;
    active = state == 0;
    // v0.18.3: short texts, so they fit the narrow top of the round screen.
    if (state == 0) snprintf(text, sizeof(text), "BT %d%%", v);
    else strlcpy(text, state == 1 ? "WAITING" : "NO LINK", sizeof(text));
  } else {
    snprintf(text, sizeof(text), "%d%%", volumePercent);
  }
  lv_label_set_text(volumeLabel, text);
  if (volumeArc) {
    if (lv_arc_get_value(volumeArc) != arcValue) lv_arc_set_value(volumeArc, arcValue);
    static int lastActive = -1;   // restyling redraws the whole arc object, so only on change
    if (lastActive != (int)active) {
      lastActive = active;
      lv_obj_set_style_arc_color(volumeArc, lv_color_hex(active ? VOLUME_ARC_FILL : VOLUME_ARC_INACTIVE), LV_PART_INDICATOR);
    }
  }
  lcd_lvgl_unlock();
}

static void changeVolume(int direction) {
  int nextVolume = volumePercent + direction * VOLUME_STEP;
  if (nextVolume < 0) nextVolume = 0;
  if (nextVolume > 100) nextVolume = 100;
  if (nextVolume == volumePercent) return;
  volumePercent = nextVolume;
  {
    PlayerLock lock;
    player.setVolume(volumePercent / 100.0f);
  }
  Serial.printf("[VOLUME] %d%%\n", volumePercent);
  updateVolumeDisplay();
}

// Waveshare's official encoder driver calls these from its 3 ms timer.
static void encoderLeftCallback(void *arg, void *data) {
  portENTER_CRITICAL(&encoderEventMux);
  --pendingEncoderSteps;
  portEXIT_CRITICAL(&encoderEventMux);
}

static void encoderRightCallback(void *arg, void *data) {
  portENTER_CRITICAL(&encoderEventMux);
  ++pendingEncoderSteps;
  portEXIT_CRITICAL(&encoderEventMux);
}

static void processEncoderEvents() {
  int32_t steps;
  portENTER_CRITICAL(&encoderEventMux);
  steps = pendingEncoderSteps;
  pendingEncoderSteps = 0;
  portEXIT_CRITICAL(&encoderEventMux);

  if (steps == 0) return;
  // v0.17.3: on browser screens the knob turns pages (clockwise = NEXT).
  if (browserVisible && !settingsOpen && !playlistNameKeyboard) {
    size_t pages = (browserTotalEntries + BROWSER_PAGE_SIZE - 1) / BROWSER_PAGE_SIZE;
    if (pages == 0) pages = 1;
    long target = (long)browserPage + steps;
    if (target < 0) target = 0;
    if (target > (long)pages - 1) target = (long)pages - 1;
    if ((size_t)target != browserPage) {
      browserPage = (size_t)target;
      refreshBrowserScreen();
    }
    return;
  }
  // v0.16.8: in Bluetooth mode the knob sets the speaker's own volume.
  if (outputMode == OUTPUT_BLUETOOTH) { btChangeVolume(steps); return; }
  // Waveshare's official audio example maps RIGHT to volume up.
  int nextVolume = volumePercent + (steps * VOLUME_STEP);
  if (nextVolume < 0) nextVolume = 0;
  if (nextVolume > 100) nextVolume = 100;
  if (nextVolume == volumePercent) return;

  volumePercent = nextVolume;
  {
    PlayerLock lock;
    player.setVolume(volumePercent / 100.0f);
  }
  Serial.printf("[VOLUME] %d%% (encoder steps: %+ld)\n",
                volumePercent, (long)steps);
  updateVolumeDisplay();
}

static void formatTime(uint32_t seconds, char *out, size_t capacity) {
  snprintf(out, capacity, "%u:%02u", (unsigned)(seconds / 60), (unsigned)(seconds % 60));
}

static void updatePlaybackDisplay() {
  if (!elapsedLabel || !durationLabel || !progressBar) return;
  uint64_t elapsedMs;
  float levels[EQ_BANDS];
  float peaks[EQ_BANDS];
  uint32_t peakTimes[EQ_BANDS];
  portENTER_CRITICAL(&audioStatsMux);
  elapsedMs = decodedTimeUs / 1000ULL;
  for (int i = 0; i < EQ_BANDS; ++i) {
    levels[i] = eqLevels[i];
    peaks[i] = eqPeakLevels[i];
    peakTimes[i] = eqPeakTimes[i];
  }
  portEXIT_CRITICAL(&audioStatsMux);

  char timeText[16], totalText[16];
  if (seekPreviewActive) elapsedMs = seekPreviewMs;
  formatTime((uint32_t)(elapsedMs / 1000ULL), timeText, sizeof(timeText));
  formatTime((uint32_t)(trackDurationMs / 1000ULL), totalText, sizeof(totalText));

  constexpr int EQ_MAX_HALF_HEIGHT = SPECTRUM_MAX_HALF;
  uint32_t now = millis();

  if (!lcd_lvgl_lock(20)) return;
  lv_label_set_text(elapsedLabel, timeText);
  lv_label_set_text(durationLabel, totalText);
  int progress = trackDurationMs ?
      (int)((elapsedMs * 1000ULL) / trackDurationMs) : 0;
  if (progress > 1000) progress = 1000;
  lv_bar_set_value(progressBar, progress, LV_ANIM_OFF);

  // v0.16.7c: update the drawn values, then mark only the rows in use
  // (old or new height, whichever is larger) for redraw.
  int extent = 0;
  bool changed = false;
  for (int i = 0; i < EQ_BANDS; ++i) {
    float level = constrain(levels[i], 0.0f, 100.0f);
    int halfHeight = 2 + (int)(level * (EQ_MAX_HALF_HEIGHT - 2) / 100.0f);

    float peak = peaks[i];
    if (now - peakTimes[i] > EQ_PEAK_HOLD_MS) {
      peak = fmaxf(level, peak - EQ_PEAK_FALL_PER_UPDATE);
      portENTER_CRITICAL(&audioStatsMux);
      if (eqPeakLevels[i] > peak) eqPeakLevels[i] = peak;
      portEXIT_CRITICAL(&audioStatsMux);
    }
    int peakHeight = 2 + (int)(peak * (EQ_MAX_HALF_HEIGHT - 2) / 100.0f);

    if (halfHeight != spectrumBarHalf[i] || peakHeight != spectrumPeakHalf[i]) {
      changed = true;
      // Rows touched: the taller of the old/new bar, and the old/new peak marker.
      int e = spectrumBarHalf[i];
      if (halfHeight > e) e = halfHeight;
      if (spectrumPeakHalf[i] + 1 > e) e = spectrumPeakHalf[i] + 1;
      if (peakHeight + 1 > e) e = peakHeight + 1;
      if (e > extent) extent = e;
      spectrumBarHalf[i] = halfHeight;
      spectrumPeakHalf[i] = peakHeight;
    }
  }
  // v0.17.4: other styles redraw the whole spectrum area.
  uint8_t style = spectrumStyle;
  static bool nameWasShown = false;
  bool nameShown = spectrumNameUntilMs && (int32_t)(now - spectrumNameUntilMs) < 0;
  bool fullRedraw = nameShown || nameWasShown;
  nameWasShown = nameShown;
  if (style == SPEC_SCOPE || style == SPEC_LEVELS) {
    portENTER_CRITICAL(&audioStatsMux);
    for (int i = 0; i < 64; ++i) spectrumScope[i] = scopeSnapshot[i];
    float lv[2] = { levelLeft, levelRight };
    portEXIT_CRITICAL(&audioStatsMux);
    static float peakLv[2] = {};
    static uint32_t peakAt[2] = {};
    int segs = (32 * SPECTRUM_BAR_WIDTH + 31 * SPECTRUM_BAR_GAP) / 9;   // same as the drawn meter
    for (int ch = 0; ch < 2; ++ch) {
      if (lv[ch] >= peakLv[ch]) { peakLv[ch] = lv[ch]; peakAt[ch] = now; }
      else if (now - peakAt[ch] > EQ_PEAK_HOLD_MS) peakLv[ch] = fmaxf(lv[ch], peakLv[ch] - EQ_PEAK_FALL_PER_UPDATE);
      spectrumLevelPx[ch] = (int16_t)(lv[ch] * segs / 100.0f);
      spectrumLevelPeakPx[ch] = (int16_t)(peakLv[ch] * segs / 100.0f);
      if (spectrumLevelPeakPx[ch] >= segs) spectrumLevelPeakPx[ch] = segs - 1;
    }
    fullRedraw = true;
  }
  if (style == SPEC_BARS || style == SPEC_RADIAL) fullRedraw = fullRedraw || changed;
  if (style != SPEC_MIRROR) {
    if (spectrumObj && fullRedraw) lv_obj_invalidate(spectrumObj);
    changed = false;
  } else if (fullRedraw && spectrumObj) {
    lv_obj_invalidate(spectrumObj);
    changed = false;
  }
  if (changed && spectrumObj) {
    lv_area_t coords;
    lv_obj_get_coords(spectrumObj, &coords);
    int baseY = coords.y1 + SPECTRUM_SPAN;
    if (extent > SPECTRUM_SPAN) extent = SPECTRUM_SPAN;
    lv_area_t dirty = { coords.x1, (lv_coord_t)(baseY - extent), coords.x2, (lv_coord_t)(baseY + extent - 1) };
    lv_obj_invalidate_area(spectrumObj, &dirty);
  }
  lcd_lvgl_unlock();
}

// Locate an MPEG Layer III frame at or after a byte offset. We validate the
// next frame boundary as well to reduce false sync matches inside compressed data.
static bool findMp3FrameAtOrAfter(FILE *f, long start, long fileSize, long *framePos) {
  static const int brMpeg1[16] = {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0};
  static const int brMpeg2[16] = {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0};
  static const int srBase[3] = {44100,48000,32000};
  uint8_t h[4], h2[4];
  long limit = start + 8192;
  if (limit > fileSize - 8) limit = fileSize - 8;
  for (long pos = start; pos <= limit; ++pos) {
    if (fseek(f, pos, SEEK_SET) != 0 || fread(h, 1, 4, f) != 4) return false;
    uint32_t v = ((uint32_t)h[0] << 24) | ((uint32_t)h[1] << 16) |
                 ((uint32_t)h[2] << 8) | h[3];
    int ver = (v >> 19) & 3, layer = (v >> 17) & 3;
    int bi = (v >> 12) & 15, si = (v >> 10) & 3;
    if ((v & 0xFFE00000UL) != 0xFFE00000UL || ver == 1 || layer != 1 ||
        bi == 0 || bi == 15 || si == 3) continue;
    int sr = srBase[si];
    if (ver == 2) sr /= 2; else if (ver == 0) sr /= 4;
    int kbps = (ver == 3 ? brMpeg1[bi] : brMpeg2[bi]);
    int padding = (v >> 9) & 1;
    int frameLen = ((ver == 3 ? 144 : 72) * kbps * 1000) / sr + padding;
    long nextPos = pos + frameLen;
    if (nextPos + 4 > fileSize) continue;
    if (fseek(f, nextPos, SEEK_SET) != 0 || fread(h2, 1, 4, f) != 4) continue;
    uint32_t v2 = ((uint32_t)h2[0] << 24) | ((uint32_t)h2[1] << 16) |
                  ((uint32_t)h2[2] << 8) | h2[3];
    if ((v2 & 0xFFE00000UL) == 0xFFE00000UL && ((v2 >> 17) & 3) == 1) {
      *framePos = pos;
      return true;
    }
  }
  return false;
}

static long getMp3AudioStart(FILE *f, long fileSize) {
  uint8_t id3[10];
  rewind(f);
  if (fread(id3, 1, sizeof(id3), f) == sizeof(id3) &&
      id3[0] == 'I' && id3[1] == 'D' && id3[2] == '3') {
    uint32_t tagSize = ((uint32_t)(id3[6] & 0x7F) << 21) |
                       ((uint32_t)(id3[7] & 0x7F) << 14) |
                       ((uint32_t)(id3[8] & 0x7F) << 7) | (uint32_t)(id3[9] & 0x7F);
    long start = 10L + tagSize + ((id3[5] & 0x10) ? 10L : 0L);
    return (start < fileSize) ? start : 0;
  }
  return 0;
}

static void performSeek(uint64_t targetMs) {
  if (!playerStarted || !trackDurationMs || !audioFileStream.isOpen()) return;
  if (targetMs > trackDurationMs) targetMs = trackDurationMs;
  bool resumeAfterSeek = player.isActive();
  FILE *f = audioFileStream.getFile();
  if (!f) return;
  if (fseek(f, 0, SEEK_END) != 0) return;
  long fileSize = ftell(f);
  long audioStart = getMp3AudioStart(f, fileSize);
  long audioEnd = fileSize;
  uint8_t tail[128];
  if (fileSize >= 128 && fseek(f, fileSize - 128, SEEK_SET) == 0 &&
      fread(tail, 1, 128, f) == 128 && !memcmp(tail, "TAG", 3)) audioEnd -= 128;
  long audioBytes = audioEnd - audioStart;
  if (audioBytes <= 0 || trackDurationMs == 0) return;

  // Start about 1.5 seconds before the requested point. The decoder needs
  // earlier frames for MP3 bit-reservoir data; decoded preroll is discarded.
  uint64_t requestedOffset = ((uint64_t)audioBytes * targetMs) / trackDurationMs;
  uint64_t prerollBytes = ((uint64_t)audioBytes * 1500ULL) / trackDurationMs;
  long approximateStart = audioStart + (long)(requestedOffset > prerollBytes ?
                                      requestedOffset - prerollBytes : 0);
  long framePos = 0;
  if (!findMp3FrameAtOrAfter(f, approximateStart, audioEnd, &framePos)) {
    Serial.println("[SEEK] Could not locate a valid MP3 frame near requested position.");
    return;
  }

  uint64_t startMs = ((uint64_t)(framePos - audioStart) * trackDurationMs) / (uint64_t)audioBytes;
  if (startMs > targetMs) startMs = targetMs;
  portENTER_CRITICAL(&audioStatsMux);
  decodedSampleValues = 0;
  decodedTimeUs = startMs * 1000ULL;
  seekDiscardUntilUs = targetMs * 1000ULL;
  for (int i = 0; i < EQ_BANDS; ++i) {
    eqLevels[i] = 0.0f;
    eqPeakLevels[i] = 0.0f;
    eqPeakTimes[i] = 0;
  }
  portEXIT_CRITICAL(&audioStatsMux);
  fftSampleCount = 0;

  if (fseek(f, framePos, SEEK_SET) != 0) {
    Serial.println("[SEEK] File reposition failed.");
    return;
  }
  // AudioPlayer::setStream() resets its decoder and intermediate pipeline.
  // The same FILE-backed stream remains open at the new frame boundary.
  if (!player.setStream(&audioFileStream)) {
    Serial.println("[SEEK] AudioPlayer could not restart the current stream.");
    playerStarted = false;
    return;
  }
  player.setActive(resumeAfterSeek);
  pausedByUser = !resumeAfterSeek;
  seekPreviewActive = false;
  lastUiUpdateMs = 0;
  Serial.printf("[SEEK] Requested %llu ms; starting at %llu ms (MP3 preroll discarded).\n",
                (unsigned long long)targetMs, (unsigned long long)startMs);
}

// --------------------------- Audio source callbacks --------------------
static Stream *selectTrack(int index) {
  if (index < 0 || static_cast<size_t>(index) >= trackCount) return nullptr;

  currentTrack = static_cast<size_t>(index);
  if (queuePos >= trackCount || playOrder[queuePos] != currentTrack) {   // v0.17.5
    bool found = false;
    for (size_t i = 0; i < trackCount; ++i) if (playOrder[i] == currentTrack) { queuePos = i; found = true; break; }
    if (!found) rebuildPlayOrder();
  }
  resetTrackStats();
  readId3Metadata(trackPaths[currentTrack], currentTrackMetadata);
  trackDurationMs = scanMp3DurationMs(trackPaths[currentTrack]);
  Serial.printf("[DURATION] Track length: %llu ms\n",
                (unsigned long long)trackDurationMs);
  Serial.printf("\n[TRACK %u/%u] %s\n",
                static_cast<unsigned>(currentTrack + 1),
                static_cast<unsigned>(trackCount),
                trackPaths[currentTrack]);
  printTrackMetadata();

  if (!audioFileStream.open(trackPaths[currentTrack])) return nullptr;
  trackChanged = true;
  return &audioFileStream;
}

static Stream *nextTrack(int offset) {
  // v0.17.5: step through playOrder; RPT ALL / ONE wrap around the ends.
  if (!trackCount) return nullptr;
  long pos = (long)queuePos + offset;
  if (pos < 0 || pos >= (long)trackCount) {
    if (repeatMode == REPEAT_OFF) return nullptr;
    pos = ((pos % (long)trackCount) + (long)trackCount) % (long)trackCount;
  }
  queuePos = (size_t)pos;
  return selectTrack(playOrder[queuePos]);
}

// --------------------------- Setup --------------------------------------
void setup() {
  Serial.begin(115200);
  // v0.16.7: created first, before any UI callback or player call can run.
  playerMutex = xSemaphoreCreateRecursiveMutex();
  playbackActionQueue = xQueueCreate(8, sizeof(uint8_t));
  knob_config_t encoderConfig = {};
  encoderConfig.gpio_encoder_a = BOARD_ENCODER_A;
  encoderConfig.gpio_encoder_b = BOARD_ENCODER_B;
  volumeEncoder = iot_knob_create(&encoderConfig);
  if (volumeEncoder == nullptr) {
    Serial.println("[ENCODER] ERROR: Waveshare encoder driver creation failed.");
  } else {
    ESP_ERROR_CHECK(iot_knob_register_cb(volumeEncoder, KNOB_LEFT, encoderLeftCallback, nullptr));
    ESP_ERROR_CHECK(iot_knob_register_cb(volumeEncoder, KNOB_RIGHT, encoderRightCallback, nullptr));
    Serial.printf("[ENCODER] Waveshare driver ready A=GPIO%d B=GPIO%d\n",
                  BOARD_ENCODER_A, BOARD_ENCODER_B);
  }
  delay(2500);

  Serial.println();
  Serial.println("==========================================");
  Serial.printf(" RotaTune v%s\n", PLAYER_VERSION);
  Serial.println(" ESP32-S3 / MP3 + SH8601 display");
  Serial.println("==========================================");

  Serial.println("[LCD] Initializing CST816 touch controller...");
  Touch_Init();

  Serial.println("[LCD] Initializing SH8601 AMOLED and LVGL 8...");
  lcd_lvgl_Init();
  lcd_bl_pwm_bsp_init(LCD_PWM_MODE_255);

  Serial.println("[SD] Initializing Waveshare SDMMC driver...");
  if (!sd_card_Init()) {
    Serial.println("[ERROR] SD card mount failed; player UI cannot load without the SD card.");
    return;
  }

  // Load customizer settings before constructing either screen so all initial
  // LVGL styles use the saved values. Missing keys retain safe built-in defaults.
  loadPlayerColorSettings();
  if (lcd_lvgl_lock(1000)) {
    createPlayerSettingsScreen();
    createMusicScreen();
    lcd_lvgl_unlock();
  } else {
    Serial.println("[LCD] Could not acquire LVGL lock to create player screen.");
  }

  // Build missing persistent indexes once. Existing indexes are reused on later boots;
  // press RESCAN on the library root after adding/removing music files.
  ensureMusicLibraryIndexes(false);
  trackCount = 0;

  gpio_config_t dacPinConfig = {};
  dacPinConfig.pin_bit_mask = (1ULL << BOARD_DAC_ENABLE);
  dacPinConfig.mode = GPIO_MODE_OUTPUT;
  dacPinConfig.pull_up_en = GPIO_PULLUP_DISABLE;
  dacPinConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
  dacPinConfig.intr_type = GPIO_INTR_DISABLE;
  esp_err_t gpioResult = gpio_config(&dacPinConfig);
  if (gpioResult != ESP_OK) {
    Serial.printf("[ERROR] DAC GPIO setup failed: %s\n", esp_err_to_name(gpioResult));
    updateMusicScreen();
    return;
  }
  gpio_set_level(static_cast<gpio_num_t>(BOARD_DAC_ENABLE), 1);

  // v0.16.8: Bluetooth link (UART1 to the classic ESP32) and output setting.
  loadOutputSettings();
  btCmdSeq = (uint16_t)(esp_random() & 0xFFFF);   // v0.16.9: never matches a command done before a reboot
  if (!createPcmStream() && outputMode == OUTPUT_BLUETOOTH) {
    outputMode = OUTPUT_WIRED;                     // never decode unpaced into nothing
    Serial.println("[OUTPUT] Falling back to wired output.");
  }
  Link.setRxBufferSize(LINK_RX_BUFFER);
  Link.setTxBufferSize(LINK_TX_BUFFER);
  Link.begin(wslink::LINK_BAUD, SERIAL_8N1, LINK_RX_PIN, LINK_TX_PIN);
  xTaskCreatePinnedToCore(linkTask, "link", 6144, nullptr, 3, nullptr, 0);
  Serial.printf("[OUTPUT] %s output (saved setting). Link UART1 TX GPIO%d RX GPIO%d, %lu baud, protocol v%u.\n",
                outputMode == OUTPUT_BLUETOOTH ? "Bluetooth" : "Wired", LINK_TX_PIN, LINK_RX_PIN,
                (unsigned long)wslink::LINK_BAUD, (unsigned)wslink::LINK_VERSION);
  if (outputMode == OUTPUT_BLUETOOTH)
    Serial.printf("[OUTPUT] Bluetooth speaker volume %d%%\n", btVolumePercent);
  updateShuffleRepeatButtons();                                      // v0.17.5
  Serial.printf("[PLAY] Shuffle %s, repeat %s (saved settings).\n", shuffleOn ? "ON" : "OFF",
                repeatMode == REPEAT_ALL ? "ALL" : repeatMode == REPEAT_ONE ? "ONE" : "OFF");

  Serial.println("[I2S] Starting onboard audio output...");
  auto i2sConfig = i2s.defaultConfig(TX_MODE);
  i2sConfig.pin_bck = BOARD_AUDIO_BCLK;
  i2sConfig.pin_ws = BOARD_AUDIO_WS;
  i2sConfig.pin_data = BOARD_AUDIO_DOUT;
  i2sConfig.sample_rate = 44100;
  i2sConfig.channels = 2;
  i2sConfig.bits_per_sample = 16;
  i2sConfig.buffer_count = 8;
  i2sConfig.buffer_size = 512;

  if (!i2s.begin(i2sConfig)) {
    Serial.println("[ERROR] I2S initialization failed.");
    updateMusicScreen();
    return;
  }
  Serial.println("[I2S] Output initialized.");
  if (outputMode == OUTPUT_BLUETOOTH) writeI2sSilence();   // wired output quiet in Bluetooth mode

  source.setCallbackNextStream(nextTrack);
  source.setCallbackSelectStream(selectTrack);
  player.setAutoNext(false);
  player.setVolume(volumePercent / 100.0f);
  decoder.driver()->setDataCallback(mp3PcmCallback);

  // v0.16.7: decoding moves out of loop() into its own task.
  xTaskCreatePinnedToCore(audioTask, "audio", 12288, nullptr, 3, &audioTaskHandle, 1);
  Serial.printf("[AUDIO] Audio task started (core 1, priority 3, %u-byte decode steps).\n",
                (unsigned)DECODE_STEP_BYTES);

  if (lcd_lvgl_lock(100)) {
    createBrowserScreen();
    lv_scr_load(browserScreen);
    lcd_lvgl_unlock();
  }
  strlcpy(browsePath, "/sdcard/Music_Library", sizeof(browsePath));
  browserView = BROWSER_ROOT;
  browserHistoryDepth = 0;
  browserPage = 0;
  showBrowser();
  updateShuffleRepeatButtons();   // v0.17.7: label the buttons now they exist (they showed "Text")
  Serial.printf("[BROWSE] Ready. %u music librar%s found.\n", (unsigned)MUSIC_LIBRARY_COUNT, MUSIC_LIBRARY_COUNT == 1 ? "y" : "ies");
}

static void handleOnePlaybackAction(PlaybackAction action);

static void handlePlaybackActions() {
  uint8_t queued;
  while (playbackActionQueue && xQueueReceive(playbackActionQueue, &queued, 0) == pdTRUE) {
    PlaybackAction action = (PlaybackAction)queued;
    if (action == ACTION_SHUFFLE || action == ACTION_REPEAT) {   // v0.17.5: work before playback too
      PlayerLock lock;
      handleOnePlaybackAction(action);
      continue;
    }
    if (!playerStarted) {
      Serial.println("[PLAY] Button ignored: nothing is loaded yet.");
      continue;
    }
    if (action == ACTION_NEXT) Serial.println("[PLAY] Next requested.");
    else if (action == ACTION_PREVIOUS) Serial.println("[PLAY] Previous requested.");
    PlayerLock lock;
    if (action == ACTION_NEXT || action == ACTION_PREVIOUS) btFlushQueuedAudio();
    handleOnePlaybackAction(action);
  }
}

static void updateShuffleRepeatButtons() {
  if (!lcd_lvgl_lock(50)) return;
  applyShuffleRepeatButtonsLocked();
  lcd_lvgl_unlock();
}

static void applyShuffleRepeatButtonsLocked() {
  // v0.17.6: on = black on bright green; text states the setting.
  // v0.17.7: off uses the "Library choices" colours (SETTINGS -> Library choices).
  const uint32_t ON_BG = 0x2ECC71, ON_TEXT = 0x000000;
  const uint32_t OFF_BG = uiLibraryEntryBg, OFF_TEXT = uiLibraryEntryText;
  bool rOn = repeatMode != REPEAT_OFF;
  if (shuffleLabel) {
    lv_label_set_text(shuffleLabel, shuffleOn ? "SHUF ON" : "SHUF OFF");
    lv_obj_set_style_text_color(shuffleLabel, lv_color_hex(shuffleOn ? ON_TEXT : OFF_TEXT), LV_PART_MAIN);
  }
  if (shuffleButton) lv_obj_set_style_bg_color(shuffleButton, lv_color_hex(shuffleOn ? ON_BG : OFF_BG), LV_PART_MAIN);
  if (repeatLabel) {
    lv_label_set_text(repeatLabel, repeatMode == REPEAT_ALL ? "RPT ALL" : repeatMode == REPEAT_ONE ? "RPT 1" : "RPT OFF");
    lv_obj_set_style_text_color(repeatLabel, lv_color_hex(rOn ? ON_TEXT : OFF_TEXT), LV_PART_MAIN);
  }
  if (repeatButton) lv_obj_set_style_bg_color(repeatButton, lv_color_hex(rOn ? ON_BG : OFF_BG), LV_PART_MAIN);
}

static void saveShuffleRepeat() {
  playerPrefs.begin("wsplayer", false);
  if (playerPrefs.isKey("shuffle")) playerPrefs.remove("shuffle");   // v0.17.7: shuffle is no longer saved
  playerPrefs.putUChar("repeat", repeatMode);
  playerPrefs.end();
}

static void handleOnePlaybackAction(PlaybackAction action) {
  switch (action) {
    case ACTION_SHUFFLE:                                    // v0.17.5
      shuffleOn = !shuffleOn;
      rebuildPlayOrder();                                   // current track keeps playing
      Serial.printf("[PLAY] Shuffle %s.\n", shuffleOn ? "ON" : "OFF");
      saveShuffleRepeat();
      updateShuffleRepeatButtons();
      return;
    case ACTION_REPEAT:
      repeatMode = (uint8_t)((repeatMode + 1) % 3);
      Serial.printf("[PLAY] Repeat %s.\n", repeatMode == REPEAT_ALL ? "ALL" : repeatMode == REPEAT_ONE ? "ONE" : "OFF");
      saveShuffleRepeat();
      updateShuffleRepeatButtons();
      return;
    case ACTION_PREVIOUS:
      if (player.previous(1)) {
        playerStarted = true;
        pausedByUser = false;
        trackChanged = true;
      }
      break;
    case ACTION_TOGGLE:
      if (pausedByUser) {
        Serial.println("[PLAY] Resume requested.");
        player.setActive(true);
        pausedByUser = false;
      } else if (player.isActive()) {
        Serial.println("[PLAY] Pause requested.");
        player.setActive(false);
        pausedByUser = true;
      } else {
        Serial.println("[PLAY] Start requested.");
        player.setActive(true);
        pausedByUser = false;
      }
      trackChanged = true;
      break;
    case ACTION_NEXT:
      if (player.next(1)) {
        playerStarted = true;
        pausedByUser = false;
        trackChanged = true;
      }
      break;
    default:
      break;
  }
}

static void handlePendingSeek() {
  if (!pendingSeek) return;
  uint64_t target = pendingSeekMs;
  pendingSeek = false;
  PlayerLock lock;
  btFlushQueuedAudio();
  performSeek(target);
}

static void printPerfStats() {
  lcd_perf_t lcd;
  lcd_perf_take(&lcd);
  lv_mem_monitor_t mem = {};
  bool haveMem = false;
  if (lcd_lvgl_lock(20)) { lv_mem_monitor(&mem); lcd_lvgl_unlock(); haveMem = true; }
  uint32_t maxCopy = perfMaxCopyUs, maxGap = perfMaxCopyGapUs, gaps = perfIdleOver50ms;
  uint32_t maxWait = perfMaxPlayerWaitUs;
  perfMaxCopyUs = 0; perfMaxCopyGapUs = 0; perfMaxPlayerWaitUs = 0;
  Serial.printf("[PERF] loop: longest pass %lu ms, passes >100 ms %lu | audio: longest step %lu ms, "
                "longest idle between steps %lu ms, idle >50 ms %lu (total) | player-lock wait %lu ms\n",
                (unsigned long)(perfMaxLoopUs / 1000), (unsigned long)perfLoopsOver100ms,
                (unsigned long)(maxCopy / 1000), (unsigned long)(maxGap / 1000),
                (unsigned long)gaps, (unsigned long)(maxWait / 1000));
  Serial.printf("[PERF] LVGL: longest redraw %lu ms (%lu runs) | longest gap between touch reads %lu ms | "
                "LVGL lock wait %lu ms | touch I2C errors %lu (total)\n",
                (unsigned long)(lcd.max_handler_us / 1000), (unsigned long)lcd.handler_runs,
                (unsigned long)(lcd.max_touch_gap_us / 1000), (unsigned long)(lcd.max_lock_wait_us / 1000),
                (unsigned long)lcd.touch_errors);
  if (haveMem)
    Serial.printf("[PERF] LVGL memory: %u%% used (%u KB free of %u KB, largest free block %u KB)\n",
                  (unsigned)mem.used_pct, (unsigned)(mem.free_size / 1024), (unsigned)(mem.total_size / 1024),
                  (unsigned)(mem.free_biggest_size / 1024));
  perfMaxLoopUs = 0;
  perfLoopsOver100ms = 0;
}

void loop() {
  static bool reportedStopped = false;
  static uint32_t lastPcmDiagnosticMs = 0;
  static uint32_t lastPerfMs = 0;
  int64_t loopStartUs = esp_timer_get_time();
  // Press-to-handled timing for the most recent touch event.
  uint32_t eventMs = uiEventMs;
  const char *eventName = uiEventName;
  uint32_t handleStartMs = millis();
  processEncoderEvents();
  btServiceLoop();
  serviceOutputPage();
  servicePlaylistKeyboard();
  {                                            // v0.17.4: save the spectrum style 2 s after the last tap
    static uint32_t specSaveAt = 0;
    if (spectrumStyleSaveDue) { spectrumStyleSaveDue = false; specSaveAt = millis() + 2000; if (!specSaveAt) specSaveAt = 1; }
    if (specSaveAt && (int32_t)(millis() - specSaveAt) >= 0) {
      specSaveAt = 0;
      playerPrefs.begin("wsplayer", false);
      playerPrefs.putUChar("specstyle", spectrumStyle);
      playerPrefs.end();
      Serial.printf("[UI] Spectrum style: %s (saved)\n", SPECTRUM_STYLE_NAMES[spectrumStyle]);
    }
  }
  handlePlaybackActions();
  handleBrowserSelection();
  handleLibraryRescan();
  handlePendingSeek();

  // Decoding happens in audioTask; loop() only watches for the end of play.
  bool activeNow = playerActiveFlag;
  if (playerStarted && activeNow) {
    reportedStopped = false;
  } else if (playerStarted && pausedByUser) {
    // User pause: retain playerStarted so the Play button can resume.
  } else if (playerStarted && !reportedStopped) {
    uint64_t samples;
    portENTER_CRITICAL(&audioStatsMux);
    samples = decodedSampleValues;
    portEXIT_CRITICAL(&audioStatsMux);
    Serial.printf("[PLAY] Player became inactive. Decoded PCM sample values: %llu\n",
                  (unsigned long long)samples);
    if (samples == 0) {
      Serial.println("[PLAY] WARNING: No decoded PCM was received for this track.");
    }
    reportedStopped = true;
    playerStarted = false;
    pausedByUser = false;
    trackChanged = true;
  }

  if (trackChanged) updateMusicScreen();
  if (eventMs != 0 && eventMs == uiEventMs) {
    uiEventMs = 0;
    uint32_t doneMs = millis();
    Serial.printf("[UI] %s: waited %lu ms before handling, handled in %lu ms\n",
                  eventName ? eventName : "event", (unsigned long)(handleStartMs - eventMs),
                  (unsigned long)(doneMs - handleStartMs));
  }
  uint32_t now = millis();
  if (playerStarted && now - lastUiUpdateMs >= 150) {
    lastUiUpdateMs = now;
    updatePlaybackDisplay();
  }
  if (playerStarted && playerActiveFlag && now - lastPcmDiagnosticMs >= 10000) {
    lastPcmDiagnosticMs = now;
    uint64_t samples;
    portENTER_CRITICAL(&audioStatsMux);
    samples = decodedSampleValues;
    portEXIT_CRITICAL(&audioStatsMux);
    Serial.printf("[PLAY] PCM sample values decoded so far: %llu\n",
                  (unsigned long long)samples);
  }
  if (now - lastPerfMs >= 10000) {
    lastPerfMs = now;
    printPerfStats();
    if (outputMode == OUTPUT_BLUETOOTH) printBtStatus();
    else {
      static uint64_t lastWired = 0;
      uint64_t w = wiredBytesWritten;
      if (playerStarted)
        Serial.printf("[WIRED] %llu bytes/s written to the headphone output (real time for 44.1 kHz stereo = 176400)\n",
                      (unsigned long long)((w - lastWired) / 10));
      lastWired = w;
    }
  }
  uint32_t loopUs = (uint32_t)(esp_timer_get_time() - loopStartUs);
  if (loopUs > perfMaxLoopUs) perfMaxLoopUs = loopUs;
  if (loopUs > 100000) ++perfLoopsOver100ms;
  delay(1);
}
