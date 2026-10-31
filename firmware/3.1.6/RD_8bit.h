// ====================================================================
//  RD_8bit.h - caras 8 bit
//  No se incluye directamente: lo hace RelojDinamico.h.
// ====================================================================
#ifndef RD_8BIT_H
#define RD_8BIT_H

#ifndef RELOJ_DINAMICO_H
#error "Incluye RelojDinamico.h, no este fichero"
#endif

// ────────────────────────────────────────────────────────────────────
//  "Mario"
// ────────────────────────────────────────────────────────────────────

struct RDMEstado {
  int8_t   d[4];        // dígitos que se ven ahora
  int8_t   obj[4];      // dígitos que toca mostrar
  uint8_t  pend;        // máscara de bloques por golpear
  uint8_t  modo;        // 0 pasea, 1 va al bloque, 2 salta
  int8_t   bloque;      // bloque objetivo (-1 = elegir)
  int8_t   dir;         // +1 derecha, -1 izquierda
  int8_t   rise;        // elevación actual del salto
  uint8_t  salto;       // tick dentro del salto
  uint8_t  anim;        // 0/1: fotograma de la zancada
  uint8_t  cielo;       // 0 día, 1 atardecer, 2 noche
  int8_t   bump[4];     // ticks que le quedan al "golpe" de cada bloque
  int16_t  x;           // x del sprite
  int16_t  nube[2];     // x de las nubes
  bool     dp;          // dos puntos visibles
  uint32_t n;           // contador de ticks
  uint32_t ult;         // ms del último tick
  bool     ultOk;
};
static RDMEstado rdm;

static void rdm_marcarMario() { rdMarcar(rdm.x, RDM_Y0 - rdm.rise, 11, 13); }
static void rdm_marcarBloque(int i) { rdMarcar(RDM_BLK_X[i], 0, 12, 13); }
static void rdm_marcarNube(int i) { rdMarcar(rdm.nube[i], RDM_NUBE_Y[i], 14, 5); }

static void rdm_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdm.d);
  for (int i = 0; i < 4; i++) { rdm.obj[i] = rdm.d[i]; rdm.bump[i] = 0; }
  rdm.pend = 0; rdm.modo = 0; rdm.bloque = -1; rdm.dir = 1;
  rdm.rise = 0; rdm.salto = 0; rdm.anim = 0; rdm.n = 0;
  rdm.x = 24 + (m % 40);                 // no siempre arranca en el mismo sitio
  rdm.nube[0] = 8; rdm.nube[1] = 96;
  rdm.dp = (s % 2 == 0);
  rdm.cielo = rdm_cieloPorHora(h);
  rdm.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdm_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  uint8_t mask = 0;
  for (int i = 0; i < 4; i++) { rdm.obj[i] = nd[i]; if (nd[i] != rdm.d[i]) mask |= (1 << i); }
  uint8_t c = rdm_cieloPorHora(h);
  if (c != rdm.cielo) { rdm.cielo = c; rdMarcar(0, 0, RD_W, RD_H); }
  rdm.pend = mask;
  if (mask && rdm.modo == 0) { rdm.modo = 1; rdm.bloque = -1; }
}

static void rdm_segundo(int s) {
  rdm.dp = (s % 2 == 0);
  rdMarcar(64, 3, 2, 8);
}

static void rdm_elegirBloque() {
  int mejor = -1, dmin = 9999;
  for (int i = 0; i < 4; i++) {
    if (!(rdm.pend & (1 << i))) continue;
    int dist = abs((RDM_BLK_X[i] + 1) - rdm.x);
    if (dist < dmin) { dmin = dist; mejor = i; }
  }
  rdm.bloque = mejor;
}

static void rdm_golpe(int b) {
  rdm.d[b] = rdm.obj[b];
  rdm.bump[b] = 3;
  rdm.pend &= ~(1 << b);
  rdm_marcarBloque(b);
}

static void rdm_tick() {
  rdm_marcarMario();
  rdm.n++;

  for (int i = 0; i < 4; i++)                       // bloques golpeados vuelven a su sitio
    if (rdm.bump[i] > 0) { rdm.bump[i]--; rdm_marcarBloque(i); }

  if ((rdm.n % 12) == 0) { rdm_marcarNube(0); if (--rdm.nube[0] < -14) rdm.nube[0] = RD_W; rdm_marcarNube(0); }
  if ((rdm.n % 20) == 0) { rdm_marcarNube(1); if (--rdm.nube[1] < -14) rdm.nube[1] = RD_W; rdm_marcarNube(1); }

  switch (rdm.modo) {
    case 0:                                         // paseo tranquilo
      if ((rdm.n & 1) == 0) {
        rdm.x += rdm.dir;
        if (rdm.x >= RDM_XMAX) rdm.dir = -1;
        if (rdm.x <= RDM_XMIN) rdm.dir = 1;
      }
      if ((rdm.n % 3) == 0) rdm.anim ^= 1;
      break;

    case 1: {                                       // corre hasta el bloque
      if (rdm.bloque < 0) rdm_elegirBloque();
      if (rdm.bloque < 0) { rdm.modo = 0; break; }
      int objetivo = RDM_BLK_X[rdm.bloque] + 1;
      if (rdm.x < objetivo)      { rdm.x += (objetivo - rdm.x >= 2) ? 2 : 1; rdm.dir = 1; }
      else if (rdm.x > objetivo) { rdm.x -= (rdm.x - objetivo >= 2) ? 2 : 1; rdm.dir = -1; }
      if ((rdm.n % 2) == 0) rdm.anim ^= 1;
      if (rdm.x == objetivo) { rdm.modo = 2; rdm.salto = 0; }
    } break;

    case 2:                                         // salto y golpe
      rdm.rise = RDM_SALTO[rdm.salto];
      if (rdm.salto == RDM_SALTO_GOLPE) rdm_golpe(rdm.bloque);
      rdm.salto++;
      if (rdm.salto >= RDM_SALTO_N) {
        rdm.rise = 0;
        rdm.bloque = -1;
        rdm.modo = rdm.pend ? 1 : 0;
      }
      break;
  }
  rdm_marcarMario();
}

static void rdm_paso(uint32_t ms) {
  if (!rdm.ultOk) { rdm.ult = ms; rdm.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdm.ult) >= RDM_TICK_MS && n < 3) { rdm.ult += RDM_TICK_MS; rdm_tick(); n++; }
  if (n == 3) rdm.ult = ms;                         // si vamos con retraso, no acumular
}

static uint16_t rdm_pixel(int x, int y) {
  // 1) Mario
  {
    int lx = x - rdm.x, ly = y - (RDM_Y0 - rdm.rise);
    if ((unsigned)lx < 11 && (unsigned)ly < 13) {
      const char (*spr)[12] = (rdm.modo == 2) ? RDM_JUMP : (rdm.anim ? RDM_RUN1 : RDM_STAND);
      char ch = (char)pgm_read_byte(&spr[ly][(rdm.dir < 0) ? 10 - lx : lx]);
      if (ch != '.') return rdm_colorSprite(ch);
    }
  }
  // 2) Suelo (nada lo tapa salvo el Mario)
  if (y >= RDM_SUELO) {
    bool mortero = (y == RDM_SUELO) ? ((x & 7) == 0) : (y == RDM_SUELO + 1) ? ((x & 7) == 4) : ((x & 7) == 0);
    if (mortero) return RD_C(80, 30, 0);
    return (y == RDM_SUELO) ? RD_C(252, 160, 68) : RD_C(200, 76, 12);
  }
  // 3) Bloques con dígito
  if (y <= 12) {
    for (int i = 0; i < 4; i++) {
      int lx = x - RDM_BLK_X[i];
      if ((unsigned)lx >= 12) continue;
      int ly = y - (RDM_BLK_Y - (rdm.bump[i] > 0 ? 1 : 0));
      if ((unsigned)ly >= 12) continue;
      if (lx == 0 || lx == 11 || ly == 0 || ly == 11) return RD_C(176, 96, 0);
      if ((lx == 1 || lx == 10) && (ly == 1 || ly == 10)) return RD_C(104, 48, 0);
      int dx = lx - 3, dy = ly - 2;
      if ((unsigned)dx < 5 && (unsigned)dy < 7 &&
          (pgm_read_byte(&RDM_FUENTE[rdm.d[i]][dy]) & (0x10 >> dx)))
        return RD_C(88, 32, 0);
      return RD_C(252, 188, 32);
    }
    // 4) Dos puntos
    if (rdm.dp && (unsigned)(x - 64) < 2 && ((unsigned)(y - 3) < 2 || (unsigned)(y - 9) < 2))
      return RD_C(252, 252, 252);
  }
  // 5) Nubes (detrás de los bloques)
  for (int i = 0; i < 2; i++) {
    int lx = x - rdm.nube[i], ly = y - RDM_NUBE_Y[i];
    if ((unsigned)lx < 14 && (unsigned)ly < 5) {
      char ch = (char)pgm_read_byte(&RDM_NUBE[ly][lx]);
      if (ch == 'W') return rdm.cielo == 0 ? RD_C(252, 252, 252) : rdm.cielo == 1 ? RD_C(161, 161, 181) : RD_C(70, 70, 110);
      if (ch == 'G') return rdm.cielo == 0 ? RD_C(188, 208, 252) : rdm.cielo == 1 ? RD_C(118, 128, 168) : RD_C(48, 48, 84);
    }
  }
  // 6) Arbusto
  {
    int lx = x - 104, ly = y - 24;
    if ((unsigned)lx < 16 && (unsigned)ly < 5) {
      char ch = (char)pgm_read_byte(&RDM_ARBUSTO[ly][lx]);
      if (ch == 'L') return RD_C(60, 200, 60);
      if (ch == 'D') return RD_C(0, 140, 0);
    }
  }
  // 7) Colina
  {
    int dy = y - 21;
    if (dy >= 0 && dy < 8) {
      int half = 2 + dy * 2;
      if (x >= 16 - half && x < 16 + half) return RD_C(16, 120, 48);
    }
  }
  // 8) Cielo
  return rdm.cielo == 0 ? RD_C(92, 148, 252) : rdm.cielo == 1 ? RD_C(49, 77, 146) : RD_C(6, 6, 40);
}

// ────────────────────────────────────────────────────────────────────
//  "Luigi"
// ────────────────────────────────────────────────────────────────────
#define RDL_TICK_MS   40       
#define RDL_SUELO     29       
#define RDL_Y0        16       
#define RDL_BLK_Y     1        
#define RDL_XMIN      3
#define RDL_XMAX      114      
static const int16_t RDL_BLK_X[4] = { 35, 49, 69, 83 };   
static const int8_t  RDL_SALTO[]  = { 1, 2, 3, 4, 4, 3, 2, 1, 0 };   
#define RDL_SALTO_N   ((int)sizeof(RDL_SALTO))
#define RDL_SALTO_GOLPE 3      

// Sprites 11x13 de Luigi (cara hacia la derecha). G=Verde, U=Azul, S=Piel, K=Negro
static const char RDL_STAND[13][12] PROGMEM = {
  "...GGGGG...", "..GGGGGGGGG", "..BBBSSKS..", ".BSBSSSKSSS", ".BSBBSSSKSS",
  ".GGSSSSKKK.", "...SSSSSS..", "..GGUUUGG..", ".GGGUUUGGG.", ".SSUUYUUSS.",
  "..UUUUUUU..", "..UUU.UUU..", ".BBBB.BBBB."
};
static const char RDL_RUN1[13][12] PROGMEM = {
  "...GGGGG...", "..GGGGGGGGG", "..BBBSSKS..", ".BSBSSSKSSS", ".BSBBSSSKSS",
  ".GGSSSSKKK.", "...SSSSSS..", "..GGUUUGG..", ".GGGUUUGGG.", "..SUUYUUS..",
  "..UUUUUUU..", ".UUU...UUU.", "BBBB...BBBB"
};
static const char RDL_JUMP[13][12] PROGMEM = {
  "...GGGGG...", "..GGGGGGGGG", "..BBBSSKS..", ".BSBSSSKSSS", ".BSBBSSSKSS",
  ".GGSSSSKKK.", "...SSSSSS..", ".SGGUUUGGS.", ".SGGUUUGGS.", "..UUUYUUU..",
  "..UUUUUUU..", ".UUUU.UUUU.", ".BBB...BBB."
};

struct RDLEstado {
  int8_t   d[4];        
  int8_t   obj[4];      
  uint8_t  pend;        
  uint8_t  modo;        
  int8_t   bloque;      
  int8_t   dir;         
  int8_t   rise;        
  uint8_t  salto;       
  uint8_t  anim;        
  uint8_t  cielo;       
  int8_t   bump[4];     
  int16_t  x;           
  int16_t  nube[2];     
  bool     dp;          
  uint32_t n;           
  uint32_t ult;         
  bool     ultOk;
};
static RDLEstado rdl;
static const int8_t RDL_NUBE_Y[2] = { 2, 9 };

static void rdl_digitos(int h, int m, int8_t* o) { o[0] = h / 10; o[1] = h % 10; o[2] = m / 10; o[3] = m % 10; }

static void rdl_marcarLuigi() { rdMarcar(rdl.x, RDL_Y0 - rdl.rise, 11, 13); }
static void rdl_marcarBloque(int i) { rdMarcar(RDL_BLK_X[i], 0, 12, 13); }
static void rdl_marcarNube(int i) { rdMarcar(rdl.nube[i], RDL_NUBE_Y[i], 14, 5); }

static void rdl_iniciar(int h, int m, int s) {
  rdl_digitos(h, m, rdl.d);
  for (int i = 0; i < 4; i++) { rdl.obj[i] = rdl.d[i]; rdl.bump[i] = 0; }
  rdl.pend = 0; rdl.modo = 0; rdl.bloque = -1; rdl.dir = 1;
  rdl.rise = 0; rdl.salto = 0; rdl.anim = 0; rdl.n = 0;
  rdl.x = 24 + (m % 40);                 
  rdl.nube[0] = 8; rdl.nube[1] = 96;
  rdl.dp = (s % 2 == 0);
  rdl.cielo = rdm_cieloPorHora(h);
  rdl.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdl_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdl_digitos(h, m, nd);
  uint8_t mask = 0;
  for (int i = 0; i < 4; i++) { rdl.obj[i] = nd[i]; if (nd[i] != rdl.d[i]) mask |= (1 << i); }
  uint8_t c = rdm_cieloPorHora(h);
  if (c != rdl.cielo) { rdl.cielo = c; rdMarcar(0, 0, RD_W, RD_H); }
  rdl.pend = mask;
  if (mask && rdl.modo == 0) { rdl.modo = 1; rdl.bloque = -1; }
}

static void rdl_segundo(int s) {
  rdl.dp = (s % 2 == 0);
  rdMarcar(64, 3, 2, 8);
}

static void rdl_elegirBloque() {
  int mejor = -1, dmin = 9999;
  for (int i = 0; i < 4; i++) {
    if (!(rdl.pend & (1 << i))) continue;
    int dist = abs((RDL_BLK_X[i] + 1) - rdl.x);
    if (dist < dmin) { dmin = dist; mejor = i; }
  }
  rdl.bloque = mejor;
}

