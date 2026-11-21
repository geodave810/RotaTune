#pragma once
/*
  Waveshare Music Player - user-adjustable UI configuration
  ----------------------------------------------------------
  This file is intended for small layout and appearance adjustments without
  editing the main player sketch.

  DISPLAY COORDINATES
    The display is 360 x 360 pixels.
    X positions are offsets from the horizontal center when LV_ALIGN_TOP_MID
    is used: negative = left, positive = right.
    Y positions are pixels down from the top edge.

  COLORS
    RGB888 format: 0xRRGGBB. Example: 0x20D6B0 is a turquoise/green.
    The touchscreen Appearance page and player_colors.txt can override the
    default colors and font sizes below.

  BUTTON / LAYOUT SIZES
    Width and height are pixels. Increase width to give text more room.
    Keep controls within the 0..359 display area and avoid overlapping buttons.

  FONT SIZES
    Must match a font included in playerFont() in the main sketch:
    12, 14, 16, 18, 20, 22, 24, 26, or 28.
*/
namespace PlayerUIConfig {

// ------------------------- Default colors -------------------------------
// Main SELECT MUSIC page: heading and four library-choice buttons.
constexpr unsigned long DEFAULT_LIBRARY_HEADER_BG   = 0x90EE90; // Light green
constexpr unsigned long DEFAULT_LIBRARY_HEADER_TEXT = 0x000000; // Black
constexpr unsigned long DEFAULT_LIBRARY_ENTRY_BG    = 0x173B55; // Dark blue
constexpr unsigned long DEFAULT_LIBRARY_ENTRY_TEXT  = 0xFFFFFF; // White

// Browser rows and general browser controls.
constexpr unsigned long DEFAULT_BROWSER_ENTRY_BG    = 0x173B55; // Dark blue
constexpr unsigned long DEFAULT_BROWSER_ENTRY_TEXT  = 0xFFFFFF; // White
constexpr unsigned long DEFAULT_NAV_BUTTON_BG       = 0xADD8E6; // Light blue
constexpr unsigned long DEFAULT_NAV_BUTTON_TEXT     = 0x000000; // Black
constexpr unsigned long DEFAULT_NAV_DISABLED_BG     = 0xADD8E6; // Light blue: unavailable-page highlight background
constexpr unsigned long DEFAULT_NAV_DISABLED_TEXT   = 0x000000; // Black: unavailable-page highlight text
constexpr unsigned long DEFAULT_BROWSER_CONTROL_BG  = 0x28533D; // Dark green
constexpr unsigned long DEFAULT_BROWSER_CONTROL_TEXT= 0xFFFFFF; // White

// Playback screen defaults.
constexpr unsigned long DEFAULT_PLAYBACK_BACKGROUND = 0x080D13; // Very dark blue-black
constexpr unsigned long DEFAULT_PLAYBACK_TITLE_TEXT = 0xFFFFFF; // White
constexpr unsigned long DEFAULT_PLAYBACK_BUTTON_BG  = 0x075A9C; // Deep blue
constexpr unsigned long DEFAULT_PLAYBACK_BUTTON_TEXT= 0xFFFFFF; // White
constexpr unsigned long DEFAULT_PLAYBACK_STATUS_BG  = 0x080D13; // Very dark blue-black
constexpr unsigned long DEFAULT_PLAYBACK_STATE_TEXT = 0x20D6B0; // Turquoise

// -------------------------- Default fonts -------------------------------
// These are starting defaults only. Saved touchscreen settings override them.
constexpr unsigned char FONT_LIBRARY_HEADER   = 18;
constexpr unsigned char FONT_LIBRARY_ENTRY    = 20;
constexpr unsigned char FONT_BROWSER_ENTRY    = 18;
constexpr unsigned char FONT_NAV_BUTTON       = 16;
constexpr unsigned char FONT_BROWSER_CONTROL  = 16;
constexpr unsigned char FONT_PLAYBACK_TITLE   = 20;
constexpr unsigned char FONT_PLAYBACK_BUTTON  = 16;
constexpr unsigned char FONT_PLAYBACK_STATUS  = 16;

// ---------------------- Main library choice list -----------------------
// Four green buttons on SELECT MUSIC. The row buttons are inside this list.
constexpr int LIBRARY_LIST_WIDTH       = 344;
constexpr int LIBRARY_LIST_HEIGHT      = 170;
constexpr int LIBRARY_BUTTON_WIDTH     = 336;
constexpr int LIBRARY_BUTTON_HEIGHT    = 30;
constexpr int LIBRARY_BUTTON_X         = 4;    // left inset inside list
constexpr int LIBRARY_BUTTON_ROW_STEP  = 33;   // vertical distance between rows
constexpr int LIBRARY_BUTTON_LABEL_WIDTH = 316;

// ------------------------- Browser controls -----------------------------
// RESCAN and SETTINGS sit side-by-side under BACK / PLAYING.
constexpr int RESCAN_SETTINGS_WIDTH  = 102;  // was 82; +20 px
constexpr int RESCAN_SETTINGS_HEIGHT = 28;
constexpr int RESCAN_X               = -58;  // center of RESCAN, left of center
constexpr int SETTINGS_X             = 58;   // center of SETTINGS, right of center
constexpr int RESCAN_SETTINGS_Y      = 88;

// ------------------------- Alphabet browser -----------------------------
// Moves the complete A-Z/# grid down one button row from its prior position.
// Panel Y was 100; 132 moves it down 32 px. Keep panel height large enough
// for all five rows of six buttons.
constexpr int ALPHABET_PANEL_WIDTH  = 284;
constexpr int ALPHABET_PANEL_HEIGHT = 170;
constexpr int ALPHABET_PANEL_Y      = 132;
constexpr int ALPHABET_BUTTON_WIDTH = 42;
constexpr int ALPHABET_BUTTON_HEIGHT= 27;
constexpr int ALPHABET_BUTTON_X     = 4;
constexpr int ALPHABET_BUTTON_Y     = 3;
constexpr int ALPHABET_COLUMN_STEP  = 46;
constexpr int ALPHABET_ROW_STEP     = 32;

// ---------------------- Appearance settings screen ---------------------
// Buttons use LV_ALIGN_TOP_MID; X is offset from screen center, Y from top.
constexpr int SETTINGS_TITLE_Y = 4;
constexpr int SETTINGS_ELEMENT_Y = 27;
constexpr int SETTINGS_PREV_NEXT_WIDTH = 72;
constexpr int SETTINGS_PREV_NEXT_HEIGHT = 27;
constexpr int SETTINGS_PREV_X = -88;
constexpr int SETTINGS_NEXT_X = 88;
constexpr int SETTINGS_PREV_NEXT_Y = 48;
constexpr int SETTINGS_PREVIEW_WIDTH = 224;
constexpr int SETTINGS_PREVIEW_HEIGHT = 38;
constexpr int SETTINGS_PREVIEW_Y = 80;
constexpr int SETTINGS_MODE_Y = 122;
constexpr int SETTINGS_BACKGROUND_WIDTH = 112;
constexpr int SETTINGS_BACKGROUND_X = -62;
constexpr int SETTINGS_TEXT_WIDTH = 70;
constexpr int SETTINGS_TEXT_X = 62;
constexpr int SETTINGS_MODE_BUTTON_HEIGHT = 27;
constexpr int SETTINGS_MODE_BUTTON_Y = 143;
constexpr int SETTINGS_FONT_BUTTON_WIDTH = 48;
constexpr int SETTINGS_FONT_BUTTON_HEIGHT = 25;
constexpr int SETTINGS_FONT_DOWN_X = -65;
constexpr int SETTINGS_FONT_UP_X = 65;
constexpr int SETTINGS_FONT_BUTTON_Y = 176;
constexpr int SETTINGS_FONT_LABEL_Y = 181;
constexpr int SETTINGS_SAVE_BACK_WIDTH = 64;
constexpr int SETTINGS_SAVE_BACK_HEIGHT = 25;
constexpr int SETTINGS_SAVE_X = -132;
constexpr int SETTINGS_BACK_X = 132;
constexpr int SETTINGS_SAVE_BACK_Y = 176;
constexpr int SETTINGS_RGB_Y[3] = {210, 234, 258};
constexpr int SETTINGS_RGB_LABEL_X = 40;
constexpr int SETTINGS_RGB_SLIDER_X = 64;
constexpr int SETTINGS_RGB_SLIDER_WIDTH = 196;
constexpr int SETTINGS_RGB_VALUE_X = 272;
constexpr int SETTINGS_HEX_LABEL_Y = 278;
constexpr int SETTINGS_STATUS_LABEL_Y = 300;
constexpr int SETTINGS_DEFAULT_BUTTON_WIDTH = 88;
constexpr int SETTINGS_DEFAULT_BUTTON_HEIGHT = 27;
constexpr int SETTINGS_DEFAULT_BUTTON_Y = 326;

// ------------------------- Browser top buttons ---------------------------
// BACK and PLAYING row at top of browser screens.
constexpr int BROWSER_BACK_WIDTH = 72;
constexpr int BROWSER_BACK_HEIGHT = 34;
constexpr int BROWSER_BACK_X = -48;
constexpr int BROWSER_BACK_Y = 50;
constexpr int BROWSER_PLAYING_WIDTH = 84;
constexpr int BROWSER_PLAYING_HEIGHT = 34;
constexpr int BROWSER_PLAYING_X = 48;
constexpr int BROWSER_PLAYING_Y = 50;

// PREV/NEXT page navigation.
constexpr int BROWSER_PAGE_BUTTON_WIDTH = 70;
constexpr int BROWSER_PAGE_BUTTON_HEIGHT = 30;
constexpr int BROWSER_PAGE_PREV_X = 76;
constexpr int BROWSER_PAGE_NEXT_X = 214;
constexpr int BROWSER_PAGE_BUTTON_Y = 294;

// ------------------------- Playback controls ----------------------------
// Three main playback buttons. Their horizontal offsets are passed by the
// sketch (-92, 0, +92); this controls their common size and vertical position.
constexpr int PLAYBACK_BUTTON_WIDTH  = 76;
constexpr int PLAYBACK_BUTTON_HEIGHT = 42;
constexpr int PLAYBACK_BUTTON_Y      = 232;

} // namespace PlayerUIConfig
