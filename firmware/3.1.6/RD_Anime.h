// ====================================================================
//  RD_Anime.h - caras de anime
//  No se incluye directamente: lo hace RelojDinamico.h.
// ====================================================================
#ifndef RD_ANIME_H
#define RD_ANIME_H

#ifndef RELOJ_DINAMICO_H
#error "Incluye RelojDinamico.h, no este fichero"
#endif

// ────────────────────────────────────────────────────────────────────
// "Dragon Ball"
// ────────────────────────────────────────────────────────────────────
#define RDBZ_BX       8         // x izquierda del sprite (16 de ancho)
#define RDBZ_BY       6         // fila superior del sprite (22 de alto: pies en y=27)
#define RDBZ_DIG_Y    10        // dígitos grandes: filas 10..23
#define RDBZ_RAYO_Y   17        // centro del rayo (7 de grosor)
static const int16_t RDBZ_DIG_X[4] = { 35, 48, 69, 82 };
static const int8_t  RDBZ_NUBE_Y[2] = { 1, 4 };
static const uint8_t RDBZ_FLASH_K[3] PROGMEM = { 0, 3, 6 };

// Colores: 1 pelo, 2 piel, 3 traje, 4 traje sombra, 5 azul, 6 oscuro
static const RDRect RDBZ_GUARDIA[] PROGMEM = {
  {3,2,3,2,1},{5,0,2,4,1},{8,0,2,3,1},{10,1,2,3,1},{5,3,7,2,1},
  {6,5,6,4,2},{8,5,4,1,1},{9,6,1,1,6},{10,8,1,1,4},{7,9,3,1,2},
  {4,10,8,7,3},{4,11,2,5,4},{7,10,3,2,5},{4,16,8,1,5},
  {2,11,3,3,3},{1,13,3,2,2},
  {11,11,3,2,3},{13,10,3,3,2},
  {4,17,4,3,3},{8,17,4,3,3},{3,20,5,2,5},{8,20,6,2,5}
};
static const RDRect RDBZ_CARGA[] PROGMEM = {
  {3,2,3,2,1},{5,0,2,4,1},{8,0,2,3,1},{10,1,2,3,1},{5,3,7,2,1},
  {6,5,6,4,2},{8,5,4,1,1},{9,6,1,1,6},{10,8,1,1,4},{7,9,3,1,2},
  {4,10,8,7,3},{4,11,2,5,4},{7,10,3,2,5},{4,16,8,1,5},
  {0,11,6,3,3},{0,12,2,3,2},
  {3,17,4,3,3},{9,17,4,3,3},{2,20,5,2,5},{9,20,6,2,5}
};
static const RDRect RDBZ_DISPARO[] PROGMEM = {
  {3,2,3,2,1},{5,0,2,4,1},{8,0,2,3,1},{10,1,2,3,1},{5,3,7,2,1},
  {6,5,6,4,2},{8,5,4,1,1},{9,6,1,1,6},{10,8,1,1,4},{7,9,3,1,2},
  {4,10,8,7,3},{4,11,2,5,4},{7,10,3,2,5},{4,16,8,1,5},
  {4,11,6,2,3},{10,10,6,3,3},{14,10,2,3,2},
  {3,17,4,3,3},{9,17,4,3,3},{2,20,5,2,5},{9,20,6,2,5}
};
#define RDBZ_N(a) ((int)(sizeof(a) / sizeof(a[0])))
// Mesetas del fondo (32x16, origen en x=96, y=12). Colores: 1 roca, 2 cima clara, 3 sombra
static const RDRect RDBZ_MESA[] PROGMEM = {
  {3,2,14,14,1},{2,2,16,2,2},{3,4,3,12,3},{20,6,10,10,1},{19,6,12,2,2},{20,8,2,8,3},{0,12,3,4,1}
};

struct RDBZEstado {
  int8_t   d[4], obj[4];
  uint8_t  pend;
  uint8_t  modo;        // 0 reposo 1 carga 2 dispara 3 sostiene 4 se apaga
  uint8_t  t;
  uint8_t  pose;        // 0 guardia 1 carga 2 disparo
  uint8_t  bob;
  bool     super;       // pelo dorado y aura
  bool     beam;
  int16_t  bx, bt;      // cabeza y cola del rayo
  uint8_t  exp[4];
  int16_t  nube[2];
  uint8_t  flash;
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDBZEstado rdbz;

static void rdbz_marcarGuerrero() { rdMarcar(RDBZ_BX - 6, RDBZ_BY - 6, 30, 32); }
static void rdbz_marcarRayo()     { rdMarcar(rdbz.bt - 1, RDBZ_RAYO_Y - 5, rdbz.bx - rdbz.bt + 9, 11); }
static void rdbz_marcarDigito(int i) { rdMarcar(RDBZ_DIG_X[i] - 1, RDBZ_DIG_Y, 12, 15); }
static void rdbz_marcarNube(int i) { rdMarcar(rdbz.nube[i], RDBZ_NUBE_Y[i], 14, 5); }

static bool rdbz_difiere() {
  for (int i = 0; i < 4; i++) if (rdbz.d[i] != rdbz.obj[i]) return true;
  return false;
}

static void rdbz_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdbz.d);
  for (int i = 0; i < 4; i++) { rdbz.obj[i] = rdbz.d[i]; rdbz.exp[i] = 0; }
  rdbz.pend = 0; rdbz.modo = 0; rdbz.t = 0; rdbz.pose = 0; rdbz.bob = 0;
  rdbz.super = false; rdbz.beam = false; rdbz.bx = rdbz.bt = 0; rdbz.flash = 0;
  rdbz.nube[0] = 30; rdbz.nube[1] = 100;
  rdbz.dp = (s % 2 == 0); rdbz.n = 0; rdbz.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdbz_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  for (int i = 0; i < 4; i++) rdbz.obj[i] = nd[i];
}

static void rdbz_segundo(int s) { rdbz.dp = (s % 2 == 0); rdMarcar(60, 12, 6, 10); }

static void rdbz_tick() {
  rdbz.n++;

  if ((rdbz.n % 10) == 0 && rdbz.modo == 0) { rdbz.bob ^= 1; rdbz_marcarGuerrero(); }
  if ((rdbz.n % 14) == 0) { rdbz_marcarNube(0); if (--rdbz.nube[0] < -14) rdbz.nube[0] = RD_W; rdbz_marcarNube(0); }
  if ((rdbz.n % 22) == 0) { rdbz_marcarNube(1); if (--rdbz.nube[1] < -14) rdbz.nube[1] = RD_W; rdbz_marcarNube(1); }
  if (rdbz.flash) { rdbz.flash--; rdMarcar(0, 0, RD_W, RD_H); }
  for (int i = 0; i < 4; i++) if (rdbz.exp[i]) {
    rdbz.exp[i]--;
    if (!rdbz.exp[i]) rdbz.d[i] = rdbz.obj[i];
    rdbz_marcarDigito(i);
  }

  switch (rdbz.modo) {
    case 0:
      if (rdbz_difiere()) {
        rdbz.pend = 0;
        for (int i = 0; i < 4; i++) if (rdbz.d[i] != rdbz.obj[i]) rdbz.pend |= (1 << i);
        rdbz.modo = 1; rdbz.t = 0; rdbz.pose = 1; rdbz.super = true;
        rdbz_marcarGuerrero();
      }
      break;
    case 1:                                              // carga la bola de energía
      rdbz.t++;
      rdbz_marcarGuerrero();
      if (rdbz.t >= 12) {
        rdbz.modo = 2; rdbz.pose = 2;
        rdbz.beam = true; rdbz.bt = rdbz.bx = RDBZ_BX + 16;
        rdbz.flash = 2;
        rdMarcar(0, 0, RD_W, RD_H);
      }
      break;
    case 2:                                              // el rayo avanza
      rdbz_marcarGuerrero();
      rdbz_marcarRayo();
      rdbz.bx += 10; if (rdbz.bx > 127) rdbz.bx = 127;
      for (int i = 0; i < 4; i++)
        if ((rdbz.pend & (1 << i)) && rdbz.bx >= RDBZ_DIG_X[i] + 5) {
          rdbz.exp[i] = 5; rdbz.pend &= ~(1 << i);
          rdbz_marcarDigito(i);
        }
      if (rdbz.bx >= 127) { rdbz.modo = 3; rdbz.t = 0; }
      rdbz_marcarRayo();
      break;
    case 3:                                              // lo mantiene un instante
      rdbz.t++;
      rdbz_marcarGuerrero();
      rdbz_marcarRayo();                                  // la onda del rayo cambia en cada tick
      if (rdbz.t >= 4) rdbz.modo = 4;
      break;
    case 4:                                              // la cola alcanza a la cabeza: se apaga
      rdbz_marcarGuerrero();                              // el aura sigue parpadeando
      rdbz_marcarRayo();
      rdbz.bt += 14;
      if (rdbz.bt >= rdbz.bx) {
        rdbz.beam = false; rdbz.pose = 0; rdbz.super = false; rdbz.modo = 0;
        rdbz_marcarGuerrero();
        rdMarcar(0, RDBZ_RAYO_Y - 5, RD_W, 11);
      } else rdbz_marcarRayo();
      break;
  }
}

