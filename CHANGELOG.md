# Changelog

## 0.18.0: first public release
- Libraries are found on the SD card: each folder in `Music_Library` is a library (up to 8).
- SHUF/RPT buttons are labelled correctly at start-up.

## 0.17.x
- 0.17.7:
  - Shuffle starts off at power-up and is no longer saved.
  - Wider SHUF/RPT buttons with the same text size as SETTINGS.
  - Their OFF state uses the library colours.
- 0.17.6: SHUF and RPT moved beside SETTINGS; the label shows the current setting.
- 0.17.5: Shuffle and repeat (off / all / one).
- 0.17.4: Spectrum styles (Mirror, Bars, Radial, Scope, Levels, Off). Tap the spectrum to change.
- 0.17.3:
  - The knob turns pages in the browser.
  - RESCAN moved to the bottom and needs a second tap to confirm.
- 0.17.2:
  - M3U playlists: create, play, add tracks, delete.
  - Tap the volume line to open the Output page.

## 0.16.x
- 0.16.9:
  - Output page: wired or Bluetooth, find speakers, two saved speakers, forget.
  - Needs the ESP32 receiver v0.5.0.
- 0.16.8:
  - Bluetooth output through the board's ESP32: UART link protocol v3 with flow control, resampling to 44.1 kHz.
  - The knob sets the speaker's volume over AVRCP.
- 0.16.7:
  - MP3 decoding in its own task, so browsing no longer interrupts audio.
  - Spectrum drawn as a single object.
  - Library index kept in PSRAM.
  - PLAY FOLDER added.
  - Automatic advance to the next track.
