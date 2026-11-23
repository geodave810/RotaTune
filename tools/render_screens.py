"""Renders representative mock-ups of the RotaTune screens.

Layout values (positions, sizes, colours, font sizes, labels) are copied from
RotaTune.ino (v0.17.7 layout) and player_ui_config.h (default colours).
Text uses Inter in place of LVGL's Montserrat, so glyph widths differ slightly.
All names, titles and numbers are invented sample data.
"""
import math, random
from PIL import Image, ImageDraw, ImageFont, ImageFilter

SS = 4            # supersampling factor
W = 360           # native display size
OUT = 720         # published image size (2x native)
FONT = "/usr/share/fonts/opentype/inter/Inter-Medium.otf"
_fc = {}

def font(px):
    if px not in _fc:
        _fc[px] = ImageFont.truetype(FONT, int(px * SS * 0.92))
    return _fc[px]

def hexc(v):
    return ((v >> 16) & 255, (v >> 8) & 255, v & 255)

# ---- colours (defaults from the source) ----
DEEP_NAVY = 0x080D13; LIB_HDR_BG = 0x90EE90; LIB_HDR_TX = 0x000000
LIB_BG = 0x173B55; LIB_TX = 0xFFFFFF; ENTRY_BG = 0x173B55; ENTRY_TX = 0xFFFFFF
NAV_BG = 0xADD8E6; NAV_TX = 0x000000; CTRL_BG = 0x28533D; CTRL_TX = 0xFFFFFF
PB_BG = 0x080D13; PB_TITLE = 0xFFFFFF; PB_BTN = 0x075A9C; PB_TOGGLE = 0x087A50
PB_STATE = 0x20D6B0; VOL_TX = 0x9FB3C8; TRACK_TX = 0xDCE6F1
PROG_BG = 0x263342; PROG_FILL = 0x2997F0; EQ_BASE = 0x24313D
EQ_PAL = [0xA66BDF, 0x946DEB, 0x806FF4, 0x687AF7, 0x4D91F4, 0x35A9E8, 0x25C4D2, 0x20D6A5]
ON_BG = 0x2ECC71; ON_TX = 0x000000


class Screen:
    def __init__(self, bg):
        self.img = Image.new("RGB", (W * SS, W * SS), hexc(bg))
        self.d = ImageDraw.Draw(self.img)

    def rect(self, x, y, w, h, color, radius=0, opa=255):
        box = [x * SS, y * SS, (x + w) * SS - 1, (y + h) * SS - 1]
        if opa >= 255:
            self.d.rounded_rectangle(box, radius=radius * SS, fill=hexc(color))
        else:
            layer = Image.new("RGBA", self.img.size, (0, 0, 0, 0))
            ImageDraw.Draw(layer).rounded_rectangle(box, radius=radius * SS, fill=hexc(color) + (opa,))
            self.img.paste(Image.alpha_composite(self.img.convert("RGBA"), layer).convert("RGB"))
            self.d = ImageDraw.Draw(self.img)

    def text(self, s, x, y, w, h, px, color, align="center", spacing=0):
        """Draw text vertically centred in the box (x, y, w, h)."""
        f = font(px)
        asc, desc = f.getmetrics()
        th = asc + desc
        ty = y * SS + (h * SS - th) / 2
        if spacing:
            widths = [f.getlength(c) for c in s]
            tw = sum(widths) + spacing * SS * (len(s) - 1)
        else:
            tw = f.getlength(s)
        maxw = w * SS
        if tw > maxw and not spacing:          # LV_LABEL_LONG_DOT
            while s and f.getlength(s + "...") > maxw:
                s = s[:-1]
            s = s.rstrip() + "..."
            tw = f.getlength(s)
        if align == "center":
            tx = x * SS + (maxw - tw) / 2
        elif align == "right":
            tx = x * SS + maxw - tw
        else:
            tx = x * SS
        if spacing:
            for c, cw in zip(s, widths):
                self.d.text((tx, ty), c, font=f, fill=hexc(color))
                tx += cw + spacing * SS
        else:
            self.d.text((tx, ty), s, font=f, fill=hexc(color))

    def button(self, x, y, w, h, bg, label, tx, px, radius=8):
        self.rect(x, y, w, h, bg, radius)
        self.text(label, x, y, w, h, px, tx)

    def line(self, pts, color, width):
        self.d.line([(px * SS, py * SS) for px, py in pts], fill=hexc(color), width=int(width * SS))

    def finish(self, path):
        img = self.img.resize((OUT, OUT), Image.LANCZOS)
        # Round 1.8" 360x360 display: circular mask on a dark bezel.
        S2 = OUT * 4
        mask = Image.new("L", (S2, S2), 0)
        ImageDraw.Draw(mask).ellipse([0, 0, S2 - 1, S2 - 1], fill=255)
        mask = mask.resize((OUT, OUT), Image.LANCZOS)
        pad = 40
        canvas = Image.new("RGBA", (OUT + 2 * pad, OUT + 2 * pad), (0, 0, 0, 0))
        bsz = OUT + 2 * pad - 8
        bezel = Image.new("L", (bsz * 4, bsz * 4), 0)
        ImageDraw.Draw(bezel).ellipse([0, 0, bsz * 4 - 1, bsz * 4 - 1], fill=255)
        bezel = bezel.resize((bsz, bsz), Image.LANCZOS)
        ring = Image.new("RGBA", (bsz, bsz), (28, 30, 34, 255))
        canvas.paste(ring, (4, 4), bezel)
        screen = img.convert("RGBA")
        canvas.paste(screen, (pad, pad), mask)
        canvas.save(path)