static void rdbz_paso(uint32_t ms) {
  if (!rdbz.ultOk) { rdbz.ult = ms; rdbz.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdbz.ult) >= RDM_TICK_MS && n < 3) { rdbz.ult += RDM_TICK_MS; rdbz_tick(); n++; }
  if (n == 3) rdbz.ult = ms;
}

static uint16_t rdbz_color(uint8_t c) {
  switch (c) {
    case 1: return rdbz.super ? RD_C(252, 222, 40) : RD_C(24, 24, 36);
    case 2: return RD_C(245, 190, 140);
    case 3: return RD_C(240, 120, 20);
    case 4: return RD_C(190, 80, 10);
    case 5: return RD_C(30, 80, 200);
    case 6: return RD_C(20, 20, 30);
  }
  return 0;
}

static uint16_t rdbz_hash(int x, int y, int t) {
  unsigned v = (unsigned)x * 7919u + (unsigned)y * 104729u + (unsigned)t * 1299709u;
  v ^= v >> 13; v *= 0x5bd1e995u; v ^= v >> 15;
  return (uint16_t)(v & 0xFFFF);
}

static uint16_t rdbz_rayo(int x, int y) {
  if (!rdbz.beam || x < rdbz.bt || x > rdbz.bx) return 0;
  int dy = y - RDBZ_RAYO_Y; if (dy < 0) dy = -dy;
  int resto = rdbz.bx - x;
  int mitad = (resto < 4) ? resto + 1 : 4;                 // punta redondeada
  if (dy > mitad) return 0;
  if (dy <= 1 || (resto <= 2 && dy <= 2)) return RD_C(255, 255, 255);
  bool onda = ((x + (int)rdbz.n * 3) & 4) != 0;
  if (dy == 2) return onda ? RD_C(170, 245, 255) : RD_C(110, 220, 255);
  if (dy == 3) return onda ? RD_C(70, 170, 255) : RD_C(40, 130, 250);
  return onda ? RD_C(40, 100, 235) : RD_C(24, 70, 200);
}

static uint16_t rdbz_fondo(int x, int y) {
  if (y >= 28) {                                           // suelo de roca y arena
    if (y == 28) return RD_C(235, 200, 130);
    if ((rdbz_hash(x, y, 7) % 23) == 0) return RD_C(150, 100, 60);
    return (y == 31) ? RD_C(185, 140, 85) : RD_C(205, 160, 100);
  }
  {
    int lx = x - 96, ly = y - 12;                          // mesetas
    if ((unsigned)lx < 32 && (unsigned)ly < 16) {
      uint8_t c = rdRectColor(RDBZ_MESA, RDBZ_N(RDBZ_MESA), lx, ly);
      if (c == 1) return RD_C(150, 100, 60);
      if (c == 2) return RD_C(205, 155, 100);
      if (c == 3) return RD_C(110, 70, 40);
    }
  }
  for (int i = 0; i < 2; i++) {                            // nubes
    int lx = x - rdbz.nube[i], ly = y - RDBZ_NUBE_Y[i];
    if ((unsigned)lx < 14 && (unsigned)ly < 5) {
      char ch = (char)pgm_read_byte(&RDM_NUBE[ly][lx]);
      if (ch == 'W') return RD_C(252, 252, 252);
      if (ch == 'G') return RD_C(200, 220, 250);
    }
  }
  return RD_C(40 + y * 3, 110 + y * 3, 220 + (y >> 1));   // cielo
}

static uint16_t rdbz_capas(int x, int y) {
  // 1) bola de energía mientras carga
  if (rdbz.modo == 1) {
    int r = 1 + rdbz.t / 3;
    int dx = x - (RDBZ_BX + 1), dy = y - (RDBZ_BY + 13);
    int d2 = dx * dx + dy * dy;
    if (d2 <= r * r) {
      if (d2 <= (r - 2) * (r - 2) && r > 2) return RD_C(255, 255, 255);
      if (d2 <= (r - 1) * (r - 1)) return RD_C(190, 250, 255);
      return RD_C(70, 170, 255);
    }
  }
  // 2) guerrero
  {
    int lx = x - RDBZ_BX, ly = y - (RDBZ_BY + (rdbz.pose == 0 ? rdbz.bob : 0));
    if ((unsigned)lx < 16 && (unsigned)ly < 22) {
      uint8_t c;
      if (rdbz.pose == 1)      c = rdRectColor(RDBZ_CARGA,   RDBZ_N(RDBZ_CARGA),   lx, ly);
      else if (rdbz.pose == 2) c = rdRectColor(RDBZ_DISPARO, RDBZ_N(RDBZ_DISPARO), lx, ly);
      else                    c = rdRectColor(RDBZ_GUARDIA, RDBZ_N(RDBZ_GUARDIA), lx, ly);
      if (c) return rdbz_color(c);
      // 3) aura dorada alrededor (solo transformado)
      if (rdbz.super) {
        int ex = lx - 8, ey = ly - 10;
        if ((ex * ex) * 4 + (ey * ey) <= 150 && (rdbz_hash(x, y, (int)rdbz.n) % 100) < 45)
          return (rdbz_hash(y, x, (int)rdbz.n) & 1) ? RD_C(255, 245, 140) : RD_C(255, 255, 220);
      }
    } else if (rdbz.super) {
      int ex = lx - 8, ey = ly - 10;
      if ((ex * ex) * 4 + (ey * ey) <= 260 && (rdbz_hash(x, y, (int)rdbz.n) % 100) < 30)
        return (rdbz_hash(y, x, (int)rdbz.n) & 1) ? RD_C(255, 245, 140) : RD_C(255, 255, 220);
    }
  }
  // 4) hora
  if (y >= RDBZ_DIG_Y && y <= RDBZ_DIG_Y + 14 && x >= 35 && x <= 93) {
    for (int i = 0; i < 4; i++) {
      int lx = x - RDBZ_DIG_X[i], ly = y - RDBZ_DIG_Y;
      if ((unsigned)lx > 10) continue;
      if (rdbz.exp[i]) {
        static const uint8_t DENS[6] = { 0, 60, 110, 160, 210, 240 };
        uint16_t hh = rdbz_hash(x, y, rdbz.exp[i]);
        if ((hh & 0xFF) < DENS[rdbz.exp[i]]) {
          uint8_t c = (hh >> 8) & 3;
          return c == 0 ? RD_C(255, 255, 255) : c == 1 ? RD_C(255, 240, 80) : c == 2 ? RD_C(255, 150, 30) : RD_C(120, 220, 255);
        }
        break;
      }
      if (rdDigito2(rdbz.d[i], lx, ly)) return RD_C(255, 255, 255);
      if (rdDigito2(rdbz.d[i], lx - 1, ly) || rdDigito2(rdbz.d[i], lx, ly - 1) || rdDigito2(rdbz.d[i], lx - 1, ly - 1) ||
          rdDigito2(rdbz.d[i], lx + 1, ly) || rdDigito2(rdbz.d[i], lx, ly + 1)) return RD_C(16, 24, 80);
    }
    if (rdbz.dp) {
      int lx = x - 62;
      if ((unsigned)lx < 2 && ((unsigned)(y - 13) < 2 || (unsigned)(y - 19) < 2)) return RD_C(255, 255, 255);
    }
  }
  // 5) rayo y fondo
  { uint16_t c = rdbz_rayo(x, y); if (c) return c; }
  return rdbz_fondo(x, y);
}

static uint16_t rdbz_pixel(int x, int y) {
  uint16_t c = rdbz_capas(x, y);
  if (rdbz.flash) c = rdBlanco(c, pgm_read_byte(&RDBZ_FLASH_K[rdbz.flash > 2 ? 2 : rdbz.flash]));
  return c;
}

// ────────────────────────────────────────────────────────────────────
// "One Piece"
// ────────────────────────────────────────────────────────────────────
#define RDPI_BX        8        // x izquierda del sprite (14 de ancho)
#define RDPI_BY        3        // fila superior del sprite (20 de alto: pies en y=22, sobre la cubierta y=23)
#define RDPI_HOMBRO_X  (RDPI_BX + 12)
#define RDPI_BRAZO_Y   14       // el brazo ocupa las filas 14..15
#define RDPI_DIG_Y     8        // dígitos grandes: filas 8..21 dentro del cartel
static const int16_t RDPI_DIG_X[4] = { 41, 54, 72, 85 };
static const int8_t  RDPI_NUBE_Y[2] = { 0, 2 };
static const uint8_t RDPI_OLA[16] PROGMEM = { 1,1,0,0,0,1,2,2,2,1,0,0,0,1,1,2 };