static void rdl_golpe(int b) {
  rdl.d[b] = rdl.obj[b];
  rdl.bump[b] = 3;
  rdl.pend &= ~(1 << b);
  rdl_marcarBloque(b);
}

static void rdl_tick() {
  rdl_marcarLuigi();
  rdl.n++;

  for (int i = 0; i < 4; i++)                       
    if (rdl.bump[i] > 0) { rdl.bump[i]--; rdl_marcarBloque(i); }

  if ((rdl.n % 12) == 0) { rdl_marcarNube(0); if (--rdl.nube[0] < -14) rdl.nube[0] = RD_W; rdl_marcarNube(0); }
  if ((rdl.n % 20) == 0) { rdl_marcarNube(1); if (--rdl.nube[1] < -14) rdl.nube[1] = RD_W; rdl_marcarNube(1); }

  switch (rdl.modo) {
    case 0:                                         
      if ((rdl.n & 1) == 0) {
        rdl.x += rdl.dir;
        if (rdl.x >= RDL_XMAX) rdl.dir = -1;
        if (rdl.x <= RDL_XMIN) rdl.dir = 1;
      }
      if ((rdl.n % 3) == 0) rdl.anim ^= 1;
      break;

    case 1: {                                       
      if (rdl.bloque < 0) rdl_elegirBloque();
      if (rdl.bloque < 0) { rdl.modo = 0; break; }
      int objetivo = RDL_BLK_X[rdl.bloque] + 1;
      if (rdl.x < objetivo)      { rdl.x += (objetivo - rdl.x >= 2) ? 2 : 1; rdl.dir = 1; }
      else if (rdl.x > objetivo) { rdl.x -= (rdl.x - objetivo >= 2) ? 2 : 1; rdl.dir = -1; }
      if ((rdl.n % 2) == 0) rdl.anim ^= 1;
      if (rdl.x == objetivo) { rdl.modo = 2; rdl.salto = 0; }
    } break;

    case 2:                                         
      rdl.rise = RDL_SALTO[rdl.salto];
      if (rdl.salto == RDL_SALTO_GOLPE) rdl_golpe(rdl.bloque);
      rdl.salto++;
      if (rdl.salto >= RDL_SALTO_N) {
        rdl.rise = 0;
        rdl.bloque = -1;
        rdl.modo = rdl.pend ? 1 : 0;
      }
      break;
  }
  rdl_marcarLuigi();
}

static void rdl_paso(uint32_t ms) {
  if (!rdl.ultOk) { rdl.ult = ms; rdl.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdl.ult) >= RDL_TICK_MS && n < 3) { rdl.ult += RDL_TICK_MS; rdl_tick(); n++; }
  if (n == 3) rdl.ult = ms;                         
}

static uint16_t rdl_colorSprite(char ch) {
  switch (ch) {
    case 'G': return RD_C(32, 192, 64);   // Verde Luigi
    case 'B': return RD_C(120, 64, 0);    // Marrón zapatos
    case 'S': return RD_C(252, 184, 120); // Piel
    case 'K': return RD_C(24, 16, 16);    // Negro
    case 'U': return RD_C(40, 80, 216);   // Azul peto
    case 'Y': return RD_C(252, 216, 0);   // Amarillo botones
  }
  return 0;
}

static uint16_t rdl_pixel(int x, int y) {
  // 1) Luigi
  {
    int lx = x - rdl.x, ly = y - (RDL_Y0 - rdl.rise);
    if ((unsigned)lx < 11 && (unsigned)ly < 13) {
      const char (*spr)[12] = (rdl.modo == 2) ? RDL_JUMP : (rdl.anim ? RDL_RUN1 : RDL_STAND);
      char ch = (char)pgm_read_byte(&spr[ly][(rdl.dir < 0) ? 10 - lx : lx]);
      if (ch != '.') return rdl_colorSprite(ch);
    }
  }
  // 2) Suelo
  if (y >= RDL_SUELO) {
    bool mortero = (y == RDL_SUELO) ? ((x & 7) == 0) : (y == RDL_SUELO + 1) ? ((x & 7) == 4) : ((x & 7) == 0);
    if (mortero) return RD_C(80, 30, 0);
    return (y == RDL_SUELO) ? RD_C(252, 160, 68) : RD_C(200, 76, 12);
  }
  // 3) Bloques con dígito
  if (y <= 12) {
    for (int i = 0; i < 4; i++) {
      int lx = x - RDL_BLK_X[i];
      if ((unsigned)lx >= 12) continue;
      int ly = y - (RDL_BLK_Y - (rdl.bump[i] > 0 ? 1 : 0));
      if ((unsigned)ly >= 12) continue;
      if (lx == 0 || lx == 11 || ly == 0 || ly == 11) return RD_C(176, 96, 0);
      if ((lx == 1 || lx == 10) && (ly == 1 || ly == 10)) return RD_C(104, 48, 0);
      int dx = lx - 3, dy = ly - 2;
      if ((unsigned)dx < 5 && (unsigned)dy < 7 &&
          (pgm_read_byte(&RDM_FUENTE[rdl.d[i]][dy]) & (0x10 >> dx)))
        return RD_C(88, 32, 0);
      return RD_C(252, 188, 32);
    }
    // 4) Dos puntos
    if (rdl.dp && (unsigned)(x - 64) < 2 && ((unsigned)(y - 3) < 2 || (unsigned)(y - 9) < 2))
      return RD_C(252, 252, 252);
  }
  // 5) Nubes
  for (int i = 0; i < 2; i++) {
    int lx = x - rdl.nube[i], ly = y - RDL_NUBE_Y[i];
    if ((unsigned)lx < 14 && (unsigned)ly < 5) {
      char ch = (char)pgm_read_byte(&RDM_NUBE[ly][lx]);
      if (ch == 'W') return rdl.cielo == 0 ? RD_C(252, 252, 252) : rdl.cielo == 1 ? RD_C(161, 161, 181) : RD_C(70, 70, 110);
      if (ch == 'G') return rdl.cielo == 0 ? RD_C(188, 208, 252) : rdl.cielo == 1 ? RD_C(118, 128, 168) : RD_C(48, 48, 84);
    }
  }
  // 6) Arbusto
  {
    int lx = x - 104, ly = y - 24;
    if ((unsigned)lx < 16 && (unsigned)ly < 5) {
      char ch = (char)pgm_read_byte(&RDM_ARBUSTO[ly][lx]);
      if (ch == 'L') return RD_C(60, 200, 60);
      if (ch == 'D') return RD_C(0, 140, 0);
    }
  }
  // 7) Colina
  {
    int dy = y - 21;
    if (dy >= 0 && dy < 8) {
      int half = 2 + dy * 2;
      if (x >= 16 - half && x < 16 + half) return RD_C(16, 120, 48);
    }
  }
  // 8) Cielo
  return rdl.cielo == 0 ? RD_C(92, 148, 252) : rdl.cielo == 1 ? RD_C(49, 77, 146) : RD_C(6, 6, 40);
}

// ────────────────────────────────────────────────────────────────────
// "Peach"
// ────────────────────────────────────────────────────────────────────
static const int8_t RDPE_SALTO[] = { 1, 2, 3, 4, 4, 4, 4, 3, 2, 1, 0 };   // elevación por tick (con flotación)
#define RDPE_SALTO_N     ((int)sizeof(RDPE_SALTO))
#define RDPE_SALTO_GOLPE 3      // tick del salto en el que se golpea (primer 4)

// Sprites 11x13 (cara hacia la derecha). '.' = transparente
// G corona, R gema/labios, Y pelo, S piel, E ojo, W guantes, P vestido, p vestido oscuro, U broche
static const char RDPE_STAND[13][12] PROGMEM = {
  "....G.G.G..", "...GGRGGG..", "..YYYYYYY..", ".YYYSSSSS..", ".YYSSSESS..",
  ".YYYSSRSS..", ".YY..SSS...", "..WPPUPPW..", ".WPPPPPPPW.", "..PPPPPPP..",
  ".PPPPPPPPP.", "PPPPPPPPPPP", "pPpPpPpPpPp"
};
static const char RDPE_RUN[13][12] PROGMEM = {
  "....G.G.G..", "...GGRGGG..", "..YYYYYYY..", ".YYYSSSSS..", ".YYSSSESS..",
  ".YYYSSRSS..", ".YY..SSS...", "..WPPUPPW..", ".WPPPPPPPW.", "..PPPPPPP..",
  "..PPPPPPPP.", ".PPPPPPPPPP", "pPpPpPpPpP."
};
static const char RDPE_JUMP0[13][12] PROGMEM = {
  "....G.G.G..", "...GGRGGG..", "..YYYYYYY..", ".YYYSSSSS..", ".YYSSSESS..",
  ".YYYSSRSS..", ".YY..SSS...", ".WWPPUPPWW.", ".WPPPPPPPW.", ".PPPPPPPPP.",
  "PPPPPPPPPPP", "PPPPPPPPPPP", "pPpPpPpPpPp"
};
static const char RDPE_JUMP1[13][12] PROGMEM = {
  "....G.G.G..", "...GGRGGG..", "..YYYYYYY..", ".YYYSSSSS..", ".YYSSSESS..",
  ".YYYSSRSS..", ".YY..SSS...", ".WWPPUPPWW.", ".WPPPPPPPW.", ".PPPPPPPPP.",
  "PPPPPPPPPPP", "PPPPPPPPPPP", "PpPpPpPpPpP"
};

struct RDPEEstado {
  int8_t   d[4];        // dígitos que se ven ahora
  int8_t   obj[4];      // dígitos que toca mostrar
  uint8_t  pend;        // máscara de bloques por golpear
  uint8_t  modo;        // 0 pasea, 1 va al bloque, 2 salta
  int8_t   bloque;      // bloque objetivo (-1 = elegir)
  int8_t   dir;         // +1 derecha, -1 izquierda
  int8_t   rise;        // elevación actual del salto
  uint8_t  salto;       // tick dentro del salto
  uint8_t  anim;        // 0/1: fotograma de la zancada
  uint8_t  cielo;       // 0 día, 1 atardecer, 2 noche
  int8_t   bump[4];     // ticks que le quedan al "golpe" de cada bloque
  uint8_t  star[4];     // ticks que le quedan a los destellos de cada bloque
  int16_t  x;           // x del sprite
  int16_t  nube[2];     // x de las nubes
  bool     dp;          // dos puntos visibles
  uint32_t n, ult;
  bool     ultOk;
};
static RDPEEstado rdpe;

static void rdpe_marcarPeach()   { rdMarcar(rdpe.x, RDM_Y0 - rdpe.rise, 11, 13); }
static void rdpe_marcarBloque(int i) { rdMarcar(RDM_BLK_X[i], 0, 12, 13); }
static void rdpe_marcarNube(int i)   { rdMarcar(rdpe.nube[i], RDM_NUBE_Y[i], 14, 5); }

static void rdpe_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdpe.d);
  for (int i = 0; i < 4; i++) { rdpe.obj[i] = rdpe.d[i]; rdpe.bump[i] = 0; rdpe.star[i] = 0; }
  rdpe.pend = 0; rdpe.modo = 0; rdpe.bloque = -1; rdpe.dir = 1;
  rdpe.rise = 0; rdpe.salto = 0; rdpe.anim = 0; rdpe.n = 0;
  rdpe.x = 24 + (m % 40);                 // no siempre arranca en el mismo sitio
  rdpe.nube[0] = 8; rdpe.nube[1] = 96;
  rdpe.dp = (s % 2 == 0);
  rdpe.cielo = rdm_cieloPorHora(h);
  rdpe.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdpe_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  uint8_t mask = 0;
  for (int i = 0; i < 4; i++) { rdpe.obj[i] = nd[i]; if (nd[i] != rdpe.d[i]) mask |= (1 << i); }
  uint8_t c = rdm_cieloPorHora(h);
  if (c != rdpe.cielo) { rdpe.cielo = c; rdMarcar(0, 0, RD_W, RD_H); }
  rdpe.pend = mask;
  if (mask && rdpe.modo == 0) { rdpe.modo = 1; rdpe.bloque = -1; }
}

static void rdpe_segundo(int s) {
  rdpe.dp = (s % 2 == 0);
  rdMarcar(64, 3, 2, 8);
}

static void rdpe_elegirBloque() {
  int mejor = -1, dmin = 9999;
  for (int i = 0; i < 4; i++) {
    if (!(rdpe.pend & (1 << i))) continue;
    int dist = abs((RDM_BLK_X[i] + 1) - rdpe.x);
    if (dist < dmin) { dmin = dist; mejor = i; }
  }
  rdpe.bloque = mejor;
}

static void rdpe_golpe(int b) {
  rdpe.d[b] = rdpe.obj[b];
  rdpe.bump[b] = 3;
  rdpe.star[b] = 6;
  rdpe.pend &= ~(1 << b);
  rdpe_marcarBloque(b);
}

static void rdpe_tick() {
  rdpe_marcarPeach();
  rdpe.n++;

  for (int i = 0; i < 4; i++) {                     // bloques golpeados vuelven a su sitio; los destellos se apagan
    if (rdpe.bump[i] > 0) { rdpe.bump[i]--; rdpe_marcarBloque(i); }
    if (rdpe.star[i] > 0) { rdpe.star[i]--; rdpe_marcarBloque(i); }
  }

  if ((rdpe.n % 12) == 0) { rdpe_marcarNube(0); if (--rdpe.nube[0] < -14) rdpe.nube[0] = RD_W; rdpe_marcarNube(0); }
  if ((rdpe.n % 20) == 0) { rdpe_marcarNube(1); if (--rdpe.nube[1] < -14) rdpe.nube[1] = RD_W; rdpe_marcarNube(1); }

  switch (rdpe.modo) {
    case 0:                                         // paseo tranquilo
      if ((rdpe.n & 1) == 0) {
        rdpe.x += rdpe.dir;
        if (rdpe.x >= RDM_XMAX) rdpe.dir = -1;
        if (rdpe.x <= RDM_XMIN) rdpe.dir = 1;
      }
      if ((rdpe.n % 3) == 0) rdpe.anim ^= 1;
      break;

    case 1: {                                       // corre hasta el bloque
      if (rdpe.bloque < 0) rdpe_elegirBloque();
      if (rdpe.bloque < 0) { rdpe.modo = 0; break; }
      int objetivo = RDM_BLK_X[rdpe.bloque] + 1;
      if (rdpe.x < objetivo)      { rdpe.x += (objetivo - rdpe.x >= 2) ? 2 : 1; rdpe.dir = 1; }
      else if (rdpe.x > objetivo) { rdpe.x -= (rdpe.x - objetivo >= 2) ? 2 : 1; rdpe.dir = -1; }
      if ((rdpe.n % 2) == 0) rdpe.anim ^= 1;
      if (rdpe.x == objetivo) { rdpe.modo = 2; rdpe.salto = 0; }
    } break;

    case 2:                                         // salto, flotación y golpe
      rdpe.rise = RDPE_SALTO[rdpe.salto];
      if (rdpe.salto == RDPE_SALTO_GOLPE) rdpe_golpe(rdpe.bloque);
      rdpe.salto++;
      if (rdpe.salto >= RDPE_SALTO_N) {
        rdpe.rise = 0;
        rdpe.bloque = -1;
        rdpe.modo = rdpe.pend ? 1 : 0;
      }
      break;
  }
  rdpe_marcarPeach();
}

