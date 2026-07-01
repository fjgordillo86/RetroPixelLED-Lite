#!/usr/bin/env python3
"""
generate_clock_themes.py — Retro Game Pixel Art Clock Generator
For 128x32 DMD (HUB75) on ESP32, compiles via arduino-cli

Usage:
    python generate_clock_themes.py                     # Generate clock_themes.h + .ino
    python generate_clock_themes.py --compile           # Generate + compile
    python generate_clock_themes.py --compile COM4      # Generate + compile + upload
    python generate_clock_themes.py --help              # This help

Config (config.ini on SD card root):
    [CLOCK]
    CLOCK_ENABLED=1          # 0=OFF, 1=ON
    CLOCK_THEME=-1           # -1=random, 0=Mario, 1=Tetris, 2=Pac-Man,
                             # 3=Space Invaders, 4=Pong,
                             # 5=Neon, 6=Matrix, 7=Fire, 8=Rainbow
    CLOCK_DURATION=10        # seconds per theme (when random)
    CLOCK_INTERVAL_MIN=0     # 0=continuous, >0=show every N minutes
    TZ=CET-1CEST,...         # timezone
    brightness=80            # 0-100%
    wifi_enabled=1           # 0/1
    wifi_ssid=...            # WiFi name
    wifi_password=...        # WiFi password

Integration into RecalBox_DMD.ino:
    1. Copy "clock_themes.h" into the sketch folder
    2. Add '#include "clock_themes.h"' at the top
    3. Replace the existing clock drawing in showClock() with:
         drawRetroClockTheme(random(0, RETRO_THEME_COUNT), h, m, s, millis());
    4. The 6 themes auto-rotate. All existing clock config (CLOCK_ENABLED,
       CLOCK_INTERVAL, etc.) stays the same.
    5. Customize pixel art in THIS file (generate_clock_themes.py) then
       regenerate: `python generate_clock_themes.py`
"""

import os, sys, subprocess, struct, math

SKETCH_DIR = os.path.dirname(os.path.abspath(__file__))
OUTPUT_H = os.path.join(SKETCH_DIR, "clock_themes.h")
SKETCH_INO = os.path.join(SKETCH_DIR, "retro_clock.ino")

# ── RGB565 helper ──────────────────────────────────────────────
def rgb565(r, g, b):
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)

# ── Font 5x7 (classic pixel font) ─────────────────────────────
FONT_5x7 = {
    '0': [0x0E,0x11,0x13,0x15,0x19,0x11,0x0E],
    '1': [0x04,0x0C,0x04,0x04,0x04,0x04,0x0E],
    '2': [0x0E,0x11,0x01,0x02,0x04,0x08,0x1F],
    '3': [0x1E,0x01,0x06,0x01,0x01,0x01,0x1E],
    '4': [0x02,0x06,0x0A,0x12,0x1F,0x02,0x02],
    '5': [0x1F,0x10,0x1E,0x01,0x01,0x01,0x1E],
    '6': [0x0E,0x10,0x1E,0x11,0x11,0x11,0x0E],
    '7': [0x1F,0x01,0x02,0x04,0x04,0x04,0x04],
    '8': [0x0E,0x11,0x0E,0x11,0x11,0x11,0x0E],
    '9': [0x0E,0x11,0x0F,0x01,0x01,0x01,0x0E],
    ':': [0x00,0x00,0x04,0x00,0x00,0x04,0x00],
}

# ── Color palettes per theme ──────────────────────────────────
C = type('C',(),{})

# Mario
C.MARIO_SKY    = rgb565(100,180,255)
C.MARIO_RED    = rgb565(200,16,16)
C.MARIO_BLUE   = rgb565(32,48,200)
C.MARIO_SKIN   = rgb565(252,188,120)
C.MARIO_BROWN  = rgb565(120,72,24)
C.MARIO_YELLOW = rgb565(252,220,32)
C.MARIO_WHITE  = rgb565(240,240,240)
C.MARIO_BLACK  = rgb565(0,0,0)
C.MARIO_GREEN  = rgb565(32,160,32)
C.MARIO_BRICK  = rgb565(180,120,60)
C.MARIO_CLOUD  = rgb565(220,220,240)

# Tetris
C.TETRIS_BG    = rgb565(0,0,20)
C.TETRIS_CYAN  = rgb565(0,255,255)
C.TETRIS_YELLOW= rgb565(255,255,0)
C.TETRIS_PURPLE= rgb565(180,0,255)
C.TETRIS_GREEN = rgb565(0,220,0)
C.TETRIS_RED   = rgb565(220,0,0)
C.TETRIS_BLUE  = rgb565(0,80,255)
C.TETRIS_ORANGE= rgb565(255,160,0)
C.TETRIS_GREY  = rgb565(40,40,60)

# Pac-Man
C.PAC_BG       = rgb565(0,0,0)
C.PAC_YELLOW   = rgb565(255,220,0)
C.PAC_RED      = rgb565(255,0,0)
C.PAC_PINK     = rgb565(255,120,200)
C.PAC_CYAN     = rgb565(0,200,255)
C.PAC_ORANGE   = rgb565(255,160,60)
C.PAC_WHITE    = rgb565(240,240,240)
C.PAC_BLUE     = rgb565(0,0,200)
C.PAC_DOT      = rgb565(255,200,100)
C.PAC_WALL     = rgb565(40,40,200)

# Space Invaders
C.INV_BG       = rgb565(0,0,0)
C.INV_GREEN    = rgb565(0,220,0)
C.INV_RED      = rgb565(220,0,0)
C.INV_YELLOW   = rgb565(220,220,0)
C.INV_WHITE    = rgb565(240,240,240)
C.INV_MAGENTA  = rgb565(220,0,220)
C.INV_AMBER    = rgb565(255,160,0)

# Game Boy
DMG0 = rgb565(155,188,15)  # lightest
DMG1 = rgb565(139,172,15)
DMG2 = rgb565(48,98,48)
DMG3 = rgb565(15,56,15)   # darkest
C.DMG_LIGHT   = DMG0
C.DMG_MED     = DMG1
C.DMG_DARK    = DMG2
C.DMG_BLACK   = DMG3
C.DMG_BG      = DMG0

# Pokémon
C.POKE_RED     = rgb565(220,16,16)
C.POKE_WHITE   = rgb565(255,255,255)
C.POKE_BLACK   = rgb565(20,20,20)
C.POKE_GREEN   = rgb565(32,200,32)
C.POKE_YELLOW  = rgb565(240,200,0)
C.POKE_BLUE    = rgb565(50,80,255)
C.POKE_GOLD    = rgb565(255,200,0)
C.POKE_BG      = rgb565(240,240,240)

# Sonic
C.SONIC_BLUE    = rgb565(32,80,255)
C.SONIC_SKY     = rgb565(100,200,255)
C.SONIC_GREEN   = rgb565(32,200,32)
C.SONIC_GOLD    = rgb565(255,200,0)
C.SONIC_BROWN   = rgb565(140,80,20)

# Mega Man
C.MEGA_BLUE     = rgb565(32,80,255)
C.MEGA_LBLUE    = rgb565(100,180,255)
C.MEGA_SKIN     = rgb565(252,188,120)
C.MEGA_RED      = rgb565(220,16,16)
C.MEGA_YELLOW   = rgb565(255,200,0)
C.MEGA_WHITE    = rgb565(200,200,200)

# Pong/Arcade
C.PONG_GREEN    = rgb565(0,220,0)
C.PONG_WHITE    = rgb565(200,200,200)
C.PONG_AMBER    = rgb565(255,180,0)

# ── Sprite definitions ─────────────────────────────────────────
# Each sprite: (width, height, [rows of chars from colormap])

def parse_sprite(lines, colormap, default=None):
    """Convert ASCII art lines into list of RGB565 values (row-major)."""
    pixels = []
    for line in lines:
        for ch in line:
            if ch in colormap:
                pixels.append(colormap[ch])
            else:
                pixels.append(default or 0x0000)
    return pixels

def sprite_to_c_array(pixels, width, height, name):
    """Generate C PROGMEM array string."""
    lines = []
    line = []
    for i, p in enumerate(pixels):
        line.append(f"0x{p:04X}")
        if len(line) == 12 or i == len(pixels)-1:
            indent = "  " if lines else "  "
            lines.append(indent + ",".join(line))
            line = []
    arr = f"static const uint16_t {name}[{width*height}] PROGMEM = {{\n"
    arr += ",\n".join(lines)
    arr += "\n};\n"
    return arr

# Max sprite width across all themes (used for static rowbuf)
MAX_SPRITE_W = 16