// Colores: 1 sombrero, 2 cinta, 3 piel, 4 pelo, 5 chaleco, 6 pantalón, 7 sandalias, 8 oscuro
static const RDRect RDPI_REPOSO[] PROGMEM = {
  {1,3,12,2,1},{3,0,8,4,1},{3,3,8,1,2},{3,5,2,2,4},{9,5,2,2,4},
  {4,5,6,5,3},{8,6,1,1,8},{5,9,4,1,8},
  {3,10,8,6,5},{6,10,2,4,3},{1,11,2,5,3},{10,11,2,5,3},
  {3,16,8,2,6},{4,18,2,1,3},{8,18,2,1,3},{3,19,4,1,7},{8,19,4,1,7}
};
static const RDRect RDPI_PREPARA[] PROGMEM = {
  {1,3,12,2,1},{3,0,8,4,1},{3,3,8,1,2},{3,5,2,2,4},{9,5,2,2,4},
  {4,5,6,5,3},{8,6,1,1,8},{5,9,4,1,8},
  {3,10,8,6,5},{6,10,2,4,3},
  {0,11,5,2,3},{0,10,2,4,3},{10,11,3,2,3},
  {3,16,8,2,6},{2,18,2,1,3},{9,18,2,1,3},{1,19,4,1,7},{9,19,4,1,7}
};
static const RDRect RDPI_PUNO[] PROGMEM = {
  {1,3,12,2,1},{3,0,8,4,1},{3,3,8,1,2},{3,5,2,2,4},{9,5,2,2,4},
  {4,5,6,5,3},{8,6,1,1,8},{5,9,4,1,8},
  {3,10,8,6,5},{6,10,2,4,3},{1,11,2,5,3},
  {3,16,8,2,6},{3,18,2,1,3},{9,18,2,1,3},{2,19,4,1,7},{9,19,4,1,7}
};
#define RDPI_N(a) ((int)(sizeof(a) / sizeof(a[0])))

struct RDPIEstado {
  int8_t   d[4], obj[4];
  uint8_t  pend;
  uint8_t  modo;        // 0 reposo 1 se prepara 2 puñetazo 3 sostiene 4 recoge el brazo
  uint8_t  t;
  uint8_t  pose;        // 0 reposo 1 prepara 2 puño
  uint8_t  bob;
  bool     brazo;
  int16_t  fx;          // x del puño
  uint8_t  exp[4];
  int16_t  starX;  uint8_t starT;
  int16_t  nube[2];
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDPIEstado rdpi;

static uint16_t rdpi_hash(int x, int y, int t) {
  unsigned v = (unsigned)x * 7919u + (unsigned)y * 104729u + (unsigned)t * 1299709u;
  v ^= v >> 13; v *= 0x5bd1e995u; v ^= v >> 15;
  return (uint16_t)(v & 0xFFFF);
}

static void rdpi_marcarPirata() { rdMarcar(RDPI_BX - 3, RDPI_BY - 1, 22, 25); }
static void rdpi_marcarBrazo()  { rdMarcar(RDPI_HOMBRO_X - 1, 11, rdpi.fx - RDPI_HOMBRO_X + 6, 9); }
static void rdpi_marcarDigito(int i) { rdMarcar(RDPI_DIG_X[i] - 1, RDPI_DIG_Y, 12, 15); }
static void rdpi_marcarEstrella() { rdMarcar(rdpi.starX - 9, RDPI_BRAZO_Y - 9, 19, 19); }
static void rdpi_marcarNube(int i) { rdMarcar(rdpi.nube[i], RDPI_NUBE_Y[i], 14, 5); }

static bool rdpi_difiere() {
  for (int i = 0; i < 4; i++) if (rdpi.d[i] != rdpi.obj[i]) return true;
  return false;
}

static void rdpi_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdpi.d);
  for (int i = 0; i < 4; i++) { rdpi.obj[i] = rdpi.d[i]; rdpi.exp[i] = 0; }
  rdpi.pend = 0; rdpi.modo = 0; rdpi.t = 0; rdpi.pose = 0; rdpi.bob = 0;
  rdpi.brazo = false; rdpi.fx = RDPI_HOMBRO_X; rdpi.starT = 0;
  rdpi.nube[0] = 20; rdpi.nube[1] = 104;
  rdpi.dp = (s % 2 == 0); rdpi.n = 0; rdpi.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdpi_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  for (int i = 0; i < 4; i++) rdpi.obj[i] = nd[i];
}

static void rdpi_segundo(int s) { rdpi.dp = (s % 2 == 0); rdMarcar(65, 11, 6, 10); }

static void rdpi_tick() {
  rdpi.n++;

  if ((rdpi.n & 1) == 0) rdMarcar(0, 23, RD_W, 9);                       // olas (la fase cambia cada 2 ticks)
  if ((rdpi.n % 10) == 0 && rdpi.modo == 0) { rdpi.bob ^= 1; rdpi_marcarPirata(); }
  if ((rdpi.n % 14) == 0) { rdpi_marcarNube(0); if (--rdpi.nube[0] < -14) rdpi.nube[0] = RD_W; rdpi_marcarNube(0); }
  if ((rdpi.n % 22) == 0) { rdpi_marcarNube(1); if (--rdpi.nube[1] < -14) rdpi.nube[1] = RD_W; rdpi_marcarNube(1); }
  if (rdpi.starT) { rdpi_marcarEstrella(); rdpi.starT--; rdpi_marcarEstrella(); }
  for (int i = 0; i < 4; i++) if (rdpi.exp[i]) {
    rdpi.exp[i]--;
    if (!rdpi.exp[i]) rdpi.d[i] = rdpi.obj[i];
    rdpi_marcarDigito(i);
  }

  switch (rdpi.modo) {
    case 0:
      if (rdpi_difiere()) {
        rdpi.pend = 0;
        for (int i = 0; i < 4; i++) if (rdpi.d[i] != rdpi.obj[i]) rdpi.pend |= (1 << i);
        rdpi.modo = 1; rdpi.t = 0; rdpi.pose = 1;
        rdpi_marcarPirata();
      }
      break;
    case 1:                                                             // se echa atrás
      rdpi.t++;
      if (rdpi.t >= 8) {
        rdpi.modo = 2; rdpi.pose = 2; rdpi.brazo = true; rdpi.fx = RDPI_HOMBRO_X + 4;
        rdpi_marcarPirata(); rdpi_marcarBrazo();
      }
      break;
    case 2:                                                             // el brazo de goma se estira
      rdpi_marcarBrazo();
      rdpi.fx += 14; if (rdpi.fx > 127) rdpi.fx = 127;
      for (int i = 0; i < 4; i++)
        if ((rdpi.pend & (1 << i)) && rdpi.fx >= RDPI_DIG_X[i] + 5) {
          rdpi.exp[i] = 5; rdpi.pend &= ~(1 << i);
          rdpi.starX = RDPI_DIG_X[i] + 5; rdpi.starT = 4;
          rdpi_marcarDigito(i); rdpi_marcarEstrella();
        }
      rdpi_marcarBrazo();
      if (rdpi.fx >= 127) { rdpi.modo = 3; rdpi.t = 0; }
      break;
    case 3:                                                             // lo mantiene un instante
      rdpi.t++;
      if (rdpi.t >= 3) rdpi.modo = 4;
      break;
    case 4:                                                             // el brazo vuelve de golpe
      rdpi_marcarBrazo();
      rdpi.fx -= 22;
      if (rdpi.fx <= RDPI_HOMBRO_X + 4) {
        rdpi.brazo = false; rdpi.pose = 0; rdpi.modo = 0;
        rdpi_marcarPirata();
        rdMarcar(RDPI_HOMBRO_X - 1, 11, RD_W - RDPI_HOMBRO_X, 9);
      } else rdpi_marcarBrazo();
      break;
  }
}

static void rdpi_paso(uint32_t ms) {
  if (!rdpi.ultOk) { rdpi.ult = ms; rdpi.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdpi.ult) >= RDM_TICK_MS && n < 3) { rdpi.ult += RDM_TICK_MS; rdpi_tick(); n++; }
  if (n == 3) rdpi.ult = ms;
}

static uint16_t rdpi_color(uint8_t c) {
  switch (c) {
    case 1: return RD_C(250, 214, 90);
    case 2: return RD_C(220, 40, 40);
    case 3: return RD_C(245, 190, 140);
    case 4: return RD_C(24, 20, 28);
    case 5: return RD_C(220, 40, 40);
    case 6: return RD_C(40, 90, 190);
    case 7: return RD_C(110, 70, 40);
    case 8: return RD_C(30, 20, 20);
  }
  return 0;
}

static bool rdpi_estrella(int dx, int dy, int age, int r) {
  int ax = dx < 0 ? -dx : dx, ay = dy < 0 ? -dy : dy;
  int d = (ax > ay ? ax : ay) + ((ax > ay ? ay : ax) >> 1);
  if (d > r) return false;
  if (d >= r - 1) return true;
  if (ax <= 1 || ay <= 1 || (ax - ay <= 1 && ay - ax <= 1)) return true;
  return d <= 4 - age;
}