static void rdpe_paso(uint32_t ms) {
  if (!rdpe.ultOk) { rdpe.ult = ms; rdpe.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdpe.ult) >= RDM_TICK_MS && n < 3) { rdpe.ult += RDM_TICK_MS; rdpe_tick(); n++; }
  if (n == 3) rdpe.ult = ms;                         // si vamos con retraso, no acumular
}

static uint16_t rdpe_colorSprite(char ch) {
  switch (ch) {
    case 'G': return RD_C(252, 200, 0);
    case 'R': return RD_C(216, 40, 60);
    case 'Y': return RD_C(252, 224, 96);
    case 'S': return RD_C(252, 200, 160);
    case 'E': return RD_C(40, 100, 220);
    case 'W': return RD_C(252, 252, 252);
    case 'P': return RD_C(255, 140, 200);
    case 'p': return RD_C(214, 80, 150);
    case 'U': return RD_C(60, 120, 255);
  }
  return 0;
}

static uint16_t rdpe_pixel(int x, int y) {
  // 1) Peach
  {
    int lx = x - rdpe.x, ly = y - (RDM_Y0 - rdpe.rise);
    if ((unsigned)lx < 11 && (unsigned)ly < 13) {
      const char (*spr)[12];
      if (rdpe.modo == 2) spr = ((rdpe.salto >= 4 && rdpe.salto <= 8) && (rdpe.salto & 1)) ? RDPE_JUMP1 : RDPE_JUMP0;   // el vestido ondea al flotar
      else spr = rdpe.anim ? RDPE_RUN : RDPE_STAND;
      char ch = (char)pgm_read_byte(&spr[ly][(rdpe.dir < 0) ? 10 - lx : lx]);
      if (ch != '.') return rdpe_colorSprite(ch);
    }
  }
  // 2) Suelo (nada lo tapa salvo Peach)
  if (y >= RDM_SUELO) {
    bool mortero = (y == RDM_SUELO) ? ((x & 7) == 0) : (y == RDM_SUELO + 1) ? ((x & 7) == 4) : ((x & 7) == 0);
    if (mortero) return RD_C(80, 30, 0);
    return (y == RDM_SUELO) ? RD_C(252, 160, 68) : RD_C(200, 76, 12);
  }
  // 3) Bloques con dígito (y destellos al ser golpeados)
  if (y <= 12) {
    for (int i = 0; i < 4; i++) {
      int lx = x - RDM_BLK_X[i];
      if ((unsigned)lx >= 12) continue;
      if (rdpe.star[i]) {                                          // dos estrellitas que se alternan
        int k = rdpe.star[i] & 1;
        int dx = lx - (k ? 9 : 2), dy = y - (k ? 8 : 3);
        if ((dx == 0 && abs(dy) <= 1) || (dy == 0 && abs(dx) <= 1))
          return (rdpe.star[i] & 2) ? RD_C(255, 255, 255) : RD_C(255, 160, 210);
      }
      int ly = y - (RDM_BLK_Y - (rdpe.bump[i] > 0 ? 1 : 0));
      if ((unsigned)ly >= 12) continue;
      if (lx == 0 || lx == 11 || ly == 0 || ly == 11) return RD_C(176, 96, 0);
      if ((lx == 1 || lx == 10) && (ly == 1 || ly == 10)) return RD_C(104, 48, 0);
      int dx = lx - 3, dy = ly - 2;
      if ((unsigned)dx < 5 && (unsigned)dy < 7 &&
          (pgm_read_byte(&RDM_FUENTE[rdpe.d[i]][dy]) & (0x10 >> dx)))
        return RD_C(88, 32, 0);
      return RD_C(252, 188, 32);
    }
    // 4) Dos puntos
    if (rdpe.dp && (unsigned)(x - 64) < 2 && ((unsigned)(y - 3) < 2 || (unsigned)(y - 9) < 2))
      return RD_C(252, 252, 252);
  }
  // 5) Nubes (detrás de los bloques)
  for (int i = 0; i < 2; i++) {
    int lx = x - rdpe.nube[i], ly = y - RDM_NUBE_Y[i];
    if ((unsigned)lx < 14 && (unsigned)ly < 5) {
      char ch = (char)pgm_read_byte(&RDM_NUBE[ly][lx]);
      if (ch == 'W') return rdpe.cielo == 0 ? RD_C(252, 252, 252) : rdpe.cielo == 1 ? RD_C(161, 161, 181) : RD_C(70, 70, 110);
      if (ch == 'G') return rdpe.cielo == 0 ? RD_C(188, 208, 252) : rdpe.cielo == 1 ? RD_C(118, 128, 168) : RD_C(48, 48, 84);
    }
  }
  // 6) Arbusto
  {
    int lx = x - 104, ly = y - 24;
    if ((unsigned)lx < 16 && (unsigned)ly < 5) {
      char ch = (char)pgm_read_byte(&RDM_ARBUSTO[ly][lx]);
      if (ch == 'L') return RD_C(60, 200, 60);
      if (ch == 'D') return RD_C(0, 140, 0);
    }
  }
  // 7) Colina
  {
    int dy = y - 21;
    if (dy >= 0 && dy < 8) {
      int half = 2 + dy * 2;
      if (x >= 16 - half && x < 16 + half) return RD_C(16, 120, 48);
    }
  }
  // 8) Cielo
  return rdpe.cielo == 0 ? RD_C(92, 148, 252) : rdpe.cielo == 1 ? RD_C(49, 77, 146) : RD_C(6, 6, 40);
}

// ────────────────────────────────────────────────────────────────────
// "Invaders"
// ────────────────────────────────────────────────────────────────────
#define RDI_DIG_Y     2        // fila superior de los dígitos (14 de alto -> hasta y=15)
#define RDI_CANON_Y   25       // fila superior del cañón (6 de alto -> hasta y=30)
#define RDI_SUELO     31
#define RDI_PASO_MS   14       // ticks entre saltos de la formación
static const int16_t RDI_DIG_X[4] = { 35, 48, 69, 82 };     // dígitos de 10 de ancho
static const int16_t RDI_INV_X[4] = { 5, 19, 99, 113 };     // invasores de 9 de ancho
static const int8_t  RDI_MARCHA[8] = { 0, 2, 4, 2, 0, -2, -4, -2 };

static const char RDI_CANGREJO[2][6][10] PROGMEM = {
  { "..X...X..", ".XXXXXXX.", "XX.XXX.XX", "XXXXXXXXX", "X.XXXXX.X", "X.X...X.X" },
  { "..X...X..", "X.XXXXX.X", "XXXXXXXXX", "XX.XXX.XX", ".XXXXXXX.", ".X.....X." }
};
static const char RDI_CALAMAR[2][6][10] PROGMEM = {
  { "....X....", "..XXXXX..", ".XXXXXXX.", "XX.XXX.XX", "XXXXXXXXX", ".X.X.X.X." },
  { "....X....", "..XXXXX..", ".XXXXXXX.", "XX.XXX.XX", "XXXXXXXXX", "X.X...X.X" }
};
static const char RDI_CANON[6][10] PROGMEM = {
  "....X....", "...XXX...", ".XXXXXXX.", "XXXXXXXXX", "XXXXXXXXX", "XXXXXXXXX"
};
static const char RDI_VIDA[3][6] PROGMEM = { "..X..", ".XXX.", "XXXXX" };
static const char RDI_BOMBA[2][5][4] PROGMEM = {
  { ".X.", "X..", ".X.", "..X", ".X." },
  { ".X.", "..X", ".X.", "X..", ".X." }
};
static const char RDI_SPLASH[3][6] PROGMEM = { ".X.X.", "XXXXX", ".X.X." };

struct RDIEstado {
  int8_t   d[4], obj[4];
  uint8_t  pend;
  int8_t   marcha;      // desplazamiento actual de la formación
  uint8_t  pasoM;       // 0..7 dentro del ciclo de marcha
  uint8_t  patas;       // fotograma de los invasores
  int16_t  cx;          // centro del cañón
  int8_t   cdir;
  uint8_t  modo;        // 0 patrulla 1 apunta 2 bala en vuelo 3 pausa
  int8_t   objetivo;    // dígito al que apunta
  uint8_t  espera;
  bool     bala;  int16_t balX, balY;     // balY = fila superior (3 de alto)
  uint8_t  exp[4];      // ticks de explosión de cada dígito
  bool     bomba; int16_t bombX, bombY; uint8_t bombF, bombExp;
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDIEstado rdi;

static void rdi_marcarCanon() { rdMarcar(rdi.cx - 6, RDI_CANON_Y - 1, 13, 7); }
static void rdi_marcarDigito(int i) { rdMarcar(RDI_DIG_X[i] + rdi.marcha - 1, RDI_DIG_Y, 12, 14); }
static void rdi_marcarBala() { rdMarcar(rdi.balX - 1, rdi.balY - 1, 3, 5); }
static void rdi_marcarBomba() { rdMarcar(rdi.bombX - 3, rdi.bombY - 1, 7, 8); }
static void rdi_marcarSplash() { rdMarcar(rdi.bombX - 4, 27, 9, 4); }
static void rdi_marcarDosPuntos() { rdMarcar(60 + rdi.marcha, 4, 6, 10); }

static void rdi_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdi.d);
  for (int i = 0; i < 4; i++) { rdi.obj[i] = rdi.d[i]; rdi.exp[i] = 0; }
  rdi.pend = 0; rdi.marcha = 0; rdi.pasoM = 0; rdi.patas = 0;
  rdi.cx = 50 + (m % 30); rdi.cdir = 1; rdi.modo = 0; rdi.objetivo = -1; rdi.espera = 0;
  rdi.bala = false; rdi.bomba = false; rdi.bombExp = 0; rdi.bombF = 0;
  rdi.dp = (s % 2 == 0); rdi.n = 0; rdi.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdi_elegir() {
  int mejor = -1, dmin = 9999;
  for (int i = 0; i < 4; i++) {
    if (!(rdi.pend & (1 << i))) continue;
    int dist = abs((RDI_DIG_X[i] + rdi.marcha + 5) - rdi.cx);
    if (dist < dmin) { dmin = dist; mejor = i; }
  }
  rdi.objetivo = mejor;
}

static void rdi_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  uint8_t mask = 0;
  for (int i = 0; i < 4; i++) { rdi.obj[i] = nd[i]; if (nd[i] != rdi.d[i] && !rdi.exp[i]) mask |= (1 << i); }
  rdi.pend = mask;
  if (mask && rdi.modo == 0) { rdi_elegir(); rdi.modo = 1; }
}

static void rdi_segundo(int s) { rdi.dp = (s % 2 == 0); rdi_marcarDosPuntos(); }

static void rdi_tick() {
  rdi_marcarCanon();
  rdi.n++;

  // La formación avanza a saltitos
  if ((rdi.n % RDI_PASO_MS) == 0) {
    rdi.pasoM = (rdi.pasoM + 1) & 7;
    rdi.marcha = RDI_MARCHA[rdi.pasoM];
    rdi.patas ^= 1;
    rdMarcar(0, 0, RD_W, 17);
  }

  // Explosiones de dígitos: al terminar aparece el dígito nuevo
  for (int i = 0; i < 4; i++) if (rdi.exp[i]) {
    rdi.exp[i]--;
    if (!rdi.exp[i]) rdi.d[i] = rdi.obj[i];
    rdi_marcarDigito(i);
  }

  // Bomba de un invasor (de uno en uno)
  if (rdi.bombExp) { rdi.bombExp--; rdi_marcarSplash(); }
  else if (rdi.bomba) {
    rdi_marcarBomba();
    rdi.bombY++;
    if ((rdi.n & 1) == 0) rdi.bombF ^= 1;
    if (rdi.bombY >= 27) { rdi.bomba = false; rdi.bombExp = 3; rdi_marcarSplash(); }
    rdi_marcarBomba();
  } else if ((rdi.n % 55) == 0) {
    int k = (rdi.n / 55) & 3;
    rdi.bombX = RDI_INV_X[k] + 4 + rdi.marcha; rdi.bombY = 12; rdi.bomba = true;
    rdi_marcarBomba();
  }

  switch (rdi.modo) {
    case 0:                                              // patrulla despacio
      if ((rdi.n & 1) == 0) {
        rdi.cx += rdi.cdir;
        if (rdi.cx >= 96) rdi.cdir = -1;
        if (rdi.cx <= 32) rdi.cdir = 1;
      }
      break;
    case 1: {                                            // se coloca bajo el dígito
      int obj = RDI_DIG_X[rdi.objetivo] + rdi.marcha + 5;
      int dx = obj - rdi.cx;
      if (abs(dx) <= 2) {
        rdi.cx = obj;
        rdi.bala = true; rdi.balX = obj; rdi.balY = 22;
        rdi_marcarBala();
        rdi.modo = 2;
      } else rdi.cx += (dx > 0) ? 2 : -2;
    } break;
    case 2:                                              // la bala sube
      rdi_marcarBala();
      rdi.balY -= 2;
      if (rdi.balY <= RDI_DIG_Y + 13) {                  // toca la base del dígito
        int b = rdi.objetivo;
        rdi.bala = false;
        rdi.exp[b] = 4;
        rdi.pend &= ~(1 << b);
        rdi_marcarDigito(b);
        rdi.modo = 3; rdi.espera = 4;
      } else rdi_marcarBala();
      break;
    case 3:
      if (rdi.espera) rdi.espera--;
      if (!rdi.espera) {
        if (rdi.pend) { rdi_elegir(); rdi.modo = 1; }
        else rdi.modo = 0;
      }
      break;
  }
  rdi_marcarCanon();
}

static void rdi_paso(uint32_t ms) {
  if (!rdi.ultOk) { rdi.ult = ms; rdi.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdi.ult) >= RDM_TICK_MS && n < 3) { rdi.ult += RDM_TICK_MS; rdi_tick(); n++; }
  if (n == 3) rdi.ult = ms;
}