# ---------------------------------------------------------------- browser
def top_mid(w, xoff):
    return (W - w) // 2 + xoff


def browser_base(heading, shuffle=False, repeat="OFF", show_rescan=False):
    s = Screen(DEEP_NAVY)
    # heading: width 180, centred, y=20, font 18, pad 6/2
    hh = 22 + 4
    s.rect(top_mid(180, 0), 20, 180, hh, LIB_HDR_BG, 6)
    s.text(heading, top_mid(180, 0) + 6, 20, 168, hh, 18, LIB_HDR_TX)
    # BACK / PLAYING
    s.button(top_mid(72, -48), 50, 72, 34, CTRL_BG, "BACK", CTRL_TX, 16, 9)
    s.button(top_mid(84, 48), 50, 84, 34, CTRL_BG, "PLAYING", CTRL_TX, 16, 9)
    # SHUF / SETTINGS / RPT
    s.button(top_mid(102, 0), 88, 102, 28, CTRL_BG, "SETTINGS", CTRL_TX, 16, 8)
    on = shuffle
    s.button(top_mid(98, -104), 88, 98, 28, ON_BG if on else LIB_BG,
             "SHUF ON" if on else "SHUF OFF", ON_TX if on else LIB_TX, 16, 8)
    ron = repeat != "OFF"
    s.button(top_mid(98, 104), 88, 98, 28, ON_BG if ron else LIB_BG,
             "RPT " + repeat, ON_TX if ron else LIB_TX, 16, 8)
    if show_rescan:
        s.button(top_mid(84, 0), 327, 84, 24, CTRL_BG, "RESCAN", CTRL_TX, 16, 8)
    return s


def browser_rows(s, rows, page, pages, library=False):
    bg, tx, px = (LIB_BG, LIB_TX, 20) if library else (ENTRY_BG, ENTRY_TX, 18)
    for i, label in enumerate(rows[:5]):
        x, y = 8 + 4 + 4, 116 + 4 + i * 33
        s.rect(x, y, 336, 30, bg, 8)
        s.text(label, x + 13 + 8, y, 308, 30, px, tx, align="left")
    # PREV / page / NEXT
    s.button(76, 294, 70, 30, NAV_BG, "PREV", NAV_TX, 16)
    s.button(214, 294, 70, 30, NAV_BG, "NEXT", NAV_TX, 16)
    s.text(f"{page} / {pages}", top_mid(80, 0), 301, 80, 17, 14, 0xFFFFFF)