def blit16_func(name, w, h):
    """Generate C function that blits a sprite to display at (x,y).
    Uses static row buffer (allocated once) for stack safety.
    Bounds-checks to avoid drawing off-screen."""
    return f"""
static void blit_{name}(int x, int y) {{
  if (x >= 128 || y >= 32 || x + {w} <= 0 || y + {h} <= 0) return;
  int x0 = (x < 0) ? -x : 0;
  int y0 = (y < 0) ? -y : 0;
  int x1 = (x + {w} > 128) ? 128 - x : {w};
  int y1 = (y + {h} > 32)  ? 32 - y  : {h};
  static uint16_t rowbuf[{MAX_SPRITE_W}];
  for (int row = y0; row < y1; row++) {{
    memcpy_P(rowbuf, &{name}[row*{w}], {w}*2);
    int dy = y + row;
    for (int col = x0; col < x1; col++) {{
      uint16_t c = rowbuf[col];
      if (c) display->drawPixel(x + col, dy, c);
    }}
  }}
}}
"""

# ── THEME 1: SUPER MARIO ──────────────────────────────────────
MARIO_COLORS = {
    '.': 0x0000, 'R': C.MARIO_RED, 'B': C.MARIO_BLUE, 'S': C.MARIO_SKIN,
    'N': C.MARIO_BROWN, 'Y': C.MARIO_YELLOW, 'W': C.MARIO_WHITE,
    'K': 0x0000, 'G': C.MARIO_GREEN, 'T': C.MARIO_BRICK, 'C': C.MARIO_CLOUD,
    'X': C.MARIO_SKY,
}

MARIO_SPRITES = {
    "mushroom": [
        "......RRRR......",
        ".....RRRRRR.....",
        "....RRRRRRRR....",
        "...RRRRRRRRRR...",
        "...RRRRRRRRRR...",
        "..RRRRRRRRRRRR..",
        "..RWRRRRRRRWRR..",
        "..RRRRRRRRRRRR..",
        "....NNNNNNNN....",
        "....NNNNNNNN....",
        "....SSSSSSSS....",
        "....SSSSSSSS....",
        "...SSSSSSSSSS...",
        "...SSSSSSSSSS...",
        "..SS....SSSS....",
        "..SS.....SS.....",
    ],
    "pipe": [
        "..GGGGGGGG..",
        "..GGGGGGGG..",
        ".GGGGGGGGGG.",
        ".GGGGGGGGGG.",
        ".GGGGGGGGGG.",
        ".GG......GG.",
        ".GG......GG.",
        ".GG......GG.",
        ".GG......GG.",
        ".GG......GG.",
        ".GG......GG.",
        ".GG......GG.",
        ".GG......GG.",
        ".GG......GG.",
        ".GG......GG.",
        ".GG......GG.",
    ],
    "small_pipe": [
        ".GGGGGG.",
        ".GGGGGG.",
        "GGGGGGGG",
        "GGGGGGGG",
        "GG....GG",
        "GG....GG",
        "GG....GG",
        "GG....GG",
        "GG....GG",
        "GG....GG",
        "GG....GG",
        "GG....GG",
    ],
    "mario_run0": [
        "..RRRRR...R...",
        "..RRRRR...R...",
        "..RRRRR..RR...",
        "..SSSSS..R....",
        ".SSSSSSS......",
        ".SS..SS..W....",
        "..BBBBBBB.....",
        "..BBBBBBB.....",
        "..BBBBBBB.....",
        "..BBBBBBB.....",
        "..BB..BBB.....",
        "..NN...NN.....",
        "..NN...NN.....",
        "......NN..NN..",
        "......NN..NN..",
        "..............",
    ],
    "mario_run1": [
        "..RRRRR...R...",
        "..RRRRR...R...",
        "..RRRRR..RR...",
        "..SSSSS..R....",
        ".SSSSSSS......",
        ".SS..SS..W....",
        "..BBBBBBB.....",
        "..BBBBBBB.....",
        "..BBBBBBB.....",
        "..BBBBBBB.....",
        "..BB..BBB.....",
        "..NN...NN.....",
        "..NN...NN.....",
        "NN..NN........",
        "NN..NN........",
        "..............",
    ],
    "mario_jump": [
        "..RRRRR...R...",
        "..RRRRR...R...",
        "..RRRRR..RR...",
        "..SSSSS..R....",
        ".SSSSSSS......",
        ".SS..SS..W....",
        "..BBBBBBB.....",
        "..BBBBBBB.....",
        "..BBBBBBB.....",
        "..BB..BBB.....",
        "..NN...NN.....",
        "..NN...NN.....",
        "......NN..NN..",
        "..............",
        "..............",
        "..............",
    ],
    "cloud": [
        "..........",
        ".CCCC.....",
        "CCCCCCCCC.",
        "CCCCCCCCCC",
        ".CCCCCCCC.",
        "..........",
    ],
    "question_block": [
        ".YYYYY.",
        "Y.....Y",
        "Y..W..Y",
        "Y.....Y",
        "Y..W..Y",
        "Y.....Y",
        ".YYYYY.",
    ],
    "brick": [
        "TTTTTTTT",
        "TT.TT.TT",
        "TTTTTTTT",
        "TT.TT.TT",
        "TTTTTTTT",
        "TT.TT.TT",
        "TTTTTTTT",
        "TT.TT.TT",
    ],
    "ground": [
        "TTTTTTTTTTTTTTTT",
        "TTTTTTTTTTTTTTTT",
    ],
    "bush": [
        "..GGGG..",
        ".GGGGGGG",
        "GGGGGGGG",
    ],
}

for name, art in MARIO_SPRITES.items():
    h = len(art)
    w = max(len(row) for row in art)
    MARIO_SPRITES[name] = (w, h, parse_sprite(art, MARIO_COLORS))

# ── THEME 2: TETRIS ───────────────────────────────────────────
TETRIS_COLORS = {
    '.': 0x0000, 'C': C.TETRIS_CYAN, 'Y': C.TETRIS_YELLOW,
    'P': C.TETRIS_PURPLE, 'G': C.TETRIS_GREEN, 'R': C.TETRIS_RED,
    'B': C.TETRIS_BLUE, 'O': C.TETRIS_ORANGE, 'K': 0x0000,
}

# Tetris pieces 4x4 (each block 2x2 drawn pixels)
TETRIS_SPRITES = {
    "piece_I": [
        "....",
        "CCCC",
        "....",
        "....",
    ],
    "piece_O": [
        "YY",
        "YY",
    ],
    "piece_T": [
        "PPP",
        ".P.",
        "...",
    ],
    "piece_S": [
        ".GG",
        "GG.",
        "...",
    ],
    "piece_Z": [
        "RR.",
        ".RR",
        "...",
    ],
    "piece_J": [
        "B..",
        "BBB",
        "...",
    ],
    "piece_L": [
        "..O",
        "OOO",
        "...",
    ],
}

for name, art in TETRIS_SPRITES.items():
    h = len(art)
    w = max(len(row) for row in art)
    TETRIS_SPRITES[name] = (w, h, parse_sprite(art, TETRIS_COLORS))

# ── THEME 3: PAC-MAN ──────────────────────────────────────────
PAC_COLORS = {
    '.': 0x0000, 'Y': C.PAC_YELLOW, 'P': C.PAC_PINK, 'R': C.PAC_RED,
    'C': C.PAC_CYAN, 'O': C.PAC_ORANGE, 'W': C.PAC_WHITE, 'B': C.PAC_BLUE,
    'D': C.PAC_DOT, 'L': C.PAC_WALL, 'K': 0x0000,
}

PAC_SPRITES = {
    "pacman1": [
        "..YYYYYY..",
        ".YYYYYYYY.",
        "YYYYYYYYYY",
        "YYYYYYYY..",
        "YYYYYY....",
        "YYYYYY....",
        "YYYYYYY...",
        "YYYYYYYYYY",
        ".YYYYYYYY.",
        "..YYYYYY..",
    ],
    "pacman2": [
        "..YYYYYY..",
        ".YYYYYYYY.",
        "YYYYYYYYYY",
        "YYYYYYYYY.",
        "YYYYYYY...",
        "YYYYYYY...",
        "YYYYYYYY..",
        "YYYYYYYYYY",
        ".YYYYYYYY.",
        "..YYYYYY..",
    ],
    "pacman3": [
        "..YYYYYY..",
        ".YYYYYYYY.",
        "YYYYYYYYYY",
        "YYYYYYYYYY",
        "YYYYYYYYYY",
        "YYYYYYYYYY",
        "YYYYYYYYYY",
        "YYYYYYYYYY",
        ".YYYYYYYY.",
        "..YYYYYY..",
    ],
    "ghost_red": [
        ".RRRRRR.",
        "RRRRRRRR",
        "RRWWWRRR",
        "RRRBBRRR",
        "RRRRRRRR",
        "RRRRRRRR",
        "RR.RR.RR",
        "R..R..RR",
    ],
    "ghost_pink": [
        ".PPPPPPP.",
        "PPPPPPPP",
        "PPWWWPPP",
        "PPPBBPPP",
        "PPPPPPPP",
        "PPPPPPPP",
        "PP.PP.PP",
        "P..P..PP",
    ],
    "pellet": [
        "..",
        "..",
    ],
    "ghost_cyan": [
        ".CCCCCC.",
        "CCCCCCCC",
        "CCWWWCCC",
        "CCCBBCCC",
        "CCCCCCCC",
        "CCCCCCCC",
        "CC.CC.CC",
        "C..C..CC",
    ],
    "ghost_orange": [
        ".OOOOOO.",
        "OOOOOOOO",
        "OOWWWOOO",
        "OOOBBOOO",
        "OOOOOOOO",
        "OOOOOOOO",
        "OO.OO.OO",
        "O..O..OO",
    ],
    "wall_h": [
        "LLLL",
    ],
}