static uint16_t rdi_pixel(int x, int y) {
  // 1) bala del cañón
  if (rdi.bala && x == rdi.balX && (unsigned)(y - rdi.balY) < 3) return RD_C(255, 255, 255);

  // 2) bomba enemiga y su salpicadura
  if (rdi.bomba) {
    int lx = x - (rdi.bombX - 1), ly = y - rdi.bombY;
    if ((unsigned)lx < 3 && (unsigned)ly < 5 && pgm_read_byte(&RDI_BOMBA[rdi.bombF][ly][lx]) == 'X') return RD_C(255, 150, 40);
  }
  if (rdi.bombExp) {
    int lx = x - (rdi.bombX - 2), ly = y - 28;
    if ((unsigned)lx < 5 && (unsigned)ly < 3 && pgm_read_byte(&RDI_SPLASH[ly][lx]) == 'X') return RD_C(255, 200, 60);
  }

  // 3) cañón
  if (y >= RDI_CANON_Y && y < RDI_CANON_Y + 6) {
    int lx = x - (rdi.cx - 4);
    if ((unsigned)lx < 9 && pgm_read_byte(&RDI_CANON[y - RDI_CANON_Y][lx]) == 'X') return RD_C(80, 255, 80);
  }

  // 4) línea de suelo y vidas
  if (y == RDI_SUELO) return RD_C(20, 190, 20);
  if (y >= 28 && y <= 30 && x >= 3 && x < 24) {
    int k = (x - 3) / 7, lx = (x - 3) % 7;
    if (k < 3 && lx < 5 && pgm_read_byte(&RDI_VIDA[y - 28][lx]) == 'X') return RD_C(60, 200, 60);
  }

  // 5) banda superior: dígitos, dos puntos e invasores
  if (y <= 15) {
    for (int i = 0; i < 4; i++) {
      int lx = x - (RDI_DIG_X[i] + rdi.marcha);
      if ((unsigned)lx >= 10) continue;
      int ly = y - RDI_DIG_Y;
      if ((unsigned)ly >= 14) break;
      if (rdi.exp[i]) {                                  // chispas que se van apagando
        static const uint8_t DENS[5] = { 0, 70, 120, 180, 230 };
        uint8_t hh = rdi_hash(x, y, rdi.exp[i]);
        if (hh < DENS[rdi.exp[i]]) {
          uint8_t c = rdi_hash(y, x, rdi.n) & 3;
          return c == 0 ? RD_C(255, 255, 255) : c == 1 ? RD_C(255, 230, 60) : c == 2 ? RD_C(255, 140, 30) : RD_C(230, 50, 20);
        }
        break;
      }
      if (pgm_read_byte(&RDM_FUENTE[rdi.d[i]][ly >> 1]) & (0x10 >> (lx >> 1))) return RD_C(250, 250, 225);
      break;
    }
    if (rdi.dp) {
      int lx = x - (62 + rdi.marcha);
      if ((unsigned)lx < 2 && ((unsigned)(y - 5) < 2 || (unsigned)(y - 11) < 2)) return RD_C(250, 250, 225);
    }
    for (int k = 0; k < 4; k++) {
      int lx = x - (RDI_INV_X[k] + rdi.marcha), ly = y - 6;
      if ((unsigned)lx >= 9 || (unsigned)ly >= 6) continue;
      bool cangrejo = (k == 0 || k == 3);
      char ch = (char)pgm_read_byte(cangrejo ? &RDI_CANGREJO[rdi.patas][ly][lx] : &RDI_CALAMAR[rdi.patas][ly][lx]);
      if (ch == 'X') return cangrejo ? RD_C(255, 100, 220) : RD_C(80, 230, 255);
    }
  }

  // 6) estrellas lejanas
  if (y < 30 && ((x * 131 + y * 197) % 97) == 0) return RD_C(90, 90, 140);
  return 0;
}

// ────────────────────────────────────────────────────────────────────
// "Pacman"
// ────────────────────────────────────────────────────────────────────
#define RDP_BLK_Y     1        // fila superior de las habitaciones (12x12 -> hasta y=12)
#define RDP_PAC_Y     20       // fila superior de Pacman y fantasmas (9x9 -> hasta y=28)
#define RDP_PARED_Y   16       // techo del pasillo (el suelo es y=31)
#define RDP_XMIN      0
#define RDP_XMAX      119      // el sprite mide 9 de ancho
#define RDP_TODAS     0x7FFFFFFFUL   // 31 bolitas: bit 0 y bit 30 = píldoras de poder
static const int16_t RDP_BLK_X[4] = { 35, 49, 69, 83 };       // H1 H2 : M1 M2
static const uint8_t RDP_SEQ[4]   = { 1, 2, 1, 0 };           // boca: media, abierta, media, cerrada

// Sprites 9x9 (mirando a la derecha). '.' = transparente
static const char RDP_PAC[3][9][10] PROGMEM = {
  { "..XXXXX..", ".XXXXXXX.", "XXXXXXXXX", "XXXXXXXXX", "XXXXXXXXX",
    "XXXXXXXXX", "XXXXXXXXX", ".XXXXXXX.", "..XXXXX.." },
  { "..XXXXX..", ".XXXXXXX.", "XXXXXXXX.", "XXXXXX...", "XXXX.....",
    "XXXXXX...", "XXXXXXXX.", ".XXXXXXX.", "..XXXXX.." },
  { "..XXXXX..", ".XXXXX...", "XXXXXX...", "XXXX.....", "XXX......",
    "XXXX.....", "XXXXXX...", ".XXXXX...", "..XXXXX.." }
};
// X = cuerpo, W = ojo, P = pupila
static const char RDP_FANT[2][9][10] PROGMEM = {
  { "..XXXXX..", ".XXXXXXX.", "XXXXXXXXX", "XWWXXXWWX", "XWPXXXWPX",
    "XXXXXXXXX", "XXXXXXXXX", "XXXXXXXXX", "XX.XXX.XX" },
  { "..XXXXX..", ".XXXXXXX.", "XXXXXXXXX", "XWWXXXWWX", "XWPXXXWPX",
    "XXXXXXXXX", "XXXXXXXXX", "XXXXXXXXX", "X.XXXXX.X" }
};
static const uint16_t RDP_COLOR_FANT[4] = {
  RD_C(255, 0, 0), RD_C(255, 184, 255), RD_C(0, 255, 255), RD_C(255, 184, 82)   // rojo, rosa, cian, naranja
};
// Mini fuente 3x5 para el "200": [0] = '2', [1] = '0'
static const uint8_t RDP_MINI[2][5] PROGMEM = { {7,1,7,4,7}, {7,5,5,5,7} };

struct RDPEstado {
  int8_t   d[4];        // dígitos que se ven ahora
  int8_t   obj[4];      // dígitos que toca mostrar
  uint8_t  pend;        // máscara de dígitos por cambiar
  uint8_t  modo;        // 0 patrulla 1 va a su sitio 2 cae el fantasma 3 persecución 4 "200"
  int8_t   dir;         // dirección del Pacman (+1 derecha)
  int16_t  px;          // x del Pacman
  uint32_t mask;        // bolitas que quedan (bit k -> x = 4 + 4k)
  int8_t   gb;          // habitación de la que sale el fantasma
  int8_t   gdir;        // dirección de huida
  int16_t  gx, gy;      // esquina superior izquierda del fantasma
  int16_t  tx;          // dónde espera el Pacman
  uint8_t  t;           // ticks dentro del modo
  bool     gOn;         // fantasma visible
  bool     dp;          // dos puntos / píldoras visibles
  uint32_t n, ult;
  bool     ultOk;
};
static RDPEstado rdp;

static void rdp_marcarPac()      { rdMarcar(rdp.px - 1, RDP_PAC_Y - 1, 11, 11); }
static void rdp_marcarFant()     { rdMarcar(rdp.gx - 1, rdp.gy - 1, 11, 11); }
static void rdp_marcarBloque(int i) { rdMarcar(RDP_BLK_X[i], 0, 12, 13); }
static void rdp_marcarPuerta(int i) { rdMarcar(RDP_BLK_X[i], RDP_PARED_Y - 1, 12, 3); }
static void rdp_marcarPuntos()   { rdMarcar(rdp.gx - 2, 21, 15, 7); }

static void rdp_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdp.d);
  for (int i = 0; i < 4; i++) rdp.obj[i] = rdp.d[i];
  rdp.pend = 0; rdp.modo = 0; rdp.dir = 1;
  rdp.px = 20 + (m % 60);                 // no siempre arranca en el mismo sitio
  rdp.mask = RDP_TODAS;
  rdp.gb = -1; rdp.gdir = 1; rdp.gx = 0; rdp.gy = 0; rdp.tx = 0; rdp.t = 0;
  rdp.gOn = false;
  rdp.dp = (s % 2 == 0);
  rdp.n = 0; rdp.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

// Elige la habitación pendiente más cercana y decide por dónde huirá el fantasma
static void rdp_elegir() {
  int mejor = -1, dmin = 9999;
  for (int i = 0; i < 4; i++) {
    if (!(rdp.pend & (1 << i))) continue;
    int dist = abs((RDP_BLK_X[i] + 2) - rdp.px);
    if (dist < dmin) { dmin = dist; mejor = i; }
  }
  rdp.gb = mejor;
  if (mejor < 0) return;
  rdp.gdir = (RDP_BLK_X[mejor] + 6 < 64) ? 1 : -1;               // huye hacia el lado con más sitio
  rdp.tx = RDP_BLK_X[mejor] + 2 - rdp.gdir * 12;                  // el Pacman espera a su espalda
}

static void rdp_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  uint8_t mask = 0;
  for (int i = 0; i < 4; i++) { rdp.obj[i] = nd[i]; if (nd[i] != rdp.d[i]) mask |= (1 << i); }
  rdp.pend = mask;
  if (mask && rdp.modo == 0) { rdp_elegir(); rdp.modo = 1; }
}

static void rdp_segundo(int s) {
  rdp.dp = (s % 2 == 0);
  rdMarcar(64, 3, 2, 8);                  // dos puntos
  rdMarcar(1, 21, 6, 6);                  // píldoras de poder
  rdMarcar(121, 21, 6, 6);
}

// Se come la bolita que tenga bajo la boca
static void rdp_comer() {
  int pc = rdp.px + 4;                    // centro del Pacman
  int k = (pc < 2) ? 0 : ((pc - 2) >> 2);
  if (k > 30) k = 30;
  int dx = 4 + 4 * k;
  uint32_t bit = (uint32_t)1 << k;
  if (abs(pc - dx) <= 1 && (rdp.mask & bit)) {
    rdp.mask &= ~bit;
    rdMarcar(dx - 2, 21, 6, 6);
    if (!rdp.mask) { rdp.mask = RDP_TODAS; rdMarcar(0, 21, RD_W, 6); }   // nivel nuevo
  }
}

static void rdp_tick() {
  rdp_marcarPac();
  rdp.n++;

  switch (rdp.modo) {
    case 0:                                         // patrulla comiendo
      rdp.px += rdp.dir;
      if (rdp.px >= RDP_XMAX) { rdp.px = RDP_XMAX; rdp.dir = -1; }
      if (rdp.px <= RDP_XMIN) { rdp.px = RDP_XMIN; rdp.dir = 1; }
      rdp_comer();
      break;

    case 1:                                         // corre hasta su sitio
      if (rdp.px < rdp.tx)      { rdp.px += (rdp.tx - rdp.px >= 2) ? 2 : 1; rdp.dir = 1; }
      else if (rdp.px > rdp.tx) { rdp.px -= (rdp.px - rdp.tx >= 2) ? 2 : 1; rdp.dir = -1; }
      rdp_comer();
      if (rdp.px == rdp.tx) {                       // ¡sale el fantasma con el dígito viejo!
        int b = rdp.gb;
        rdp.dir = rdp.gdir;
        rdp.gx = RDP_BLK_X[b] + 2; rdp.gy = 2; rdp.gOn = true;
        rdp.d[b] = rdp.obj[b];
        rdp.pend &= ~(1 << b);
        rdp_marcarBloque(b); rdp_marcarPuerta(b);
        rdp.modo = 2;
      }
      break;

    case 2:                                         // el fantasma cae por la puerta
      rdp_marcarFant();
      rdp.gy += 2;
      if (rdp.gy >= RDP_PAC_Y) {
        rdp.gy = RDP_PAC_Y;
        rdp.modo = 3; rdp.t = 0;
        rdp_marcarPuerta(rdp.gb);                   // la puerta se cierra
      }
      rdp_marcarFant();
      break;

    case 3:                                         // huye asustado, el Pacman lo caza
      rdp_marcarFant();
      rdp.t++;
      rdp.gx += rdp.gdir;
      if (rdp.t > 6) { rdp.px += 2 * rdp.gdir; rdp_comer(); }
      rdp_marcarFant();
      if ((rdp.gx - rdp.px) * rdp.gdir <= 5) {      // ¡ñam!
        rdp.gOn = false;
        rdp.modo = 4; rdp.t = 0;
        rdp_marcarPuntos();
      }
      break;

    case 4:                                         // "200" y el Pacman reaparece
      rdp.t++;
      if (rdp.t >= 12) {
        rdp_marcarPuntos();
        if (rdp.pend) { rdp_elegir(); rdp.modo = 1; }
        else rdp.modo = 0;
      }
      break;
  }
  rdp_marcarPac();
}

static void rdp_paso(uint32_t ms) {
  if (!rdp.ultOk) { rdp.ult = ms; rdp.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdp.ult) >= RDM_TICK_MS && n < 3) { rdp.ult += RDM_TICK_MS; rdp_tick(); n++; }
  if (n == 3) rdp.ult = ms;
}

static uint16_t rdp_pixel(int x, int y) {
  const uint16_t AZUL = RD_C(40, 40, 230);

  // 1) "200" donde se comió al fantasma
  if (rdp.modo == 4 && (unsigned)(y - 22) < 5) {
    int lx = x - (rdp.gx - 1);
    if ((unsigned)lx < 11 && (lx & 3) < 3) {
      int g = ((lx >> 2) == 0) ? 0 : 1;
      if (pgm_read_byte(&RDP_MINI[g][y - 22]) & (4 >> (lx & 3))) return RD_C(33, 222, 255);
    }
  }

  // 2) fantasma (de color al caer, azul asustado en el pasillo)
  if (rdp.gOn) {
    int lx = x - rdp.gx, ly = y - rdp.gy;
    if ((unsigned)lx < 9 && (unsigned)ly < 9) {
      char ch = (char)pgm_read_byte(&RDP_FANT[(rdp.n >> 2) & 1][ly][(rdp.gdir < 0) ? 8 - lx : lx]);
      if (ch != '.') {
        if (rdp.modo == 3) {
          bool flash = (rdp.t >= 12) && ((rdp.t >> 1) & 1);
          if (ch == 'X') return flash ? RD_C(240, 240, 255) : RD_C(33, 33, 255);
          return flash ? RD_C(255, 0, 0) : RD_C(255, 184, 174);
        }
        if (ch == 'X') return RDP_COLOR_FANT[rdp.gb & 3];
        return (ch == 'W') ? RD_C(255, 255, 255) : RD_C(33, 33, 255);
      }
    }
  }

  // 3) Pacman (se oculta mientras sale el "200")
  if (rdp.modo != 4) {
    int lx = x - rdp.px, ly = y - RDP_PAC_Y;
    if ((unsigned)lx < 9 && (unsigned)ly < 9) {
      char ch = (char)pgm_read_byte(&RDP_PAC[RDP_SEQ[(rdp.n >> 1) & 3]][ly][(rdp.dir < 0) ? 8 - lx : lx]);
      if (ch == 'X') return RD_C(255, 224, 0);
    }
  }

  // 4) bolitas y píldoras de poder
  if (y >= 22 && y <= 25 && x >= 2 && x <= 125) {
    if (x <= 5   && (rdp.mask & 1UL)          && rdp.dp) return RD_C(255, 184, 174);
    if (x >= 122 && (rdp.mask & (1UL << 30))  && rdp.dp) return RD_C(255, 184, 174);
    if (y >= 23 && y <= 24 && x >= 8 && x <= 121) {
      int k = (x - 4) >> 2;
      if (((x - 4) & 3) < 2 && (rdp.mask & ((uint32_t)1 << k))) return RD_C(255, 184, 174);
    }
  }

  // 5) paredes del pasillo (el techo se abre bajo la puerta mientras cae el fantasma)
  if (y == 31) return AZUL;
  if (y == RDP_PARED_Y) {
    bool puerta = (rdp.gOn && rdp.modo == 2 && (unsigned)(x - RDP_BLK_X[rdp.gb]) < 12);
    if (!puerta) return AZUL;
  }

  // 6) habitaciones con dígito y dos puntos
  if (y <= 12) {
    for (int i = 0; i < 4; i++) {
      int lx = x - RDP_BLK_X[i];
      if ((unsigned)lx >= 12) continue;
      int ly = y - RDP_BLK_Y;
      if ((unsigned)ly >= 12) continue;
      if ((lx == 0 || lx == 11) && (ly == 0 || ly == 11)) return 0;   // esquina cortada
      if (lx == 0 || lx == 11 || ly == 0 || ly == 11) return AZUL;
      int dx = lx - 3, dy = ly - 2;
      if ((unsigned)dx < 5 && (unsigned)dy < 7 &&
          (pgm_read_byte(&RDM_FUENTE[rdp.d[i]][dy]) & (0x10 >> dx)))
        return RD_C(255, 255, 255);
      return 0;
    }
    if (rdp.dp && (unsigned)(x - 64) < 2 && ((unsigned)(y - 3) < 2 || (unsigned)(y - 9) < 2))
      return RD_C(255, 255, 255);
  }

  // 7) fondo negro
  return 0;
}

