// ====================================================================
//  RD_Cine.h - caras de cine
//  No se incluye directamente: lo hace RelojDinamico.h.
// ====================================================================
#ifndef RD_CINE_H
#define RD_CINE_H

#ifndef RELOJ_DINAMICO_H
#error "Incluye RelojDinamico.h, no este fichero"
#endif

// ────────────────────────────────────────────────────────────────────
// "StarWars"
// ────────────────────────────────────────────────────────────────────
#define RDW_DIG_Y     9        // fila superior de los dígitos (14 de alto -> hasta y=22)
#define RDW_FX        4        // esquina del Halcón (22x11)
#define RDW_FY        11
#define RDW_SX        112      // centro de la Estrella de la Muerte (radio 11)
#define RDW_SY        15
static const int16_t RDW_DIG_X[4] = { 35, 48, 69, 82 };

// Caza TIE 7x7. P panel, G cabina, K cristal
static const char RDW_TIE[7][8] PROGMEM = {
  "P.....P", "P.....P", "P.GGG.P", "PPGKGPP", "P.GGG.P", "P.....P", "P.....P"
};

struct RDWEstado {
  int8_t   d[4], obj[4];
  uint8_t  hs;              // 0 = nada, 1..26 = ticks del salto al hiperespacio
  bool     hsAnt;
  bool     tie, dp;
  int16_t  tx;              // x del caza TIE
  uint16_t off[3];          // desplazamiento de las tres capas de estrellas
  uint32_t n, ult;
  bool     ultOk;
};
static RDWEstado rdw;

static void rdw_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdw.d);
  for (int i = 0; i < 4; i++) rdw.obj[i] = rdw.d[i];
  rdw.hs = 0; rdw.hsAnt = false; rdw.tie = false; rdw.tx = 128;
  rdw.off[0] = (uint16_t)(m * 7); rdw.off[1] = (uint16_t)(m * 3); rdw.off[2] = 0;
  rdw.dp = (s % 2 == 0); rdw.n = 0; rdw.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdw_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  for (int i = 0; i < 4; i++) rdw.obj[i] = nd[i];      // el tick arranca el salto cuando hay algo que cambiar
}

static void rdw_segundo(int s) { rdw.dp = (s % 2 == 0); rdMarcar(62, 12, 2, 9); }

static void rdw_tick() {
  rdw.n++;
  bool todo = false;

  // Las estrellas se deslizan hacia la izquierda; la capa rápida es la más brillante
  if ((rdw.n % 12) == 0) { rdw.off[0]++; todo = true; }
  if ((rdw.n % 6) == 0)  { rdw.off[1]++; todo = true; }
  if ((rdw.n % 3) == 0)  { rdw.off[2]++; todo = true; }

  // Salto al hiperespacio
  if (rdw.hs == 0) {
    for (int i = 0; i < 4; i++) if (rdw.d[i] != rdw.obj[i]) { rdw.hs = 1; break; }
  } else {
    rdw.hs++;
    if (rdw.hs == 12) for (int i = 0; i < 4; i++) rdw.d[i] = rdw.obj[i];   // en pleno fogonazo
    if (rdw.hs > 26) rdw.hs = 0;
  }
  bool hiper = rdw.hs != 0;
  if (hiper || rdw.hsAnt) todo = true;
  rdw.hsAnt = hiper;
  if (todo) rdMarcar(0, 0, RD_W, RD_H);

  // Caza TIE que cruza el cielo de vez en cuando
  if (!rdw.tie && (rdw.n % 260) == 120) { rdw.tie = true; rdw.tx = 128; }
  if (rdw.tie) {
    rdMarcar(rdw.tx - 1, 0, 9, 9);
    rdw.tx--;
    if (rdw.tx < -8) rdw.tie = false; else rdMarcar(rdw.tx - 1, 0, 9, 9);
  }
  // Motor del Halcón y ojo del superláser
  if ((rdw.n & 1) == 0)     rdMarcar(RDW_FX, RDW_FY + 2, 5, 7);
  if ((rdw.n % 6) == 0)     rdMarcar(RDW_SX - 10, RDW_SY - 9, 9, 9);
}

static void rdw_paso(uint32_t ms) {
  if (!rdw.ultOk) { rdw.ult = ms; rdw.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdw.ult) >= RDM_TICK_MS && n < 3) { rdw.ult += RDM_TICK_MS; rdw_tick(); n++; }
  if (n == 3) rdw.ult = ms;
}

// Estrellas: tres capas; en el hiperespacio cada estrella deja una raya hacia el centro
static uint16_t rdw_estrellas(int x, int y) {
  int L = 0;
  if (rdw.hs) {
    int t = rdw.hs;
    int Lm = (t <= 10) ? 2 * t : (t <= 14 ? 20 : 20 * (26 - t) / 12);
    if (Lm < 0) Lm = 0;
    L = Lm * (abs(x - 64) + 16) / 80;                  // más larga cuanto más lejos del centro
  }
  int dirx = (x < 64) ? -1 : 1;
  for (int k = 2; k >= 0; k--) {
    int thr  = (k == 2) ? 1 : 2;
    int base = (k == 2) ? 255 : (k == 1) ? 170 : 90;
    for (int j = 0; j <= L; j++) {
      if (rdi_hash(x + dirx * j + (int)rdw.off[k], y, k) < thr) {
        int br = base * (L - j + 1) / (L + 1);          // cabeza brillante, cola que se apaga
        return RD_C(br, br, (br + 40 > 255) ? 255 : br + 40);
      }
    }
  }
  return 0;
}

static uint16_t rdw_halcon(int x, int y) {
  bool salto = (rdw.hs >= 1 && rdw.hs <= 14);
  int u = x - RDW_FX, v = y - RDW_FY - (salto ? (int)(rdw.n & 1) : 0);
  if ((unsigned)u >= 22 || (unsigned)v >= 11) return 0;
  if (u <= 3 && v >= 3 && v <= 7) {                     // motor
    if (salto) return RD_C(235, 245, 255);
    return ((rdw.n >> 1) & 1) ? RD_C(160, 210, 255) : RD_C(70, 150, 255);
  }
  int dx = u - 9, dy = v - 5, d2 = dx * dx + dy * dy;
  int cx = u - 13, cy = v - 8;
  if (u >= 11 && u <= 15 && cx * cx + cy * cy <= 2) return (cx == -1 && cy == -1) ? RD_C(110, 160, 200) : RD_C(40, 48, 70);   // cabina
  if (u <= 14 && d2 <= 25) {                            // casco circular
    if (d2 >= 9 && d2 <= 11) return RD_C(120, 120, 134);
    int sh = u + v;
    return (sh < 9) ? RD_C(206, 206, 216) : (sh > 13) ? RD_C(148, 148, 162) : RD_C(178, 178, 190);
  }
  if (u >= 14 && ((v >= 3 && v <= 4) || (v >= 6 && v <= 7)))   // las dos pinzas delanteras
    return (u == 21) ? RD_C(110, 110, 124) : RD_C(160, 160, 172);
  return 0;
}

static uint16_t rdw_estrellaMuerte(int x, int y) {
  int dx = x - RDW_SX, dy = y - RDW_SY;
  if (dx * dx + dy * dy > 121) return 0;
  int ex = x - (RDW_SX - 5), ey = y - (RDW_SY - 4), e2 = ex * ex + ey * ey;   // plato del superláser
  if (e2 <= 4) {
    static const uint8_t brillo[4] = { 110, 170, 230, 170 };
    return RD_C(30, brillo[(rdw.n / 6) & 3], 70);
  }
  if (e2 <= 9) return RD_C(28, 32, 38);
  int sh = 120 - (dx * 4 + dy * 3);                      // luz desde arriba a la izquierda
  if (sh < 30) sh = 30;
  if (sh > 200) sh = 200;
  if (dy == 0) sh /= 2;                                  // trinchera ecuatorial
  return RD_C(sh, sh + 4, sh + 14);
}

static uint16_t rdw_mezcla(uint16_t c, int a) {          // mezcla con blanco, a = 1..4 (cuartos)
  int r = (c >> 11) & 31, g = (c >> 5) & 63, b = c & 31;
  r += ((31 - r) * a) / 4; g += ((63 - g) * a) / 4; b += ((31 - b) * a) / 4;
  return (uint16_t)((r << 11) | (g << 5) | b);
}

static uint16_t rdw_base(int x, int y) {
  // 1) caza TIE
  if (rdw.tie) {
    int lx = x - rdw.tx, ly = y - 1;
    if ((unsigned)lx < 7 && (unsigned)ly < 7) {
      char ch = (char)pgm_read_byte(&RDW_TIE[ly][lx]);
      if (ch == 'P') return (lx == 0 || lx == 6) ? RD_C(96, 100, 116) : RD_C(120, 126, 142);
      if (ch == 'G') return RD_C(150, 156, 168);
      if (ch == 'K') return RD_C(24, 26, 34);
    }
  }
  // 2) dígitos amarillos
  for (int i = 0; i < 4; i++) {
    int lx = x - RDW_DIG_X[i];
    if ((unsigned)lx >= 10) continue;
    int ly = y - RDW_DIG_Y;
    if ((unsigned)ly < 14 && (pgm_read_byte(&RDM_FUENTE[rdw.d[i]][ly >> 1]) & (0x10 >> (lx >> 1))))
      return (ly & 1) ? RD_C(214, 176, 0) : RD_C(255, 232, 31);
    break;
  }
  if (rdw.dp && (unsigned)(x - 62) < 2 && ((unsigned)(y - 13) < 2 || (unsigned)(y - 18) < 2))
    return RD_C(255, 232, 31);
  // 3) Halcón Milenario y Estrella de la Muerte
  if (x < 28) { uint16_t c = rdw_halcon(x, y); if (c) return c; }
  if (x >= 100) { uint16_t c = rdw_estrellaMuerte(x, y); if (c) return c; }
  // 4) estrellas
  return rdw_estrellas(x, y);
}