PAC_RAW = dict(PAC_SPRITES)

for name, art in PAC_SPRITES.items():
    h = len(art)
    w = max(len(row) for row in art)
    PAC_SPRITES[name] = (w, h, parse_sprite(art, PAC_COLORS))

def rotate_cw(raw):
    h = len(raw)
    w = max(len(r) for r in raw)
    return [''.join(raw[h-1-c][r] if r < len(raw[h-1-c]) else '.' for c in range(h)) for r in range(w)]

def flip_h(raw):
    return [row[::-1] for row in raw]

for base in ["pacman1", "pacman2", "pacman3"]:
    raw = PAC_RAW[base]
    down_art = rotate_cw(raw)
    h, w = len(down_art), max(len(r) for r in down_art)
    PAC_SPRITES[base + "_down"] = (w, h, parse_sprite(down_art, PAC_COLORS))
    left_art = flip_h(raw)
    h, w = len(left_art), max(len(r) for r in left_art)
    PAC_SPRITES[base + "_left"] = (w, h, parse_sprite(left_art, PAC_COLORS))
    up_art = rotate_cw(rotate_cw(rotate_cw(raw)))
    h, w = len(up_art), max(len(r) for r in up_art)
    PAC_SPRITES[base + "_up"] = (w, h, parse_sprite(up_art, PAC_COLORS))

# ── THEME 4: SPACE INVADERS ───────────────────────────────────
INV_COLORS = {
    '.': 0x0000, 'G': C.INV_GREEN, 'R': C.INV_RED, 'Y': C.INV_YELLOW,
    'W': C.INV_WHITE, 'M': C.INV_MAGENTA, 'K': 0x0000,
}

INV_SPRITES = {
    "crab_0": [   # Type A (Crab) - frame 0 (legs spread) - 11x8
        "..G.....G..",
        "..G.....G..",
        ".GG.....GG.",
        "GGGGGGGGGGG",
        "GGGGGGGGGGG",
        "GG.GGGGG.GG",
        "GG.G.G.G.GG",
        "G..GG.GG..G",
    ],
    "crab_1": [   # Type A (Crab) - frame 1 (legs together) - 11x8
        "..G.....G..",
        "..G.....G..",
        ".GG.....GG.",
        "GGGGGGGGGGG",
        "GGGGGGGGGGG",
        "G.GGGGGGG.G",
        "G.G.G.G.G.G",
        "G..GG.GG..G",
    ],
    "octopus_0": [   # Type B (Octopus) - frame 0 (tentacles spread) - 11x8
        "...RRRR...",
        "..RRRRRR..",
        "..RRRRRR..",
        ".RRRRRRRR.",
        "RRRRRRRRRR",
        "R.RRRRRR.R",
        "RR..RR..RR",
        ".R......R.",
    ],
    "octopus_1": [   # Type B (Octopus) - frame 1 (tentacles together) - 11x8
        "...RRRR...",
        "..RRRRRR..",
        "..RRRRRR..",
        ".RRRRRRRR.",
        "RRRRRRRRRR",
        "RR..RR..RR",
        "R.R....R.R",
        ".RR....RR.",
    ],
    "squid_0": [   # Type C (Squid) - frame 0 - 8x8
        "...YY...",
        "..YYYY..",
        ".YYYYYY.",
        ".YYYYYY.",
        ".YYYYYY.",
        "..Y..Y..",
        "..Y..Y..",
        "..Y..Y..",
    ],
    "squid_1": [   # Type C (Squid) - frame 1 - 8x8
        "...YY...",
        "..YYYY..",
        ".YYYYYY.",
        ".YYYYYY.",
        ".YYYYYY.",
        "..Y..Y..",
        ".YY..YY.",
        "..Y..Y..",
    ],
    "ship": [   # Player cannon - 11x8
        ".....W.....",
        "....WWW....",
        "...WWWWW...",
        "..WWWWWWW..",
        "...WWWWW...",
        "....W.W....",
        "...W...W...",
    ],
    "saucer": [   # Mystery ship - 8x6
        "..MMMM..",
        ".MMMMMM.",
        "MMMMMMMM",
        "M..MM..M",
        ".M....M.",
        "..M..M..",
    ],
    "missile": [
        "..W..",
        "..W..",
        ".WWW.",
        ".WWW.",
        "..W..",
        "..W..",
    ],
    "explosion": [
        ".Y.Y.",
        "Y...Y",
        "..Y..",
        "Y...Y",
        ".Y.Y.",
    ],
}

for name, art in INV_SPRITES.items():
    h = len(art)
    w = max(len(row) for row in art)
    INV_SPRITES[name] = (w, h, parse_sprite(art, INV_COLORS))

# ── THEME 9: PONG / ARCADE ────────────────────────────────────
PONG_COLORS = {
    '.': 0x0000, 'G': C.PONG_GREEN, 'W': C.PONG_WHITE, 'A': C.PONG_AMBER,
}

PONG_SPRITES = {}

for name, art in PONG_SPRITES.items():
    h = len(art)
    w = max(len(row) for row in art)
    PONG_SPRITES[name] = (w, h, parse_sprite(art, PONG_COLORS))

# ──────────────────────────────────────────────────────────────
# C++ TEMPLATES
# ──────────────────────────────────────────────────────────────

def generate_pixel_font(theme_name, color_fg, color_bg=0):
    """Generate a 5x7 font in theme colors."""
    out = f"\n// {theme_name} pixel font\n"
    for ch in '0123456789:':
        rows = FONT_5x7[ch]
        arr = f"static const uint16_t font_{theme_name}_{ch}[7] PROGMEM = {{\n  "
        vals = []
        for row in rows:
            val = 0
            for bit in range(5):
                if row & (1 << (4 - bit)):
                    pass
            # Store each row as 5 pixels
        # Actually let's store font as full pixels, not bit-compressed
        # because we want colored font per theme
        out += f"// {ch}: {', '.join(f'0b{row:07b}' for row in rows)}\n"
    return out

def generate_font_bitmaps():
    """Generate standard 5x7 monochrome font (pixels stored row-by-row)."""
    out = "// Font 5x7 (row bitmasks, MSB=leftmost pixel)\n"
    out += "static const uint8_t font5x7[11][7] PROGMEM = {\n"
    digits = '0123456789:'
    for ch in digits:
        rows = FONT_5x7[ch]
        out += f"  {{ {', '.join(f'0b{row:07b}' for row in rows)} }}, // {ch}\n"
    out += "};\n"
    out += "static const char font_chars[11] = {'0','1','2','3','4','5','6','7','8','9',':'};\n"
    return out

def generate_theme_sprites(theme_name, sprites_dict):
    """Generate all sprite arrays for a theme."""
    out = f"\n// === {theme_name.upper()} Sprites ===\n"
    for name, (w, h, pixels) in sprites_dict.items():
        cname = f"{theme_name}_{name}"
        out += sprite_to_c_array(pixels, w, h, cname)
        out += blit16_func(cname, w, h)
    return out

def generate_all_sprites():
    """Generate sprite data for all themes."""
    out = "// ============================================================\n"
    out += "// AUTO-GENERATED by generate_clock_themes.py\n"
    out += "// ============================================================\n\n"
    out += "#ifndef CLOCK_THEMES_H\n#define CLOCK_THEMES_H\n\n"
    out += '#include <Arduino.h>\n#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>\n\n'

    # External display reference
    out += "extern MatrixPanel_I2S_DMA *display;\n\n"

    # Font
    out += generate_font_bitmaps()

    # Theme sprites
    out += generate_theme_sprites("mario", MARIO_SPRITES)
    out += generate_theme_sprites("tetris", TETRIS_SPRITES)
    out += generate_theme_sprites("pac", PAC_SPRITES)
    out += generate_theme_sprites("inv", INV_SPRITES)

    # ── Drawing functions ──
    out += generate_drawing_functions()

    out += "\n#endif // CLOCK_THEMES_H\n"
    return out