// ────────────────────────────────────────────────────────────────────
// "Tetris"
// ────────────────────────────────────────────────────────────────────
#define RDT_DIG_Y     9        // fila superior de los dígitos (14 de alto -> hasta y=22)
#define RDT_STEP      6        // ticks por fila de caída de una pieza
static const int16_t RDT_DIG_X[4]   = { 35, 48, 69, 82 };    // dígitos de 10 de ancho
static const uint8_t RDT_DIG_COL[4] = { 1, 2, 5, 4 };        // cian, amarillo, rojo, verde
// Colores de pieza (índice 1..7): I O T S Z J L
static const uint8_t RDT_COL[8][3] PROGMEM = {
  {0,0,0}, {0,240,240}, {240,240,0}, {160,0,240}, {0,240,0}, {240,0,0}, {0,60,240}, {240,160,0}
};
// Celdas (x,y) de cada pieza dentro de una caja 4x4
static const uint8_t RDT_PIEZA[7][4][2] PROGMEM = {
  {{0,1},{1,1},{2,1},{3,1}}, {{1,1},{2,1},{1,2},{2,2}}, {{0,1},{1,1},{2,1},{1,2}},
  {{1,1},{2,1},{0,2},{1,2}}, {{0,1},{1,1},{1,2},{2,2}}, {{0,1},{1,1},{2,1},{2,2}},
  {{0,1},{1,1},{2,1},{0,2}}
};

struct RDTPozo {
  uint8_t g[8][8];      // celdas fijas (0 = vacía, 1..7 = color)
  int8_t  c[4][2];      // celdas de la pieza que cae (ya rotada)
  int8_t  cx, cy;       // esquina de su caja 4x4 en celdas (cy puede ser negativo)
  uint8_t col, cnt, wait;
  bool    act;
};
struct RDTEstado {
  int8_t   d[4], obj[4];
  uint8_t  ph[4];       // 0 quieto 1 parpadea 2 caen las filas
  uint8_t  t[4];
  RDTPozo  p[2];        // pozo izquierdo (x 0..31) y derecho (x 96..127)
  bool     dp;
  uint32_t n, ult, rng;
  bool     ultOk;
};
static RDTEstado rdt;

static uint32_t rdt_rand() { uint32_t x = rdt.rng; x ^= x << 13; x ^= x >> 17; x ^= x << 5; rdt.rng = x; return x; }

static void rdt_marcarDigito(int i) { rdMarcar(RDT_DIG_X[i], 0, 10, 24); }
static void rdt_marcarPozo(int i)   { rdMarcar(i ? 96 : 0, 0, 32, 32); }
static void rdt_marcarPieza(int i)  { RDTPozo& w = rdt.p[i]; rdMarcar((i ? 96 : 0) + w.cx * 4, w.cy * 4, 16, 16); }

static void rdt_reset(int i) {
  RDTPozo& w = rdt.p[i];
  memset(w.g, 0, sizeof(w.g));
  for (int r = 5; r < 8; r++) {                        // un montón inicial con huecos
    int prob = (r == 7) ? 85 : (r == 6) ? 55 : 25, llenas = 0;
    for (int c = 0; c < 8; c++)
      if ((int)(rdt_rand() % 100) < prob) { w.g[r][c] = 1 + rdt_rand() % 7; llenas++; }
    if (llenas == 8) w.g[r][rdt_rand() % 8] = 0;
  }
  w.act = false; w.wait = 6 + i * 20;
  rdt_marcarPozo(i);
}

static void rdt_spawn(int i) {
  RDTPozo& w = rdt.p[i];
  int pt = rdt_rand() % 7, rot = rdt_rand() % 4;
  for (int k = 0; k < 4; k++) {
    w.c[k][0] = pgm_read_byte(&RDT_PIEZA[pt][k][0]);
    w.c[k][1] = pgm_read_byte(&RDT_PIEZA[pt][k][1]);
  }
  for (int r = 0; r < rot; r++)
    for (int k = 0; k < 4; k++) { int x = w.c[k][0], y = w.c[k][1]; w.c[k][0] = 3 - y; w.c[k][1] = x; }
  int minx = 9, maxx = -9, maxy = -9;
  for (int k = 0; k < 4; k++) {
    if (w.c[k][0] < minx) minx = w.c[k][0];
    if (w.c[k][0] > maxx) maxx = w.c[k][0];
    if (w.c[k][1] > maxy) maxy = w.c[k][1];
  }
  w.cx = -minx + (int)(rdt_rand() % (8 - (maxx - minx)));
  w.cy = -maxy - 1;                                    // empieza justo por encima del pozo
  w.col = pt + 1; w.cnt = 0; w.act = true;
}

static bool rdt_choca(const RDTPozo& w, int cx, int cy) {
  for (int k = 0; k < 4; k++) {
    int x = cx + w.c[k][0], y = cy + w.c[k][1];
    if (x < 0 || x > 7 || y >= 8) return true;
    if (y >= 0 && w.g[y][x]) return true;
  }
  return false;
}

static void rdt_fijar(int i) {
  RDTPozo& w = rdt.p[i];
  bool fuera = false;
  for (int k = 0; k < 4; k++) {
    int x = w.cx + w.c[k][0], y = w.cy + w.c[k][1];
    if (y < 0) fuera = true; else w.g[y][x] = w.col;
  }
  for (int r = 7; r >= 0; r--) {                       // limpia líneas completas
    bool llena = true;
    for (int c = 0; c < 8; c++) if (!w.g[r][c]) llena = false;
    if (llena) {
      for (int rr = r; rr > 0; rr--) memcpy(w.g[rr], w.g[rr - 1], 8);
      memset(w.g[0], 0, 8);
      r++;
    }
  }
  bool alta = false;
  for (int c = 0; c < 8; c++) if (w.g[2][c]) alta = true;
  w.act = false; w.wait = 8 + rdt_rand() % 25;
  if (fuera || alta) rdt_reset(i);                     // "game over": empieza de nuevo
  rdt_marcarPozo(i);
}

static void rdt_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdt.d);
  for (int i = 0; i < 4; i++) { rdt.obj[i] = rdt.d[i]; rdt.ph[i] = 0; rdt.t[i] = 0; }
  rdt.rng = 0x9E3779B9u ^ (uint32_t)(h * 3600 + m * 60 + s);
  for (int i = 0; i < 2; i++) rdt_reset(i);
  rdt.dp = (s % 2 == 0); rdt.n = 0; rdt.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdt_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  for (int i = 0; i < 4; i++) rdt.obj[i] = nd[i];      // el tick arranca la animación de cada dígito
}

static void rdt_segundo(int s) { rdt.dp = (s % 2 == 0); rdMarcar(62, 10, 4, 12); }

static void rdt_tick() {
  rdt.n++;
  for (int i = 0; i < 4; i++) {
    if (rdt.ph[i] == 0) {
      if (rdt.d[i] != rdt.obj[i]) { rdt.ph[i] = 1; rdt.t[i] = 0; rdt_marcarDigito(i); }
    } else if (rdt.ph[i] == 1) {                       // parpadeo de "línea completa"
      rdt.t[i]++; rdt_marcarDigito(i);
      if (rdt.t[i] >= 8) { rdt.ph[i] = 2; rdt.t[i] = 0; rdt.d[i] = rdt.obj[i]; }
    } else {                                           // caen las filas del dígito nuevo
      rdt.t[i]++; rdt_marcarDigito(i);
      if (rdt.t[i] >= 22) { rdt.ph[i] = 0; rdt.t[i] = 0; }
    }
  }
  for (int i = 0; i < 2; i++) {
    RDTPozo& w = rdt.p[i];
    if (!w.act) { if (w.wait) w.wait--; else rdt_spawn(i); continue; }
    if (++w.cnt >= RDT_STEP) {
      w.cnt = 0;
      rdt_marcarPieza(i);
      if (!rdt_choca(w, w.cx, w.cy + 1)) { w.cy++; rdt_marcarPieza(i); }
      else rdt_fijar(i);
    }
  }
}

static void rdt_paso(uint32_t ms) {
  if (!rdt.ultOk) { rdt.ult = ms; rdt.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdt.ult) >= RDM_TICK_MS && n < 3) { rdt.ult += RDM_TICK_MS; rdt_tick(); n++; }
  if (n == 3) rdt.ult = ms;
}

// Celda de 4x4 con bisel (claro arriba/izquierda, oscuro abajo/derecha)
static uint16_t rdt_celda(int col, int px, int py) {
  int r = pgm_read_byte(&RDT_COL[col][0]), g = pgm_read_byte(&RDT_COL[col][1]), b = pgm_read_byte(&RDT_COL[col][2]);
  if (px == 0 || py == 0)      { r = (r + 255) >> 1; g = (g + 255) >> 1; b = (b + 255) >> 1; }
  else if (px == 3 || py == 3) { r >>= 1; g >>= 1; b >>= 1; }
  return RD_C(r, g, b);
}

// Brillo suave de la esquina de las celdas 2x2 de los dígitos
static uint16_t rdt_luz(int col) {
  int r = pgm_read_byte(&RDT_COL[col][0]), g = pgm_read_byte(&RDT_COL[col][1]), b = pgm_read_byte(&RDT_COL[col][2]);
  return RD_C(r + ((255 - r) >> 2), g + ((255 - g) >> 2), b + ((255 - b) >> 2));
}

static uint16_t rdt_pixel(int x, int y) {
  // 1) paredes de los pozos
  if (x == 32 || x == 94) return RD_C(150, 150, 175);
  if (x == 33 || x == 95) return RD_C(70, 70, 95);

  // 2) pozos laterales
  if (x < 32 || x >= 96) {
    int i = (x >= 96) ? 1 : 0, lx = x - (i ? 96 : 0);
    const RDTPozo& w = rdt.p[i];
    int cxl = lx >> 2, cyl = y >> 2;
    int col = w.g[cyl][cxl];
    if (!col && w.act)
      for (int k = 0; k < 4; k++)
        if (w.cx + w.c[k][0] == cxl && w.cy + w.c[k][1] == cyl) { col = w.col; break; }
    if (col) return rdt_celda(col, lx & 3, y & 3);
    return ((lx & 3) == 0 && (y & 3) == 0) ? RD_C(28, 28, 44) : 0;     // rejilla tenue
  }

  // 3) dígitos de bloques
  for (int i = 0; i < 4; i++) {
    int lx = x - RDT_DIG_X[i];
    if ((unsigned)lx >= 10) continue;
    int ly, row;
    bool flash = (rdt.ph[i] == 1) && (((rdt.t[i] >> 1) & 1) == 0);
    if (rdt.ph[i] == 2) {
      for (int r = 0; r < 7; r++) {
        int tt = rdt.t[i] - (6 - r) * 2;
        if (tt < 0) continue;
        int fy = RDT_DIG_Y + 2 * r, yy = fy - 16 + 2 * tt;
        if (yy > fy) yy = fy;
        if (y < yy || y >= yy + 2) continue;
        if (pgm_read_byte(&RDM_FUENTE[rdt.d[i]][r]) & (0x10 >> (lx >> 1))) {
          int col = RDT_DIG_COL[i];
          int px = lx & 1, py = y - yy;
          if (px == 0 && py == 0) return rdt_luz(col);
          return rdt_celda(col, 1, 1);
        }
        return 0;
      }
      return 0;
    }
    ly = y - RDT_DIG_Y; row = ly >> 1;
    if ((unsigned)ly >= 14) return 0;
    if (pgm_read_byte(&RDM_FUENTE[rdt.d[i]][row]) & (0x10 >> (lx >> 1))) {
      int col = RDT_DIG_COL[i];
      if (flash) return ((lx & 1) == 0 && (ly & 1) == 0) ? RD_C(255, 255, 255) : RD_C(210, 210, 225);
      if ((lx & 1) == 0 && (ly & 1) == 0)
        return rdt_luz(col);
      return rdt_celda(col, 1, 1);
    }
    return 0;
  }

  // 4) dos puntos
  if (rdt.dp && (unsigned)(x - 62) < 2 && ((unsigned)(y - 12) < 2 || (unsigned)(y - 18) < 2))
    return RD_C(255, 255, 255);
  return 0;
}

// ────────────────────────────────────────────────────────────────────
// "Zelda"
// ────────────────────────────────────────────────────────────────────
#define RDZ_Y0        16       // fila superior del héroe (pies en y=28)
#define RDZ_BLK_Y     1
#define RDZ_XMIN      3
#define RDZ_XMAX      114
static const int16_t RDZ_BLK_X[4]   = { 35, 49, 69, 83 };
static const int16_t RDZ_TORCH_X[2] = { 10, 113 };
static const uint8_t RDZ_ESPADA[8]  = { 0, 2, 4, 5, 5, 5, 3, 0 };   // largo de la hoja por tick
#define RDZ_GOLPE     3        // tick en el que la espada toca la tablilla