static uint16_t rdw_pixel(int x, int y) {
  uint16_t c = rdw_base(x, y);
  int a = (rdw.hs == 11) ? 2 : (rdw.hs == 12) ? 4 : (rdw.hs == 13) ? 3 : (rdw.hs == 14) ? 1 : 0;   // fogonazo
  return a ? rdw_mezcla(c, a) : c;
}

// ────────────────────────────────────────────────────────────────────
// "Matrix"
// ────────────────────────────────────────────────────────────────────
#define RDN_DIG_Y     9        // fila superior de los dígitos (14 de alto -> hasta y=22)
static const int16_t RDN_DIG_X[4] = { 35, 48, 69, 82 };
// Letras 3x5 (A-Z): cada fila son 3 bits, el bit 2 es la columna de la izquierda
static const uint8_t RDN_LETRA[26][5] PROGMEM = {
  {2,5,7,5,5}, {6,5,6,5,6}, {3,4,4,4,3}, {6,5,5,5,6}, {7,4,6,4,7}, {7,4,6,4,4}, {3,4,5,5,3},
  {5,5,7,5,5}, {7,2,2,2,7}, {1,1,1,5,2}, {5,5,6,5,5}, {4,4,4,4,7}, {5,7,7,5,5}, {6,5,5,5,5},
  {2,5,5,5,2}, {6,5,6,4,4}, {2,5,5,6,3}, {6,5,6,5,5}, {3,4,2,1,6}, {7,2,2,2,2}, {5,5,5,5,7},
  {5,5,5,5,2}, {5,5,7,7,5}, {5,5,2,5,5}, {5,5,2,2,2}, {7,1,2,4,7}
};
static const char* const RDN_MSG[4] = {
  "WAKE UP NEO", "FOLLOW THE WHITE RABBIT", "THE MATRIX HAS YOU", "KNOCK KNOCK NEO"
};

struct RDNEstado {
  int8_t   d[4], obj[4];
  uint8_t  pm;              // dígitos que cambian en este evento
  uint16_t ev;              // ticks del evento (0 = nada)
  uint16_t t0;              // tick en el que empiezan a corromperse los dígitos
  uint8_t  len, msgE, msg;  // longitud y número del mensaje (el del evento y el del último minuto)
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDNEstado rdn;

static void rdn_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdn.d);
  for (int i = 0; i < 4; i++) rdn.obj[i] = rdn.d[i];
  rdn.pm = 0; rdn.ev = 0; rdn.t0 = 0; rdn.len = 0; rdn.msgE = 0; rdn.msg = m % 4;
  rdn.dp = (s % 2 == 0); rdn.n = 0; rdn.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdn_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  for (int i = 0; i < 4; i++) rdn.obj[i] = nd[i];
  rdn.msg = m % 4;                                   // el tick arranca el evento cuando hay algo que cambiar
}

static void rdn_segundo(int s) { rdn.dp = (s % 2 == 0); rdMarcar(62, 13, 2, 7); }

static void rdn_tick() {
  rdn.n++;
  bool todo = (rdn.n & 1) == 0;                      // la lluvia avanza cada 2 ticks

  if (rdn.ev == 0) {
    uint8_t mask = 0;
    for (int i = 0; i < 4; i++) if (rdn.d[i] != rdn.obj[i]) mask |= (1 << i);
    if (mask) {
      rdn.pm = mask; rdn.ev = 1; rdn.msgE = rdn.msg;
      rdn.len = (uint8_t)strlen(RDN_MSG[rdn.msgE]);
      rdn.t0 = rdn.len * 2 + 8;                       // primero se escribe el mensaje
    }
  } else {
    rdn.ev++;
    for (int i = 0; i < 4; i++)
      if ((rdn.pm & (1 << i)) && (int)rdn.ev - rdn.t0 - 2 * i == 22) rdn.d[i] = rdn.obj[i];   // dígito ya reconstruido
    if (rdn.ev > rdn.t0 + 36) { rdn.ev = 0; rdn.pm = 0; todo = true; }
  }
  if (rdn.ev) todo = true;
  if (todo) rdMarcar(0, 0, RD_W, RD_H);
}

static void rdn_paso(uint32_t ms) {
  if (!rdn.ultOk) { rdn.ult = ms; rdn.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdn.ult) >= RDM_TICK_MS && n < 3) { rdn.ult += RDM_TICK_MS; rdn_tick(); n++; }
  if (n == 3) rdn.ult = ms;
}

// Tick de la transición del dígito i (0 = no está cambiando)
static int rdn_t(int i) {
  if (!rdn.ev || !(rdn.pm & (1 << i))) return 0;
  int t = (int)rdn.ev - rdn.t0 - 2 * i;
  return (t >= 1 && t <= 21) ? t : 0;
}
static bool rdn_fuente(int dig, int lx, int ly) {
  return (unsigned)lx < 10 && (unsigned)ly < 14 &&
         (pgm_read_byte(&RDM_FUENTE[dig][ly >> 1]) & (0x10 >> (lx >> 1)));
}
static uint16_t rdn_colorDigito(int ly) {
  return (ly < 4) ? RD_C(190, 255, 210) : (ly < 9) ? RD_C(90, 245, 140) : RD_C(30, 200, 80);
}

// ¿Hay un bloque del dígito i en (lx,ly)? Si lo hay, devuelve su color en *col
static bool rdn_lit(int i, int lx, int ly, uint16_t* col) {
  int t = rdn_t(i);
  if (t == 0) {
    if (!rdn_fuente(rdn.d[i], lx, ly)) return false;
    *col = rdn_colorDigito(ly);
    return true;
  }
  if (t <= 8) {                                      // corrupción: bloques que parpadean y bloques de más
    bool on = rdn_fuente(rdn.d[i], lx, ly), ruido = false;
    if ((unsigned)lx < 10 && (unsigned)ly < 14) {
      ruido = rdi_hash(i * 50 + (lx >> 1) * 7 + (ly >> 1), (int)(rdn.n >> 1), 3) < t * 28;
      if (ruido) on = !on;
    }
    if (!on) return false;
    *col = ruido ? RD_C(225, 255, 235) : rdn_colorDigito(ly);
    return true;
  }
  if ((unsigned)lx >= 10) return false;              // el dígito se deshace y se rehace columna a columna
  int s = (t - 8) - (lx >> 1);
  int dyn = (s <= 0) ? -26 : -((26 - s * 3) > 0 ? (26 - s * 3) : 0);
  if (rdn_fuente(rdn.obj[i], lx, ly - dyn)) {        // dígito nuevo cayendo desde arriba
    *col = (dyn != 0) ? RD_C(210, 255, 225) : rdn_colorDigito(ly);
    return true;
  }
  int dyo = (s <= 0) ? 0 : s * 3;
  if (rdn_fuente(rdn.d[i], lx, ly - dyo)) {          // dígito viejo cayendo y apagándose
    int b = 255 - dyo * 9; if (b < 70) b = 70;
    *col = RD_C(b / 8, b, b / 4);
    return true;
  }
  return false;
}

// Lluvia de código: una gota por columna, con la cabeza blanca y la cola que se apaga
static uint16_t rdn_lluvia(int x, int y) {
  if ((x & 3) == 3) return 0;
  int col = x >> 2;
  int sp = 1 + (rdi_hash(col, 1, 0) % 3), T = 8 + (rdi_hash(col, 2, 0) % 14);
  int per = 32 + T + 4;
  int hp = (int)((((rdn.n >> 1) * (uint32_t)sp) + rdi_hash(col, 3, 0) + col * 5) % (uint32_t)per);
  int d = hp - y;
  if (d < 0 || d > T) return 0;
  int gy = y % 6;
  if (gy == 5) return 0;
  int cell = y / 6, gx = x & 3;
  int ver = (int)((rdn.n >> 1) + col * 5 + cell * 3) / 5;
  unsigned patt = ((unsigned)rdi_hash(col, cell, ver) << 8) | rdi_hash(cell, col, ver + 1);
  if (!((patt >> (gy * 3 + gx)) & 1)) return 0;      // símbolo de 3x5 aleatorio que cambia de vez en cuando
  if (d <= 1) return RD_C(170, 255, 190);
  int b = 255 * (T - d) * (T - d) / (T * T);
  return RD_C(0, 20 + b * 130 / 255, b / 8);
}

static uint16_t rdn_pixel(int x, int y) {
  // 1) mensaje que se escribe arriba, con fondo negro para que se lea
  if (rdn.ev && y <= 6) {
    int len = rdn.len, w = len * 4 - 1, x0 = (RD_W - w) / 2;
    if (x >= x0 - 2 && x <= x0 + w + 1) {
      int px = x - x0;
      if (px >= 0 && (px & 3) < 3 && y >= 1 && y <= 5) {
        int ci = px >> 2, gy = y - 1, gx = px & 3;
        int nchar = (int)(rdn.ev >> 1); if (nchar > len) nchar = len;
        if (ci < nchar) {
          char ch = RDN_MSG[rdn.msgE][ci];
          if (ch >= 'A' && ch <= 'Z' && (pgm_read_byte(&RDN_LETRA[ch - 'A'][gy]) & (4 >> gx))) return RD_C(160, 255, 185);
        } else if (ci == nchar && ((rdn.n >> 2) & 1)) return RD_C(160, 255, 185);       // cursor
      }
      return 0;
    }
  }
  // 2) dígitos con contorno oscuro
  for (int i = 0; i < 4; i++) {
    int lx = x - RDN_DIG_X[i];
    if (lx < -1 || lx > 10) continue;
    int ly = y - RDN_DIG_Y;
    if (!rdn_t(i) && (ly < -1 || ly > 14)) break;
    uint16_t c;
    if (rdn_lit(i, lx, ly, &c)) return c;
    for (int dy = -1; dy <= 1; dy++)
      for (int dx = -1; dx <= 1; dx++)
        if ((dx || dy) && rdn_lit(i, lx + dx, ly + dy, &c)) return RD_C(0, 8, 0);
    break;
  }
  if (rdn.dp && (unsigned)(x - 62) < 2 && ((unsigned)(y - 13) < 2 || (unsigned)(y - 18) < 2))
    return RD_C(150, 255, 180);
  // 3) lluvia
  return rdn_lluvia(x, y);
}