def scr_root(path):
    s = browser_base("SELECT MUSIC", show_rescan=True)
    browser_rows(s, ["LIBRARY  Classical", "LIBRARY  Jazz", "LIBRARY  Rock",
                     "LIBRARY  Spoken_Word", "PLAYLISTS"], 1, 2, library=True)
    s.finish(path)


def scr_collection(path):
    s = browser_base("Rock")
    browser_rows(s, ["MUSIC  Browse Artists", "FILES  Browse Folders"], 1, 1, library=True)
    s.finish(path)


def scr_letters(path):
    s = browser_base("BROWSE ARTISTS")
    px0, py0 = top_mid(284, 0), 132
    for i in range(27):
        col, row = i % 6, i // 6
        letter = chr(65 + i) if i < 26 else "#"
        s.button(px0 + 4 + col * 46, py0 + 3 + row * 32, 42, 27, CTRL_BG, letter, CTRL_TX, 16, 6)
    s.finish(path)


def scr_albums(path):
    s = browser_base("The Blue Lanterns")
    browser_rows(s, ["Harbor Lights (2014)", "Midnight Harbor", "Paper Moons",
                     "Static & Starlight", "Winter Radio"], 1, 1)
    s.finish(path)


def scr_tracks(path):
    s = browser_base("Midnight Harbor", shuffle=True, repeat="ALL")
    browser_rows(s, ["PLAY  ALBUM", "Lighthouse Song", "Northbound Train",
                     "Copper Skies", "The Long Way Home"], 1, 3)
    s.finish(path)


def scr_playlists(path):
    s = browser_base("PLAYLISTS")
    browser_rows(s, ["NEW PLAYLIST", "Dinner Party", "Road Trip",
                     "Sunday Morning", "Workout Mix"], 1, 1)
    s.finish(path)


def scr_playlist(path):
    s = browser_base("Road Trip")
    browser_rows(s, ["PLAY  ALL", "ADD TRACKS", "DELETE PLAYLIST",
                     "Northbound Train", "Open Highway"], 1, 4)
    s.finish(path)


def scr_output(path):
    s = browser_base("OUTPUT")
    browser_rows(s, ["USE  WIRED", "USING  BLUETOOTH",
                     "SPEAKER  Kitchen  - playing",
                     "SPEAKER  Travel Earbuds", "FIND SPEAKERS"], 1, 2)
    s.finish(path)


# ---------------------------------------------------------------- playback
EQ_LEFT = (360 - (32 * 5 + 31 * 2)) // 2
EQ_W = 32 * 5 + 31 * 2
BASE = 96
SPAN = 56
MAXH = 54


REST = False

def sample_levels(seed=3):
    if REST:
        return [1] * 32, [1] * 32
    rnd = random.Random(seed)
    bars, peaks = [], []
    for i in range(32):
        env = 0.9 * math.exp(-i / 14.0) + 0.12
        v = min(1.0, env * (0.65 + 0.5 * rnd.random()))
        h = max(1, int(v * MAXH))
        bars.append(h)
        peaks.append(min(MAXH, h + rnd.randint(2, 9)))
    return bars, peaks


