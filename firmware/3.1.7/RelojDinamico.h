// ====================================================================
//                            RELOJ DINÁMICO
// ====================================================================
#ifndef RELOJ_DINAMICO_H
#define RELOJ_DINAMICO_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef ARDUINO
  #include <Arduino.h>
  #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
  extern MatrixPanel_I2S_DMA *display;
#endif
#ifndef PROGMEM
  #define PROGMEM
#endif
#ifndef pgm_read_byte
  #define pgm_read_byte(p) (*(const uint8_t*)(p))
#endif
#ifndef RD_PUTPIXEL
  #define RD_PUTPIXEL(x, y, c) display->drawPixel((x), (y), (c))
#endif

#define RD_W 128
#define RD_H 32
#define RD_C(r, g, b) ((uint16_t)((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3)))

// ────────────────────────────────────────────────────────────────────
//  Interfaz de una cara
// ────────────────────────────────────────────────────────────────────
struct RDCara {
  const char* nombre;                       // corto (cabe en el OSD)
  void     (*iniciar)(int h, int m, int s); // estado inicial, SIN animar
  void     (*cambioMinuto)(int h, int m);   // dispara la animación
  void     (*segundo)(int s);               // tictac (parpadeo de los dos puntos...)
  void     (*paso)(uint32_t ms);            // avanza la animación (llamar a menudo)
  uint16_t (*pixel)(int x, int y);          // color final del punto (x,y) de la escena
};

// ────────────────────────────────────────────────────────────────────
//  Motor: rectángulo sucio + volcado al panel
// ────────────────────────────────────────────────────────────────────
static const RDCara* rd_cara = nullptr;
static int      rd_ox = 0, rd_oy = 0;            // origen de la escena en coordenadas de drawPixel()
static int      rd_cx0 = 0, rd_cx1 = RD_W - 1;   // columnas visibles del panel (coordenadas de drawPixel)
static int      rd_x0 = 1, rd_y0 = 1, rd_x1 = 0, rd_y1 = 0;   // sucio (vacío si x0>x1)
static bool     rd_doble = false;                // el panel usa doble buffer
static uint8_t  rd_pend = 0;                     // frames completos que faltan (doble buffer)
static int      rd_ultHM = -1, rd_ultSeg = -1;

// Marca un rectángulo de la escena como "a repintar" (se recorta a 128x32).
static void rdMarcar(int x, int y, int w, int h) {
  int ax = x, ay = y, bx = x + w - 1, by = y + h - 1;
  if (ax < 0) ax = 0;
  if (ay < 0) ay = 0;
  if (bx > RD_W - 1) bx = RD_W - 1;
  if (by > RD_H - 1) by = RD_H - 1;
  if (ax > bx || ay > by) return;
  if (rd_x0 > rd_x1) { rd_x0 = ax; rd_y0 = ay; rd_x1 = bx; rd_y1 = by; return; }
  if (ax < rd_x0) rd_x0 = ax;
  if (ay < rd_y0) rd_y0 = ay;
  if (bx > rd_x1) rd_x1 = bx;
  if (by > rd_y1) rd_y1 = by;
}

static void rdRenderRect(int x0, int y0, int x1, int y1) {
  if (rd_cx0 - rd_ox > x0) x0 = rd_cx0 - rd_ox;     // nunca escribir fuera de la zona visible
  if (rd_cx1 - rd_ox < x1) x1 = rd_cx1 - rd_ox;
  for (int y = y0; y <= y1; y++)
    for (int x = x0; x <= x1; x++)
      RD_PUTPIXEL(rd_ox + x, rd_oy + y, rd_cara->pixel(x, y));
}

// Escribe en el panel lo que haga falta. Devuelve true si hay que hacer
// flipDMABuffer(). Con doble buffer el buffer de atrás está "obsoleto", así que
// tras cada cambio se repinta la escena completa en 2 frames seguidos.
static bool rdVolcar() {
  const bool sucio = (rd_x0 <= rd_x1);
  if (rd_doble) {
    if (sucio) rd_pend = 2;
    if (rd_pend == 0) return false;
    rdRenderRect(0, 0, RD_W - 1, RD_H - 1);
    rd_pend--;
    rd_x0 = 1; rd_x1 = 0;
    return true;
  }
  if (!sucio) return false;
  rdRenderRect(rd_x0, rd_y0, rd_x1, rd_y1);
  rd_x0 = 1; rd_x1 = 0;
  return true;
}

// ────────────────────────────────────────────────────────────────────
//  Caras (por temática)
// ────────────────────────────────────────────────────────────────────
#include "RD_Comun.h"
#include "RD_8bit.h"
#include "RD_16bit.h"
#include "RD_Cine.h"
#include "RD_Anime.h"