// ────────────────────────────────────────────────────────────────────
// "Jurassic Park" 
// ────────────────────────────────────────────────────────────────────
#define RDJ_REX_Y     4        // fila superior del T-Rex (27 de alto: pies en y=30)
#define RDJ_DIG_Y     6        // dígitos grandes: filas 6..19 dentro del panel
static const int16_t RDJ_DIG_X[4] = { 37, 50, 70, 83 };
static const uint8_t RDJ_SELVA[32] PROGMEM = { 3,6,4,8,5,7,3,6,8,5,4,7,6,3,8,5, 4,7,6,3,8,5,7,4,6,8,3,5,7,4,6,8 };
static const uint8_t RDJ_FLASH_K[5] PROGMEM = { 0, 2, 4, 6, 8 };

// Colores: 1 piel oscura, 2 vientre, 3 sombra, 4 ojo y dientes, 5 interior de la boca
static const RDRect RDJ_REX_A[] PROGMEM = {
  {24,16,9,8,3},{26,24,4,2,1},{24,26,9,1,1},
  {18,17,8,7,3},{19,24,4,2,1},{15,26,9,1,1},
  {32,7,6,7,1},{36,9,2,4,1},
  {14,5,19,14,1},{16,14,14,5,2},{16,5,15,2,3},
  {12,14,4,2,1},{11,15,2,2,1},
  {11,4,8,10,1},{0,1,14,9,1},{0,3,2,5,1},{10,3,2,2,4}
};
static const RDRect RDJ_REX_B[] PROGMEM = {
  {22,16,9,8,3},{23,24,4,2,1},{21,26,9,1,1},
  {20,17,8,7,3},{21,24,4,2,1},{18,26,9,1,1},
  {32,7,6,7,1},{36,9,2,4,1},
  {14,5,19,14,1},{16,14,14,5,2},{16,5,15,2,3},
  {12,14,4,2,1},{11,15,2,2,1},
  {11,4,8,10,1},{0,1,14,9,1},{0,3,2,5,1},{10,3,2,2,4}
};
static const RDRect RDJ_BOCA_CERRADA[] PROGMEM = { {1,10,12,2,3},{2,10,1,1,4},{5,10,1,1,4},{8,10,1,1,4} };
static const RDRect RDJ_BOCA_ABIERTA[] PROGMEM = {
  {1,9,12,4,5},{1,13,12,3,3},{2,9,1,2,4},{4,9,1,2,4},{6,9,1,2,4},{8,9,1,2,4},{10,9,1,2,4},{3,13,1,1,4},{6,13,1,1,4},{9,13,1,1,4}
};
#define RDJ_N(a) ((int)(sizeof(a) / sizeof(a[0])))

struct RDJEstado {
  int8_t   d[4], obj[4];
  uint8_t  modo;        // 0 reposo 1 llega 2 ruge 3 se va
  uint8_t  t;
  int16_t  tx;          // x izquierda del T-Rex (128 = fuera)
  uint8_t  pata;
  bool     boca;
  int8_t   temblor;
  uint8_t  flash;
  bool     luz;
  uint8_t  lluvia;      // fotograma de la lluvia
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDJEstado rdj;

static bool rdj_difiere() {
  for (int i = 0; i < 4; i++) if (rdj.d[i] != rdj.obj[i]) return true;
  return false;
}

static void rdj_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdj.d);
  for (int i = 0; i < 4; i++) rdj.obj[i] = rdj.d[i];
  rdj.modo = 0; rdj.t = 0; rdj.tx = 128; rdj.pata = 0; rdj.boca = false;
  rdj.temblor = 0; rdj.flash = 0; rdj.luz = true; rdj.lluvia = 0;
  rdj.dp = (s % 2 == 0); rdj.n = 0; rdj.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdj_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  for (int i = 0; i < 4; i++) rdj.obj[i] = nd[i];
}

static void rdj_segundo(int s) { rdj.dp = (s % 2 == 0); rdMarcar(62, 6, 6, 14); }

static void rdj_tick() {
  rdj.n++;
  bool completo = false;

  if ((rdj.n % 3) == 0) { rdj.lluvia++; completo = true; }                     // la lluvia cae a 8 fps
  if ((rdj.n % 12) == 0) { rdMarcar(0, 5, 30, 4); rdMarcar(98, 5, 30, 4); }   // lámparas de la valla
  if ((rdj.n & 1) == 0) rdMarcar(10, 26, 26, 6);                              // ondas del charco
  if (rdj.flash) { rdj.flash--; completo = true; }
  else if (rdj.modo == 0 && (rdj.n % 800) == 500) { rdj.flash = 3; completo = true; }   // relámpago lejano
  rdj.temblor = 0;

  switch (rdj.modo) {
    case 0:
      if (rdj_difiere()) { rdj.modo = 1; rdj.t = 0; rdj.tx = 128; completo = true; }
      break;
    case 1:                                                                    // llega con pisadas que sacuden la escena
      rdj.t++; rdj.tx -= 3; completo = true;
      if ((rdj.t % 3) == 0) { rdj.pata ^= 1; rdj.temblor = 1; }
      if (rdj.tx <= 97) { rdj.tx = 97; rdj.modo = 2; rdj.t = 0; rdj.boca = true; }       // se detiene junto al panel, sin taparlo
      break;
    case 2:                                                                    // ruge, se va la luz y cambia la hora
      rdj.t++; completo = true;
      rdj.temblor = (rdj.t & 1) ? 1 : -1;
      if (rdj.t == 2) rdj.flash = 4;
      if (rdj.t == 3) { rdj.luz = false; for (int i = 0; i < 4; i++) rdj.d[i] = rdj.obj[i]; }
      if (rdj.t == 9 || rdj.t == 11) rdj.luz = true;
      if (rdj.t == 10) rdj.luz = false;
      if (rdj.t >= 14) { rdj.luz = true; rdj.boca = false; rdj.modo = 3; rdj.t = 0; }
      break;
    case 3:                                                                    // se marcha
      rdj.t++; rdj.tx += 3; completo = true;
      if ((rdj.t % 3) == 0) { rdj.pata ^= 1; rdj.temblor = 1; }
      if (rdj.tx >= 128) { rdj.tx = 128; rdj.modo = 0; }
      break;
  }
  if (completo) rdMarcar(0, 0, RD_W, RD_H);
}

static void rdj_paso(uint32_t ms) {
  if (!rdj.ultOk) { rdj.ult = ms; rdj.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdj.ult) >= RDM_TICK_MS && n < 3) { rdj.ult += RDM_TICK_MS; rdj_tick(); n++; }
  if (n == 3) rdj.ult = ms;
}

static uint16_t rdj_colorRex(uint8_t c) {
  switch (c) {
    case 1: return RD_C(44, 78, 52);
    case 2: return RD_C(86, 120, 74);
    case 3: return RD_C(26, 46, 34);
    case 4: return RD_C(240, 236, 190);
    case 5: return RD_C(150, 28, 30);
  }
  return 0;
}

static uint16_t rdj_fondo(int x, int y) {
  // calle mojada con charco y ondas
  if (y >= 27) {
    int dx = x - 22, dy = y - 29;
    if (dx * dx + dy * dy * 16 <= 100) {                                       // charco
      int v = dx * dx + dy * dy * 16;
      int R = 2 + ((int)(rdj.n >> 1) % 9);
      int dif = v - R * R; if (dif < 0) dif = -dif;
      if (dif <= R) return RD_C(110, 140, 180);
      return RD_C(30, 42, 64);
    }
    // reflejo de las lámparas
    if (rdj.luz && (x == 3 || x == 15 || x == 27 || x == 101 || x == 113 || x == 125) && ((rdj.n / 12) & 1) && y <= 30) return RD_C(90, 20, 22);
    return RD_C(20, 22, 32);
  }
  // selva en silueta
  int cima = 22 - (int)pgm_read_byte(&RDJ_SELVA[(x >> 1) & 31]) * 2;
  if (y >= cima) return RD_C(4, 14, 12);
  return RD_C(5 + (y >> 2), 14 + (y >> 1), 24 + (y >> 1));                      // cielo de tormenta
}

static uint16_t rdj_capas(int x, int y) {
  // 1) tiranosaurio
  if (rdj.tx < 128) {
    int lx = x - rdj.tx, ly = y - RDJ_REX_Y;
    if ((unsigned)lx < 38 && (unsigned)ly < 27) {
      uint8_t c = rdj.boca ? rdRectColor(RDJ_BOCA_ABIERTA, RDJ_N(RDJ_BOCA_ABIERTA), lx, ly)
                           : rdRectColor(RDJ_BOCA_CERRADA, RDJ_N(RDJ_BOCA_CERRADA), lx, ly);
      if (!c) c = rdj.pata ? rdRectColor(RDJ_REX_B, RDJ_N(RDJ_REX_B), lx, ly) : rdRectColor(RDJ_REX_A, RDJ_N(RDJ_REX_A), lx, ly);
      if (c) return rdj_colorRex(c);
    }
  }
  // 2) panel de seguridad con la hora
  if (x >= 33 && x <= 96 && y >= 3 && y <= 24) {
    if (x == 33 || x == 96 || y == 3 || y == 24) return RD_C(80, 84, 94);
    if (!rdj.luz) return RD_C(4, 4, 6);
    if (y >= 21 && y <= 23) return (((x + y) >> 1) & 1) ? RD_C(240, 200, 20) : RD_C(14, 14, 16);       // franjas de peligro
    for (int i = 0; i < 4; i++) {
      int lx = x - RDJ_DIG_X[i], ly = y - RDJ_DIG_Y;
      if ((unsigned)lx >= 10 || (unsigned)ly >= 14) continue;
      if (rdDigito2(rdj.d[i], lx, ly)) return RD_C(255, 176, 40);
      if (rdDigito2(8, lx, ly)) return RD_C(46, 26, 6);
    }
    if (x >= 64 && x <= 65 && ((y >= 9 && y <= 10) || (y >= 15 && y <= 16))) return rdj.dp ? RD_C(255, 176, 40) : RD_C(46, 26, 6);
    if ((x == 35 || x == 94) && (y == 5 || y == 22)) return RD_C(130, 134, 144);                         // remaches
    return RD_C(10, 10, 12);
  }
  // 3) lluvia
  {
    unsigned u = (unsigned)(x * 7 + y * 3 + rdj.lluvia * 5);
    if ((u % 31) == 0 && (((unsigned)y + rdj.lluvia) & 3) < 2) return RD_C(120, 150, 190);
  }
  // 4) valla eléctrica: postes, cables y lámparas
  for (int k = 0; k < 6; k++) {
    int px = (k < 3) ? 2 + k * 12 : 100 + (k - 3) * 12;
    if (x >= px && x <= px + 1 && y >= 8 && y <= 27) return RD_C(60, 66, 76);
    if (x >= px && x <= px + 1 && y >= 6 && y <= 7)
      return rdj.luz && ((((int)(rdj.n / 12) + k) & 1) == 0) ? RD_C(255, 60, 50) : RD_C(80, 20, 20);
  }
  if ((y == 11 || y == 17 || y == 23) && (x <= 30 || x >= 98)) return RD_C(72, 80, 92);
  // 5) fondo
  return rdj_fondo(x, y);
}