static uint16_t rdpi_fondo(int x, int y) {
  // olas (la cresta sube y baja) y casco del barco
  int cresta = 24 + (int)pgm_read_byte(&RDPI_OLA[((x >> 1) + (int)(rdpi.n >> 1)) & 15]);
  if (y >= 23 && x <= 34 - (y - 23)) {
    if (y == 23) return RD_C(200, 150, 90);                             // cubierta
    if (x >= 13 && x <= 15 && y >= 26 && y <= 28) return RD_C(30, 70, 120);   // ojo de buey
    if (((y - 23) % 3) == 0) return RD_C(110, 70, 40);
    return RD_C(150, 100, 60);
  }
  if (y >= cresta) {
    if (y == cresta) return RD_C(240, 250, 255);                        // espuma
    if (y == cresta + 1) return RD_C(110, 190, 250);
    return RD_C(30 - (y >> 2), 100 - (y >> 1), 190 - y);
  }
  // sol
  { int dx = x - 118, dy = y - 6; if (dx * dx + dy * dy <= 14) return RD_C(255, 232, 110); }
  // nubes
  for (int i = 0; i < 2; i++) {
    int lx = x - rdpi.nube[i], ly = y - RDPI_NUBE_Y[i];
    if ((unsigned)lx < 14 && (unsigned)ly < 5) {
      char ch = (char)pgm_read_byte(&RDM_NUBE[ly][lx]);
      if (ch == 'W') return RD_C(252, 252, 252);
      if (ch == 'G') return RD_C(200, 222, 250);
    }
  }
  return RD_C(70 + y * 3, 150 + y * 2, 240 + (y >> 2));               // cielo
}

static uint16_t rdpi_capas(int x, int y) {
  // 1) estrella de impacto
  if (rdpi.starT) {
    int age = 4 - rdpi.starT;
    if (rdpi_estrella(x - rdpi.starX, y - RDPI_BRAZO_Y, age, 2 + age * 2))
      return age < 2 ? RD_C(255, 255, 255) : RD_C(255, 225, 70);
  }
  // 2) brazo de goma y puño
  if (rdpi.brazo) {
    if (y >= RDPI_BRAZO_Y && y <= RDPI_BRAZO_Y + 1 && x >= RDPI_HOMBRO_X && x <= rdpi.fx - 4) return RD_C(245, 190, 140);
    if (x >= rdpi.fx - 4 && x <= rdpi.fx && y >= RDPI_BRAZO_Y - 1 && y <= RDPI_BRAZO_Y + 2)
      return (x == rdpi.fx - 4 || y == RDPI_BRAZO_Y - 1) ? RD_C(255, 215, 170) : RD_C(230, 165, 115);
  }
  // 3) pirata
  {
    int lx = x - RDPI_BX, ly = y - (RDPI_BY + (rdpi.pose == 0 ? rdpi.bob : 0));
    if ((unsigned)lx < 14 && (unsigned)ly < 20) {
      uint8_t c;
      if (rdpi.pose == 1)      c = rdRectColor(RDPI_PREPARA, RDPI_N(RDPI_PREPARA), lx, ly);
      else if (rdpi.pose == 2) c = rdRectColor(RDPI_PUNO,    RDPI_N(RDPI_PUNO),    lx, ly);
      else                    c = rdRectColor(RDPI_REPOSO,  RDPI_N(RDPI_REPOSO),  lx, ly);
      if (c) return rdpi_color(c);
    }
  }
  // 4) cartel de "se busca"
  if (x >= 36 && x <= 98 && y >= 1 && y <= 26) {
    // hora impresa en tinta
    if (y >= RDPI_DIG_Y && y <= RDPI_DIG_Y + 13) {
      for (int i = 0; i < 4; i++) {
        int lx = x - RDPI_DIG_X[i], ly = y - RDPI_DIG_Y;
        if ((unsigned)lx >= 10) continue;
        if (rdpi.exp[i]) {
          static const uint8_t DENS[6] = { 0, 60, 110, 160, 210, 240 };
          uint16_t hh = rdpi_hash(x, y, rdpi.exp[i]);
          if ((hh & 0xFF) < DENS[rdpi.exp[i]]) {
            uint8_t c = (hh >> 8) & 3;
            return c == 0 ? RD_C(240, 220, 170) : c == 1 ? RD_C(150, 100, 60) : c == 2 ? RD_C(255, 225, 90) : RD_C(60, 36, 20);
          }
          break;
        }
        if (rdDigito2(rdpi.d[i], lx, ly)) return RD_C(60, 36, 20);
      }
      if (rdpi.dp && x >= 67 && x <= 68 && ((y >= 11 && y <= 12) || (y >= 17 && y <= 18))) return RD_C(60, 36, 20);
    }
    if (x == 36 || x == 98 || y == 1 || y == 26) return RD_C(120, 80, 40);
    if ((x == 38 || x == 96) && y == 2) return RD_C(60, 40, 30);               // clavos
    if (y >= 3 && y <= 5 && x >= 41 && x <= 93 && ((x - 41) % 7) < 5 && ((x + y) % 3) != 0) return RD_C(60, 36, 20);   // "SE BUSCA"
    if (y == 6 && x >= 41 && x <= 93) return RD_C(150, 110, 60);
    if (y >= 23 && y <= 24 && x >= 45 && x <= 89 && ((x - 45) % 5) < 3) return RD_C(120, 80, 40);                       // recompensa
    return (rdpi_hash(x, y, 2) % 9 == 0) ? RD_C(222, 196, 148) : RD_C(232, 208, 160);
  }
  // 5) mascarón de proa del barco
  if (x >= 31 && x <= 35 && y >= 18 && y <= 22) {
    if (x >= 34 && y >= 20 && y <= 21) return RD_C(240, 200, 90);
    if (x <= 33 && y >= 19) return RD_C(248, 248, 244);
    if (y == 18 && x == 32) return RD_C(200, 160, 60);
  }
  return rdpi_fondo(x, y);
}

static uint16_t rdpi_pixel(int x, int y) { return rdpi_capas(x, y); }

// ────────────────────────────────────────────────────────────────────
// "Naruto"
// ────────────────────────────────────────────────────────────────────
#define RDNR_CASA_X    6        // x izquierda del ninja en casa (caja de 16x16)
#define RDNR_CASA_Y    12       // fila superior en casa (pies en y=27)
#define RDNR_DIG_Y     4        // dígitos grandes: filas 4..17
static const int16_t RDNR_DIG_X[4] = { 35, 48, 69, 82 };
static const uint8_t RDNR_TEJADO[16] PROGMEM = { 5,8,6,9,7,5,8,6,9,5,7,8,6,9,5,7 };
static const int8_t  RDNR_DIR[5][2] PROGMEM = { {0,0}, {-4,1}, {4,1}, {-2,-3}, {3,-3} };

// Colores: 1 pelo, 2 piel, 3 naranja, 4 azul oscuro, 5 protector, 6 placa, 7 oscuro, 8 sandalias
#define RDNR_CABEZA \
  {2,0,2,3,1},{5,0,2,2,1},{8,0,2,3,1},{2,2,8,2,1}, \
  {3,4,6,5,2},{2,3,8,2,5},{5,3,3,2,6},{4,6,1,1,7},{7,6,1,1,7},{3,7,1,1,7},{8,7,1,1,7}
static const RDRect RDNR_REPOSO[] PROGMEM = {
  RDNR_CABEZA,
  {2,9,8,5,3},{4,9,4,1,4},{0,10,2,4,3},{10,10,2,4,3},{0,13,2,1,2},{10,13,2,1,2},
  {3,14,3,2,3},{6,14,3,2,3},{3,15,3,1,8},{6,15,3,1,8}
};
static const RDRect RDNR_CARGA[] PROGMEM = {
  RDNR_CABEZA,
  {2,9,8,5,3},{4,9,4,1,4},{0,10,2,4,3},{10,10,4,2,3},{13,9,2,3,2},{0,13,2,1,2},
  {3,14,3,2,3},{6,14,3,2,3},{3,15,3,1,8},{6,15,3,1,8}
};
static const RDRect RDNR_SALTO[] PROGMEM = {
  RDNR_CABEZA,
  {2,9,8,5,3},{4,9,4,1,4},{0,10,4,2,3},{10,10,4,2,3},{13,9,2,3,2},{0,9,2,3,2},
  {2,13,3,2,3},{7,13,3,2,3},{2,15,3,1,8},{7,15,3,1,8}
};
#define RDNR_N(a) ((int)(sizeof(a) / sizeof(a[0])))