// Sprites 11x13 (mirando a la derecha). '.' = transparente
static const char RDZ_STAND[13][12] PROGMEM = {
  "...GGGGG...", "..GGGGGGG..", ".HGGGSSSG..", ".HHSSSKSS..", ".HHSSSSSS..",
  "..HSSSSS...", "..GGGGGGDD.", ".SGGGGGDYD.", "..GGGGGDDD.", "...GGGGG...",
  "...GGGGG...", "...SS.SS...", "..BBB.BBB.."
};
static const char RDZ_WALK[13][12] PROGMEM = {
  "...GGGGG...", "..GGGGGGG..", ".HGGGSSSG..", ".HHSSSKSS..", ".HHSSSSSS..",
  "..HSSSSS...", "..GGGGGGDD.", ".SGGGGGDYD.", "..GGGGGDDD.", "...GGGGG...",
  "...GGGGG...", "..SS...SS..", ".BBB...BBB."
};
static const char RDZ_UP[13][12] PROGMEM = {
  "...GGGGG...", "..GGGGGGG..", ".HHHHHHHHH.", ".HHHHHHHHH.", "..HHHHHHH..",
  "...SSSSS...", ".SGGGGGGGS.", ".SGGGGGGGS.", "..GGGGGGG..", "...GGGGG...",
  "...GGGGG...", "...SS.SS...", "..BBB.BBB.."
};
static const char RDZ_RUPIA[8][6] PROGMEM = {
  "..G..", ".GGG.", "GGGGG", "GGWGG", "GGWGG", "GGGGG", ".GGG.", "..G.."
};
static const char RDZ_LLAMA[2][5][6] PROGMEM = {
  { "..Y..", ".YYY.", "YYOYY", ".YOY.", "..O.." },
  { ".Y...", ".YYY.", ".YOYY", "YYOY.", "..OO." }
};
static const char RDZ_BASE[4][6] PROGMEM = { ".GGG.", "..G..", "..G..", ".GGG." };

struct RDZEstado {
  int8_t   d[4], obj[4];
  uint8_t  pend;
  uint8_t  modo;        // 0 pasea 1 va a la tablilla 2 espadazo 3 cae la rupia 4 la recoge
  int8_t   bloque, dir;
  uint8_t  anim, t, chispa, flash[4];
  int16_t  x;
  bool     rup;         // rupia visible
  int16_t  rx;  int8_t ry, lado;
  bool     tw;          // fotograma de las antorchas
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDZEstado rdz;

static void rdz_marcarHeroe()  { rdMarcar(rdz.x - 1, 10, 13, 20); }
static void rdz_marcarBloque(int i) { rdMarcar(RDZ_BLK_X[i], 0, 12, 13); }
static void rdz_marcarRupia()  { rdMarcar(rdz.rx - 1, rdz.ry - 1, 7, 10); }
static void rdz_marcarAntorchas() { rdMarcar(RDZ_TORCH_X[0], 4, 5, 9); rdMarcar(RDZ_TORCH_X[1], 4, 5, 9); }

static void rdz_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdz.d);
  for (int i = 0; i < 4; i++) { rdz.obj[i] = rdz.d[i]; rdz.flash[i] = 0; }
  rdz.pend = 0; rdz.modo = 0; rdz.bloque = -1; rdz.dir = 1;
  rdz.anim = 0; rdz.t = 0; rdz.chispa = 0; rdz.rup = false; rdz.tw = false;
  rdz.x = 24 + (m % 40);
  rdz.dp = (s % 2 == 0); rdz.n = 0; rdz.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdz_elegir() {
  int mejor = -1, dmin = 9999;
  for (int i = 0; i < 4; i++) {
    if (!(rdz.pend & (1 << i))) continue;
    int dist = abs((RDZ_BLK_X[i] + 1) - rdz.x);
    if (dist < dmin) { dmin = dist; mejor = i; }
  }
  rdz.bloque = mejor;
}

static void rdz_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  uint8_t mask = 0;
  for (int i = 0; i < 4; i++) { rdz.obj[i] = nd[i]; if (nd[i] != rdz.d[i]) mask |= (1 << i); }
  rdz.pend = mask;
  if (mask && rdz.modo == 0) { rdz.modo = 1; rdz.bloque = -1; }
}

static void rdz_segundo(int s) { rdz.dp = (s % 2 == 0); rdMarcar(64, 3, 2, 8); }

static void rdz_tick() {
  rdz_marcarHeroe();
  rdz.n++;

  for (int i = 0; i < 4; i++) if (rdz.flash[i]) { rdz.flash[i]--; rdz_marcarBloque(i); }
  if (rdz.chispa) { rdz.chispa--; rdMarcar(rdz.rx - 1, 20, 8, 8); }
  if ((rdz.n % 6) == 0) { rdz.tw = !rdz.tw; rdz_marcarAntorchas(); }

  switch (rdz.modo) {
    case 0:                                         // paseo tranquilo
      if ((rdz.n & 1) == 0) {
        rdz.x += rdz.dir;
        if (rdz.x >= RDZ_XMAX) rdz.dir = -1;
        if (rdz.x <= RDZ_XMIN) rdz.dir = 1;
      }
      if ((rdz.n % 3) == 0) rdz.anim ^= 1;
      break;

    case 1: {                                       // anda hasta la tablilla
      if (rdz.bloque < 0) rdz_elegir();
      if (rdz.bloque < 0) { rdz.modo = 0; break; }
      int objetivo = RDZ_BLK_X[rdz.bloque] + 1;
      if (rdz.x < objetivo)      { rdz.x += (objetivo - rdz.x >= 2) ? 2 : 1; rdz.dir = 1; }
      else if (rdz.x > objetivo) { rdz.x -= (rdz.x - objetivo >= 2) ? 2 : 1; rdz.dir = -1; }
      if ((rdz.n % 2) == 0) rdz.anim ^= 1;
      if (rdz.x == objetivo) { rdz.modo = 2; rdz.t = 0; rdz.anim = 0; }
    } break;

    case 2: {                                       // espadazo
      int b = rdz.bloque;
      if (rdz.t == RDZ_GOLPE) {
        rdz.d[b] = rdz.obj[b];
        rdz.flash[b] = 4;
        rdz.pend &= ~(1 << b);
        rdz_marcarBloque(b);
        rdz.lado = (rdz.x + 24 <= 125) ? 1 : -1;    // la rupia cae a su lado
        rdz.rx = rdz.x + (rdz.lado > 0 ? 13 : -7);
        rdz.ry = 13; rdz.rup = true;
        rdz_marcarRupia();
      }
      rdz.t++;
      if (rdz.t >= 8) { rdz.modo = 3; rdz.anim = 0; }
    } break;

    case 3:                                         // la rupia cae al suelo
      rdz_marcarRupia();
      rdz.ry += 2;
      if (rdz.ry >= 21) { rdz.ry = 21; rdz.modo = 4; }
      rdz_marcarRupia();
      break;

    case 4: {                                       // el héroe la recoge
      bool llega = (rdz.lado > 0) ? (rdz.x + 10 >= rdz.rx) : (rdz.x <= rdz.rx + 4);
      if (llega) {
        rdz_marcarRupia();
        rdz.rup = false; rdz.chispa = 3;
        rdz.bloque = -1; rdz.anim = 0;
        rdz.modo = rdz.pend ? 1 : 0;
      } else {
        rdz.x += rdz.lado; rdz.dir = rdz.lado;
        if ((rdz.n % 2) == 0) rdz.anim ^= 1;
      }
    } break;
  }
  rdz_marcarHeroe();
}

static void rdz_paso(uint32_t ms) {
  if (!rdz.ultOk) { rdz.ult = ms; rdz.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdz.ult) >= RDM_TICK_MS && n < 3) { rdz.ult += RDM_TICK_MS; rdz_tick(); n++; }
  if (n == 3) rdz.ult = ms;
}

static uint16_t rdz_colorHeroe(char ch) {
  switch (ch) {
    case 'G': return RD_C(0, 176, 0);
    case 'S': return RD_C(252, 184, 120);
    case 'H': return RD_C(136, 64, 0);
    case 'K': return RD_C(0, 0, 0);
    case 'B': return RD_C(100, 50, 0);
    case 'D': return RD_C(200, 100, 30);
    case 'Y': return RD_C(252, 216, 0);
  }
  return 0;
}

static uint16_t rdz_pixel(int x, int y) {
  // 1) destello al recoger la rupia
  if (rdz.chispa) {
    int dx = x - (rdz.rx + 2), dy = y - 24;
    if ((dx == 0 && abs(dy) <= 2) || (dy == 0 && abs(dx) <= 2)) return RD_C(255, 255, 255);
  }
  // 2) rupia
  if (rdz.rup) {
    int lx = x - rdz.rx, ly = y - rdz.ry;
    if ((unsigned)lx < 5 && (unsigned)ly < 8) {
      char ch = (char)pgm_read_byte(&RDZ_RUPIA[ly][lx]);
      if (ch == 'G') return RD_C(0, 200, 60);
      if (ch == 'W') return RD_C(190, 255, 210);
    }
  }
  // 3) espada y héroe
  {
    int lx = x - rdz.x, ly = y - RDZ_Y0;
    if (rdz.modo == 2) {
      int len = RDZ_ESPADA[rdz.t < 8 ? rdz.t : 7];
      if (len) {
        if (ly == -1 && (unsigned)(lx - 4) < 3) return RD_C(252, 216, 0);
        if (lx == 5 && ly <= -2 && ly >= -1 - len) return RD_C(200, 224, 255);
      }
    }
    if ((unsigned)lx < 11 && (unsigned)ly < 13) {
      const char (*spr)[12] = (rdz.modo == 2) ? RDZ_UP : (rdz.anim ? RDZ_WALK : RDZ_STAND);
      char ch = (char)pgm_read_byte(&spr[ly][(rdz.dir < 0 && rdz.modo != 2) ? 10 - lx : lx]);
      if (ch != '.') return rdz_colorHeroe(ch);
    }
  }
  // 4) tablillas con dígito y dos puntos
  if (y <= 12) {
    for (int i = 0; i < 4; i++) {
      int lx = x - RDZ_BLK_X[i];
      if ((unsigned)lx >= 12) continue;
      int ly = y - RDZ_BLK_Y;
      if ((unsigned)ly >= 12) continue;
      bool fl = rdz.flash[i] != 0;
      if (lx == 0 || lx == 11 || ly == 0 || ly == 11) return fl ? RD_C(255, 255, 255) : RD_C(216, 168, 40);
      int dx = lx - 3, dy = ly - 2;
      if ((unsigned)dx < 5 && (unsigned)dy < 7 &&
          (pgm_read_byte(&RDM_FUENTE[rdz.d[i]][dy]) & (0x10 >> dx)))
        return fl ? RD_C(24, 24, 72) : RD_C(255, 240, 160);
      return fl ? RD_C(255, 255, 255) : RD_C(20, 20, 56);
    }
    if (rdz.dp && (unsigned)(x - 64) < 2 && ((unsigned)(y - 3) < 2 || (unsigned)(y - 9) < 2))
      return RD_C(252, 252, 252);
  }
  // 5) antorchas
  for (int k = 0; k < 2; k++) {
    int lx = x - RDZ_TORCH_X[k], ly = y - 4;
    if ((unsigned)lx < 5 && (unsigned)ly < 9) {
      char ch = (ly < 5) ? (char)pgm_read_byte(&RDZ_LLAMA[rdz.tw ? 1 : 0][ly][lx])
                         : (char)pgm_read_byte(&RDZ_BASE[ly - 5][lx]);
      if (ch == 'Y') return RD_C(252, 220, 60);
      if (ch == 'O') return RD_C(252, 120, 20);
      if (ch == 'G') return RD_C(150, 110, 70);
    }
  }
  // 6) muro de ladrillo
  if (y <= 13) {
    int off = ((y >> 2) & 1) * 4;
    if ((y & 3) == 3 || ((x + off) & 7) == 0) return RD_C(6, 12, 60);
    return ((y & 3) == 0) ? RD_C(40, 84, 210) : RD_C(24, 56, 170);
  }
  if (y == 14) return RD_C(4, 8, 40);
  // 7) suelo de la mazmorra
  if (y == 31 || ((x + 4) & 15) == 0 || y == 23) return RD_C(20, 20, 56);
  return RD_C(10, 10, 30);
}

// ────────────────────────────────────────────────────────────────────
//  "DonkeyKong"
// ────────────────────────────────────────────────────────────────────
#define RDK_BLK_Y     1
#define RDK_KX        2        // esquina del gorila (16x16)
#define RDK_KY        13
#define RDK_JY        16       // fila de los pies del saltarín (en la viga)
static const int16_t RDK_BLK_X[4] = { 35, 49, 69, 83 };

// Gorila 16x16 (B marrón, T piel, K negro, W blanco). 0 reposo, 1 golpe de pecho, 2 levanta el barril
static const char RDK_KONG[3][16][17] PROGMEM = {
  { "....BBBBBBBB....", "...BBBBBBBBBB...", "...BBTTTTTTBB...", "...BTTKTTKTTB...",
    "...BTTTTTTTTB...", "...BBTTWWTTBB...", "..BBBBTTTTBBBB..", ".BBBBBBBBBBBBBB.",
    "BBBBTTTTTTTTBBBB", "BBBBTTTTTTTTBBBB", "BBBBBTTTTTTBBBBB", ".BBBBBBBBBBBBBB.",
    "..BBBBBBBBBBBB..", "..BBBB....BBBB..", "..BBBB....BBBB..", ".BBBBB....BBBBB." },
  { "....BBBBBBBB....", "...BBBBBBBBBB...", "...BBTTTTTTBB...", "...BTTKTTKTTB...",
    "...BTTTTTTTTB...", "...BBTTWWTTBB...", ".BBBBBTTTTBBBBB.", "BBBBBBBBBBBBBBBB",
    "BBBTTTTTTTTTTBBB", ".BBBTTTTTTTTBBB.", "..BBBTTTTTTBBB..", ".BBBBBBBBBBBBBB.",
    "..BBBBBBBBBBBB..", "..BBBB....BBBB..", "..BBBB....BBBB..", ".BBBBB....BBBBB." },
  { "BB..BBBBBBBB..BB", "BB.BBBBBBBBBB.BB", "BBBBBTTTTTTBBBBB", "BBBBTTKTTKTTBBBB",
    ".BBBTTTTTTTTBBB.", "..BBBTTWWTTBBB..", "..BBBBTTTTBBBB..", ".BBBBBBBBBBBBBB.",
    "BBBBTTTTTTTTBBBB", "BBBBTTTTTTTTBBBB", "BBBBBTTTTTTBBBBB", ".BBBBBBBBBBBBBB.",
    "..BBBBBBBBBBBB..", "..BBBB....BBBB..", "..BBBB....BBBB..", ".BBBBB....BBBBB." }
};
static const char RDK_BARRIL[2][7][8] PROGMEM = {
  { ".BBBBB.", "BLLLLLB", "BBBBBBB", "BLLLLLB", "BBBBBBB", "BLLLLLB", ".BBBBB." },
  { ".BLBLB.", "BBLBLBB", "BLBLBLB", "BBLBLBB", "BLBLBLB", "BBLBLBB", ".BLBLB." }
};
static const char RDK_LLAMA[2][5][8] PROGMEM = {
  { "...Y...", "..YYY..", ".YYOYY.", ".YOOOY.", "..OOO.." },
  { "..Y.Y..", ".YYYY..", ".YOYYY.", "YOOOOY.", ".OOOO.." }
};