static uint16_t rdj_pixel(int x, int y) {
  int yy = y + rdj.temblor;
  if (yy < 0) yy = 0; else if (yy > 31) yy = 31;
  uint16_t c = rdj_capas(x, yy);
  if (!rdj.luz) c = (uint16_t)((c >> 1) & 0x7BEF);                              // apagón: todo a media luz
  if (rdj.flash) c = rdBlanco(c, pgm_read_byte(&RDJ_FLASH_K[rdj.flash > 4 ? 4 : rdj.flash]));
  return c;
}

// ────────────────────────────────────────────────────────────────────
// "Terminator"
// ────────────────────────────────────────────────────────────────────
#define RDTE_BX        6        // x izquierda del sprite (20 de ancho)
#define RDTE_BY        6        // fila superior del sprite (22 de alto: pies en y=27)
#define RDTE_DIG_Y     8        // dígitos grandes: filas 8..21
#define RDTE_LASER_Y   15
static const int16_t RDTE_DIG_X[4] = { 35, 48, 69, 82 };
static const uint8_t RDTE_EDIF[32] PROGMEM = { 9,13,7,16,11,6,14,18,10,8,15,12,7,17,13,9, 11,16,8,6,14,19,12,9,15,7,13,17,10,8,14,11 };
static const int16_t RDTE_FUEGO_X[5] = { 13, 41, 69, 97, 121 };
static const int8_t  RDTE_BARRIDO[16] PROGMEM = { 0,3,6,9,10,9,6,3,0,-3,-6,-9,-10,-9,-6,-3 };

// Colores: 1 chaqueta, 2 chaqueta clara, 3 piel, 4 piel sombra, 5 gafas, 6 ojo, 7 pelo, 8 pantalón, 9 botas, 10 arma, 11 culata, 12 mira
static const RDRect RDTE_REPOSO[] PROGMEM = {
  {5,0,6,2,7},{5,2,6,5,3},{5,2,1,5,4},{6,3,5,1,5},{10,3,1,1,6},{8,6,2,1,4},{7,7,3,1,4},
  {3,8,10,8,1},{4,8,2,3,2},{8,8,3,1,2},
  {2,9,2,5,1},
  {10,10,4,2,1},{13,10,2,2,3},{11,12,3,3,11},{14,12,6,2,10},{15,11,2,1,12},
  {4,16,4,4,8},{9,16,4,4,8},{3,20,5,2,9},{9,20,6,2,9}
};
static const RDRect RDTE_APUNTA[] PROGMEM = {
  {5,0,6,2,7},{5,2,6,5,3},{5,2,1,5,4},{6,3,5,1,5},{10,3,1,1,6},{8,6,2,1,4},{7,7,3,1,4},
  {3,8,10,8,1},{4,8,2,3,2},{8,8,3,1,2},
  {2,9,2,5,1},
  {9,9,2,3,11},{10,9,4,2,1},{13,9,2,2,3},{14,9,6,2,10},{15,8,2,1,12},
  {4,16,4,4,8},{9,16,4,4,8},{3,20,5,2,9},{9,20,6,2,9}
};
#define RDTE_N(a) ((int)(sizeof(a) / sizeof(a[0])))
static const char RDTE_LLAMA[3][4][4] PROGMEM = {
  { ".X.", "XXX", "XXX", ".X." }, { "..X", ".XX", "XXX", "XXX" }, { "X..", "XX.", "XXX", "XXX" }
};
static const char RDTE_CALAVERA[4][6] PROGMEM = { ".XXX.", "X.X.X", "XXXXX", ".X.X." };

struct RDTEEstado {
  int8_t   d[4], obj[4];
  uint8_t  pend;
  uint8_t  modo;        // 0 reposo 1 apunta 2 dispara
  int8_t   objetivo;
  uint8_t  t;
  uint8_t  pose;        // 0 reposo 1 apuntando
  uint8_t  muzzle, disparo;
  uint8_t  exp[4];
  int16_t  hk;          // x del dron cazador (-99 = no hay)
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDTEEstado rdte;

static void rdte_marcarSprite() { rdMarcar(RDTE_BX - 3, RDTE_BY - 1, 30, 25); }
static void rdte_marcarLaser()  { rdMarcar(RDTE_BX + 19, RDTE_LASER_Y - 2, 100, 5); }
static void rdte_marcarDigito(int i) { rdMarcar(RDTE_DIG_X[i] - 4, 5, 19, 20); }
static void rdte_marcarOjo()    { rdMarcar(RDTE_BX + 4, RDTE_BY + 2, 8, 3); }
static void rdte_marcarHK()     { rdMarcar(rdte.hk - 14, 0, 42, 29); }
static int  rdte_cima(int x) { return 27 - (int)pgm_read_byte(&RDTE_EDIF[(x >> 2) & 31]); }

static uint16_t rdte_hash(int x, int y, int t) {
  unsigned v = (unsigned)x * 7919u + (unsigned)y * 104729u + (unsigned)t * 1299709u;
  v ^= v >> 13; v *= 0x5bd1e995u; v ^= v >> 15;
  return (uint16_t)(v & 0xFFFF);
}

static bool rdte_difiere() {
  for (int i = 0; i < 4; i++) if (rdte.d[i] != rdte.obj[i]) return true;
  return false;
}

static void rdte_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdte.d);
  for (int i = 0; i < 4; i++) { rdte.obj[i] = rdte.d[i]; rdte.exp[i] = 0; }
  rdte.pend = 0; rdte.modo = 0; rdte.objetivo = -1; rdte.t = 0; rdte.pose = 0;
  rdte.muzzle = rdte.disparo = 0; rdte.hk = -99;
  rdte.dp = (s % 2 == 0); rdte.n = 0; rdte.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdte_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  for (int i = 0; i < 4; i++) rdte.obj[i] = nd[i];
}

static void rdte_segundo(int s) { rdte.dp = (s % 2 == 0); rdMarcar(60, 10, 6, 10); }

static int rdte_siguiente() {
  for (int i = 0; i < 4; i++) if (rdte.pend & (1 << i)) return i;
  return -1;
}

static void rdte_tick() {
  rdte.n++;

  if ((rdte.n % 8) == 0) rdte_marcarOjo();                              // el ojo rojo late
  if ((rdte.n % 3) == 0) for (int k = 0; k < 5; k++) rdMarcar(RDTE_FUEGO_X[k] - 1, rdte_cima(RDTE_FUEGO_X[k]) - 5, 5, 6);   // incendios

  if (rdte.hk > -40) {                                                 // dron cazador con su foco
    rdte_marcarHK();
    if ((rdte.n & 1) == 0) rdte.hk--;
    if (rdte.hk < -40) rdte.hk = -99; else rdte_marcarHK();
  } else if ((rdte.n % 650) == 300) { rdte.hk = 140; rdte_marcarHK(); }

  for (int i = 0; i < 4; i++) if (rdte.exp[i]) {
    rdte.exp[i]--;
    if (!rdte.exp[i]) rdte.d[i] = rdte.obj[i];
    rdte_marcarDigito(i);
  }

  switch (rdte.modo) {
    case 0:
      if (rdte_difiere()) {
        rdte.pend = 0;
        for (int i = 0; i < 4; i++) if (rdte.d[i] != rdte.obj[i]) rdte.pend |= (1 << i);
        rdte.objetivo = rdte_siguiente();
        rdte.modo = 1; rdte.t = 0; rdte.pose = 1;
        rdte_marcarSprite(); rdte_marcarLaser();
      }
      break;
    case 1:                                                           // apunta: láser fino y retículo parpadeando
      rdte.t++;
      rdte_marcarDigito(rdte.objetivo);
      if (rdte.t >= 8) {
        rdte.modo = 2; rdte.t = 0; rdte.muzzle = 2; rdte.disparo = 2;
        rdte.exp[rdte.objetivo] = 5; rdte.pend &= ~(1 << rdte.objetivo);
        rdte_marcarSprite(); rdte_marcarLaser(); rdte_marcarDigito(rdte.objetivo);
      }
      break;
    case 2:                                                           // dispara
      rdte.t++;
      rdte_marcarDigito(rdte.objetivo);
      if (rdte.muzzle) { rdte.muzzle--; rdte_marcarSprite(); }
      if (rdte.disparo) { rdte.disparo--; rdte_marcarLaser(); }
      if (rdte.t >= 5) {
        rdte_marcarLaser();
        int nxt = rdte_siguiente();
        if (nxt >= 0) { rdte.objetivo = nxt; rdte.modo = 1; rdte.t = 0; rdte_marcarDigito(nxt); }
        else { rdte.modo = 0; rdte.pose = 0; rdte.objetivo = -1; rdte_marcarSprite(); }
      }
      break;
  }
}