// ────────────────────────────────────────────────────────────────────
//  Registro de caras (el orden es el índice que se guarda en config.ini)
// ────────────────────────────────────────────────────────────────────
static const RDCara RD_CARAS[] = {
  { "Mario",        rdm_iniciar, rdm_cambioMinuto, rdm_segundo, rdm_paso, rdm_pixel },
  { "Luigi",        rdl_iniciar, rdl_cambioMinuto, rdl_segundo, rdl_paso, rdl_pixel },
  { "Peach",        rdpe_iniciar, rdpe_cambioMinuto, rdpe_segundo, rdpe_paso, rdpe_pixel },
  { "Sonic",        rds_iniciar, rds_cambioMinuto, rds_segundo, rds_paso, rds_pixel },
  { "Invaders",     rdi_iniciar, rdi_cambioMinuto, rdi_segundo, rdi_paso, rdi_pixel },
  { "Pacman",       rdp_iniciar, rdp_cambioMinuto, rdp_segundo, rdp_paso, rdp_pixel },
  { "Tetris",       rdt_iniciar, rdt_cambioMinuto, rdt_segundo, rdt_paso, rdt_pixel },
  { "Zelda",        rdz_iniciar, rdz_cambioMinuto, rdz_segundo, rdz_paso, rdz_pixel },
  { "Donkey",       rdk_iniciar, rdk_cambioMinuto, rdk_segundo, rdk_paso, rdk_pixel },
  { "Bubble",       rdu_iniciar, rdu_cambioMinuto, rdu_segundo, rdu_paso, rdu_pixel },
  { "MegaManX",     rdx_iniciar, rdx_cambioMinuto, rdx_segundo, rdx_paso, rdx_pixel },
  { "Bomberman",    rdb_iniciar, rdb_cambioMinuto, rdb_segundo, rdb_paso, rdb_pixel },
  { "Streets",      rdr_iniciar, rdr_cambioMinuto, rdr_segundo, rdr_paso, rdr_pixel },
  { "Castelvania",  rdv_iniciar, rdv_cambioMinuto, rdv_segundo, rdv_paso, rdv_pixel },
  { "Fighter",      rdc_iniciar, rdc_cambioMinuto, rdc_segundo, rdc_paso, rdc_pixel },
  { "Chrono",       rdh_iniciar, rdh_cambioMinuto, rdh_segundo, rdh_paso, rdh_pixel },
  { "MetalSlug",    rde_iniciar, rde_cambioMinuto, rde_segundo, rde_paso, rde_pixel },
  { "StarWars",     rdw_iniciar, rdw_cambioMinuto, rdw_segundo, rdw_paso, rdw_pixel },
  { "Matrix",       rdn_iniciar, rdn_cambioMinuto, rdn_segundo, rdn_paso, rdn_pixel },
  { "Jurassic",     rdj_iniciar, rdj_cambioMinuto, rdj_segundo, rdj_paso, rdj_pixel },
  { "Terminator",   rdte_iniciar, rdte_cambioMinuto, rdte_segundo, rdte_paso, rdte_pixel },
  { "Futuro",       rdf_iniciar, rdf_cambioMinuto, rdf_segundo, rdf_paso, rdf_pixel },
  { "Fantasma",     rdg_iniciar, rdg_cambioMinuto, rdg_segundo, rdg_paso, rdg_pixel },
  { "DragonBall",   rdbz_iniciar, rdbz_cambioMinuto, rdbz_segundo, rdbz_paso, rdbz_pixel },
  { "OnePice",      rdpi_iniciar, rdpi_cambioMinuto, rdpi_segundo, rdpi_paso, rdpi_pixel },
  { "Naruto",       rdnr_iniciar, rdnr_cambioMinuto, rdnr_segundo, rdnr_paso, rdnr_pixel },
  { "Pokemon",      rdq_iniciar, rdq_cambioMinuto, rdq_segundo, rdq_paso, rdq_pixel },
};
#define RD_NUM_CARAS ((int)(sizeof(RD_CARAS) / sizeof(RD_CARAS[0])))

// ────────────────────────────────────────────────────────────────────
//  API para el firmware
// ────────────────────────────────────────────────────────────────────
// Arranca una cara y pinta la escena completa SIN animar.
//   xPanel0    = primera columna visible en coordenadas de drawPixel()
//                (en este firmware es `offset`: los GIF dibujan en x + offset)
//   anchoPanel = ancho real del panel en píxeles (PANEL_RES_X * panelChain)
//   doble      = buffer doble activo
static void rdIniciar(int idx, int h, int m, int s, int xPanel0, int anchoPanel, bool doble) {
  if (idx < 0 || idx >= RD_NUM_CARAS) idx = 0;
  rd_cara  = &RD_CARAS[idx];
  rd_ox    = xPanel0 + (anchoPanel - RD_W) / 2;   // escena de 128 centrada en el panel
  rd_cx0   = xPanel0;
  rd_cx1   = xPanel0 + anchoPanel - 1;
  rd_oy    = 0;
  rd_doble = doble;
  rd_pend  = 2;
  rd_ultHM = h * 60 + m;
  rd_ultSeg = s;
  rd_x0 = 1; rd_x1 = 0;
  rd_cara->iniciar(h, m, s);
  rdVolcar();
}

// Llamar en cada vuelta del bucle del reloj. true => hacer flipDMABuffer().
static bool rdActualizar(int h, int m, int s, uint32_t ms) {
  if (!rd_cara) return false;
  if (h * 60 + m != rd_ultHM) { rd_ultHM = h * 60 + m; rd_cara->cambioMinuto(h, m); }
  if (s != rd_ultSeg)         { rd_ultSeg = s;         rd_cara->segundo(s); }
  rd_cara->paso(ms);
  return rdVolcar();
}

#endif // RELOJ_DINAMICO_H