def col(i):
    return EQ_PAL[(i * 8) // 32]


def draw_spectrum(s, style, seed=3):
    bars, peaks = sample_levels(seed)
    x1, y1, y2 = EQ_LEFT, BASE - SPAN, BASE + SPAN - 1
    if style == "MIRROR":
        for i in range(32):
            x = x1 + i * 7
            h, p = bars[i], peaks[i]
            s.rect(x, BASE - h, 5, 2 * h, col(i))
            s.rect(x, BASE - p - 1, 5, 2, col(i))
    elif style == "BARS":
        for i in range(32):
            x = x1 + i * 7
            h, p = bars[i] * 2, peaks[i] * 2
            s.rect(x, y2 - h + 1, 5, h, col(i))
            s.rect(x, y2 - p - 1, 5, 2, col(i))
    elif style == "RADIAL":
        cx, cy = x1 + EQ_W // 2, BASE
        r0, rmax = 14, SPAN - 3
        for i in range(32):
            a = -math.pi / 2 + i * 2 * math.pi / 32
            ln = bars[i] * (rmax - r0) // MAXH
            pl = peaks[i] * (rmax - r0) // MAXH
            ca, sa = math.cos(a), math.sin(a)
            s.line([(cx + ca * r0, cy + sa * r0), (cx + ca * (r0 + ln), cy + sa * (r0 + ln))], col(i), 3)
            px, py = cx + ca * (r0 + pl + 2), cy + sa * (r0 + pl + 2)
            s.rect(int(px) - 1, int(py) - 1, 2, 2, col(i))
    elif style == "SCOPE":
        pts = []
        for i in range(64):
            t = i / 63
            v = 0.55 * math.sin(2 * math.pi * 3.2 * t) + 0.25 * math.sin(2 * math.pi * 7.7 * t + 1) \
                + 0.08 * math.sin(2 * math.pi * 19 * t)
            v = max(-1, min(1, v))
            pts.append((x1 + i * (EQ_W - 1) / 63, BASE - v * (SPAN - 2)))
        s.line(pts, col(12), 2)
    elif style == "LEVELS":
        segs = EQ_W // 9
        for ch, (lit, pk) in enumerate([(17, 19), (15, 18)]):
            yy = BASE - 22 if ch == 0 else BASE + 6
            for k in range(segs):
                on = k < lit or k == pk
                c = 0x22CC55 if k < segs * 7 // 10 else (0xE8C020 if k < segs * 9 // 10 else 0xE03030)
                s.rect(x1 + k * 9, yy, 8, 16, c, opa=255 if on else 51)


def scr_playing(path, style="MIRROR", bt=False, state="PLAYING", show_name=False):
    global REST
    REST = state != "PLAYING"
    s = Screen(PB_BG)
    s.text("BT VOLUME 40%" if bt else "VOLUME 45%", 0, 22, 360, 22, 18, VOL_TX)
    if style != "OFF":
        s.rect(EQ_LEFT - 4, BASE - 1, EQ_W + 8, 2, EQ_BASE)
        draw_spectrum(s, style)
    else:
        s.rect(EQ_LEFT - 4, BASE - 1, EQ_W + 8, 2, EQ_BASE)
    if show_name:
        s.text(style if style != "OFF" else "SPECTRUM OFF", EQ_LEFT, BASE - SPAN, EQ_W, 19, 14, 0xFFFFFF)
    # title (330 x 72 at y=125, font 20, centred)
    s.text("Lighthouse Song", 15, 125, 330, 72, 20, PB_TITLE)
    s.text("TRACK 3 OF 12", 42, 198, 150, 24, 18, TRACK_TX, align="left")
    s.button(360 - 42 - 100, 194, 100, 34, 0x075A9C, "BROWSE", 0xFFFFFF, 14, 9)
    for xo, cap, c in [(-92, "|<", PB_BTN), (0, "PAUSE" if state == "PLAYING" else "PLAY", PB_TOGGLE),
                       (92, ">|", PB_BTN)]:
        s.button(top_mid(76, xo), 232, 76, 42, c, cap, 0xFFFFFF, 16, 12)
    s.text("1:42", 42, 280, 58, 22, 18, 0xFFFFFF, align="left")
    s.text("4:07", 360 - 42 - 58, 280, 58, 22, 18, VOL_TX, align="right")
    s.rect(55, 305, 250, 14, PROG_BG, 4)
    s.rect(55, 305, int(250 * 102 / 247), 14, PROG_FILL, 4)
    s.text(state, 0, 330, 360, 20, 16, PB_STATE, spacing=2)
    s.finish(path)


# ---------------------------------------------------------------- settings
def scr_settings(path):
    s = Screen(0x080D13)
    s.text("PLAYER APPEARANCE", 0, 4, 360, 17, 14, 0xFFFFFF)
    s.text("Library choices  (2/9)", 0, 27, 360, 17, 14, 0xDCE6F1)
    sb = 0x24465F
    s.button(top_mid(72, -88), 48, 72, 27, sb, "< PREV", 0xFFFFFF, 14, 7)
    s.button(top_mid(72, 88), 48, 72, 27, sb, "NEXT >", 0xFFFFFF, 14, 7)
    px, pw = top_mid(224, 0), 224
    s.rect(px, 80, pw, 38, 0xFFFFFF, 8)
    s.rect(px + 2, 82, pw - 4, 34, LIB_BG, 6)
    s.text("Aa  BUTTON  123", px, 80, pw, 38, 20, LIB_TX)
    s.text("EDITING BACKGROUND COLOR", 0, 122, 360, 15, 12, 0xDCE6F1)
    s.button(top_mid(112, -62), 143, 112, 27, sb, "BACKGROUND", 0xFFFFFF, 14, 7)
    s.button(top_mid(70, 62), 143, 70, 27, sb, "TEXT", 0xFFFFFF, 14, 7)
    s.button(top_mid(48, -65), 176, 48, 25, sb, "A-", 0xFFFFFF, 14, 7)
    s.text("SIZE 20", 0, 181, 360, 17, 14, 0xFFFFFF)
    s.button(top_mid(48, 65), 176, 48, 25, sb, "A+", 0xFFFFFF, 14, 7)
    s.button(top_mid(64, -132), 176, 64, 25, sb, "SAVE", 0xFFFFFF, 14, 7)
    s.button(top_mid(64, 132), 176, 64, 25, sb, "BACK", 0xFFFFFF, 14, 7)
    vals = [0x17, 0x3B, 0x55]
    for i, (n, y) in enumerate(zip("RGB", [210, 234, 258])):
        s.text(n, 40, y - 7, 14, 17, 14, 0xFFFFFF, align="left")
        s.rect(64, y, 196, 10, 0x2196F3, 5, opa=60)
        fw = int(196 * vals[i] / 255)
        s.rect(64, y, max(fw, 10), 10, 0x2196F3, 5)
        kx = 64 + fw
        s.d.ellipse([(kx - 10) * SS, (y - 5) * SS, (kx + 10) * SS, (y + 15) * SS], fill=hexc(0x2196F3))
        s.text(f"{vals[i]:02X}", 272, y - 7, 30, 15, 12, 0xFFFFFF, align="left")
    s.text("BACKGROUND  #173B55", 0, 278, 360, 17, 14, 0xFFFFFF)
    s.text("ADJUST COLORS AND SAVE", top_mid(260, 0), 300, 260, 15, 12, 0xDCE6F1)
    s.button(top_mid(88, 0), 326, 88, 27, sb, "DEFAULT", 0xFFFFFF, 14, 7)
    s.finish(path)


if __name__ == "__main__":
    import os, sys
    out = sys.argv[1] if len(sys.argv) > 1 else "."
    os.makedirs(out, exist_ok=True)
    j = lambda n: os.path.join(out, n)
    scr_root(j("01_select_music.png"))
    scr_collection(j("02_library_menu.png"))
    scr_letters(j("03_browse_artists.png"))
    scr_albums(j("04_albums.png"))
    scr_tracks(j("05_album_tracks.png"))
    scr_playing(j("06_now_playing.png"))
    scr_playing(j("07_spectrum_bars.png"), "BARS", show_name=True)
    scr_playing(j("08_spectrum_radial.png"), "RADIAL", show_name=True)
    scr_playing(j("09_spectrum_scope.png"), "SCOPE", show_name=True)
    scr_playing(j("10_spectrum_levels.png"), "LEVELS", show_name=True)
    scr_playing(j("11_bluetooth_paused.png"), "MIRROR", bt=True, state="PAUSED")
    scr_playlists(j("12_playlists.png"))
    scr_playlist(j("13_playlist.png"))
    scr_output(j("14_output.png"))
    scr_settings(j("15_settings.png"))
    print("done")