static void rdte_paso(uint32_t ms) {
  if (!rdte.ultOk) { rdte.ult = ms; rdte.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdte.ult) >= RDM_TICK_MS && n < 3) { rdte.ult += RDM_TICK_MS; rdte_tick(); n++; }
  if (n == 3) rdte.ult = ms;
}

static uint16_t rdte_color(uint8_t c) {
  switch (c) {
    case 1:  return RD_C(40, 44, 62);
    case 2:  return RD_C(104, 112, 138);
    case 3:  return RD_C(225, 170, 130);
    case 4:  return RD_C(175, 120, 90);
    case 5:  return RD_C(6, 6, 8);
    case 6:  return ((rdte.n >> 3) & 1) ? RD_C(255, 70, 50) : RD_C(150, 10, 10);
    case 7:  return RD_C(44, 30, 26);
    case 8:  return RD_C(64, 72, 96);
    case 9:  return RD_C(18, 16, 20);
    case 10: return RD_C(120, 126, 142);
    case 11: return RD_C(110, 70, 40);
    case 12: return RD_C(60, 64, 76);
  }
  return 0;
}

static uint16_t rdte_fondo(int x, int y) {
  uint16_t c;
  if (y >= 28) {                                                      // escombros
    if (y == 28) c = RD_C(70, 44, 40);
    else c = (rdte_hash(x, y, 3) % 7 == 0) ? RD_C(70, 56, 60) : RD_C(34, 26, 30);
  } else {
    int cima = rdte_cima(x);
    c = 0;
    // calaveras en el suelo
    { int lx = x - 28, ly = y - 24;
      if ((unsigned)lx < 5 && (unsigned)ly < 4 && pgm_read_byte(&RDTE_CALAVERA[ly][lx]) == 'X') return RD_C(205, 195, 175); }
    { int lx = x - 104, ly = y - 24;
      if ((unsigned)lx < 5 && (unsigned)ly < 4 && pgm_read_byte(&RDTE_CALAVERA[ly][lx]) == 'X') return RD_C(205, 195, 175); }
    // incendios sobre los tejados
    for (int k = 0; k < 5; k++) {
      int lx = x - RDTE_FUEGO_X[k], ly = y - (rdte_cima(RDTE_FUEGO_X[k]) - 4);
      if ((unsigned)lx < 3 && (unsigned)ly < 4 &&
          pgm_read_byte(&RDTE_LLAMA[(rdte.n / 3 + k) % 3][ly][lx]) == 'X') return ly < 2 ? RD_C(255, 220, 90) : RD_C(255, 120, 20);
    }
    if (y >= cima) {                                                  // edificios en ruinas
      if (y > cima + 2 && (rdte_hash(x, y, 9) % 47) == 0) c = RD_C(255, 150, 40);
      else c = RD_C(8, 6, 12);
    } else {
      c = RD_C(14 + y * 5, 4 + y, 14 - (y >> 2));                     // cielo con resplandor rojo
    }
  }
  // foco del dron
  if (rdte.hk > -40 && y > 6 && y < 28) {
    int t = y - 6;
    int cx = rdte.hk + 6 + ((int)(int8_t)pgm_read_byte(&RDTE_BARRIDO[(rdte.n >> 2) & 15]) * t) / 21;
    int dx = x - cx; if (dx < 0) dx = -dx;
    if (dx <= 1 + t / 5) c = rdBlanco(c, 3);
  }
  return c;
}

static uint16_t rdte_capas(int x, int y) {
  // 1) fogonazo del arma
  if (rdte.muzzle) {
    int dx = x - (RDTE_BX + 22), dy = y - RDTE_LASER_Y;
    int a = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
    if (a <= 3) return a == 3 ? RD_C(255, 170, 40) : RD_C(255, 250, 200);
  }
  // 2) cyborg
  {
    int lx = x - (RDTE_BX - (rdte.muzzle ? 1 : 0)), ly = y - RDTE_BY;
    if ((unsigned)lx < 20 && (unsigned)ly < 22) {
      uint8_t c = (rdte.pose == 1) ? rdRectColor(RDTE_APUNTA, RDTE_N(RDTE_APUNTA), lx, ly)
                                  : rdRectColor(RDTE_REPOSO, RDTE_N(RDTE_REPOSO), lx, ly);
      if (c) return rdte_color(c);
    }
  }
  // 3) retículo sobre el dígito objetivo
  if (rdte.modo != 0 && rdte.objetivo >= 0 && (rdte.modo == 2 || (rdte.n & 2) == 0)) {
    int x0 = RDTE_DIG_X[rdte.objetivo] - 3, x1 = RDTE_DIG_X[rdte.objetivo] + 12, y0 = 5, y1 = 24;
    if (x >= x0 && x <= x1 && y >= y0 && y <= y1) {
      bool horiz = (y == y0 || y == y1) && (x - x0 < 3 || x1 - x < 3);
      bool vert  = (x == x0 || x == x1) && (y - y0 < 3 || y1 - y < 3);
      if (horiz || vert) return RD_C(255, 60, 40);
    }
  }
  // 4) hora en pantalla de visor rojo, con líneas de barrido
  if (y >= RDTE_DIG_Y && y <= RDTE_DIG_Y + 14 && x >= 35 && x <= 93) {
    uint16_t rojo = (y & 1) ? RD_C(185, 28, 22) : RD_C(255, 52, 38);
    for (int i = 0; i < 4; i++) {
      int lx = x - RDTE_DIG_X[i], ly = y - RDTE_DIG_Y;
      if ((unsigned)lx >= 10) continue;
      if (rdte.exp[i]) {
        static const uint8_t DENS[6] = { 0, 60, 110, 160, 210, 240 };
        uint16_t hh = rdte_hash(x, y, rdte.exp[i]);
        if ((hh & 0xFF) < DENS[rdte.exp[i]]) {
          uint8_t cc = (hh >> 8) & 3;
          return cc == 0 ? RD_C(255, 255, 255) : cc == 1 ? RD_C(255, 90, 60) : cc == 2 ? RD_C(255, 170, 60) : RD_C(160, 20, 20);
        }
        break;
      }
      if (rdDigito2(rdte.d[i], lx, ly)) return rojo;
    }
    if (rdte.dp) {
      int lx = x - 62;
      if ((unsigned)lx < 2 && ((unsigned)(y - 11) < 2 || (unsigned)(y - 17) < 2)) return rojo;
    }
  }
  // 5) láser: punteado al apuntar, brillante al disparar (pasa por detrás de los dígitos)
  if ((rdte.modo == 1 || rdte.modo == 2) && rdte.objetivo >= 0 && x >= RDTE_BX + 20 && x < RDTE_DIG_X[rdte.objetivo]) {
    if (rdte.disparo) {
      if (y == RDTE_LASER_Y) return RD_C(255, 255, 255);
      if (y == RDTE_LASER_Y - 1 || y == RDTE_LASER_Y + 1) return RD_C(120, 170, 255);
    } else if (y == RDTE_LASER_Y && (x & 1) == 0) return RD_C(210, 24, 24);
  }
  // 6) dron cazador
  if (rdte.hk > -40) {
    int lx = x - rdte.hk, ly = y - 2;
    bool cuerpo = (ly == 0 && lx >= 3 && lx <= 9) || (ly == 1 && lx >= 1 && lx <= 11) ||
                  (ly == 2 && lx >= 0 && lx <= 12) || (ly == 3 && lx >= 1 && lx <= 11);
    if (cuerpo) return (ly == 0) ? RD_C(120, 128, 150) : RD_C(78, 84, 104);
    if (ly == 4 && lx >= 5 && lx <= 7) return RD_C(210, 235, 255);
  }
  return rdte_fondo(x, y);
}

static uint16_t rdte_pixel(int x, int y) { return rdte_capas(x, y); }

// ────────────────────────────────────────────────────────────────────
// "Regreso al Futuro"
// ────────────────────────────────────────────────────────────────────
#define RDF_CAR_Y     18       // fila superior del coche (28x10: ruedas en y=27)
#define RDF_CAR_X0    10       // dónde aparca
#define RDF_DIG_Y     3        // dígitos grandes: filas 3..16
static const int16_t RDF_DIG_X[4] = { 35, 48, 69, 82 };
static const uint8_t RDF_FLASH_K[7] PROGMEM = { 0, 2, 4, 6, 8, 8, 8 };
static const uint8_t RDF_CASAS[16] PROGMEM = { 8,11,6,9,12,7,10,6,9,12,7,8,11,6,10,8 };
static const int8_t  RDF_NUBE_Y[2] = { 0, 3 };

// Colores: 1 acero, 2 acero claro, 3 cristal, 4 gris oscuro, 5 piloto trasero, 6 faro, 7 rueda, 8 llanta
static const RDRect RDF_COCHE[] PROGMEM = {
  {0,5,28,2,1},{1,3,22,2,1},{22,4,6,1,1},{6,1,13,2,2},{8,2,8,2,3},{16,2,3,2,3},{18,3,3,1,3},
  {1,3,5,2,4},{0,4,2,2,5},{26,4,2,2,6},{0,7,28,1,4},
  {4,7,5,3,7},{19,7,5,3,7},{5,8,3,1,8},{20,8,3,1,8}
};
#define RDF_N(a) ((int)(sizeof(a) / sizeof(a[0])))
// Torre del reloj (24x21, origen en x=102, y=7). Colores: 1 piedra, 2 tejado, 3 oscuro
static const RDRect RDF_TORRE[] PROGMEM = {
  {6,5,12,16,1},{6,3,12,1,2},{7,2,10,1,2},{8,1,8,1,2},{10,0,4,1,2},{5,4,14,1,2},
  {10,15,1,2,3},{14,15,1,2,3},{11,18,3,3,3}
};