struct RDNREstado {
  int8_t   d[4], obj[4];
  uint8_t  pend;
  uint8_t  modo;        // 0 reposo 1 carga 2 salta 3 impacto 4 humo
  int8_t   objetivo;
  uint8_t  t, N;
  uint8_t  pose;        // 0 reposo 1 carga 2 salto
  uint8_t  bob;
  int16_t  nx, ny;
  int16_t  x0, y0, x1, y1;
  uint8_t  ds[4], dt[4]; // 0 normal 1 destruido 2 asomando del humo
  int16_t  hx;  uint8_t humoT;   // humo del impacto
  uint8_t  humoCasa;             // humo en casa
  bool     tronco;
  uint8_t  golpe;
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDNREstado rdnr;

static uint16_t rdnr_hash(int x, int y, int t) {
  unsigned v = (unsigned)x * 7919u + (unsigned)y * 104729u + (unsigned)t * 1299709u;
  v ^= v >> 13; v *= 0x5bd1e995u; v ^= v >> 15;
  return (uint16_t)(v & 0xFFFF);
}

static void rdnr_hoja(int k, int nn, int* x, int* y) {
  *x = (k * 37 + (nn >> 1)) % 140 - 6;
  *y = (k * 9 + (nn >> 2) + ((nn >> 3) & 1)) % 30;
}

static void rdnr_marcarNinja() { rdMarcar(rdnr.nx - 9, rdnr.ny - 3, 33, 22); }
static void rdnr_marcarImpacto(int cx) { rdMarcar(cx - 26, 0, 53, 32); }             // el humo se extiende bastante
static void rdnr_marcarCasa() { rdMarcar(0, 0, 40, 32); }

static bool rdnr_difiere() {
  for (int i = 0; i < 4; i++) if (rdnr.d[i] != rdnr.obj[i]) return true;
  return false;
}
static int rdnr_siguiente() {
  for (int i = 0; i < 4; i++) if (rdnr.pend & (1 << i)) return i;
  return -1;
}

static void rdnr_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdnr.d);
  for (int i = 0; i < 4; i++) { rdnr.obj[i] = rdnr.d[i]; rdnr.ds[i] = 0; rdnr.dt[i] = 0; }
  rdnr.pend = 0; rdnr.modo = 0; rdnr.objetivo = -1; rdnr.t = 0; rdnr.pose = 0; rdnr.bob = 0;
  rdnr.nx = RDNR_CASA_X; rdnr.ny = RDNR_CASA_Y;
  rdnr.humoT = 0; rdnr.humoCasa = 0; rdnr.tronco = false; rdnr.golpe = 0;
  rdnr.dp = (s % 2 == 0); rdnr.n = 0; rdnr.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdnr_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  for (int i = 0; i < 4; i++) rdnr.obj[i] = nd[i];
}

static void rdnr_segundo(int s) { rdnr.dp = (s % 2 == 0); rdMarcar(60, 6, 8, 11); }

static void rdnr_tick() {
  rdnr.n++;

  if ((rdnr.n & 1) == 0) for (int k = 0; k < 4; k++) {                         // hojas que caen: borrar la anterior y pintar la nueva
    int x, y;
    rdnr_hoja(k, (int)rdnr.n, &x, &y);     rdMarcar(x - 1, y - 1, 6, 5);
    rdnr_hoja(k, (int)rdnr.n - 2, &x, &y); rdMarcar(x - 1, y - 1, 6, 5);
  }
  if ((rdnr.n % 10) == 0 && rdnr.modo == 0) { rdnr.bob ^= 1; rdnr_marcarNinja(); }

  switch (rdnr.modo) {
    case 0:
      if (rdnr_difiere()) {
        rdnr.pend = 0;
        for (int i = 0; i < 4; i++) if (rdnr.d[i] != rdnr.obj[i]) rdnr.pend |= (1 << i);
        rdnr.objetivo = rdnr_siguiente();
        rdnr.modo = 1; rdnr.t = 0; rdnr.pose = 1;
        rdnr_marcarNinja();
      }
      break;
    case 1:                                                                    // carga la esfera giratoria
      rdnr.t++;
      rdnr_marcarNinja();
      if (rdnr.t >= 8) {
        rdnr.modo = 2; rdnr.t = 0; rdnr.pose = 2;
        rdnr.x0 = rdnr.nx; rdnr.y0 = rdnr.ny;
        rdnr.x1 = RDNR_DIG_X[rdnr.objetivo] + 5 - 15; rdnr.y1 = 1;
        int dist = rdnr.x1 - rdnr.x0; if (dist < 0) dist = -dist;
        rdnr.N = 5 + dist / 8;
      }
      break;
    case 2: {                                                                  // salta en arco hasta el dígito
      rdnr_marcarNinja();
      rdnr.t++;
      int p = (rdnr.t * 256) / rdnr.N;
      rdnr.nx = rdnr.x0 + ((rdnr.x1 - rdnr.x0) * p) / 256;
      rdnr.ny = rdnr.y0 + ((rdnr.y1 - rdnr.y0) * p) / 256 - ((4 * 5 * p * (256 - p)) >> 16);
      if (rdnr.t >= rdnr.N) {
        rdnr.nx = rdnr.x1; rdnr.ny = rdnr.y1;
        rdnr.modo = 3; rdnr.t = 0; rdnr.golpe = 0;
        rdnr.ds[rdnr.objetivo] = 1;
        rdnr_marcarImpacto(RDNR_DIG_X[rdnr.objetivo] + 5);
      }
      rdnr_marcarNinja();
    } break;
    case 3:                                                                    // el torbellino revienta el dígito
      rdnr.t++; rdnr.golpe = rdnr.t;
      rdnr_marcarImpacto(RDNR_DIG_X[rdnr.objetivo] + 5);
      rdnr_marcarNinja();
      if (rdnr.t >= 4) {
        int o = rdnr.objetivo;
        rdnr.modo = 4; rdnr.t = 0; rdnr.golpe = 0;
        rdnr.hx = RDNR_DIG_X[o] + 5; rdnr.humoT = 1; rdnr.humoCasa = 1; rdnr.tronco = true;
        rdnr.d[o] = rdnr.obj[o]; rdnr.ds[o] = 2; rdnr.dt[o] = 5; rdnr.pend &= ~(1 << o);
        rdnr.nx = RDNR_CASA_X; rdnr.ny = RDNR_CASA_Y; rdnr.pose = 0;                // se cambia por un tronco y reaparece en casa
        rdnr_marcarImpacto(rdnr.hx); rdnr_marcarCasa(); rdnr_marcarNinja();
      }
      break;
    case 4:                                                                    // humo: asoma el dígito nuevo
      rdnr.t++;
      rdnr_marcarImpacto(rdnr.hx); rdnr_marcarCasa();
      if (rdnr.humoT) rdnr.humoT++;
      if (rdnr.humoCasa) rdnr.humoCasa++;
      if (rdnr.dt[rdnr.objetivo]) rdnr.dt[rdnr.objetivo]--;
      if (rdnr.t >= 6) {
        rdnr.humoT = 0; rdnr.humoCasa = 0; rdnr.tronco = false; rdnr.ds[rdnr.objetivo] = 0;
        int nxt = rdnr_siguiente();
        if (nxt >= 0) { rdnr.objetivo = nxt; rdnr.modo = 1; rdnr.t = 0; rdnr.pose = 1; }
        else { rdnr.modo = 0; rdnr.objetivo = -1; }
        rdnr_marcarNinja();
      }
      break;
  }
}

static void rdnr_paso(uint32_t ms) {
  if (!rdnr.ultOk) { rdnr.ult = ms; rdnr.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdnr.ult) >= RDM_TICK_MS && n < 3) { rdnr.ult += RDM_TICK_MS; rdnr_tick(); n++; }
  if (n == 3) rdnr.ult = ms;
}

static uint16_t rdnr_color(uint8_t c) {
  switch (c) {
    case 1: return RD_C(255, 225, 60);
    case 2: return RD_C(245, 190, 140);
    case 3: return RD_C(255, 140, 30);
    case 4: return RD_C(30, 40, 110);
    case 5: return RD_C(40, 70, 190);
    case 6: return RD_C(200, 210, 225);
    case 7: return RD_C(30, 20, 30);
    case 8: return RD_C(40, 60, 170);
  }
  return 0;
}

// Ángulo aproximado 0..255 alrededor de un punto (sin trigonometría)
static uint8_t rdnr_ang(int dx, int dy) {
  int ax = dx < 0 ? -dx : dx, ay = dy < 0 ? -dy : dy;
  int s = ax + ay;
  if (s == 0) return 0;
  int p = (ay * 64) / s;
  if (dx >= 0) return (uint8_t)((dy >= 0) ? p : 256 - p);
  return (uint8_t)((dy >= 0) ? 128 - p : 128 + p);
}

// Esfera de energía giratoria de radio r centrada en (dx, dy) relativo; 0 si queda fuera
static uint16_t rdnr_esfera(int dx, int dy, int r) {
  int d2 = dx * dx + dy * dy;
  if (d2 > r * r) return 0;
  if (d2 >= (r - 1) * (r - 1) && r > 2) return RD_C(190, 235, 255);
  int ax = dx < 0 ? -dx : dx, ay = dy < 0 ? -dy : dy;
  int rr = (ax > ay ? ax : ay) + ((ax > ay ? ay : ax) >> 1);
  int v = (int)rdnr_ang(dx, dy) * 2 + rr * 22 - (int)rdnr.n * 38;
  int tri = (v & 255) - 128; if (tri < 0) tri = -tri;
  if (tri < 36) return RD_C(30, 90, 235);
  if (tri < 80) return RD_C(80, 170, 255);
  return RD_C(230, 248, 255);
}