def generate_drawing_functions():
    """Generate C++ drawing functions for all themes."""
    sin8_vals = []
    for i in range(256):
        v = int(127.0 * math.sin(i * 2 * math.pi / 256))
        sin8_vals.append(v)
    sin8_table = ",".join(str(v) for v in sin8_vals)

    return f"""

// ── Helpers ─────────────────────────────────────────────────
// Integer sine table (8-bit, -127..127), avoids sinf() crashes
static const int8_t sin8[256] = {{ {sin8_table} }};
#define sin8_ms(ms, period)  sin8[((ms) / (period)) % 256]

// Color565 helper (inlined for speed)
static uint16_t c565(uint8_t r, uint8_t g, uint8_t b) {{
  return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}}

// Draw a 5x7 font character at (x,y) using direct pixel drawing
static void drawFontChar(int x, int y, char ch, uint16_t color) {{
  int idx = -1;
  for (int i = 0; i < 11; i++) {{ if (font_chars[i] == ch) {{ idx = i; break; }} }}
  if (idx < 0) return;
  uint8_t rows[7];
  memcpy_P(rows, font5x7[idx], 7);
  for (int row = 0; row < 7; row++) {{
    for (int col = 0; col < 5; col++) {{
      if (rows[row] & (0x10 >> col))
        display->drawPixel(x+col, y+row, color);
    }}
  }}
}}

// Draw a 5x7 font character at 4x scale (each pixel becomes a 4x4 block = 20x28 px)
// Used by large-digit artistic themes (Neon, Matrix, Fire, Rainbow)
static void drawLargeFontChar(int x, int y, char ch, uint16_t color) {{
  int idx = -1;
  for (int i = 0; i < 11; i++) {{ if (font_chars[i] == ch) {{ idx = i; break; }} }}
  if (idx < 0) return;
  uint8_t rows[7];
  memcpy_P(rows, font5x7[idx], 7);
  for (int row = 0; row < 7; row++) {{
    for (int col = 0; col < 5; col++) {{
      if (rows[row] & (0x10 >> col))
        display->fillRect(x + col*4, y + row*4, 4, 4, color);
    }}
  }}
}}

static void drawTimeHHMM(int x, int y, int h, int m, uint16_t color) {{
  char buf[5];
  buf[0] = '0' + h/10; buf[1] = '0' + h%10; buf[2] = ':';
  buf[3] = '0' + m/10; buf[4] = '0' + m%10;
  for (int i = 0; i < 5; i++) drawFontChar(x + i*6, y, buf[i], color);
}}

// ── THEME 1: SUPER MARIO ────────────────────
// Mario runs across screen with proper running animation
static void drawTheme_Mario(int h, int m, int s, unsigned long ms) {{
  uint16_t sky = c565(40,80,180);
  display->fillRect(0, 0, 128, 32, sky);
  display->fillRect(0, 26, 128, 6, c565(80,50,20));
  display->fillRect(0, 25, 128, 1, c565(100,70,30));
  int cx1 = 128 - (ms / 80) % 200;
  int cx2 = 128 - (ms / 110 + 70) % 200;
  int cx3 = 128 - (ms / 60 + 140) % 200;
  blit_mario_cloud(cx1, 1); blit_mario_cloud(cx2, 4); blit_mario_cloud(cx3, 2);
  int qbPhase = (ms / 25) % 256;
  int qb1=0, qb2=0;
  if (qbPhase < 15) qb1 = (15 - qbPhase) / 2;
  if (((qbPhase+85)%256) < 15) qb2 = (15 - (qbPhase+85)%256) / 2;
  blit_mario_question_block(20, 2 - qb1);
  blit_mario_question_block(90, 2 - qb2);
  blit_mario_brick(44, 16); blit_mario_brick(52, 16);
  blit_mario_brick(72, 16); blit_mario_brick(80, 16);
  // Pipes on ground - cycling which 2 of 3 appear
  int pipePhase = (ms / 500) % 3;
  int hideX = pipePhase == 0 ? 8 : pipePhase == 1 ? 56 : 108;
  if (hideX != 8) blit_mario_pipe(8, 10);
  if (hideX != 56) blit_mario_pipe(56, 10);
  if (hideX != 108) blit_mario_pipe(108, 10);
  // Mushroom near first ? block, bouncing with sine
  int mushBounce = sin8_ms(ms, 600);
  if (mushBounce < 0) mushBounce = -mushBounce;
  int my = 10 - mushBounce / 25;
  blit_mario_mushroom(22, my);
  // Small decorative pipe on ground
  blit_mario_small_pipe(120, 14);
  int mx = (ms / 20) % (128 + 18) - 18;
  int mf = (ms / 120) % 2;
  if (mf == 0) blit_mario_mario_run0(mx, 10);
  else blit_mario_mario_run1(mx, 10);
  drawTimeHHMM(45, 25, h, m, c565(255,255,255));
}}
// ── THEME 2: TETRIS ─────────────────────────
// Big blocks (4x4), vertical pieces, stacking at bottom
// Each piece is drawn as 4x4 pixel blocks with 1px gap
#define TETRIS_COLS 10
#define TETRIS_ROWS 8
static uint8_t tetrisGrid[TETRIS_ROWS][TETRIS_COLS];

static void tetrisGridClear() {{
  for (int r = 0; r < TETRIS_ROWS; r++)
    for (int c = 0; c < TETRIS_COLS; c++)
      tetrisGrid[r][c] = 0xFF;
}}

static void tetrisDrawBlock(int x, int y, uint16_t color) {{
  display->fillRect(x, y, 4, 4, color);
  display->drawPixel(x+1, y+0, 0xFFFF); // highlight
  display->drawPixel(x+0, y+1, 0xFFFF);
  display->drawPixel(x+3, y+2, 0x0000); // shadow
  display->drawPixel(x+2, y+3, 0x0000);
}}

static void drawTheme_Tetris(int h, int m, int s, unsigned long ms) {{
  static bool gridInit = false;
  if (!gridInit) {{ tetrisGridClear(); gridInit = true; }}
  display->fillRect(0, 0, 128, 32, c565(0,0,20));

  // Pending pieces animation on the left
  uint16_t pieceColors[7] = {{ c565(0,255,255), c565(255,255,0), c565(180,0,255),
    c565(0,220,0), c565(220,0,0), c565(0,80,255), c565(255,160,0) }};
  // Piece shapes: each is bitmask 4x4 (bits MSB=leftmost)
  const uint8_t tShapes[7][4] = {{
    {{0x8, 0x8, 0x8, 0x8}}, // I: vertical line PW=1
    {{0xC, 0xC, 0, 0}},     // O: 2x2 square PW=2
    {{0xE, 0x4, 0, 0}},     // T: row0=111 row1=010 PW=3
    {{0x6, 0xC, 0, 0}},     // S: row0=011 row1=110 PW=3
    {{0xC, 0x6, 0, 0}},     // Z: row0=110 row1=011 PW=3
    {{0x8, 0xE, 0, 0}},     // J: row0=100 row1=111 PW=3
    {{0x2, 0xE, 0, 0}},     // L: row0=001 row1=111 PW=3
  }};
  static const int tPW[7] = {{1, 2, 3, 3, 3, 3, 3}};

  // Persistent falling piece state
  static int tPiece = -1;
  static int tCol = 0;
  static int tRow = 0;
  static unsigned long tLastMs = 0;

  if (tPiece < 0) {{
    tPiece = random(0, 7);
    tCol = random(0, TETRIS_COLS - tPW[tPiece] + 1);
    tRow = 0;
    tLastMs = ms;
  }}

  bool lockPiece = false;

  if (ms - tLastMs >= 600) {{
    int nextRow = tRow + 1;
    bool canGoDown = true;
    for (int r = 3; r >= 0 && canGoDown; r--)
      if (tShapes[tPiece][r] && nextRow + r >= TETRIS_ROWS) canGoDown = false;
    if (canGoDown) {{
      for (int r = 0; r < 4; r++) {{
        if (!tShapes[tPiece][r]) continue;
        int gr = nextRow + r;
        if (gr >= TETRIS_ROWS) break;
        for (int c = 0; c < tPW[tPiece]; c++)
          if (tShapes[tPiece][r] & (1 << (3-c)))
            if (tetrisGrid[gr][tCol+c] != 0xFF) {{ canGoDown = false; break; }}
        if (!canGoDown) break;
      }}
    }}
    if (canGoDown) tRow = nextRow;
    else lockPiece = true;
    tLastMs = ms;
  }}

  if (!lockPiece) {{
    for (int r = 0; r < 4 && tRow+r < TETRIS_ROWS; r++) {{
      if (!tShapes[tPiece][r]) continue;
      for (int c = 0; c < tPW[tPiece]; c++)
        if (tShapes[tPiece][r] & (1 << (3-c)))
          if (tetrisGrid[tRow+r][tCol+c] != 0xFF) {{ lockPiece = true; break; }}
      if (lockPiece) break;
    }}
  }}

  if (lockPiece) {{
    bool gameOver = false;
    for (int r = 0; r < 4 && tRow+r < TETRIS_ROWS; r++) {{
      if (!tShapes[tPiece][r]) continue;
      for (int c = 0; c < tPW[tPiece]; c++) {{
        if (tShapes[tPiece][r] & (1 << (3-c))) {{
          if (tetrisGrid[tRow+r][tCol+c] != 0xFF) gameOver = true;
          else tetrisGrid[tRow+r][tCol+c] = tPiece;
        }}
      }}
    }}
    if (gameOver) {{
      tetrisGridClear();
    }} else {{
      for (int r = TETRIS_ROWS-1; r >= 0; r--) {{
        bool full = true;
        for (int c = 0; c < TETRIS_COLS; c++)
          if (tetrisGrid[r][c] == 0xFF) {{ full = false; break; }}
        if (full) {{
          for (int r2 = r; r2 > 0; r2--)
            for (int c2 = 0; c2 < TETRIS_COLS; c2++)
              tetrisGrid[r2][c2] = tetrisGrid[r2-1][c2];
          for (int c2 = 0; c2 < TETRIS_COLS; c2++)
            tetrisGrid[0][c2] = 0xFF;
          r++;
        }}
      }}
    }}
    tPiece = -1;
  }}

  // Draw accumulated pieces from grid
  for (int r = 0; r < TETRIS_ROWS; r++) {{
    for (int c = 0; c < TETRIS_COLS; c++) {{
      if (tetrisGrid[r][c] != 0xFF)
        tetrisDrawBlock(4 + c*5, r*4, pieceColors[tetrisGrid[r][c]]);
    }}
  }}

  // Playfield border frame
  display->drawRect(3, 0, 51, 32, c565(60,60,100));

  // Draw falling piece
  if (tPiece >= 0) {{
    for (int br = 0; br < 4 && tRow+br < TETRIS_ROWS; br++) {{
      for (int bc = 0; bc < tPW[tPiece]; bc++) {{
        if (tShapes[tPiece][br] & (1 << (3-bc))) {{
          int px = 4 + (tCol+bc)*5;
          int py = (tRow+br)*4;
          tetrisDrawBlock(px, py, pieceColors[tPiece]);
        }}
      }}
    }}
  }}

  // Simple buildings above score panel (St. Basil style)
  uint16_t gold = c565(210,190,60);
  display->fillRect(88, 4, 12, 7, c565(150,30,30));
  display->fillRect(90, 2, 8, 2, gold);
  display->drawPixel(92, 1, gold); display->drawPixel(93, 1, gold);
  display->fillRect(104, 3, 10, 8, c565(30,100,30));
  display->fillRect(106, 1, 6, 2, gold);
  display->drawPixel(108, 0, gold); display->drawPixel(109, 0, gold);
  display->fillRect(118, 5, 8, 6, c565(30,30,150));
  display->fillRect(120, 3, 4, 2, gold);
  display->drawPixel(121, 2, gold); display->drawPixel(122, 2, gold);

  // Score box outline
  display->drawRect(84, 10, 42, 20, c565(60,60,100));
  // Time display in score box
  uint16_t tc = pieceColors[(h+m+ms/200) % 7];
  char buf[5];
  buf[0] = '0'+h/10; buf[1] = '0'+h%10; buf[2]=':'; buf[3]='0'+m/10; buf[4]='0'+m%10;
  for (int i = 0; i < 5; i++) {{
    if (i == 2 && (ms/1000)%2 == 0) continue;
    drawFontChar(91 + i*6, 17, buf[i], tc);
  }}
}}

// ── THEME 3: PAC-MAN ────────────────────────
// Yellow pellet path around screen, Pac-Man & ghost follow path
static void drawTheme_Pacman(int h, int m, int s, unsigned long ms) {{
  display->fillRect(0, 0, 128, 32, c565(0,0,0));
  uint16_t dot = c565(255,200,100);
  uint16_t bigDot = c565(255,220,0);
  int totalDots = 84;
  int pathX[84], pathY[84];
  // Top: left to right (24 dots, 5px apart)
  for (int i = 0; i < 24; i++) {{ pathX[i] = 4 + i*5; pathY[i] = 5; }}
  // Right: top to bottom (6 dots, 4px apart, stays on-screen)
  for (int i = 24; i < 30; i++) {{ pathX[i] = 124; pathY[i] = 6 + (i-24)*4; }}
  // Bottom: right to left (24 dots, 5px apart)
  for (int i = 30; i < 54; i++) {{ pathX[i] = 119 - (i-30)*5; pathY[i] = 26; }}
  // Left: bottom to top (6 dots, 4px apart)
  for (int i = 54; i < 60; i++) {{ pathX[i] = 4; pathY[i] = 25 - (i-54)*4; }}
  // Close: return to start (24 dots, 5px apart)
  for (int i = 60; i < 84; i++) {{ pathX[i] = 9 + (i-60)*5; pathY[i] = 5; }}
  // Big dots at the 4 corners
   int corners[4][2] = {{{{4, 5}}, {{124, 5}}, {{124, 26}}, {{4, 26}}}};
  for (int i = 0; i < 4; i++) {{
    display->fillRect(corners[i][0]-2, corners[i][1]-2, 5, 5, bigDot);
  }}
  // Draw path dots
  for (int i = 0; i < totalDots; i++) {{
    display->drawPixel(pathX[i], pathY[i], dot);
  }}
  int pacIdx = (ms / 100) % totalDots;
  int pacFrame = (ms / 200) % 3;
  int px = pathX[pacIdx], py = pathY[pacIdx];
  int dir = 0;
  if (pacIdx >= 0 && pacIdx < 24) dir = 0;
  else if (pacIdx >= 24 && pacIdx < 30) dir = 1;
  else if (pacIdx >= 30 && pacIdx < 54) dir = 2;
  else if (pacIdx >= 54 && pacIdx < 60) dir = 3;
  else dir = 0;
  if (dir == 0) {{
    if (pacFrame == 0) blit_pac_pacman1(px-5, py-5);
    else if (pacFrame == 1) blit_pac_pacman2(px-5, py-5);
    else blit_pac_pacman3(px-5, py-5);
  }} else if (dir == 1) {{
    if (pacFrame == 0) blit_pac_pacman1_down(px-5, py-5);
    else if (pacFrame == 1) blit_pac_pacman2_down(px-5, py-5);
    else blit_pac_pacman3_down(px-5, py-5);
  }} else if (dir == 2) {{
    if (pacFrame == 0) blit_pac_pacman1_left(px-5, py-5);
    else if (pacFrame == 1) blit_pac_pacman2_left(px-5, py-5);
    else blit_pac_pacman3_left(px-5, py-5);
  }} else {{
    if (pacFrame == 0) blit_pac_pacman1_up(px-5, py-5);
    else if (pacFrame == 1) blit_pac_pacman2_up(px-5, py-5);
    else blit_pac_pacman3_up(px-5, py-5);
  }}
  int ghostOffsets[4] = {{8, 14, 22, 30}};
  for (int g = 0; g < 4; g++) {{
    int ghostIdx = (pacIdx - ghostOffsets[g] + totalDots) % totalDots;
    int gx = pathX[ghostIdx]-4, gy = pathY[ghostIdx]-4;
    if (g == 0) blit_pac_ghost_red(gx, gy);
    else if (g == 1) blit_pac_ghost_pink(gx, gy);
    else if (g == 2) blit_pac_ghost_cyan(gx, gy);
    else blit_pac_ghost_orange(gx, gy);
  }}
  char buf[5];
  buf[0]='0'+h/10; buf[1]='0'+h%10; buf[2]=':'; buf[3]='0'+m/10; buf[4]='0'+m%10;
  uint16_t yellow = c565(255,220,0);
  for (int i = 0; i < 5; i++) drawFontChar(49 + i*6, 12, buf[i], yellow);
}}
// ── THEME 4: SPACE INVADERS ──────────────────
static void drawTheme_Invaders(int h, int m, int s, unsigned long ms) {{
  display->fillRect(0, 0, 128, 32, c565(0,0,0));

  // Marching Space Invaders - ping-pong animation
  int marchPhase = (ms / 80) % 24;
  int marchX = marchPhase < 12 ? marchPhase : 23 - marchPhase;
  int invFrame = (ms / 400) % 2;

  // 6 invaders per row, tighter 18px spacing
  int nCol = 6;
  int startX = 15;

  // Row 0: Squid (8x8, top row)
  for (int col = 0; col < nCol; col++) {{
    int ix = startX + col*18 + marchX;
    if (ix > -10 && ix < 128) {{
      if (invFrame==0) blit_inv_squid_0(ix, 0);
      else blit_inv_squid_1(ix, 0);
    }}
  }}

  // Row 1: Octopus (11x8, middle row)
  for (int col = 0; col < nCol; col++) {{
    int ix = startX + col*18 + marchX;
    if (ix > -12 && ix < 128) {{
      if (invFrame==0) blit_inv_octopus_0(ix, 7);
      else blit_inv_octopus_1(ix, 7);
    }}
  }}

  // Row 2: Crab (11x8, bottom row)
  for (int col = 0; col < nCol; col++) {{
    int ix = startX + col*18 + marchX;
    if (ix > -12 && ix < 128) {{
      if (invFrame==0) blit_inv_crab_0(ix, 14);
      else blit_inv_crab_1(ix, 14);
    }}
  }}

  // Ship (player cannon)
  int shipX = 64 + (sin8_ms(ms, 50) * 20 / 127);
  blit_inv_ship(shipX, 25);

  // Saucer (mystery ship at top, every ~5s)
  int saucerPeriod = 5000;
  int saucerMs = ms % saucerPeriod;
  if (saucerMs < 1800) {{
    int saucerX = (saucerMs * 140) / 1800 - 10;
    blit_inv_saucer(saucerX, 0);
  }}

  // Missile fired from ship
  int shotMs = ms % 1200;
  if (shotMs < 600) {{
    int bulletY = 24 - (shotMs * 24 / 600);
    if (bulletY < 0) bulletY = 0;
    blit_inv_missile(shipX+4, bulletY);
  }}

  // Time split on each side of ship
  uint16_t sc = (ms/150)%2 ? c565(0,255,0) : c565(255,255,0);
  char buf[5];
  buf[0]='0'+h/10; buf[1]='0'+h%10; buf[2]=':'; buf[3]='0'+m/10; buf[4]='0'+m%10;
  // Hours on the left
  drawFontChar(15, 22, buf[0], sc);
  drawFontChar(24, 22, buf[1], sc);
  // Colon blinking in center area
  if ((ms/1000)%2) drawFontChar(62, 22, ':', sc);
  // Minutes on the right
  drawFontChar(98, 22, buf[3], sc);
  drawFontChar(107, 22, buf[4], sc);
}}

// ── THEME 9: PONG (SCORE CLOCK) ──────────────
// Ball bounces between paddles, digits are the score display.
static void drawTheme_Pong(int h, int m, int s, unsigned long ms) {{
  display->fillRect(0, 0, 128, 32, c565(0,0,0));

  // Center dashed line
  for (int y = 0; y < 32; y += 4)
    display->drawPixel(64, y, c565(60,60,60));

  // Paddles (auto-play)
  uint16_t paddleCol = c565(200,200,200);
  int paddleH = 8;
  int leftY  = 10 + (sin8[(ms/15) % 256] * 8 / 127);
  int rightY = 10 + (sin8[(ms/12 + 80) % 256] * 8 / 127);
  display->fillRect(2, leftY, 2, paddleH, paddleCol);
  display->fillRect(124, rightY, 2, paddleH, paddleCol);

  // Ball bouncing
  int ballPhase = (ms / 12) % 256;
  int ballXoff = sin8[ballPhase] * 56 / 127;
  int ballYoff = sin8[(ballPhase + 64) % 256] * 12 / 127;
  int bx = 64 + ballXoff;
  int by = 14 + ballYoff;
  if (by < 2) by = 2;
  if (by > 28) by = 28;
  display->fillRect(bx-1, by-1, 3, 3, c565(200,200,200));

  // Clock digits as score (HH:MM centered)
  uint16_t scoreCol = c565(255,180,0);
  char buf[5];
  buf[0] = '0'+h/10; buf[1] = '0'+h%10; buf[2]=':'; buf[3]='0'+m/10; buf[4]='0'+m%10;

  // Centered evenly: each char at 12px spacing, colon centered on x=64
  const uint8_t px[5] = {{40, 52, 62, 76, 88}};
  for (int i = 0; i < 5; i++) {{
    if (i == 2) {{ if ((ms/1000)%2) drawFontChar(px[i], 8, buf[i], scoreCol); }}
    else drawFontChar(px[i], 8, buf[i], scoreCol);
  }}

  // Score underline
  display->drawRect(36, 16, 56, 1, c565(80,60,0));
}}

// ── THEME 5: NEON ──────────────────────────
// Large glowing neon tube digits (pink + blue, pulsing)
static void drawTheme_Neon(int h, int m, int s, unsigned long ms) {{
  display->fillRect(0, 0, 128, 32, c565(0,0,0));
  int pulse = (sin8_ms(ms, 120) + 128) * 200 / 255;
  uint16_t pink = c565(255, 40, 120);
  uint16_t blue = c565(40, 200, 255);
  uint16_t pinkGlow = c565(pulse*160/255, pulse*25/255, pulse*75/255);
  uint16_t blueGlow = c565(pulse*25/255, pulse*120/255, pulse*160/255);
  uint16_t pinkCore = c565(pulse*220/255, pulse*35/255, pulse*105/255);
  uint16_t blueCore = c565(pulse*35/255, pulse*175/255, pulse*220/255);
  const uint8_t lx[5] = {{6, 30, 54, 78, 102}};
  char buf[5];
  buf[0]='0'+h/10; buf[1]='0'+h%10; buf[2]=':'; buf[3]='0'+m/10; buf[4]='0'+m%10;
  for (int i = 0; i < 5; i++) {{
    if (i == 2 && (ms/1000)%2 == 0) continue;
    uint16_t mid = (i==2) ? blueGlow : pinkGlow;
    uint16_t core = (i==2) ? blueCore : pinkCore;
    uint16_t bright = (i==2) ? blue : pink;
    drawLargeFontChar(lx[i]-4, 2, buf[i], c565(pulse*50/255, pulse*8/255, pulse*20/255));
    drawLargeFontChar(lx[i]+4, 2, buf[i], c565(pulse*50/255, pulse*8/255, pulse*20/255));
    drawLargeFontChar(lx[i]-2, 2, buf[i], mid);
    drawLargeFontChar(lx[i]+2, 2, buf[i], mid);
    drawLargeFontChar(lx[i], 2, buf[i], core);
    drawLargeFontChar(lx[i], 2, buf[i], bright);
  }}
}}

// ── THEME 6: MATRIX ────────────────────────
// Large matrix-green digits with glitch effects
static void drawTheme_Matrix(int h, int m, int s, unsigned long ms) {{
  display->fillRect(0, 0, 128, 32, c565(0,0,0));
  const uint8_t lx[5] = {{6, 30, 54, 78, 102}};
  char buf[5];
  buf[0]='0'+h/10; buf[1]='0'+h%10; buf[2]=':'; buf[3]='0'+m/10; buf[4]='0'+m%10;
  for (int i = 0; i < 5; i++) {{
    if (i == 2 && (ms/1000)%2 == 0) continue;
    drawLargeFontChar(lx[i]+2, 4, buf[i], c565(0,60,0));
    drawLargeFontChar(lx[i], 2, buf[i], c565(0,255,0));
  }}
  for (int p = 0; p < 5; p++) {{
    int sx = (p*31 + ms/30) % 108 + 10;
    int sy = (p*17 + ms/20) % 28 + 2;
    if ((ms/40 + p*10) % 7 < 3)
      display->drawPixel(sx, sy, c565(180,255,180));
  }}
  for (int d = 0; d < 5; d++) {{
    int rx = (d*23 + ms/60) % 100 + 14;
    int ry = (d*13 + ms/12) % 24 + 4;
    char rc = "0123456789ABCDEF"[(d*7 + ms/100) % 16];
    drawFontChar(rx, ry, rc, c565(0,100+(ry%3)*50,0));
  }}
}}

// ── THEME 7: FIRE ──────────────────────────
// Large fire digits with per-row flame coloring
static void drawTheme_Fire(int h, int m, int s, unsigned long ms) {{
  display->fillRect(0, 0, 128, 32, c565(0,0,0));
  const uint8_t lx[5] = {{6, 30, 54, 78, 102}};
  char buf[5];
  buf[0]='0'+h/10; buf[1]='0'+h%10; buf[2]=':'; buf[3]='0'+m/10; buf[4]='0'+m%10;
  for (int ci = 0; ci < 5; ci++) {{
    if (ci == 2 && (ms/1000)%2 == 0) continue;
    int idx = -1;
    for (int i = 0; i < 11; i++) {{ if (font_chars[i] == buf[ci]) {{ idx = i; break; }} }}
    if (idx < 0) continue;
    uint8_t rows[7];
    memcpy_P(rows, font5x7[idx], 7);
    for (int row = 0; row < 7; row++) {{
      for (int col = 0; col < 5; col++) {{
        if (rows[row] & (0x10 >> col)) {{
          int wobX = (sin8[(row*13+col*7+ci*30+ms/10)%256]-128)/64;
          int wobY = (sin8[(row*11+col*9+ci*40+ms/12)%256]-128)/64;
          int bx = lx[ci] + col*4 + wobX;
          int by = 2 + row*4 + wobY;
          int fl = (sin8[(row*31+col*17+ms/8)%256]+128)/10;
          uint8_t r,g,b;
          if (row >= 5) {{ r=255; g=180+fl; if(g>255)g=255; b=80+fl/2; if(b>255)b=255; }}
          else if (row >= 3) {{ r=255; g=100+fl; b=fl/4; }}
          else if (row >= 1) {{ r=180+fl; if(r>255)r=255; g=20+fl/5; b=0; }}
          else {{ r=100+fl/3; g=5; b=0; }}
          display->fillRect(bx, by, 4, 4, c565(r,g,b));
        }}
      }}
    }}
  }}
  for (int w = 0; w < 4; w++) {{
    int wx = (w*31+ms/40)%108+10;
    int wy = 31-(ms/60+w*17)%28;
    int wh = 2+(w%3);
    for (int p = 0; p < wh; p++) {{
      int t = (ms/25+w*41+p*60)%256;
      display->drawPixel(wx+p-1, wy-p, c565(255,100+t/4,0));
    }}
  }}
}}

// ── THEME 8: RAINBOW ───────────────────────
// Large rainbow-cycling digits with sparkles
static void drawTheme_Rainbow(int h, int m, int s, unsigned long ms) {{
  display->fillRect(0, 0, 128, 32, c565(0,0,15));
  const uint8_t lx[5] = {{6, 30, 54, 78, 102}};
  char buf[5];
  buf[0]='0'+h/10; buf[1]='0'+h%10; buf[2]=':'; buf[3]='0'+m/10; buf[4]='0'+m%10;
  for (int ci = 0; ci < 5; ci++) {{
    if (ci == 2 && (ms/1000)%2 == 0) continue;
    int yOff = sin8[(ms/20 + ci*60) % 256] / 30;
    int idx = -1;
    for (int i = 0; i < 11; i++) {{ if (font_chars[i] == buf[ci]) {{ idx = i; break; }} }}
    if (idx < 0) continue;
    uint8_t rows[7];
    memcpy_P(rows, font5x7[idx], 7);
    for (int row = 0; row < 7; row++) {{
      for (int col = 0; col < 5; col++) {{
        if (rows[row] & (0x10 >> col)) {{
          int bx = lx[ci] + col*4;
          int by = 2 + yOff + row*4;
          int hue = (ms/30 + ci*30 + row*20 + col*35) % 256;
          uint8_t r,g,b;
          int hp = hue % 85;
          if (hue < 85)       {{ r=255-hp*3; g=hp*3;   b=0; }}
          else if (hue < 170) {{ r=0;        g=255-hp*3; b=hp*3; }}
          else                {{ r=hp*3;     g=0;        b=255-hp*3; }}
          display->fillRect(bx, by, 4, 4, c565(r,g,b));
        }}
      }}
    }}
  }}
  for (int s2 = 0; s2 < 6; s2++) {{
    int sx = (s2*29 + ms/60) % 118 + 5;
    int sy = (s2*17 + ms/50) % 28 + 2;
    int sh = (ms/15 + s2*73) % 256;
    int h2 = sh % 85;
    uint8_t r,g,b;
    if (sh < 85)       {{ r=255-h2*3; g=h2*3;   b=0; }}
    else if (sh < 170) {{ r=0;        g=255-h2*3; b=h2*3; }}
    else               {{ r=h2*3;     g=0;        b=255-h2*3; }}
    display->drawPixel(sx, sy, c565(r,g,b));
  }}
}}
// ── MAIN DISPATCHER ──────────────────────────────────────────
#define RETRO_THEME_COUNT 9

static void drawRetroClockTheme(int theme, int h, int m, int s, unsigned long ms) {{
  switch (theme) {{
    case 0: drawTheme_Mario(h, m, s, ms); break;
    case 1: drawTheme_Tetris(h, m, s, ms); break;
    case 2: drawTheme_Pacman(h, m, s, ms); break;
    case 3: drawTheme_Invaders(h, m, s, ms); break;
    case 4: drawTheme_Pong(h, m, s, ms); break;
    case 5: drawTheme_Neon(h, m, s, ms); break;
    case 6: drawTheme_Matrix(h, m, s, ms); break;
    case 7: drawTheme_Fire(h, m, s, ms); break;
    case 8: drawTheme_Rainbow(h, m, s, ms); break;
  }}
}}

static const char* retroThemeNames[RETRO_THEME_COUNT] = {{
  "Super Mario", "Tetris", "Pac-Man",
  "Space Invaders", "Pong",
  "Neon", "Matrix", "Fire", "Rainbow"
}};
"""