struct RDFEstado {
  int8_t   d[4], obj[4];
  uint8_t  modo;        // 0 aparcado 1 arranca 2 acelera 3 estelas ardiendo 4 llega
  uint8_t  t;
  bool     coche;
  int16_t  x;           // x izquierda del coche
  uint8_t  vel;         // velocímetro 0..88
  uint8_t  trail;       // intensidad de las estelas (0 = apagadas)
  int16_t  tx0, tx1;
  uint8_t  frost;
  uint8_t  flash, rayo;
  int16_t  nube[2];
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDFEstado rdf;

static uint16_t rdf_hash(int x, int y, int t) {
  unsigned v = (unsigned)x * 7919u + (unsigned)y * 104729u + (unsigned)t * 1299709u;
  v ^= v >> 13; v *= 0x5bd1e995u; v ^= v >> 15;
  return (uint16_t)(v & 0xFFFF);
}

static void rdf_marcarCoche()   { rdMarcar(rdf.x - 9, RDF_CAR_Y - 3, 46, 14); }
static void rdf_marcarTrail()   { rdMarcar(rdf.tx0 - 1, 19, rdf.tx1 - rdf.tx0 + 3, 9); }
static void rdf_marcarVelocim() { rdMarcar(2, 2, 13, 9); }
static void rdf_marcarNube(int i) { rdMarcar(rdf.nube[i], RDF_NUBE_Y[i], 14, 5); }

static bool rdf_difiere() {
  for (int i = 0; i < 4; i++) if (rdf.d[i] != rdf.obj[i]) return true;
  return false;
}

static void rdf_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdf.d);
  for (int i = 0; i < 4; i++) rdf.obj[i] = rdf.d[i];
  rdf.modo = 0; rdf.t = 0; rdf.coche = true; rdf.x = RDF_CAR_X0; rdf.vel = 0;
  rdf.trail = 0; rdf.tx0 = rdf.tx1 = 0; rdf.frost = 0; rdf.flash = 0; rdf.rayo = 0;
  rdf.nube[0] = 20; rdf.nube[1] = 84;
  rdf.dp = (s % 2 == 0); rdf.n = 0; rdf.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdf_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  for (int i = 0; i < 4; i++) rdf.obj[i] = nd[i];
}

static void rdf_segundo(int s) { rdf.dp = (s % 2 == 0); rdMarcar(60, 5, 6, 10); }

static void rdf_tick() {
  rdf.n++;

  if ((rdf.n % 14) == 0) { rdf_marcarNube(0); if (--rdf.nube[0] < -14) rdf.nube[0] = RD_W; rdf_marcarNube(0); }
  if ((rdf.n % 22) == 0) { rdf_marcarNube(1); if (--rdf.nube[1] < -14) rdf.nube[1] = RD_W; rdf_marcarNube(1); }
  if ((rdf.n % 8) == 0) {
    rdMarcar(31, 3, 3, 13);                                           // luces del panel
    if (rdf.coche) rdMarcar(rdf.x, RDF_CAR_Y + 3, 4, 4);              // piloto trasero
  }
  if (rdf.flash) { rdf.flash--; rdMarcar(0, 0, RD_W, RD_H); }
  if (rdf.rayo) { rdf.rayo--; rdMarcar(0, 0, RD_W, RD_H); }
  else if ((rdf.n % 800) == 400) { rdf.rayo = 3; rdMarcar(0, 0, RD_W, RD_H); }
  if (rdf.trail) { rdf_marcarTrail(); rdf.trail--; rdf_marcarTrail(); }

  switch (rdf.modo) {
    case 0:
      if (rdf_difiere()) { rdf.modo = 1; rdf.t = 0; rdf.vel = 0; rdf_marcarCoche(); rdf_marcarVelocim(); }
      break;
    case 1:                                                           // arranca entre humo
      rdf.t++;
      rdf_marcarCoche();
      if (rdf.t >= 8) { rdf.modo = 2; rdf.t = 0; }
      break;
    case 2: {                                                         // acelera hasta los 88
      rdf_marcarCoche();
      rdf.t++;
      int v = 1 + rdf.t / 2; if (v > 7) v = 7;
      rdf.x += v;
      int vel = rdf.t * 6; if (vel > 88) vel = 88;
      rdf.vel = (uint8_t)vel;
      rdf_marcarVelocim();
      if (vel >= 88) {                                                // ¡desaparece!
        rdf.coche = false; rdf.vel = 0;
        rdf.flash = 4;
        for (int i = 0; i < 4; i++) rdf.d[i] = rdf.obj[i];
        rdf.trail = 38; rdf.tx0 = RDF_CAR_X0 + 4; rdf.tx1 = rdf.x + 24;
        if (rdf.tx1 > 126) rdf.tx1 = 126;
        rdf.modo = 3; rdf.t = 0;
        rdMarcar(0, 0, RD_W, RD_H);
      } else rdf_marcarCoche();
    } break;
    case 3:                                                           // las estelas arden y el coche vuelve
      rdf.t++;
      if (rdf.trail <= 14) {
        rdf.coche = true; rdf.x = RDF_CAR_X0; rdf.frost = 10; rdf.flash = 2;
        rdf.modo = 4; rdf.t = 0;
        rdMarcar(0, 0, RD_W, RD_H);
      }
      break;
    case 4:                                                           // llega envuelto en vapor helado
      rdf_marcarCoche();
      if (rdf.frost) rdf.frost--;
      if (!rdf.frost) rdf.modo = 0;
      break;
  }
}

static void rdf_paso(uint32_t ms) {
  if (!rdf.ultOk) { rdf.ult = ms; rdf.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdf.ult) >= RDM_TICK_MS && n < 3) { rdf.ult += RDM_TICK_MS; rdf_tick(); n++; }
  if (n == 3) rdf.ult = ms;
}

static uint16_t rdf_colorCoche(uint8_t c) {
  switch (c) {
    case 1: return RD_C(190, 196, 210);
    case 2: return RD_C(228, 232, 242);
    case 3: return RD_C(30, 44, 80);
    case 4: return RD_C(60, 64, 76);
    case 5: return ((rdf.n >> 3) & 1) ? RD_C(255, 40, 40) : RD_C(170, 20, 20);
    case 6: return RD_C(255, 250, 190);
    case 7: return RD_C(14, 14, 18);
    case 8: return RD_C(180, 185, 195);
  }
  return 0;
}

static uint16_t rdf_fondo(int x, int y) {
  uint16_t c;
  if (y >= 26) {                                                      // calle
    if (y == 26) c = RD_C(120, 120, 134);
    else if (y == 29 && ((x >> 2) & 1)) c = RD_C(220, 190, 40);
    else c = RD_C(48, 50, 62);
  } else {
    int cima = 25 - (int)pgm_read_byte(&RDF_CASAS[(x >> 3) & 15]);
    if (y >= cima) c = (y > cima + 1 && (rdf_hash(x, y, 5) % 41) == 0) ? RD_C(255, 200, 80) : RD_C(14, 12, 32);
    else {
      c = RD_C(8 + y, 10 + y, 36 + y * 2);                            // cielo de tormenta
      for (int i = 0; i < 2; i++) {                                   // nubarrones
        int lx = x - rdf.nube[i], ly = y - RDF_NUBE_Y[i];
        if ((unsigned)lx < 14 && (unsigned)ly < 5) {
          char ch = (char)pgm_read_byte(&RDM_NUBE[ly][lx]);
          if (ch == 'W') c = RD_C(34, 34, 70);
          else if (ch == 'G') c = RD_C(24, 24, 54);
        }
      }
    }
  }
  if (rdf.rayo) c = rdBlanco(c, 3);
  return c;
}