// Nube de humo centrada en (cx, cy) con edad a (1..); 0 si no hay humo en el punto
static uint16_t rdnr_humo(int x, int y, int cx, int cy, int a) {
  int R = 3 + a; if (R > 8) R = 8;
  bool dentro = false;
  for (int k = 0; k < 5; k++) {
    int ox = cx + ((int)(int8_t)pgm_read_byte(&RDNR_DIR[k][0]) * R) / 3, oy = cy + ((int)(int8_t)pgm_read_byte(&RDNR_DIR[k][1]) * R) / 3;
    int dx = x - ox, dy = y - oy, rr = (R * 2) / 3 + 1;
    if (dx * dx + dy * dy <= rr * rr) { dentro = true; break; }
  }
  if (!dentro) return 0;
  int densidad = 255 - a * 28; if (densidad < 50) densidad = 50;
  uint16_t h = rdnr_hash(x, y, a);
  if ((h & 0xFF) > densidad) return RD_C(215, 215, 225);              // humo ralo: velo gris claro
  return (h & 0x300) ? RD_C(245, 245, 250) : RD_C(195, 195, 210);
}

static uint16_t rdnr_fondo(int x, int y) {
  // sendero de tierra
  if (y >= 28) {
    if (y == 28) return RD_C(225, 175, 110);
    return (rdnr_hash(x, y, 3) % 9 == 0) ? RD_C(120, 85, 55) : RD_C(160, 118, 76);
  }
  // hojas que caen
  for (int k = 0; k < 4; k++) {
    int hx, hy; rdnr_hoja(k, (int)rdnr.n, &hx, &hy);
    if (x >= hx && x <= hx + 2 && y >= hy && y <= hy + 1) return (k & 1) ? RD_C(235, 150, 40) : RD_C(90, 175, 60);
  }
  // acantilado con rostros esculpidos
  if (x >= 94 && y >= 5 + (94 - x > 0 ? 0 : 0)) {
    for (int k = 0; k < 4; k++) {
      int fx = 98 + k * 7;
      if (x >= fx && x <= fx + 4 && y >= 8 && y <= 12) return (x == fx + 1 || x == fx + 3) && y == 10 ? RD_C(60, 40, 70) : RD_C(165, 120, 140);
    }
    if (y >= 6) return RD_C(84, 54, 104);
  }
  // tejados de la aldea
  {
    int lx = x & 7, H = (int)pgm_read_byte(&RDNR_TEJADO[(x >> 3) & 15]);
    int cima = 27 - H + ((lx > 4 ? lx - 4 : 4 - lx));
    if (y >= cima) return RD_C(48, 30, 64);
  }
  return RD_C(100 + y * 5, 72 + y * 3, 150 - y * 2);                           // cielo de atardecer
}

static uint16_t rdnr_capas(int x, int y) {
  int ocx = RDNR_DIG_X[rdnr.objetivo >= 0 ? rdnr.objetivo : 0] + 5;
  // 1) humo (del impacto y de casa)
  if (rdnr.humoT)    { uint16_t c = rdnr_humo(x, y, rdnr.hx, 14, rdnr.humoT);                        if (c) return c; }
  if (rdnr.humoCasa) { uint16_t c = rdnr_humo(x, y, RDNR_CASA_X + 8, RDNR_CASA_Y + 8, rdnr.humoCasa); if (c) return c; }
  // 2) esfera de energía y torbellino del impacto
  if (rdnr.modo == 1 || rdnr.modo == 2) {
    int r = (rdnr.modo == 1) ? 2 + rdnr.t / 2 : 5;
    uint16_t c = rdnr_esfera(x - (rdnr.nx + 15), y - (rdnr.ny + 10), r);
    if (c) return c;
  }
  if (rdnr.modo == 3) {
    uint16_t c = rdnr_esfera(x - ocx, y - 11, 5 + rdnr.golpe * 2);
    if (c) return c;
  }
  // 3) estelas de velocidad al saltar
  if (rdnr.modo == 2) {
    int dy = y - rdnr.ny;
    if ((dy == 4 || dy == 8 || dy == 12) && x >= rdnr.nx - 9 && x < rdnr.nx - 1 && (((x + dy) & 1) == 0)) return RD_C(255, 245, 220);
  }
  // 4) ninja
  {
    int lx = x - rdnr.nx, ly = y - (rdnr.ny + (rdnr.modo == 0 ? rdnr.bob : 0));
    if ((unsigned)lx < 16 && (unsigned)ly < 16) {
      uint8_t c = (rdnr.pose == 1) ? rdRectColor(RDNR_CARGA, RDNR_N(RDNR_CARGA), lx, ly)
                : (rdnr.pose == 2) ? rdRectColor(RDNR_SALTO, RDNR_N(RDNR_SALTO), lx, ly)
                                  : rdRectColor(RDNR_REPOSO, RDNR_N(RDNR_REPOSO), lx, ly);
      if (c) return rdnr_color(c);
    }
  }
  // 5) tronco del cambiazo
  if (rdnr.tronco && rdnr.modo == 4) {
    int lx = x - (rdnr.hx - 3), ly = y - 24;
    if ((unsigned)lx < 7 && (unsigned)ly < 4) return (lx == 0 || lx == 6) ? RD_C(190, 140, 90) : RD_C(120, 76, 40);
  }
  // 6) hora sobre los postes
  if (y >= 3 && y <= 19 && x >= 33 && x <= 93) {
    for (int i = 0; i < 4; i++) {
      int lx = x - RDNR_DIG_X[i], ly = y - RDNR_DIG_Y;
      if ((unsigned)lx > 10) continue;
      uint8_t ds = rdnr.ds[i];
      if (ds == 1) continue;
      bool tinta = rdDigito2(rdnr.d[i], lx, ly);
      bool borde = !tinta && (rdDigito2(rdnr.d[i], lx - 1, ly) || rdDigito2(rdnr.d[i], lx, ly - 1) || rdDigito2(rdnr.d[i], lx - 1, ly - 1) ||
                              rdDigito2(rdnr.d[i], lx + 1, ly) || rdDigito2(rdnr.d[i], lx, ly + 1));
      if (ds == 2 && (int)(rdnr_hash(x, y, 8) % 5) >= 5 - (int)rdnr.dt[i]) continue;      // asoma poco a poco del humo
      if (tinta) return RD_C(255, 250, 235);
      if (borde) return RD_C(60, 30, 80);
    }
    if (rdnr.dp && x >= 62 && x <= 63 && ((y >= 7 && y <= 8) || (y >= 13 && y <= 14))) return RD_C(255, 250, 235);
  }
  // 7) postes de entrenamiento bajo cada dígito
  for (int i = 0; i < 4; i++) {
    int c = RDNR_DIG_X[i] + 5;
    if (y >= 18 && y <= 27 && x >= c - 2 && x <= c + 2) {
      if (y == 18) return RD_C(170, 118, 70);
      if (y == 22) return RD_C(215, 185, 130);
      return (x == c - 2 || x == c + 2) ? RD_C(105, 66, 38) : RD_C(140, 92, 52);
    }
  }
  return rdnr_fondo(x, y);
}

static uint16_t rdnr_pixel(int x, int y) { return rdnr_capas(x, y); }

// ────────────────────────────────────────────────────────────────────
// "Pokémon"
// ────────────────────────────────────────────────────────────────────
#define RDQ_BX        6        // x izquierda del entrenador (14 de ancho)
#define RDQ_BY        9        // fila superior (18 de alto: pies en y=26)
#define RDQ_DIG_Y     5        // dígitos grandes: filas 5..18
#define RDQ_BOLA_Y    19       // centro de la bola cuando descansa en la plataforma
static const int16_t RDQ_DIG_X[4] = { 38, 51, 72, 85 };
static const int8_t  RDQ_NUBE_Y[2] = { 0, 3 };
static const uint8_t RDQ_COLINA[16] PROGMEM = { 2,3,4,5,4,3,2,1,2,3,5,6,5,4,3,2 };
static const uint8_t RDQ_CAIDA[5] PROGMEM = { 13, 16, 19, 17, 19 };
static const int8_t  RDQ_BAMBOLEO[8] PROGMEM = { -1, -1, 0, 1, 1, 0, -1, 0 };

// Colores: 1 gorra, 2 blanco, 3 piel, 4 pelo, 5 chaqueta, 6 -, 7 pantalón, 8 zapatillas, 9 mochila, 10 oscuro
static const RDRect RDQ_REPOSO[] PROGMEM = {
  {3,0,8,3,1},{9,2,4,1,1},{7,1,2,1,2},{3,3,2,2,4},{4,3,6,5,3},{8,4,1,1,10},
  {3,8,8,5,5},{6,8,2,4,2},{0,9,3,5,9},{2,9,2,4,5},{2,13,2,1,3},{10,9,2,4,5},{10,13,2,2,3},
  {3,13,8,3,7},{3,16,3,2,8},{8,16,3,2,8}
};
static const RDRect RDQ_ATRAS[] PROGMEM = {
  {3,0,8,3,1},{9,2,4,1,1},{7,1,2,1,2},{3,3,2,2,4},{4,3,6,5,3},{8,4,1,1,10},
  {3,8,8,5,5},{6,8,2,4,2},{0,9,3,5,9},{2,9,2,4,5},{2,13,2,1,3},
  {10,6,2,3,5},{11,3,2,3,3},
  {3,13,8,3,7},{3,16,3,2,8},{8,16,3,2,8}
};
static const RDRect RDQ_LANZA[] PROGMEM = {
  {3,0,8,3,1},{9,2,4,1,1},{7,1,2,1,2},{3,3,2,2,4},{4,3,6,5,3},{8,4,1,1,10},
  {3,8,8,5,5},{6,8,2,4,2},{0,9,3,5,9},{2,9,2,4,5},{2,13,2,1,3},
  {10,8,4,2,5},{13,7,2,3,3},
  {3,13,8,3,7},{3,16,3,2,8},{9,16,3,2,8}
};
#define RDQ_N(a) ((int)(sizeof(a) / sizeof(a[0])))