struct RDKEstado {
  int8_t   d[4], obj[4];
  uint8_t  pend, modo;      // 0 reposo 1 levanta el barril 2 vuelo 3 impacto
  int8_t   bloque;
  uint8_t  flash[4];
  int16_t  fx0, fy0, fx1, fy1, bx, by;
  uint8_t  t, N, espera, chispas;
  uint8_t  rbm;             // barril rodante: 0 no hay 1 cae 2 rueda
  int16_t  rbx, rby;
  uint8_t  rbEsp, llama2;
  int16_t  jx;  int8_t jdir;  uint8_t jsalto;  uint8_t janim;
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDKEstado rdk;

static void rdk_marcarKong()   { rdMarcar(0, 5, 20, 24); }
static void rdk_marcarBloque(int i) { rdMarcar(RDK_BLK_X[i], 0, 12, 13); }
static void rdk_marcarVuelo()  { rdMarcar(rdk.bx - 4, rdk.by - 4, 9, 9); }
static void rdk_marcarRodante() { rdMarcar(rdk.rbx - 1, rdk.rby - 1, 9, 9); }
static void rdk_marcarSaltarin() { rdMarcar(rdk.jx - 1, 0, 13, 17); }
static void rdk_marcarLlama()  { rdMarcar(19, 12, 9, 6); }

static void rdk_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdk.d);
  for (int i = 0; i < 4; i++) { rdk.obj[i] = rdk.d[i]; rdk.flash[i] = 0; }
  rdk.pend = 0; rdk.modo = 0; rdk.bloque = -1;
  rdk.t = 0; rdk.N = 1; rdk.espera = 0; rdk.chispas = 0; rdk.bx = 9; rdk.by = 9;
  rdk.rbm = 0; rdk.rbEsp = 30 + (m % 30); rdk.llama2 = 0; rdk.rbx = 91; rdk.rby = 10;
  rdk.jx = 100 + (m % 14); rdk.jdir = 1; rdk.jsalto = 0; rdk.janim = 0;
  rdk.dp = (s % 2 == 0); rdk.n = 0; rdk.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdk_elegir() {                     // el más a la izquierda de los pendientes
  rdk.bloque = -1;
  for (int i = 0; i < 4; i++) if (rdk.pend & (1 << i)) { rdk.bloque = i; break; }
}

static void rdk_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  uint8_t mask = 0;
  for (int i = 0; i < 4; i++) { rdk.obj[i] = nd[i]; if (nd[i] != rdk.d[i]) mask |= (1 << i); }
  rdk.pend = mask;
  if (mask && rdk.modo == 0) { rdk_elegir(); rdk.modo = 1; rdk.espera = 6; rdk_marcarKong(); }
}

static void rdk_segundo(int s) { rdk.dp = (s % 2 == 0); rdMarcar(64, 3, 2, 8); }

static void rdk_tick() {
  rdk_marcarKong(); rdk_marcarSaltarin();
  rdk.n++;

  for (int i = 0; i < 4; i++) if (rdk.flash[i]) { rdk.flash[i]--; rdk_marcarBloque(i); }
  if (rdk.chispas) { rdk.chispas--; rdk_marcarVuelo(); }
  if (rdk.llama2) { rdk.llama2--; rdk_marcarLlama(); }
  if ((rdk.n % 5) == 0) rdk_marcarLlama();
  if ((rdk.n % 8) == 0) rdk_marcarKong();

  // Saltarín: patrulla la viga y de vez en cuando salta
  if ((rdk.n & 1) == 0) {
    rdk.jx += rdk.jdir;
    if (rdk.jx >= 116) rdk.jdir = -1;
    if (rdk.jx <= 100) rdk.jdir = 1;
  }
  if ((rdk.n % 3) == 0) rdk.janim ^= 1;
  if (rdk.jsalto) { rdk.jsalto++; if (rdk.jsalto > RDM_SALTO_N) rdk.jsalto = 0; }
  else if ((rdk.n % 64) == 0) rdk.jsalto = 1;

  // Barril rodante de adorno
  switch (rdk.rbm) {
    case 0:
      if (rdk.rbEsp) rdk.rbEsp--;
      else { rdk.rbm = 1; rdk.rbx = 91; rdk.rby = 10; rdk_marcarRodante(); }
      break;
    case 1:
      rdk_marcarRodante();
      rdk.rby += 2;
      if (rdk.rby >= 22) { rdk.rby = 22; rdk.rbm = 2; }
      rdk_marcarRodante();
      break;
    case 2:
      rdk_marcarRodante();
      if ((rdk.n & 1) == 0) rdk.rbx--;
      if (rdk.rbx <= 28) { rdk.rbm = 0; rdk.rbEsp = 70 + (rdi_hash(rdk.rbx, 3, (int)rdk.n) % 60); rdk.llama2 = 5; rdk_marcarLlama(); }
      rdk_marcarRodante();
      break;
  }

  // Gorila: lanza un barril contra la placa que toca cambiar
  switch (rdk.modo) {
    case 1:
      if (rdk.espera) rdk.espera--;
      if (!rdk.espera) {
        rdk.fx0 = 9; rdk.fy0 = 9;
        rdk.fx1 = RDK_BLK_X[rdk.bloque] + 6; rdk.fy1 = 10;
        int dist = abs(rdk.fx1 - rdk.fx0);
        rdk.N = 8 + dist / 5; rdk.t = 0;
        rdk.bx = rdk.fx0; rdk.by = rdk.fy0;
        rdk.modo = 2;
      }
      break;
    case 2: {
      rdk_marcarVuelo();
      rdk.t++;
      int p = (rdk.t * 256) / rdk.N;
      rdk.bx = rdk.fx0 + ((rdk.fx1 - rdk.fx0) * p) / 256;
      rdk.by = rdk.fy0 + ((rdk.fy1 - rdk.fy0) * p) / 256 - ((4 * 8 * p * (256 - p)) >> 16);
      if (rdk.t >= rdk.N) {                       // ¡impacto!
        int b = rdk.bloque;
        rdk.bx = rdk.fx1; rdk.by = rdk.fy1;
        rdk.d[b] = rdk.obj[b];
        rdk.flash[b] = 4; rdk.chispas = 4;
        rdk.pend &= ~(1 << b);
        rdk_marcarBloque(b);
        rdk.modo = 3; rdk.espera = 5;
      }
      rdk_marcarVuelo();
    } break;
    case 3:
      if (rdk.espera) rdk.espera--;
      if (!rdk.espera) {
        if (rdk.pend) { rdk_elegir(); rdk.modo = 1; rdk.espera = 6; }
        else rdk.modo = 0;
      }
      break;
  }
  rdk_marcarKong(); rdk_marcarSaltarin();
}

static void rdk_paso(uint32_t ms) {
  if (!rdk.ultOk) { rdk.ult = ms; rdk.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdk.ult) >= RDM_TICK_MS && n < 3) { rdk.ult += RDM_TICK_MS; rdk_tick(); n++; }
  if (n == 3) rdk.ult = ms;
}

static uint16_t rdk_pixel(int x, int y) {
  // 1) chispas del impacto
  if (rdk.chispas && abs(x - rdk.bx) <= 6 && abs(y - rdk.by) <= 6) {
    uint8_t hh = rdi_hash(x, y, rdk.chispas);
    if (hh < 90) { uint8_t c = rdi_hash(y, x, (int)rdk.n) & 3; return c == 0 ? RD_C(255, 255, 255) : c == 1 ? RD_C(255, 230, 60) : RD_C(255, 130, 30); }
  }
  // 2) barril lanzado (en las manos del gorila o en el aire)
  if (rdk.modo == 1 || rdk.modo == 2) {
    int lx = x - (rdk.bx - 3), ly = y - (rdk.by - 3);
    if ((unsigned)lx < 7 && (unsigned)ly < 7) {
      char ch = (char)pgm_read_byte(&RDK_BARRIL[(rdk.n >> 2) & 1][ly][lx]);
      if (ch == 'B') return RD_C(204, 92, 16);
      if (ch == 'L') return RD_C(252, 188, 64);
    }
  }
  // 3) barril rodante
  if (rdk.rbm) {
    int lx = x - rdk.rbx, ly = y - rdk.rby;
    if ((unsigned)lx < 7 && (unsigned)ly < 7) {
      char ch = (char)pgm_read_byte(&RDK_BARRIL[(rdk.n >> 2) & 1][ly][lx]);
      if (ch == 'B') return RD_C(204, 92, 16);
      if (ch == 'L') return RD_C(252, 188, 64);
    }
  }
  // 4) gorila
  {
    int lx = x - RDK_KX, ly = y - RDK_KY;
    if ((unsigned)lx < 16 && (unsigned)ly < 16) {
      int f = (rdk.modo == 1) ? 2 : (rdk.modo == 3) ? 1 : (int)((rdk.n >> 3) & 1);
      char ch = (char)pgm_read_byte(&RDK_KONG[f][ly][lx]);
      if (ch == 'B') return RD_C(180, 72, 0);
      if (ch == 'T') return RD_C(252, 188, 120);
      if (ch == 'K') return RD_C(0, 0, 0);
      if (ch == 'W') return RD_C(252, 252, 252);
    }
  }
  // 5) saltarín sobre la viga
  {
    int rise = rdk.jsalto ? RDM_SALTO[rdk.jsalto - 1] : 0;
    int lx = x - rdk.jx, ly = y - (RDK_JY - 12 - rise);
    if ((unsigned)lx < 11 && (unsigned)ly < 13) {
      const char (*spr)[12] = rdk.jsalto ? RDM_JUMP : (rdk.janim ? RDM_RUN1 : RDM_STAND);
      char ch = (char)pgm_read_byte(&spr[ly][(rdk.jdir < 0) ? 10 - lx : lx]);
      if (ch != '.') return rdm_colorSprite(ch);
    }
  }
  // 6) llama y bidón de aceite
  {
    int lx = x - 20, ly = y - 13;
    if ((unsigned)lx < 7 && (unsigned)ly < 5) {
      char ch = (char)pgm_read_byte(&RDK_LLAMA[((rdk.n / 5) & 1)][ly][lx]);
      if (ch == 'Y') return rdk.llama2 ? RD_C(255, 255, 220) : RD_C(252, 220, 60);
      if (ch == 'O') return rdk.llama2 ? RD_C(255, 220, 90) : RD_C(252, 120, 20);
    }
    lx = x - 19; ly = y - 18;
    if ((unsigned)lx < 9 && (unsigned)ly < 11) {
      if (lx == 0 || lx == 8) return RD_C(20, 30, 140);
      if (ly == 0 || ly == 10) return RD_C(80, 110, 252);
      if (ly == 3 || ly == 7) return RD_C(120, 150, 252);
      return RD_C(40, 70, 230);
    }
  }
  // 7) escalera, viga y suelo
  if (x >= 119 && x <= 123 && y >= 19 && y <= 28) {
    if (x == 119 || x == 123 || (y & 3) == 0) return RD_C(0, 220, 252);
  }
  if (y >= 29) {
    if (y == 29) return RD_C(252, 150, 150);
    if (y == 30) return (((x >> 2) & 1) == 0) ? RD_C(80, 10, 20) : RD_C(220, 60, 60);
    return RD_C(150, 30, 40);
  }
  if (x >= 98 && (y == 17 || y == 18)) {
    if (y == 17) return RD_C(252, 150, 150);
    return (((x >> 2) & 1) == 0) ? RD_C(80, 10, 20) : RD_C(220, 60, 60);
  }
  // 8) placas de acero con dígito y dos puntos
  if (y <= 12) {
    for (int i = 0; i < 4; i++) {
      int lx = x - RDK_BLK_X[i];
      if ((unsigned)lx >= 12) continue;
      int ly = y - RDK_BLK_Y;
      if ((unsigned)ly >= 12) continue;
      bool fl = rdk.flash[i] != 0;
      if (lx == 0 || lx == 11 || ly == 0 || ly == 11) return fl ? RD_C(255, 255, 255) : RD_C(230, 72, 72);
      if (lx == 1 || lx == 10 || ly == 1 || ly == 10) return fl ? RD_C(255, 230, 200) : RD_C(140, 28, 48);
      int dx = lx - 3, dy = ly - 2;
      if ((unsigned)dx < 5 && (unsigned)dy < 7 &&
          (pgm_read_byte(&RDM_FUENTE[rdk.d[i]][dy]) & (0x10 >> dx)))
        return fl ? RD_C(60, 20, 20) : RD_C(255, 250, 220);
      return fl ? RD_C(255, 255, 255) : RD_C(24, 24, 72);
    }
    if (rdk.dp && (unsigned)(x - 64) < 2 && ((unsigned)(y - 3) < 2 || (unsigned)(y - 9) < 2))
      return RD_C(255, 250, 220);
  }
  return 0;
}

// ────────────────────────────────────────────────────────────────────
// "Bubble Bobble"
// ────────────────────────────────────────────────────────────────────
#define RDU_R         8        // radio de las burbujas
#define RDU_CY        10       // altura del centro de las burbujas
#define RDU_BUB_X     6        // x izquierda del dragón verde en casa (12 de ancho)
#define RDU_BOB_X     110      // x izquierda del dragón azul
#define RDU_BY        18       // fila superior de los dragones (11 de alto: pies en y=28)
static const int16_t RDU_CX[4] = { 30, 51, 77, 98 };
static const int8_t  RDU_OLEO[8] PROGMEM = { 0, 1, 1, 1, 0, -1, -1, -1 };
static const int8_t  RDU_SALTO[5] PROGMEM = { 2, 3, 3, 1, 0 };
static const int8_t  RDU_FRUTA_Y[7] PROGMEM = { 12, 16, 20, 24, 22, 24, 24 };
static const int8_t  RDU_DIR[8][2] PROGMEM = { {1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1} };
static const int16_t RDU_PEQ_X[6] = { 10, 40, 64, 90, 118, 23 };

// Colores: 1 verde, 2 verde oscuro, 3 vientre, 4 ojo, 5 pupila, 6 naranja, 7 rojo
static const RDRect RDU_REPOSO[] PROGMEM = {
  {0,6,2,2,2},{2,1,9,8,1},{1,3,10,6,1},{5,5,5,4,3},{7,2,2,2,4},{8,3,1,1,5},
  {10,4,2,3,1},{10,6,2,1,2},{1,0,2,2,7},{4,0,2,1,7},{2,9,3,2,6},{7,9,3,2,6}
};
static const RDRect RDU_SALTA[] PROGMEM = {
  {0,5,2,2,2},{2,1,9,8,1},{1,3,10,6,1},{5,5,5,4,3},{7,2,2,2,4},{8,3,1,1,5},
  {10,4,2,3,1},{10,6,2,1,2},{1,0,2,2,7},{4,0,2,1,7},{2,9,2,2,6},{8,9,2,2,6}
};
static const RDRect RDU_SOPLA[] PROGMEM = {
  {0,6,2,2,2},{2,1,9,8,1},{1,3,10,6,1},{5,5,5,4,3},{7,2,2,2,4},{8,3,1,1,5},
  {10,3,2,4,7},{1,0,2,2,7},{4,0,2,1,7},{2,9,3,2,6},{7,9,3,2,6}
};
#define RDU_N(a) ((int)(sizeof(a) / sizeof(a[0])))