static uint16_t rdf_capas(int x, int y) {
  // 1) velocímetro mientras acelera
  if ((rdf.modo == 1 || rdf.modo == 2) && y >= 3 && y <= 9 && x >= 3 && x <= 13) {
    int d = (x < 8) ? (rdf.vel / 10) : (rdf.vel % 10);
    int lx = (x < 8) ? x - 3 : x - 9;
    if ((unsigned)lx < 5 && (pgm_read_byte(&RDM_FUENTE[d][y - 3]) & (0x10 >> lx))) return RD_C(60, 255, 90);
  }
  // 2) coche
  if (rdf.coche) {
    int lx = x - rdf.x, ly = y - (RDF_CAR_Y + ((rdf.modo == 1 && (rdf.n & 1)) ? 1 : 0));   // tiembla al acelerar en el sitio
    if ((unsigned)lx < 28 && (unsigned)ly < 10) {
      uint8_t c = rdRectColor(RDF_COCHE, RDF_N(RDF_COCHE), lx, ly);
      if (c) return rdf_colorCoche(c);
    }
    // humo del escape al arrancar
    if (rdf.modo == 1 && y >= 21 && y <= 27 && x >= rdf.x - 9 && x < rdf.x && (rdf_hash(x, y, (int)rdf.n) % 100) < 30)
      return (rdf_hash(y, x, (int)rdf.n) & 1) ? RD_C(160, 160, 172) : RD_C(110, 110, 126);
    // arcos eléctricos a gran velocidad
    if (rdf.modo == 2 && rdf.t >= 8 && x >= rdf.x - 3 && x < rdf.x + 31 && y >= RDF_CAR_Y - 3 && y < RDF_CAR_Y + 10 &&
        (rdf_hash(x, y, (int)rdf.n) % 100) < 22)
      return (rdf_hash(y, x, (int)rdf.n) & 1) ? RD_C(220, 240, 255) : RD_C(110, 160, 255);
    // faros: haz de luz al arrancar
    if (rdf.modo == 1 && y >= 21 && y <= 25 && x >= rdf.x + 28 && x < rdf.x + 36 && ((x + y) & 1) == 0) return RD_C(255, 240, 150);
  }
  // 3) vapor helado en la llegada
  if (rdf.frost && x >= RDF_CAR_X0 - 4 && x < RDF_CAR_X0 + 34 && y >= 15 && y < 28 &&
      (rdf_hash(x, y + rdf.frost * 2, (int)rdf.n) % 100) < 10 + rdf.frost * 4)
    return (rdf_hash(y, x, (int)rdf.n) & 1) ? RD_C(235, 247, 255) : RD_C(170, 210, 240);
  // 4) estelas de fuego
  if (rdf.trail && x >= rdf.tx0 && x <= rdf.tx1 && y >= 20 && y <= 26) {
    int alto = (rdf.trail * 5 + 35) / 36 + (int)(rdf_hash(x, 0, (int)(rdf.n >> 1)) % 2);
    int dist = 26 - y;                                                // 0 = a ras de suelo
    if (dist < alto) return dist <= 1 ? RD_C(255, 235, 110) : dist <= 3 ? RD_C(255, 140, 30) : RD_C(220, 50, 10);
  }
  // 5) panel de los circuitos del tiempo
  if (x >= 30 && x <= 96 && y >= 2 && y <= 17) {
    if (x == 30 || x == 96 || y == 2 || y == 17) return RD_C(150, 100, 40);
    if (x >= 32 && x <= 33 && ((y >= 4 && y <= 5) || (y >= 8 && y <= 9) || (y >= 12 && y <= 13))) {
      int k = (y - 4) / 4;
      bool on = (((rdf.n >> 3) + k) & 1) != 0;
      return k == 0 ? (on ? RD_C(255, 40, 40) : RD_C(90, 12, 12)) : k == 1 ? (on ? RD_C(60, 255, 90) : RD_C(14, 80, 28)) : (on ? RD_C(255, 170, 0) : RD_C(90, 56, 0));
    }
    for (int i = 0; i < 4; i++) {
      int lx = x - RDF_DIG_X[i], ly = y - RDF_DIG_Y;
      if ((unsigned)lx >= 10 || (unsigned)ly >= 14) continue;
      if (rdDigito2(rdf.d[i], lx, ly)) return RD_C(60, 255, 90);
      if (rdDigito2(8, lx, ly)) return RD_C(6, 40, 14);               // segmentos apagados
    }
    if (x >= 62 && x <= 63 && ((y >= 6 && y <= 7) || (y >= 12 && y <= 13))) return rdf.dp ? RD_C(255, 170, 0) : RD_C(50, 30, 0);
    return RD_C(10, 10, 14);
  }
  // 6) torre del reloj
  {
    int lx = x - 102, ly = y - 7;
    if ((unsigned)lx < 24 && (unsigned)ly < 21) {
      int dx = x - 114, dy = y - 17, d2 = dx * dx + dy * dy;
      if (d2 <= 16) {                                                 // esfera parada en las 10:04
        if ((dx == -1 && dy == -1) || (dx == -2 && dy == -2) || (dx == 1 && dy == 0) || (dx == 2 && dy == 1) || (dx == 0 && dy == 0)) return RD_C(40, 30, 40);
        return RD_C(235, 235, 215);
      }
      if (d2 <= 26) return RD_C(60, 50, 60);
      uint8_t c = rdRectColor(RDF_TORRE, RDF_N(RDF_TORRE), lx, ly);
      if (c == 1) return RD_C(96, 92, 120);
      if (c == 2) return RD_C(120, 76, 70);
      if (c == 3) return RD_C(24, 20, 34);
    }
  }
  // 7) rayo sobre la torre
  if (rdf.rayo && y <= 6) {
    static const int8_t BX[7] = { 108, 109, 108, 110, 112, 111, 112 };
    if (x == BX[y] || x == BX[y] + 1) return RD_C(255, 255, 255);
  }
  return rdf_fondo(x, y);
}

static uint16_t rdf_pixel(int x, int y) {
  uint16_t c = rdf_capas(x, y);
  if (rdf.flash) c = rdBlanco(c, pgm_read_byte(&RDF_FLASH_K[rdf.flash > 6 ? 6 : rdf.flash]));
  return c;
}

// ────────────────────────────────────────────────────────────────────
// "Cazafantasmas"
// ────────────────────────────────────────────────────────────────────
#define RDG_DIG_Y     6        // fila superior de los dígitos (14 de alto -> hasta y=19)
#define RDG_HY        14       // fila superior de los cazafantasmas (14 de alto: pies en y=27)
#define RDG_HX_IZQ    11       // x izquierda del cazafantasmas izquierdo (9 de ancho)
#define RDG_HX_DER    108      // x izquierda del derecho (mira hacia la izquierda)
#define RDG_GUN_Y     22       // altura del cañón de protones
static const int16_t RDG_DIG_X[4] = { 35, 48, 69, 82 };     // dígitos de 10 de ancho
static const uint8_t RDG_EDIF[16] PROGMEM = { 11,15,9,17,12,8,16,13,10,18,12,9,15,11,17,10 };
static const int8_t  RDG_ONDA[8]  PROGMEM = { 0, 1, 2, 1, 0, -1, -2, -1 };
static const int8_t  RDG_ESPIRAL[10] PROGMEM = { 0, 3, 5, 3, 0, -3, -5, -3, -1, 0 };

// Cazafantasmas 9x14 (mirando a la derecha). H pelo, S piel, E ojo, K mono, W parche,
// P mochila de protones, D cinturón, B botas
static const char RDG_CAZADOR[14][10] PROGMEM = {
  "...HHHH..", "..HHHHHH.", "..HSSSSS.", "..HSESSS.", "..HSSSSS.", "...SSSS..", ".PPKKKKK.",
  "PPPKKWKKK", "PPPKKKKKK", ".PPKKKKK.", "..DDDDDD.", "..KK..KK.", "..KK..KK.", ".BBB..BBB"
};
// Fantasma 9x9 en dos fotogramas (cambia la falda). X cuerpo, K ojo, M boca
static const char RDG_FANT0[9][10] PROGMEM = {
  "..XXXXX..", ".XXXXXXX.", "XXXXXXXXX", "XXKKXKKXX", "XXKKXKKXX", "XXXXXXXXX", "XXXMMMXXX", "XXXXXXXXX", "X.XX.XX.X"
};
static const char RDG_FANT1[9][10] PROGMEM = {
  "..XXXXX..", ".XXXXXXX.", "XXXXXXXXX", "XXKKXKKXX", "XXKKXKKXX", "XXXXXXXXX", "XXXMMMXXX", "XXXXXXXXX", ".XX.X.XX."
};

struct RDGEstado {
  int8_t   d[4], obj[4];
  uint8_t  pend;            // dígitos que todavía hay que atrapar
  uint8_t  modo;            // 0 guardia 1 aparece 2 sube 3 rayos 4 arrastre 5 trampa
  int8_t   objetivo;
  uint8_t  t;
  uint8_t  ds[4], dt[4];    // estado de cada dígito: 0 normal 1 oculto 3 destellos (dt = ticks que quedan)
  uint8_t  bob;
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDGEstado rdg;

static void rdg_marcarCazadores() { rdMarcar(RDG_HX_IZQ - 1, RDG_HY - 1, 13, 16); rdMarcar(RDG_HX_DER - 1, RDG_HY - 1, 13, 16); }

static bool rdg_difiere() {
  for (int i = 0; i < 4; i++) if (rdg.d[i] != rdg.obj[i]) return true;
  return false;
}
static int rdg_siguiente() {
  for (int i = 0; i < 4; i++) if (rdg.pend & (1 << i)) return i;
  return -1;
}

static void rdg_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdg.d);
  for (int i = 0; i < 4; i++) { rdg.obj[i] = rdg.d[i]; rdg.ds[i] = 0; rdg.dt[i] = 0; }
  rdg.pend = 0; rdg.modo = 0; rdg.objetivo = -1; rdg.t = 0; rdg.bob = 0;
  rdg.dp = (s % 2 == 0); rdg.n = 0; rdg.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdg_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  for (int i = 0; i < 4; i++) rdg.obj[i] = nd[i];      // el tick arranca la caza cuando hay algo que cambiar
}

static void rdg_segundo(int s) { rdg.dp = (s % 2 == 0); rdMarcar(62, 8, 2, 10); }

static void rdg_tick() {
  rdg.n++;
  bool todo = false;

  for (int i = 0; i < 4; i++) if (rdg.ds[i] == 3) {            // destellos del dígito nuevo
    if (rdg.dt[i]) rdg.dt[i]--;
    if (!rdg.dt[i]) rdg.ds[i] = 0;
    todo = true;
  }

  switch (rdg.modo) {
    case 0:                                                    // guardia: respiran
      if ((rdg.n % 10) == 0) { rdg.bob ^= 1; rdg_marcarCazadores(); }
      if (rdg_difiere()) {
        rdg.pend = 0;
        for (int i = 0; i < 4; i++) if (rdg.d[i] != rdg.obj[i]) rdg.pend |= (1 << i);
        rdg.objetivo = rdg_siguiente();
        rdg.modo = 1; rdg.t = 0;
      }
      break;
    case 1:                                                    // el fantasma se materializa sobre el dígito
      rdg.t++; todo = true;
      if (rdg.t >= 6) { rdg.modo = 2; rdg.t = 0; rdg.ds[rdg.objetivo] = 1; }
      break;
    case 2:                                                    // se lleva el dígito y sube
      rdg.t++; todo = true;
      if (rdg.t >= 8) { rdg.modo = 3; rdg.t = 0; }
      break;
    case 3:                                                    // rayos de protones cruzados
      rdg.t++; todo = true;
      if (rdg.t >= 12) { rdg.modo = 4; rdg.t = 0; }
      break;
    case 4:                                                    // lo arrastran en espiral hasta la trampa
      rdg.t++; todo = true;
      if (rdg.t >= 10) { rdg.modo = 5; rdg.t = 0; }
      break;
    case 5: {                                                  // la trampa se abre y vuelve el dígito
      rdg.t++; todo = true;
      int o = rdg.objetivo;
      if (rdg.t == 2) { rdg.d[o] = rdg.obj[o]; rdg.ds[o] = 3; rdg.dt[o] = 6; rdg.pend &= ~(1 << o); }
      if (rdg.t >= 8) {
        int nxt = rdg_siguiente();
        if (nxt >= 0) { rdg.objetivo = nxt; rdg.modo = 1; rdg.t = 0; }
        else { rdg.modo = 0; rdg.objetivo = -1; }
      }
      break;
    }
  }
  if (todo) rdMarcar(0, 0, RD_W, RD_H);
}

static void rdg_paso(uint32_t ms) {
  if (!rdg.ultOk) { rdg.ult = ms; rdg.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdg.ult) >= RDM_TICK_MS && n < 3) { rdg.ult += RDM_TICK_MS; rdg_tick(); n++; }
  if (n == 3) rdg.ult = ms;
}

