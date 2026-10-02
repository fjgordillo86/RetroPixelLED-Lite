// retro_clock.ino — Retro Game Pixel Art Clock for 128x32 DMD
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
    String line = cfg.readStringUntil('\n');
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