# ── Generate the header ───────────────────────────────────────
def generate_header():
    print("[GEN] Generating clock_themes.h...")
    content = generate_all_sprites()
    with open(OUTPUT_H, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"[GEN] Written: {OUTPUT_H} ({len(content)} bytes)")

# ── Generate the .ino ─────────────────────────────────────────
def generate_ino():
    print("[GEN] Generating retro_clock.ino...")
    ino = """// retro_clock.ino — Retro Game Pixel Art Clock for 128x32 DMD
// Reads config.ini from SD card for clock settings.
// Compile: arduino-cli compile --fqbn esp32:esp32:esp32 .

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <WiFi.h>
#include <SD.h>
#include <SPI.h>
#include <time.h>
#include "clock_themes.h"

// ── Pin configuration (HUB75 128x32, 2x chained 64x32) ─────
#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 14
#define G2_PIN 12
#define B2_PIN 13
#define A_PIN  33
#define B_PIN  32
#define C_PIN  22
#define D_PIN  17
#define E_PIN  -1
#define LAT_PIN 4
#define OE_PIN  15
#define CLK_PIN 16
#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define PANEL_CHAIN 2

// SD card (VSPI)
#define SD_CS    5
#define SD_MOSI  23
#define SD_MISO  19
#define SD_SCLK  18

MatrixPanel_I2S_DMA *display = nullptr;
SPIClass spiSD(HSPI);

// ── Config defaults ─────────────────────────────────────────
bool   clockEnabled      = true;
int    clockTheme        = -1;      // -1=random, 0..5=theme
int    clockDuration     = 10;      // seconds per theme before switching
int    clockIntervalMin  = 0;       // 0=stay on, >0=show then wait N min
int    clockBrightness   = 80;      // 0-100
String clockTimeZone     = "CET-1CEST,M3.5.0,M10.5.0/3";
String wifiSSID          = "";
String wifiPassword      = "";
bool   wifiEnabled       = false;

int    currentTheme      = 0;
bool   ntpSynced         = false;
unsigned long lastSwitchMs = 0;
unsigned long themeStartMs = 0;
int    lastThemePick     = -1;

// ── Config parser ───────────────────────────────────────────
void loadConfig() {
  File cfg = SD.open("/config.ini");
  if (!cfg) {
    Serial.println("[CONFIG] No config.ini found, using defaults");
    return;
  }
  bool inClock = false;
  while (cfg.available()) {
    String line = cfg.readStringUntil('\\n');
    line.trim();
    if (line.length() == 0 || line.startsWith("#") || line.startsWith(";")) continue;
    if (line.startsWith("[") && line.endsWith("]")) {
      inClock = (line.equalsIgnoreCase("[CLOCK]"));
      continue;
    }
    if (!inClock) {
      // Global settings (outside [CLOCK])
      if      (line.startsWith("wifi_ssid="))        wifiSSID = line.substring(line.indexOf('=')+1);
      else if (line.startsWith("wifi_password="))    wifiPassword = line.substring(line.indexOf('=')+1);
      else if (line.startsWith("wifi_enabled="))     wifiEnabled = (line.substring(line.indexOf('=')+1).toInt() != 0);
      else if (line.startsWith("brightness="))       clockBrightness = constrain(line.substring(line.indexOf('=')+1).toInt(), 0, 100);
      continue;
    }
    // [CLOCK] section
    String val = line.substring(line.indexOf('=')+1); val.trim();
    if      (line.startsWith("CLOCK_ENABLED="))      clockEnabled = (val.toInt() != 0);
    else if (line.startsWith("CLOCK_THEME="))        clockTheme = constrain(val.toInt(), -1, RETRO_THEME_COUNT-1);
    else if (line.startsWith("CLOCK_DURATION="))     clockDuration = (val.toInt() < 1) ? 1 : val.toInt();
    else if (line.startsWith("CLOCK_INTERVAL_MIN=")) clockIntervalMin = (val.toInt() < 0) ? 0 : val.toInt();
    else if (line.startsWith("CLOCK_COLOR="))        { /* kept for compat, not used by retro themes */ }
    else if (line.startsWith("TZ="))                 { clockTimeZone = val; }
  }
  cfg.close();
  Serial.println("[CONFIG] loaded: enabled=" + String(clockEnabled) +
    " theme=" + String(clockTheme) + " dur=" + String(clockDuration) +
    "s interval=" + String(clockIntervalMin) + "min");
}

int pickTheme() {
  if (clockTheme >= 0 && clockTheme < RETRO_THEME_COUNT) return clockTheme;
  int t;
  do { t = random(0, RETRO_THEME_COUNT); } while (t == lastThemePick && RETRO_THEME_COUNT > 1);
  lastThemePick = t;
  return t;
}

// ── Wi-Fi / NTP ─────────────────────────────────────────────
void setupWiFi() {
  if (!wifiEnabled || wifiSSID.length() == 0) {
    Serial.println("[WIFI] disabled");
    return;
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
  display->setTextSize(1);
  display->setTextColor(display->color565(255,200,0));
  display->setCursor(8, 12); display->print("CONNECT...");
  for (int tries = 0; tries < 30; tries++) {
    if (WiFi.status() == WL_CONNECTED) break;
    delay(250); yield();
  }
  if (WiFi.status() == WL_CONNECTED) {
    display->setCursor(8, 12); display->setTextColor(display->color565(0,255,0));
    display->print("WiFi OK   ");
    delay(400);
    configTzTime(clockTimeZone.c_str(), "pool.ntp.org", "time.google.com");
    display->setCursor(8, 22); display->setTextColor(display->color565(255,200,100));
    display->print("NTP...");
    for (int i = 0; i < 30; i++) {
      time_t now; struct tm ti;
      time(&now); localtime_r(&now, &ti);
      if (ti.tm_year > 100) { ntpSynced = true; break; }
      delay(200); yield();
    }
    display->setCursor(8, 22);
    if (ntpSynced) { display->setTextColor(display->color565(0,255,0)); display->print("NTP OK  "); }
    else           { display->setTextColor(display->color565(255,100,0)); display->print("NTP FAIL"); }
    delay(600);
  } else {
    display->setCursor(8, 12); display->setTextColor(display->color565(255,100,0));
    display->print("WiFi FAIL");
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200); delay(100);
  randomSeed(analogRead(A0) ^ micros());

  // Display init
  HUB75_I2S_CFG::i2s_pins pins = {
    R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN,
    A_PIN, B_PIN, C_PIN, D_PIN, E_PIN,
    LAT_PIN, OE_PIN, CLK_PIN
  };
  HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN, pins);
  mxconfig.latch_blanking = 4;
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_10M;
  mxconfig.min_refresh_rate = 60;
  mxconfig.clkphase = false;
  mxconfig.double_buff = false;

  display = new MatrixPanel_I2S_DMA(mxconfig);
  display->begin();
  display->setBrightness8(map(clockBrightness, 0, 100, 0, 255));
  display->clearScreen();

  display->setTextSize(1);
  display->setTextColor(display->color565(255,200,0));
  display->setCursor(8, 2);  display->print("RETRO CLOCK");
  display->setCursor(8, 12); display->setTextColor(display->color565(100,200,255));
  display->print("v1.0 128x32");
  delay(300);

  // SD init
  spiSD.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, spiSD)) {
    display->setCursor(8, 22); display->setTextColor(display->color565(255,80,0));
    display->print("SD FAIL");
    Serial.println("[SD] No card, using defaults");
  } else {
    display->setCursor(8, 22); display->setTextColor(display->color565(0,200,0));
    display->print("SD OK  ");
    loadConfig();
  }
  delay(500);

  // Apply config brightness
  display->setBrightness8(map(clockBrightness, 0, 100, 0, 255));
  display->clearScreen();

  setupWiFi();
  display->clearScreen();

  currentTheme = pickTheme();
  themeStartMs = millis();
  lastSwitchMs = millis();
}

void loop() {
  yield();

  // Get current time
  time_t now;
  struct tm ti;
  time(&now);
  localtime_r(&now, &ti);
  int h = ti.tm_hour;
  int m = ti.tm_min;
  int s = ti.tm_sec;

  // Auto-theme switching (every clockDuration seconds)
  unsigned long ms = millis();
  if (clockTheme == -1 && (ms - themeStartMs >= (unsigned long)clockDuration * 1000UL)) {
    currentTheme = pickTheme();
    themeStartMs = ms;
    // Show theme name briefly
    display->fillRect(0, 0, 128, 32, 0);
    display->setTextSize(1);
    display->setTextColor(display->color565(255,255,255));
    int tx = (128 - strlen(retroThemeNames[currentTheme]) * 6) / 2;
    if (tx < 0) tx = 0;
    display->setCursor(tx, 12);
    display->print(retroThemeNames[currentTheme]);
    delay(800);
    display->clearScreen();
  }

  // Interval mode: if clockIntervalMin > 0, show clock then blank
  if (clockIntervalMin > 0 && clockEnabled) {
    unsigned long elapsed = (ms - lastSwitchMs) / 60000UL;
    if (elapsed >= (unsigned long)clockIntervalMin) {
      // Clock display period
      unsigned long clockEnd = millis() + ((unsigned long)clockDuration * 1000UL);
      while (millis() < clockEnd) {
        yield();
        time_t nw; struct tm t2;
        time(&nw); localtime_r(&nw, &t2);
        display->fillRect(0, 0, 128, 32, 0);
        drawRetroClockTheme(currentTheme, t2.tm_hour, t2.tm_min, t2.tm_sec, millis());
        delay(100);
      }
      display->clearScreen();
      lastSwitchMs = ms;
    } else {
      // Blank/standby - just clear
      display->fillRect(0, 0, 128, 32, 0);
      // tiny dots to show alive
      display->drawPixel(126, 30, display->color565(20,20,20));
      delay(250);
      return;
    }
  }

  // Normal continuous display
  if (!clockEnabled) {
    display->fillRect(0, 0, 128, 32, 0);
    display->setTextSize(1);
    display->setTextColor(display->color565(40,40,40));
    display->setCursor(44, 14); display->print("STANDBY");
    delay(250);
    return;
  }

  display->fillRect(0, 0, 128, 32, 0);
  drawRetroClockTheme(currentTheme, h, m, s, millis());
  delay(50);
}
"""
    with open(SKETCH_INO, 'w', encoding='utf-8') as f:
        f.write(ino)
    print(f"[GEN] Written: {SKETCH_INO} ({len(ino)} bytes)")