struct RDQEstado {
  int8_t   d[4], obj[4];
  uint8_t  pend;
  uint8_t  modo;        // 0 reposo 1 prepara 2 vuelo 3 absorbe 4 cae 5 bambolea 6 clic 7 libera
  int8_t   objetivo;
  uint8_t  t, N;
  uint8_t  pose;        // 0 reposo 1 brazo atrás 2 lanzado
  uint8_t  bob;
  bool     bola;
  int16_t  bx, by;
  int16_t  x0, y0, x1, y1;
  uint8_t  ds[4], dt[4]; // estado de cada dígito: 0 normal 1 se absorbe 2 vacío 3 se materializa
  uint8_t  brillo;       // destello de la liberación
  int16_t  nube[2];
  uint8_t  seg, minu;
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDQEstado rdq;

static uint16_t rdq_hash(int x, int y, int t) {
  unsigned v = (unsigned)x * 7919u + (unsigned)y * 104729u + (unsigned)t * 1299709u;
  v ^= v >> 13; v *= 0x5bd1e995u; v ^= v >> 15;
  return (uint16_t)(v & 0xFFFF);
}

static void rdq_marcarEntrenador() { rdMarcar(RDQ_BX - 2, RDQ_BY - 1, 20, 20); }
static void rdq_marcarBola()       { rdMarcar(rdq.bx - 11, rdq.by - 11, 23, 23); }
static void rdq_marcarDigito(int i) { rdMarcar(RDQ_DIG_X[i] - 1, RDQ_DIG_Y, 12, 15); }
static void rdq_marcarCaja()       { rdMarcar(0, 0, 36, 10); }
static void rdq_marcarNube(int i)  { rdMarcar(rdq.nube[i], RDQ_NUBE_Y[i], 14, 5); }

static bool rdq_difiere() {
  for (int i = 0; i < 4; i++) if (rdq.d[i] != rdq.obj[i]) return true;
  return false;
}

static void rdq_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdq.d);
  for (int i = 0; i < 4; i++) { rdq.obj[i] = rdq.d[i]; rdq.ds[i] = 0; rdq.dt[i] = 0; }
  rdq.pend = 0; rdq.modo = 0; rdq.objetivo = -1; rdq.t = 0; rdq.pose = 0; rdq.bob = 0;
  rdq.bola = false; rdq.brillo = 0; rdq.nube[0] = 30; rdq.nube[1] = 100;
  rdq.seg = (uint8_t)s; rdq.minu = (uint8_t)m;
  rdq.dp = (s % 2 == 0); rdq.n = 0; rdq.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdq_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  for (int i = 0; i < 4; i++) rdq.obj[i] = nd[i];
  rdq.minu = (uint8_t)m;
  rdq_marcarCaja();
}

static void rdq_segundo(int s) {
  rdq.seg = (uint8_t)s; rdq.dp = (s % 2 == 0);
  rdq_marcarCaja();
  rdMarcar(64, 7, 4, 10);
}

static int rdq_siguiente() {
  for (int i = 0; i < 4; i++) if (rdq.pend & (1 << i)) return i;
  return -1;
}

static void rdq_tick() {
  rdq.n++;

  if ((rdq.n % 10) == 0 && rdq.modo == 0) { rdq.bob ^= 1; rdq_marcarEntrenador(); }
  if ((rdq.n % 8) == 0) rdMarcar(96, 18, 32, 11);                                // la hierba alta se mece
  if ((rdq.n % 14) == 0) { rdq_marcarNube(0); if (--rdq.nube[0] < -14) rdq.nube[0] = RD_W; rdq_marcarNube(0); }
  if ((rdq.n % 22) == 0) { rdq_marcarNube(1); if (--rdq.nube[1] < -14) rdq.nube[1] = RD_W; rdq_marcarNube(1); }
  for (int i = 0; i < 4; i++) {
    if (rdq.ds[i] == 1 || rdq.ds[i] == 3) {
      rdq_marcarDigito(i);
      if (rdq.dt[i]) rdq.dt[i]--;
      if (!rdq.dt[i]) rdq.ds[i] = (rdq.ds[i] == 1) ? 2 : 0;
    }
  }
  if (rdq.brillo) { rdq_marcarBola(); rdq.brillo--; }

  switch (rdq.modo) {
    case 0:
      if (rdq_difiere()) {
        rdq.pend = 0;
        for (int i = 0; i < 4; i++) if (rdq.d[i] != rdq.obj[i]) rdq.pend |= (1 << i);
        rdq.objetivo = rdq_siguiente();
        rdq.modo = 1; rdq.t = 0; rdq.pose = 1;
        rdq_marcarEntrenador();
      }
      break;
    case 1:                                                                      // levanta el brazo
      rdq.t++;
      if (rdq.t >= 3) {
        rdq.pose = 2;
        rdq.x0 = RDQ_BX + 15; rdq.y0 = RDQ_BY + 4;
        rdq.x1 = RDQ_DIG_X[rdq.objetivo] + 5; rdq.y1 = 11;
        int dist = rdq.x1 - rdq.x0; if (dist < 0) dist = -dist;
        rdq.N = 5 + dist / 8; rdq.t = 0;
        rdq.bola = true; rdq.bx = rdq.x0; rdq.by = rdq.y0;
        rdq.modo = 2;
        rdq_marcarEntrenador(); rdq_marcarBola();
      }
      break;
    case 2: {                                                                    // la bola vuela en arco
      rdq_marcarBola();
      rdq.t++;
      int p = (rdq.t * 256) / rdq.N;
      rdq.bx = rdq.x0 + ((rdq.x1 - rdq.x0) * p) / 256;
      rdq.by = rdq.y0 + ((rdq.y1 - rdq.y0) * p) / 256 - ((4 * 7 * p * (256 - p)) >> 16);
      if (rdq.t >= rdq.N) {
        rdq.bx = rdq.x1; rdq.by = rdq.y1;
        rdq.modo = 3; rdq.t = 0;
        rdq.ds[rdq.objetivo] = 1; rdq.dt[rdq.objetivo] = 3;                      // el dígito se absorbe
        rdq_marcarDigito(rdq.objetivo);
      }
      rdq_marcarBola();
    } break;
    case 3:                                                                      // haz rojo que absorbe
      rdq_marcarBola();
      rdq.t++;
      if (rdq.t >= 3) { rdq.modo = 4; rdq.t = 0; }
      break;
    case 4:                                                                      // cae y rebota en la plataforma
      rdq_marcarBola();
      rdq.by = pgm_read_byte(&RDQ_CAIDA[rdq.t]);
      rdq.t++;
      if (rdq.t >= 5) { rdq.modo = 5; rdq.t = 0; }
      rdq_marcarBola();
      break;
    case 5:                                                                      // se bambolea tres veces
      rdq_marcarBola();
      rdq.bx = rdq.x1 + (int8_t)pgm_read_byte(&RDQ_BAMBOLEO[rdq.t]);
      rdq.t++;
      if (rdq.t >= 8) { rdq.bx = rdq.x1; rdq.modo = 6; rdq.t = 0; }
      rdq_marcarBola();
      break;
    case 6:                                                                      // "clic": estrella
      rdq_marcarBola();
      rdq.t++;
      if (rdq.t >= 3) { rdq.modo = 7; rdq.t = 0; }
      break;
    case 7: {                                                                    // se abre y suelta el dígito nuevo
      if (rdq.t == 0) {
        int o = rdq.objetivo;
        rdq.d[o] = rdq.obj[o];
        rdq.ds[o] = 3; rdq.dt[o] = 4;
        rdq.pend &= ~(1 << o);
        rdq.brillo = 4;
        rdq_marcarDigito(o); rdq_marcarBola();
      }
      rdq.t++;
      if (rdq.t >= 4) {
        rdq.bola = false;
        rdq_marcarBola();
        int nxt = rdq_siguiente();
        if (nxt >= 0) { rdq.objetivo = nxt; rdq.modo = 1; rdq.t = 0; rdq.pose = 1; }
        else { rdq.modo = 0; rdq.pose = 0; rdq.objetivo = -1; }
        rdq_marcarEntrenador();
      }
    } break;
  }
}

static void rdq_paso(uint32_t ms) {
  if (!rdq.ultOk) { rdq.ult = ms; rdq.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdq.ult) >= RDM_TICK_MS && n < 3) { rdq.ult += RDM_TICK_MS; rdq_tick(); n++; }
  if (n == 3) rdq.ult = ms;
}