// Centro del fantasma según la fase; pequeño al final del arrastre; punteado al materializarse
static void rdg_posFantasma(int* gx, int* gy, bool* peq, bool* punteado) {
  int x0 = RDG_DIG_X[rdg.objetivo >= 0 ? rdg.objetivo : 0] + 5;
  int w = (int)(int8_t)pgm_read_byte(&RDG_ONDA[(rdg.n >> 1) & 7]);
  *gx = x0; *gy = 13; *peq = false; *punteado = false;
  switch (rdg.modo) {
    case 1: *punteado = (rdg.t < 3); *gx = x0 + w / 2; break;
    case 2: *gy = 13 - rdg.t; *gx = x0 + w; break;
    case 3: *gy = 6; *gx = x0 + ((rdg.n & 1) ? 1 : -1); break;           // forcejea
    case 4: *gy = 6 + rdg.t * 2; *gx = x0 + (int)(int8_t)pgm_read_byte(&RDG_ESPIRAL[rdg.t]); *peq = (rdg.t >= 6); break;
  }
}

// ¿Está (x,y) a distancia <= w del segmento (x0,y0)-(x1,y1)?
static bool rdg_enHaz(int x, int y, int x0, int y0, int x1, int y1, int w) {
  int dx = x1 - x0, dy = y1 - y0;
  int adx = dx < 0 ? -dx : dx, ady = dy < 0 ? -dy : dy;
  if (adx == 0 && ady == 0) return false;
  int d;
  if (adx >= ady) {
    if (x < (x0 < x1 ? x0 : x1) || x > (x0 < x1 ? x1 : x0)) return false;
    d = y - (y0 + (dy * (x - x0)) / dx);
  } else {
    if (y < (y0 < y1 ? y0 : y1) || y > (y0 < y1 ? y1 : y0)) return false;
    d = x - (x0 + (dx * (y - y0)) / dy);
  }
  return (d < 0 ? -d : d) <= w;
}

static uint16_t rdg_colorCazador(char ch) {
  switch (ch) {
    case 'H': return RD_C(60, 40, 30);
    case 'S': return RD_C(245, 190, 140);
    case 'E': return RD_C(20, 16, 24);
    case 'K': return RD_C(170, 150, 90);
    case 'W': return RD_C(240, 240, 240);
    case 'P': return RD_C(110, 115, 130);
    case 'D': return RD_C(50, 40, 30);
    case 'B': return RD_C(40, 30, 25);
  }
  return 0;
}

static uint16_t rdg_fondo(int x, int y) {
  // calle: bordillo, asfalto y línea discontinua
  if (y >= 28) {
    if (y == 28) return RD_C(130, 130, 145);
    if (y == 29) return RD_C(60, 60, 72);
    if (y == 30) return ((x & 7) < 4) ? RD_C(200, 190, 60) : RD_C(34, 34, 44);
    return RD_C(34, 34, 44);
  }
  // cartel de "no fantasmas" arriba a la izquierda
  { int dx = x - 9, dy = y - 7, d2 = dx * dx + dy * dy;
    if (d2 <= 25) {
      if (d2 >= 16 || dx == dy) return RD_C(230, 40, 40);
      if (dx >= -1 && dx <= 1 && dy >= -2 && dy <= 1) return RD_C(250, 250, 250);
      return RD_C(10, 10, 18);
    } }
  // luna
  { int dx = x - 116, dy = y - 6;
    if (dx * dx + dy * dy <= 12) return RD_C(235, 235, 210); }
  // edificios con ventanas encendidas
  int cima = 27 - (int)pgm_read_byte(&RDG_EDIF[(x >> 3) & 15]);
  if (y >= cima) {
    if ((x & 3) == 1 && (y & 3) == 1) {
      uint8_t w = rdi_hash(x >> 2, y >> 2, 7);
      if (w < 70) return RD_C(255, 205, 90);
      if (w < 100) return RD_C(120, 220, 255);
    }
    return ((x & 7) == 0) ? RD_C(8, 6, 20) : RD_C(16, 14, 34);
  }
  return RD_C(10 + (y >> 1), 6 + (y >> 1), 40 + y * 2);                  // cielo nocturno
}

static uint16_t rdg_pixel(int x, int y) {
  int gx = 0, gy = 0; bool peq = false, punt = false;
  int x0 = RDG_DIG_X[rdg.objetivo >= 0 ? rdg.objetivo : 0] + 5;

  // 1) columna de luz de la trampa al abrirse
  if (rdg.modo == 5 && rdg.t >= 1 && rdg.t <= 4 && y >= 6 && y <= 25) {
    int dx = x - x0; if (dx < 0) dx = -dx;
    if (dx == 0) return RD_C(255, 255, 255);
    if (dx <= 2 && (((y + rdg.t) & 1) == 0)) return RD_C(255, 240, 150);
  }
  // 2) fantasma
  if (rdg.modo >= 1 && rdg.modo <= 4) {
    rdg_posFantasma(&gx, &gy, &peq, &punt);
    if (peq) {
      int dx = x - gx, dy = y - gy;
      if (dx >= -1 && dx <= 1 && dy >= -1 && dy <= 1) return RD_C(150, 255, 120);
    } else {
      int lx = x - (gx - 4), ly = y - (gy - 4);
      if ((unsigned)lx < 9 && (unsigned)ly < 9 && !(punt && ((x + y) & 1))) {
        char ch = (char)pgm_read_byte(((rdg.n >> 2) & 1) ? &RDG_FANT1[ly][lx] : &RDG_FANT0[ly][lx]);
        if (ch == 'X') return RD_C(150, 255, 120);
        if (ch == 'K') return RD_C(10, 40, 10);
        if (ch == 'M') return RD_C(20, 70, 20);
      }
    }
  }
  // 3) rayos de protones cruzados (izquierdo naranja, derecho rojo)
  if (rdg.modo == 3 || rdg.modo == 4) {
    if (rdg.modo == 3) rdg_posFantasma(&gx, &gy, &peq, &punt);
    int ox0 = RDG_HX_IZQ + 11, ox1 = RDG_HX_DER - 3;
    bool l0 = rdg_enHaz(x, y, ox0, RDG_GUN_Y, gx, gy, 0), r0 = rdg_enHaz(x, y, ox1, RDG_GUN_Y, gx, gy, 0);
    if (l0 && r0) return RD_C(255, 255, 255);
    if (l0) return RD_C(255, 240, 170);
    if (r0) return RD_C(255, 210, 210);
    bool l1 = rdg_enHaz(x, y, ox0, RDG_GUN_Y, gx, gy, 1), r1 = rdg_enHaz(x, y, ox1, RDG_GUN_Y, gx, gy, 1);
    if (l1 || r1) {
      if (rdi_hash(x, y, (int)rdg.n) < 50) return RD_C(255, 255, 230);
      bool a = (((x + (int)rdg.n) & 2) != 0);
      if (l1) return a ? RD_C(255, 150, 20) : RD_C(255, 200, 60);
      return a ? RD_C(240, 40, 40) : RD_C(255, 90, 90);
    }
  }
  // 4) cazafantasmas y sus cañones
  for (int p = 0; p < 2; p++) {
    int hx = p ? RDG_HX_DER : RDG_HX_IZQ;
    int lx = x - hx, ly = y - (RDG_HY + ((rdg.modo == 0) ? rdg.bob : 0));
    if ((unsigned)lx < 9 && (unsigned)ly < 14) {
      char ch = (char)pgm_read_byte(&RDG_CAZADOR[ly][p ? 8 - lx : lx]);
      if (ch != '.') return rdg_colorCazador(ch);
    }
    if (y == RDG_GUN_Y && (p ? (x == hx - 1 || x == hx - 2) : (x == hx + 9 || x == hx + 10))) return RD_C(150, 150, 165);
  }
  // 5) trampa en el suelo (se abre con luz al final)
  if (rdg.objetivo >= 0 && ((rdg.modo == 3 && rdg.t >= 4) || rdg.modo == 4 || rdg.modo == 5) && y >= 26 && y <= 27) {
    int dx = x - x0;
    if (dx >= -4 && dx <= 4) {
      if (y == 26) {
        if (rdg.modo == 5 && rdg.t <= 4) return RD_C(255, 240, 120);
        return (dx == 0) ? (((rdg.n >> 2) & 1) ? RD_C(255, 60, 60) : RD_C(90, 20, 20)) : RD_C(90, 90, 104);
      }
      return (x & 1) ? RD_C(240, 200, 20) : RD_C(24, 24, 28);
    }
  }
  // 6) panel con la hora (dígitos verdes, con destellos al reaparecer)
  if (x >= 32 && x <= 94 && y >= 4 && y <= 21) {
    if (x == 32 || x == 94 || y == 4 || y == 21) return RD_C(30, 150, 60);
    for (int i = 0; i < 4; i++) {
      int lx = x - RDG_DIG_X[i];
      if ((unsigned)(lx + 2) >= 14) continue;
      int ly = y - RDG_DIG_Y;
      uint8_t ds = rdg.ds[i];
      if (ds == 1) break;                                                    // oculto: se lo llevó el fantasma
      if (ds == 3 && rdi_hash(x, y, (int)rdg.dt[i]) < rdg.dt[i] * 16) return RD_C(200, 255, 150);
      if (rdDigito2(rdg.d[i], lx, ly)) {
        if (ds == 3 && rdg.dt[i] >= 3) return RD_C(235, 255, 215);
        return (ly < 5) ? RD_C(190, 255, 90) : (ly < 10) ? RD_C(110, 240, 50) : RD_C(60, 200, 40);
      }
      if (rdDigito2(rdg.d[i], lx - 1, ly - 1)) return RD_C(8, 50, 12);
      break;
    }
    if (rdg.dp && (unsigned)(x - 62) < 2 && ((unsigned)(y - 9) < 2 || (unsigned)(y - 15) < 2)) return RD_C(120, 255, 80);
    return RD_C(4, 10, 14);
  }
  return rdg_fondo(x, y);
}

#endif // RD_CINE_H