# ── Compile ───────────────────────────────────────────────────
def compile_sketch():
    print("\n[BUILD] Compiling via arduino-cli...")
    result = subprocess.run(
        ["arduino-cli", "compile", "--fqbn", "esp32:esp32:esp32", "--output-dir", "build", "."],
        cwd=SKETCH_DIR,
        capture_output=True, text=True
    )
    print(result.stdout)
    if result.stderr:
        print("STDERR:", result.stderr[:500])
    if result.returncode == 0:
        print(f"[BUILD] SUCCESS")
    else:
        print(f"[BUILD] FAILED (code {result.returncode})")
    return result.returncode

def upload_sketch(port):
    print(f"\n[UPLOAD] Uploading to {port}...")
    result = subprocess.run(
        ["arduino-cli", "upload", "--fqbn", "esp32:esp32:esp32", "--port", port, "--input-dir", "build", "."],
        cwd=SKETCH_DIR,
        capture_output=True, text=True
    )
    print(result.stdout)
    if result.stderr:
        print("STDERR:", result.stderr[:500])
    if result.returncode == 0:
        print(f"[UPLOAD] SUCCESS")
    else:
        print(f"[UPLOAD] FAILED (code {result.returncode})")
    return result.returncode

# ── Main ──────────────────────────────────────────────────────
if __name__ == "__main__":
    if "--help" in sys.argv or "-h" in sys.argv:
        print(__doc__)
        sys.exit(0)

    generate_header()
    generate_ino()

    if "--compile" in sys.argv:
        rc = compile_sketch()
        if rc != 0:
            sys.exit(1)
        # Check for upload port
        for arg in sys.argv:
            if arg.startswith("COM") or arg.startswith("/dev/"):
                upload_sketch(arg)
                break
    else:
        print("\n[HINT] Add --compile to also compile via arduino-cli")
        print("[HINT] Add --compile COM4 to also upload")