static uint16_t rdq_color(uint8_t c) {
  switch (c) {
    case 1:  return RD_C(220, 40, 40);
    case 2:  return RD_C(245, 245, 245);
    case 3:  return RD_C(245, 190, 140);
    case 4:  return RD_C(30, 24, 30);
    case 5:  return RD_C(50, 100, 220);
    case 7:  return RD_C(50, 60, 120);
    case 8:  return RD_C(200, 40, 40);
    case 9:  return RD_C(250, 210, 60);
    case 10: return RD_C(20, 16, 24);
  }
  return 0;
}

// Poké Ball de 7x7 centrada en (cx, cy)
static uint16_t rdq_bola(int x, int y, int cx, int cy) {
  int dx = x - cx, dy = y - cy;
  int d2 = dx * dx + dy * dy;
  if (d2 > 12) return 0;
  if (d2 >= 10) return RD_C(24, 24, 30);
  if (dy == 0) return (dx == 0) ? RD_C(250, 250, 250) : RD_C(24, 24, 30);
  if (dy < 0) return (dx == -1 && dy == -2) ? RD_C(255, 170, 160) : RD_C(230, 40, 40);
  return (dy == 2) ? RD_C(200, 200, 210) : RD_C(245, 245, 245);
}

static bool rdq_estrella(int dx, int dy, int r) {
  int ax = dx < 0 ? -dx : dx, ay = dy < 0 ? -dy : dy;
  int d = (ax > ay ? ax : ay) + ((ax > ay ? ay : ax) >> 1);
  if (d > r) return false;
  return ax <= 1 || ay <= 1 || (ax - ay <= 1 && ay - ax <= 1);
}

static uint16_t rdq_fondo(int x, int y) {
  if (y >= 24) {                                                             // pradera
    uint16_t h = rdq_hash(x, y, 4);
    if ((h % 11) == 0) return RD_C(50, 130, 45);
    if ((h % 17) == 0) return RD_C(130, 215, 100);
    return (y >= 28) ? RD_C(64, 156, 54) : RD_C(88, 184, 68);
  }
  int cima = 22 - (int)pgm_read_byte(&RDQ_COLINA[(x >> 2) & 15]);
  if (y >= cima) return RD_C(110, 185, 100);                                 // colinas lejanas
  for (int i = 0; i < 2; i++) {
    int lx = x - rdq.nube[i], ly = y - RDQ_NUBE_Y[i];
    if ((unsigned)lx < 14 && (unsigned)ly < 5) {
      char ch = (char)pgm_read_byte(&RDM_NUBE[ly][lx]);
      if (ch == 'W') return RD_C(252, 252, 252);
      if (ch == 'G') return RD_C(205, 225, 250);
    }
  }
  return RD_C(90 + y * 3, 170 + y * 2, 245 - y);                             // cielo
}

static uint16_t rdq_capas(int x, int y) {
  // 1) destello y haz rojo alrededor de la bola
  if (rdq.bola) {
    int dx = x - rdq.bx, dy = y - rdq.by, d2 = dx * dx + dy * dy;
    if (rdq.modo == 3) {                                                     // haz rojo y blanco que absorbe
      int r = 3 + rdq.t * 3;
      if (d2 <= r * r && d2 >= (r - 3) * (r - 3) && ((x + y + (int)rdq.n) & 1) == 0) return RD_C(255, 90, 90);
      if (d2 <= (r - 3) * (r - 3) && d2 > 12 && ((x + y) & 1) == 0) return RD_C(255, 255, 255);
    }
    if (rdq.modo == 6 && rdq_estrella(dx, dy, 3 + rdq.t * 2) && d2 > 12) return RD_C(255, 240, 120);   // clic
  }
  if (rdq.brillo) {
    int dx = x - rdq.x1, dy = y - RDQ_BOLA_Y, d2 = dx * dx + dy * dy;
    int r = 3 + (4 - rdq.brillo) * 2;
    if (d2 <= r * r && d2 > 12 && (rdq_hash(x, y, (int)rdq.n) & 3) != 0) return RD_C(255, 255, 255);
  }
  // 2) bola
  if (rdq.bola && rdq.brillo == 0) {
    uint16_t c = rdq_bola(x, y, rdq.bx, rdq.by);
    if (c) return c;
  }
  // 3) entrenador
  {
    int lx = x - RDQ_BX, ly = y - (RDQ_BY + (rdq.pose == 0 ? rdq.bob : 0));
    if ((unsigned)lx < 14 && (unsigned)ly < 18) {
      uint8_t c = (rdq.pose == 1) ? rdRectColor(RDQ_ATRAS,  RDQ_N(RDQ_ATRAS),  lx, ly)
                : (rdq.pose == 2) ? rdRectColor(RDQ_LANZA,  RDQ_N(RDQ_LANZA),  lx, ly)
                                  : rdRectColor(RDQ_REPOSO, RDQ_N(RDQ_REPOSO), lx, ly);
      if (c) return rdq_color(c);
    }
  }
  // 4) caja de estado: vida (segundos) y experiencia (minutos)
  if (x <= 33 && y <= 8) {
    if (x == 0 || x == 33 || y == 0 || y == 8) return RD_C(30, 30, 40);
    if (y >= 2 && y <= 3 && x >= 3 && x <= 30) {
      int len = ((60 - rdq.seg) * 28) / 60; if (len < 1) len = 1;
      if (x - 3 < len) { int f = (60 - rdq.seg); return f > 30 ? RD_C(60, 220, 70) : f > 12 ? RD_C(240, 200, 40) : RD_C(235, 50, 40); }
      return RD_C(70, 70, 70);
    }
    if (y >= 5 && y <= 6 && x >= 3 && x <= 30) return (x - 3 < (rdq.minu * 28) / 60) ? RD_C(70, 130, 240) : RD_C(170, 170, 185);
    return RD_C(238, 236, 215);
  }
  // 5) hora sobre las plataformas
  if (y >= RDQ_DIG_Y && y <= RDQ_DIG_Y + 14 && x >= 37 && x <= 96) {
    for (int i = 0; i < 4; i++) {
      int lx = x - RDQ_DIG_X[i], ly = y - RDQ_DIG_Y;
      if ((unsigned)lx > 10) continue;
      uint8_t ds = rdq.ds[i];
      if (ds == 2) break;                                                    // capturado: no hay dígito
      bool tinta  = rdDigito2(rdq.d[i], lx, ly);
      bool borde  = !tinta && (rdDigito2(rdq.d[i], lx - 1, ly) || rdDigito2(rdq.d[i], lx, ly - 1) || rdDigito2(rdq.d[i], lx - 1, ly - 1) ||
                               rdDigito2(rdq.d[i], lx + 1, ly) || rdDigito2(rdq.d[i], lx, ly + 1));
      if (ds == 1) {                                                         // se disuelve en rojo hacia la bola
        static const uint8_t DENS[4] = { 0, 90, 170, 240 };
        if (tinta && (rdq_hash(x, y, rdq.dt[i]) & 0xFF) < DENS[rdq.dt[i] > 3 ? 3 : rdq.dt[i]])
          return ((x + y) & 1) ? RD_C(255, 70, 70) : RD_C(255, 255, 255);
        break;
      }
      if (ds == 3) {                                                         // se materializa: silueta blanca que se asienta
        if (tinta || (borde && rdq.dt[i] >= 3)) return RD_C(255, 255, 255);
        if (borde) return RD_C(255, 230, 230);
        break;
      }
      if (tinta) return RD_C(255, 255, 255);
      if (borde) return RD_C(24, 40, 100);
    }
    if (rdq.dp && x >= 65 && x <= 66 && ((y >= 8 && y <= 9) || (y >= 14 && y <= 15))) return RD_C(255, 255, 255);
  }
  // 6) plataformas de combate bajo cada dígito
  for (int i = 0; i < 4; i++) {
    int dx = x - (RDQ_DIG_X[i] + 5), dy = y - 21;
    if (dx * dx * 4 + dy * dy * 49 <= 196) return (dy > 0) ? RD_C(150, 140, 98) : RD_C(205, 195, 145);
  }
  // 7) árbol y hierba alta a la derecha
  {
    int lx = x - 108, ly = y - 8;
    if ((unsigned)lx < 14 && (unsigned)ly < 16) {
      int dx = lx - 7, dy = ly - 4;
      if (dx * dx + dy * dy <= 22) return (dx + dy < 0) ? RD_C(70, 170, 70) : RD_C(40, 120, 50);
      if (lx >= 6 && lx <= 8 && ly >= 8) return RD_C(120, 80, 45);
    }
  }
  if (y >= 21 && y <= 27 && x >= 98 && x <= 126) {
    int alto = 3 + (x * 7) % 3;
    int fr = (rdq.n >> 3) & 1;
    if (y >= 27 - alto && ((x + fr) % 3) != 1) return (y == 27 - alto) ? RD_C(120, 215, 90) : RD_C(50, 140, 50);
  }
  return rdq_fondo(x, y);
}

static uint16_t rdq_pixel(int x, int y) { return rdq_capas(x, y); }

#endif // RD_ANIME_H