struct RDUEstado {
  int8_t   d[4], obj[4];
  uint8_t  pend;
  uint8_t  modo;        // 0 reposo 1 corre 2 salta 3 fruta 4 sopla 5 burbuja vuela 6 vuelve a casa
  int8_t   objetivo;
  uint8_t  t;
  int16_t  bx;          // x izquierda del dragón verde
  int8_t   dir;
  uint8_t  pose;        // 0 reposo 1 salta 2 sopla
  int8_t   alza;        // elevación del salto
  uint8_t  bs[4], bt[4]; // burbuja: 0 normal 1 estalla 2 vacía 3 se forma
  int16_t  fx, fy;  bool fruta;
  int16_t  nx, ny;  uint8_t nr;  bool nueva;
  uint8_t  bob;
  bool     dp;
  uint32_t n, ult;
  bool     ultOk;
};
static RDUEstado rdu;

static void rdu_marcarDragon() { rdMarcar(rdu.bx - 3, RDU_BY - 5, 18, 17); }
static void rdu_marcarBurbuja(int i) { rdMarcar(RDU_CX[i] - 12, 0, 25, 24); }
static void rdu_marcarFruta()  { rdMarcar(rdu.fx - 6, rdu.fy - 6, 13, 13); }
static void rdu_marcarNueva()  { rdMarcar(rdu.nx - 10, rdu.ny - 10, 21, 21); }

static bool rdu_difiere() {
  for (int i = 0; i < 4; i++) if (rdu.d[i] != rdu.obj[i]) return true;
  return false;
}
static int rdu_siguiente() {
  for (int i = 0; i < 4; i++) if (rdu.pend & (1 << i)) return i;
  return -1;
}

static void rdu_iniciar(int h, int m, int s) {
  rdm_digitos(h, m, rdu.d);
  for (int i = 0; i < 4; i++) { rdu.obj[i] = rdu.d[i]; rdu.bs[i] = 0; rdu.bt[i] = 0; }
  rdu.pend = 0; rdu.modo = 0; rdu.objetivo = -1; rdu.t = 0; rdu.bx = RDU_BUB_X; rdu.dir = 1;
  rdu.pose = 0; rdu.alza = 0; rdu.fruta = false; rdu.nueva = false; rdu.bob = 0;
  rdu.dp = (s % 2 == 0); rdu.n = 0; rdu.ultOk = false;
  rdMarcar(0, 0, RD_W, RD_H);
}

static void rdu_cambioMinuto(int h, int m) {
  int8_t nd[4];
  rdm_digitos(h, m, nd);
  for (int i = 0; i < 4; i++) rdu.obj[i] = nd[i];
}

static void rdu_segundo(int s) { rdu.dp = (s % 2 == 0); rdMarcar(61, 5, 6, 11); }

static int rdu_cy(int i) { return RDU_CY + (int)(int8_t)pgm_read_byte(&RDU_OLEO[((rdu.n >> 2) + i * 3) & 7]); }

static void rdu_tick() {
  rdu.n++;

  if ((rdu.n & 3) == 0) for (int i = 0; i < 4; i++) rdu_marcarBurbuja(i);          // las burbujas se mecen
  for (int k = 0; k < 6; k++) if ((rdu.n % (2 + (k % 3))) == 0) rdMarcar(RDU_PEQ_X[k] - 3, 0, 7, 29);   // burbujitas que suben
  if ((rdu.n % 10) == 0 && rdu.modo == 0) { rdu.bob ^= 1; rdu_marcarDragon(); rdMarcar(RDU_BOB_X - 2, RDU_BY - 2, 16, 13); }
  for (int i = 0; i < 4; i++) {
    if (rdu.bs[i] == 1) { rdu_marcarBurbuja(i); if (rdu.bt[i]) rdu.bt[i]--; if (!rdu.bt[i]) rdu.bs[i] = 2; }
    else if (rdu.bs[i] == 3) { rdu_marcarBurbuja(i); if (rdu.bt[i]) rdu.bt[i]--; if (!rdu.bt[i]) rdu.bs[i] = 0; }
  }

  switch (rdu.modo) {
    case 0:
      if (rdu_difiere()) {
        rdu.pend = 0;
        for (int i = 0; i < 4; i++) if (rdu.d[i] != rdu.obj[i]) rdu.pend |= (1 << i);
        rdu.objetivo = rdu_siguiente();
        rdu.modo = 1;
      }
      break;
    case 1: {                                                                      // corre hasta quedar bajo la burbuja
      rdu_marcarDragon();
      int obj = RDU_CX[rdu.objetivo] - 6;
      int dx = obj - rdu.bx;
      if (dx == 0) { rdu.modo = 2; rdu.t = 0; rdu.pose = 1; }
      else { rdu.dir = dx > 0 ? 1 : -1; rdu.bx += (abs(dx) >= 2) ? 2 * rdu.dir : rdu.dir; }
      rdu_marcarDragon();
    } break;
    case 2:                                                                        // salta y la revienta con la cabeza
      rdu_marcarDragon();
      rdu.alza = (int8_t)pgm_read_byte(&RDU_SALTO[rdu.t]);
      if (rdu.t == 1) {
        int o = rdu.objetivo;
        rdu.bs[o] = 1; rdu.bt[o] = 4;
        rdu.fruta = true; rdu.fx = RDU_CX[o]; rdu.fy = RDU_CY; rdu.t = 1;
        rdu_marcarBurbuja(o);
      }
      rdu.t++;
      if (rdu.t >= 5) { rdu.alza = 0; rdu.pose = 0; rdu.modo = 3; rdu.t = 0; }
      rdu_marcarDragon();
      break;
    case 3:                                                                        // la fruta cae y rebota
      rdu_marcarFruta();
      rdu.fy = (int8_t)pgm_read_byte(&RDU_FRUTA_Y[rdu.t]);
      rdu.t++;
      if (rdu.t >= 7) { rdu.fruta = false; rdu.modo = 4; rdu.t = 0; rdu.pose = 2; rdu_marcarDragon(); }
      else rdu_marcarFruta();
      break;
    case 4:                                                                        // sopla una burbuja nueva
      rdu.t++;
      if (rdu.t >= 3) {
        rdu.nueva = true; rdu.nr = 2;
        rdu.nx = rdu.bx + 13 * (rdu.dir > 0 ? 1 : -1) + (rdu.dir > 0 ? 0 : 0); rdu.ny = 21;
        rdu.modo = 5; rdu.t = 0; rdu.pose = 0;
        rdu_marcarDragon(); rdu_marcarNueva();
      }
      break;
    case 5: {                                                                      // la burbuja vuela hasta su sitio y crece
      rdu_marcarNueva();
      rdu.t++;
      int o = rdu.objetivo;
      int x0 = rdu.bx + 13, y0 = 21, x1 = RDU_CX[o], y1 = RDU_CY;
      int p = (rdu.t * 256) / 7;
      rdu.nx = x0 + ((x1 - x0) * p) / 256;
      rdu.ny = y0 + ((y1 - y0) * p) / 256;
      rdu.nr = 2 + (rdu.t * 6) / 7;
      if (rdu.t >= 7) {
        rdu.nueva = false;
        rdu.d[o] = rdu.obj[o]; rdu.bs[o] = 3; rdu.bt[o] = 3; rdu.pend &= ~(1 << o);
        rdu_marcarBurbuja(o);
        int nxt = rdu_siguiente();
        if (nxt >= 0) { rdu.objetivo = nxt; rdu.modo = 1; }
        else { rdu.objetivo = -1; rdu.modo = 6; }
      } else rdu_marcarNueva();
    } break;
    case 6: {                                                                      // vuelve a casa
      rdu_marcarDragon();
      int dx = RDU_BUB_X - rdu.bx;
      if (dx == 0) { rdu.modo = 0; rdu.dir = 1; }
      else { rdu.dir = dx > 0 ? 1 : -1; rdu.bx += (abs(dx) >= 2) ? 2 * rdu.dir : rdu.dir; }
      rdu_marcarDragon();
    } break;
  }
}

static void rdu_paso(uint32_t ms) {
  if (!rdu.ultOk) { rdu.ult = ms; rdu.ultOk = true; return; }
  int n = 0;
  while ((uint32_t)(ms - rdu.ult) >= RDM_TICK_MS && n < 3) { rdu.ult += RDM_TICK_MS; rdu_tick(); n++; }
  if (n == 3) rdu.ult = ms;
}

static uint16_t rdu_colorDragon(uint8_t c, bool azul) {
  switch (c) {
    case 1: return azul ? RD_C(70, 130, 240) : RD_C(60, 200, 70);
    case 2: return azul ? RD_C(40, 80, 180) : RD_C(30, 140, 50);
    case 3: return RD_C(232, 240, 175);
    case 4: return RD_C(250, 250, 250);
    case 5: return RD_C(20, 20, 30);
    case 6: return RD_C(250, 140, 40);
    case 7: return RD_C(230, 60, 60);
  }
  return 0;
}

// Anillo de burbuja iridiscente de radio r; 0 si el punto no está en el anillo
static uint16_t rdu_anillo(int dx, int dy, int r) {
  int d2 = dx * dx + dy * dy;
  if (d2 > r * r || d2 < (r - 1) * (r - 1)) return 0;
  if (dx + dy < -(r - 2)) return RD_C(250, 250, 255);                 // brillo arriba a la izquierda
  if (dx - dy > r - 2)    return RD_C(255, 150, 220);                 // reflejo rosa
  if (dy - dx > r - 2)    return RD_C(255, 230, 120);                 // reflejo amarillo
  return RD_C(110, 215, 255);
}

static uint16_t rdu_fondo(int x, int y) {
  // muros de los lados y suelo de ladrillos de colores
  if (x <= 1 || x >= 126) {
    int f = (y >> 2) & 1;
    return ((x & 1) ? f : !f) ? RD_C(70, 200, 230) : RD_C(190, 70, 200);
  }
  if (y >= 29) {
    int f = y - 29;
    if (f == 0) return RD_C(240, 235, 255);
    bool mortero = (f == 1) ? ((x & 7) == 0) : ((x & 7) == 4);
    if (mortero) return RD_C(40, 20, 70);
    return (((x + (f == 2 ? 4 : 0)) >> 3) & 1) ? RD_C(70, 200, 230) : RD_C(190, 70, 200);
  }
  // burbujitas que suben
  for (int k = 0; k < 6; k++) {
    int cy = 30 - (int)((rdu.n / (2 + (k % 3))) + k * 9) % 36;
    int dx = x - RDU_PEQ_X[k], dy = y - cy;
    if (dx * dx + dy * dy <= 4 && dx * dx + dy * dy >= 1) return (dx + dy < 0) ? RD_C(230, 240, 255) : RD_C(80, 150, 210);
  }
  return RD_C(4, 2, 18);
}

static uint16_t rdu_capas(int x, int y) {
  // 1) fruta que cae
  if (rdu.fruta) {
    int dx = x - rdu.fx, dy = y - rdu.fy, d2 = dx * dx + dy * dy;
    if (dy == -4 && dx >= 0 && dx <= 2) return RD_C(60, 190, 70);                // hoja
    if (d2 <= 9) return (dx == -1 && dy == -1) ? RD_C(255, 220, 140) : RD_C(255, 150, 30);
  }
  // 2) burbuja nueva que vuela
  if (rdu.nueva) { uint16_t c = rdu_anillo(x - rdu.nx, y - rdu.ny, rdu.nr); if (c) return c; }
  // 3) dragones
  {
    int lx = x - rdu.bx, ly = y - (RDU_BY - rdu.alza + (rdu.modo == 0 ? rdu.bob : 0));
    if ((unsigned)lx < 12 && (unsigned)ly < 11) {
      if (rdu.dir < 0) lx = 11 - lx;
      uint8_t c = rdu.pose == 1 ? rdRectColor(RDU_SALTA, RDU_N(RDU_SALTA), lx, ly)
                : rdu.pose == 2 ? rdRectColor(RDU_SOPLA, RDU_N(RDU_SOPLA), lx, ly)
                                : rdRectColor(RDU_REPOSO, RDU_N(RDU_REPOSO), lx, ly);
      if (c) return rdu_colorDragon(c, false);
    }
    lx = 11 - (x - RDU_BOB_X); ly = y - (RDU_BY + (rdu.bob ^ 1));
    if ((x - RDU_BOB_X) >= 0 && (unsigned)lx < 12 && (unsigned)ly < 11) {
      uint8_t c = rdRectColor(RDU_REPOSO, RDU_N(RDU_REPOSO), lx, ly);
      if (c) return rdu_colorDragon(c, true);
    }
  }
  // 4) burbujas con su dígito
  for (int i = 0; i < 4; i++) {
    int cx = RDU_CX[i];
    if (x < cx - 11 || x > cx + 11) continue;
    int cy = rdu_cy(i);
    int dx = x - cx, dy = y - cy;
    uint8_t bs = rdu.bs[i];
    if (bs == 1) {                                                               // fragmentos de la burbuja al reventar
      int age = 4 - rdu.bt[i] + 1;
      for (int k = 0; k < 8; k++) {
        int px = (int)(int8_t)pgm_read_byte(&RDU_DIR[k][0]) * (age * 2 + 4), py = (int)(int8_t)pgm_read_byte(&RDU_DIR[k][1]) * (age * 2 + 4);
        if (k & 1) { px = px * 3 / 4; py = py * 3 / 4; }
        if (dx >= px && dx <= px + 1 && dy >= py && dy <= py + 1) return (k & 2) ? RD_C(255, 150, 220) : RD_C(110, 215, 255);
      }
      continue;
    }
    if (bs == 2) continue;
    // dígito dentro de la burbuja
    int lx = dx + 5, ly = dy + 7;
    if (rdDigito2(rdu.d[i], lx, ly)) return (bs == 3 && rdu.bt[i] >= 2) ? RD_C(255, 255, 255) : RD_C(255, 236, 90);
    if (rdDigito2(rdu.d[i], lx - 1, ly - 1)) return RD_C(200, 90, 40);
    uint16_t a = rdu_anillo(dx, dy, RDU_R);
    if (a) return a;
    if (dx * dx + dy * dy < (RDU_R - 1) * (RDU_R - 1) && dx + dy < -4 && ((x + y) & 1) == 0) return RD_C(120, 150, 200);   // brillo del interior
  }
  // 5) dos puntos
  if (rdu.dp && x >= 63 && x <= 64 && ((y >= 7 && y <= 8) || (y >= 12 && y <= 13))) return RD_C(255, 236, 90);
  return rdu_fondo(x, y);
}

static uint16_t rdu_pixel(int x, int y) { return rdu_capas(x, y); }

#endif // RD_8BIT_H
