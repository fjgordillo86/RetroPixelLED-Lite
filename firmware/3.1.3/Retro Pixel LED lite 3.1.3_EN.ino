#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>  
#include <vector>
#include "SD.h"      
#include "AnimatedGIF.h"  
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Update.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <ESP32FtpServer.h>
#include <IRremote.hpp>
#include "fuente8pt7b_Bold.h"
#include "fuente8pt7b_Light.h"
#include "fuente8pt7b_Regular.h"
#include "fuente8pt7b_SemiBold.h"


// ====================================================================
//                     CONSTANTES & FIRMWARE LITE
// ====================================================================
#define FIRMWARE_VERSION "3.1.3" // New Clock styles and IR remote mapping from the PWA. GIF playback implementation in ReplayOS.
#define CURRENT_VERSION_NUM 313 // Numeric version for comparison (2.1.0 -> 210)
#define GITHUB_VERSION_URL "https://github.com/fjgordillo86/RetroPixelLED-Lite/raw/refs/heads/main/docs/version.json"
#define GITHUB_RAW_BASE_URL "https://raw.githubusercontent.com/fjgordillo86/RetroPixelLED-Lite/main/Contenido%20SD/idioma/"
#define CONFIG_FILE "/config.ini"

// --- PINES HUB75 ---
#define CLK_PIN       16
#define OE_PIN        15
#define LAT_PIN       4
#define A_PIN         33
#define B_PIN         32
#define C_PIN         22
#define D_PIN         17
#define E_PIN         -1 
#define R1_PIN        25
#define G1_PIN        26
#define B1_PIN        27
#define R2_PIN        14
#define G2_PIN        12
#define B2_PIN        13

// --- PINES SPI SD (Nativos VSPI) ---
#define SD_CS_PIN     5   
#define VSPI_MISO     19
#define VSPI_MOSI     23
#define VSPI_SCLK     18

// --- PANEL DIMENSIONS CONFIGURATION ---
const int PANEL_RES_X = 64; // Width of a single panel
const int PANEL_RES_Y = 32; // Height of a single panel
#define MATRIX_HEIGHT PANEL_RES_Y 

// --- MENU BUTTON CONFIGURATION ---
#define PIN_BOTON_MENU 21

// --- IR CONFIGURATION ---
#define IR_RECEIVE_PIN 34


// ====================================================================
//                     VARIABLES GLOBALES LITE
// ====================================================================
AnimatedGIF gif;
MatrixPanel_I2S_DMA *display = nullptr; 
File FSGifFile;

// Configuration variables (safe defaults)
bool wifiEnable = false;
bool wifiDebeEstarActivo = false;
bool mostrarIP = false;
char wifi_ssid[64] = "";
char wifi_pass[64] = "";
char time_zone[64] = "CET-1CEST,M3.5.0,M10.5.0/3";

int modoVisual = 0; // 0 = GIFs, 1 = Clock only
int panelChain = 2;
char colorOrder[4] = "RGB";
int offset = 128;
int brightness = 40;
int i2sSpeed = 2;
int refreshMin = 120;
int latchBlank = 1;
int doubleBuff = 0;
char idiomaActivo[8] = "ES";

int clockEnable = 0;
int randomMode = 1;
int autoClockInt = 5;
int clockDuration = 10;
int clockStyle = 2;
int transitionEnable = 0;
uint32_t clockColor = 0xFF0055;

int arcadeEnable = 0;
char replayOS_IP[16] = ""; 
char replayOS_Token[10] = "";
static String ultimoJuegoCargado = "";
static int replayOSFallosConsecutivos = 0;
static unsigned long replayOSIntervaloActual = 3000;

bool textEnable = false;
String textoScrollMsg = "";
String textoScrollMsgProcesado = "";
uint16_t textoScrollAnchoCache = 0;
uint32_t textoScrollColor = 0xFFFFFF;
int textoScrollVelocidad = 30;
int textoScrollX = 0;
int textoScrollAncho = 0;
unsigned long textoScrollUltimoPaso = 0;
const GFXfont* textoScrollFuente = &fuente8pt7b_Bold;

bool timerEnable = false;
int hOn = 9, mOn = 0;    // Power-on time (default 09:00)
int hOff = 23, mOff = 0; // Power-off time (default 23:00)
bool isSleeping = false;
bool manualOverride = false; // Tracks whether the panel was woken by the button

unsigned long gifCachePosition = 0;
int gifsPlayed = 0;
int x_offset = 0;
int y_offset = 0;
const char* ntpServer = "pool.ntp.org";
WebServer server(80);
WiFiServer tcpServer(8888);

// OSD state and navigation variables
bool confiAppEnable = true;
enum EstadoSistema {
    ESTADO_GIFS,
    ESTADO_ARCADE,
    ESTADO_TEXTO,
    ESTADO_CONFIG_APP,
    ESTADO_MENU_PRINCIPAL,
    ESTADO_SUBMENU_PLAYLIST,
    ESTADO_SUBMENU_REPRODUCCION,
    ESTADO_SUBMENU_BRILLO,
    ESTADO_SUBMENU_RELOJ,
    ESTADO_SUBMENU_TIEMPO,
    ESTADO_SUBMENU_WIFI,
    ESTADO_SUBMENU_TEMPORIZADOR,
    ESTADO_SUBMENU_AVANZADO,
    ESTADO_SUBMENU_MAPEADO_IR,
    ESTADO_SUBMENU_ACTUALIZACION,
    ESTADO_SUBMENU_FTP,
    ESTADO_SUBMENU_IDIOMA
};


EstadoSistema estadoActual = ESTADO_GIFS;


// Navigation variables
int cursorPrincipal = 0; // Arrow position in the main menu
int cursorSubmenu = 0;   // Arrow position within the submenus
bool requiereReinicio = false; // Set when Advanced settings are changed
bool confirmadoLargo = false;      // Trigger at 1s
bool confirmadoExtraLargo = false; // Trigger at 4s
bool botonPresionado = false;
bool bloqueoPostSalida = false;
unsigned long tiempoPresionado = 0;
unsigned long ultimoIncremento = 0;
bool saliendoAGifs = false;
bool menuNeedsRedraw = false;

// Memory storage system variables
char playlistActiva[128] = "";
bool interrumpirReproduccion = false;
bool modoMantenimiento = false;

// Language variables
JsonDocument idiomaDoc; 
bool idiomaCargado = false;

// FTP variables
FtpServer ftpSrv;

// Dynamic reading variables
std::vector<String> listaPlaylists;
std::vector<String> listaIdiomas;

// Weather variables
int weatherEnable = 0;
char weatherCustomMsg[32] = "";
char weatherCity[64] = "";
char weatherKey[48] = "";
int weatherInterval = 60; // In minutes
float currentTemp = 0.0;
int weatherConditionCode = 0;
bool isNight = false;
unsigned long lastWeatherUpdate = 0;
bool weatherDataReady = false;

// 8x8 pixel icons
const uint8_t icon_sun[] PROGMEM = {0x00, 0x3c, 0x7e, 0x7e, 0x7e, 0x7e, 0x3c, 0x00};
const uint8_t icon_cloud[] PROGMEM = {0x00, 0x00, 0x1c, 0x3f, 0x7f, 0x7f, 0x00, 0x00};
const uint8_t icon_rain[] PROGMEM = {0x1c, 0x3f, 0x7f, 0x7f, 0x22, 0x44, 0x22, 0x00};
const uint8_t icon_snow[] PROGMEM = {0x24, 0x00, 0xbd, 0x3c, 0x3c, 0xbd, 0x00, 0x24};
const uint8_t icon_storm[] PROGMEM = {0x1c, 0x3f, 0x7f, 0x1c, 0x1c, 0x08, 0x10, 0x00};
const uint8_t icon_fog[] PROGMEM = {0x00, 0x3e, 0x00, 0x7f, 0x00, 0x1c, 0x3e, 0x00};
const uint8_t icon_moon[] PROGMEM = {0x1c, 0x38, 0x70, 0x70, 0x70, 0x38, 0x1c, 0x00};

// --- CLOCK CONFIGURATION ---
const uint8_t font5x8[11][5] PROGMEM = {
  {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
  {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
  {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
  {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
  {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
  {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
  {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
  {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
  {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
  {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
  {0x00, 0x36, 0x36, 0x00, 0x00}  // :
};

// Clock color control variables
uint32_t parseHexColor(String hex) {
  if (hex.startsWith("#")) hex.remove(0, 1);
  if (hex.length() != 6) return 0xFF0055;
  return strtoul(hex.c_str(), NULL, 16);
}

struct PresetColor {
    const char* nombre;
    uint32_t colorRGB; 
};

PresetColor listaColores[] = {
    {"BLANCO",   0xFFFFFF},
    {"ROJO",     0xFF0000},
    {"VERDE",    0x00FF00},
    {"AZUL",     0x0000FF},
    {"AMARILLO", 0xFFFF00},
    {"CIAN",     0x00FFFF},
    {"MAGENTA",  0xFF00FF},
    {"NARANJA",  0xFF8000},
    {"ROSA",     0xFFC0CB}
};
const int TOTAL_COLORES = sizeof(listaColores) / sizeof(listaColores[0]);

int clockColorIndex = 0;

// --- IR REMOTE CONTROL DYNAMIC VARIABLES ---
uint32_t ir_btn_on     = 0xF20DFF00;
uint32_t ir_btn_off    = 0xE01FFF00;
uint32_t ir_btn_up     = 0xF609FF00;
uint32_t ir_btn_down   = 0xE21DFF00;
uint32_t ir_btn_menu   = 0xEA15FF00;
uint32_t ir_btn_ok     = 0xED12FF00;
uint32_t ir_btn_subir  = 0xE41BFF00;
uint32_t ir_btn_bajar  = 0xB34CFF00;

// Mapping state variable
int pasoMapeo = -1; // -1 means we are navigating, >= 0 means we are capturing
unsigned long tiempoInicioMapeo = 0;
const unsigned long TIMEOUT_MAPEO = 10000; // 10 seconds

// ====================================================================
//                     CONFIG.INI MANAGER
// ====================================================================
void leerConfigIni() {
    File configFile = SD.open(CONFIG_FILE, FILE_READ);
    if (!configFile) {
        Serial.println(F("[INI] Error: config.ini not found. Using default values."));
        return;
    }

    Serial.println(F("[INI] Loading configuration..."));

    while (configFile.available()) {
        String linea = configFile.readStringUntil('\n');
        linea.trim();
        
        // Ignore comments, section headers, or empty lines
        if (linea.startsWith("#") || linea.startsWith("[") || linea.length() == 0) continue;

        // Find the '=' separator
        int separatorIndex = linea.indexOf('=');
        if (separatorIndex == -1) continue;

        String clave = linea.substring(0, separatorIndex);
        String valor = linea.substring(separatorIndex + 1);
        
        // Remove extra spaces
        clave.trim();
        valor.trim();

        // Remove inline comments if present (e.g. BRIGHTNESS=40 # Brightness)
        int commentIdx = valor.indexOf('#');
        if (commentIdx != -1) {
            valor = valor.substring(0, commentIdx);
            valor.trim();
        }

        // --- DIRECT MAPPING TO GLOBAL VARIABLES ---
        // [WIFI_NTP]
        if (clave == "WIFI_ENABLE") wifiEnable = valor.toInt();
        else if (clave == "SSID") strlcpy(wifi_ssid,  valor.c_str(), sizeof(wifi_ssid));
        else if (clave == "PASS") strlcpy(wifi_pass,  valor.c_str(), sizeof(wifi_pass));
        else if (clave == "MOSTRAR_IP") mostrarIP = valor.toInt();
        else if (clave == "TZ") strlcpy(time_zone,  valor.c_str(), sizeof(time_zone));
          
        // [HARDWARE]
        else if (clave == "PANEL_CHAIN") panelChain = valor.toInt();
        else if (clave == "COLOR_ORDER") strlcpy(colorOrder, valor.c_str(), sizeof(colorOrder));
        else if (clave == "BRIGHTNESS") brightness = valor.toInt();
        else if (clave == "I2S_SPEED") i2sSpeed = valor.toInt();
        else if (clave == "REFRESH_MIN") refreshMin = valor.toInt();
        else if (clave == "DOUBLE_BUFF") doubleBuff = valor.toInt();
        else if (clave == "LATCH_BLANK") latchBlank = valor.toInt();

        // [LOGIC]
        else if (clave == "PLAY_MODE") modoVisual = valor.toInt();
        else if (clave == "CONFI_APP_ENABLE") confiAppEnable = valor.toInt();
        else if (clave == "ARCADE_ENABLE") arcadeEnable = valor.toInt();
        else if (clave == "TEXT_ENABLE") textEnable = valor.toInt();
        else if (clave == "CLOCK_ENABLE") clockEnable = valor.toInt();
        else if (clave == "RANDOM_MODE") randomMode = valor.toInt();
        else if (clave == "AUTO_CLOCK_INT") autoClockInt = valor.toInt();
        else if (clave == "CLOCK_DURATION") clockDuration = valor.toInt();
        else if (clave == "CLOCK_STYLE") clockStyle = valor.toInt();
        else if (clave == "TRANSITION_ENABLE") transitionEnable = valor.toInt();
        else if (clave == "CLOCK_COLOR") { clockColorIndex = valor.toInt();
            if (clockColorIndex < 0 || clockColorIndex >= TOTAL_COLORES) {
                clockColorIndex = 0; 
            }
            clockColor = listaColores[clockColorIndex].colorRGB;
        }

        // [WEATHER]
        else if (clave == "WEATHER_ENABLE") weatherEnable = valor.toInt();
        else if (clave == "CITY") strlcpy(weatherCity, valor.c_str(), sizeof(weatherCity));
        else if (clave == "API_KEY") strlcpy(weatherKey, valor.c_str(), sizeof(weatherKey));
        else if (clave == "WEATHER_INT") weatherInterval = valor.toInt();
        else if (clave == "WEATHER_MSG") strlcpy(weatherCustomMsg, valor.c_str(), sizeof(weatherCustomMsg));

        // [LANGUAGE]
        else if (clave == "LANGUAGE") strlcpy(idiomaActivo, valor.c_str(), sizeof(idiomaActivo));

        // [IR_REMOTE]
        else if (clave == "BTN_ON") ir_btn_on = strtoul(valor.c_str(), NULL, 16);
        else if (clave == "BTN_OFF") ir_btn_off = strtoul(valor.c_str(), NULL, 16);
        else if (clave == "BTN_BRILLO_UP") ir_btn_up = strtoul(valor.c_str(), NULL, 16);
        else if (clave == "BTN_BRILLO_DOWN") ir_btn_down = strtoul(valor.c_str(), NULL, 16);
        else if (clave == "BTN_MENU") ir_btn_menu = strtoul(valor.c_str(), NULL, 16);
        else if (clave == "BTN_OK") ir_btn_ok = strtoul(valor.c_str(), NULL, 16);
        else if (clave == "BTN_SUBIR") ir_btn_subir = strtoul(valor.c_str(), NULL, 16);
        else if (clave == "BTN_BAJAR") ir_btn_bajar = strtoul(valor.c_str(), NULL, 16);

        // [REPLAY_OS]
        else if (clave == "IP") strlcpy(replayOS_IP,  valor.c_str(), sizeof(replayOS_IP));
        else if (clave == "TOKEN") strlcpy(replayOS_Token,  valor.c_str(), sizeof(replayOS_Token));

    }

    configFile.close();


    // POST-READ SAFETY VALIDATION
    if (colorOrder[0] == '\0') strlcpy(colorOrder, "RGB", sizeof(colorOrder));
    if (weatherCustomMsg[0]== '\0') strlcpy(weatherCustomMsg,"Game Room", sizeof(weatherCustomMsg));
    if (idiomaActivo[0] == '\0') strlcpy(idiomaActivo, "ES", sizeof(idiomaActivo));

    if (doubleBuff && i2sSpeed == 3) {
        i2sSpeed = 2; // Force 16MHz if Double Buffer is enabled
        Serial.println(F("[SAFETY] Incompatible configuration detected on SD: Lowering I2S to 16MHz."));
    }
    
    Serial.println(F("[INI] Loading complete."));
}

void guardarConfigIni() {
    File configFile = SD.open(CONFIG_FILE, FILE_WRITE);
    if (!configFile) {
        Serial.println(F("[ERROR] Could not open config.ini for writing"));
        return;
    }

    Serial.println(F("[INI] Saving new settings to SD..."));

    // Main header
    configFile.println(F("# ============================================================"));
    configFile.printf(F("# 🕹️ RETRO PIXEL LED LITE - CONFIGURATION v%s\n"), FIRMWARE_VERSION);
    configFile.println(F("# ============================================================"));
    configFile.println(F("# Note: Do not leave spaces around the '=' symbol."));
    configFile.println(F("# Correct example: BRIGHTNESS=40\n"));

    configFile.println(F("[WIFI_NTP]"));
    configFile.println(F("# Configure your WiFi network"));
    configFile.printf("WIFI_ENABLE=%d\n", wifiEnable);
    configFile.printf("SSID=%s\n", wifi_ssid);
    configFile.printf("PASS=%s\n", wifi_pass);
    configFile.println(F("# Show IP on startup"));
    configFile.printf("MOSTRAR_IP=%d\n", mostrarIP ? 1 : 0);
    configFile.println(F("# Configure your time zone"));
    configFile.printf("TZ=%s\n\n", time_zone);

    configFile.println(F("[HARDWARE]"));
    configFile.println(F("# Number of LED panels"));
    configFile.printf("PANEL_CHAIN=%d\n", panelChain);
    configFile.println(F("# Panel color order: RGB, RBG or GBR"));
    configFile.printf("COLOR_ORDER=%s\n", colorOrder);
    configFile.println(F("# Brightness (0 to 255)"));
    configFile.printf("BRIGHTNESS=%d\n", brightness);
    configFile.println(F("# I2S speed: 0=8MHz, 1=10MHz, 2=16MHz, 3=20MHz (Turbo)"));
    configFile.printf("I2S_SPEED=%d\n", i2sSpeed);
    configFile.println(F("# Minimum refresh rate (Hz): 30 to 120"));
    configFile.printf("REFRESH_MIN=%d\n", refreshMin);
    configFile.println(F("# Double Buffer: 0=OFF, 1=ON (Eliminates flicker)"));
    configFile.printf("DOUBLE_BUFF=%d\n", doubleBuff);
    configFile.println(F("# Anti-Ghosting: 1 to 4 (Increase if you see ghosting)"));
    configFile.printf("LATCH_BLANK=%d\n\n", latchBlank);

    configFile.println(F("[LOGIC]"));
    configFile.println(F("# Display mode: 0=GIFs, 1=Clock only"));
    configFile.printf("PLAY_MODE=%d\n", modoVisual);
    configFile.println(F("# Enable or disable configuration via the APP: 0=OFF, 1=ON (Requires WiFi)"));
    configFile.printf("CONFI_APP_ENABLE=%d\n", confiAppEnable ? 1 : 0);
    configFile.println(F("# Select your Arcade system: 0=OFF, 1=Batocera, 2=Recalbox, 3=ReplayOS"));
    configFile.printf("ARCADE_ENABLE=%d\n", arcadeEnable);
    configFile.println(F("# Enable or disable scrolling text: 0=OFF, 1=ON (Requires WiFi)"));
    configFile.printf("TEXT_ENABLE=%d\n", textEnable ? 1 : 0);
    configFile.println(F("# Enable or disable the clock: 0=OFF, 1=ON (Requires WiFi)"));
    configFile.printf("CLOCK_ENABLE=%d\n", clockEnable);
    configFile.println(F("# Playback mode: 0=Sequential, 1=Random"));
    configFile.printf("RANDOM_MODE=%d\n", randomMode);
    configFile.println(F("# Interval: How many GIFs before the clock appears"));
    configFile.printf("AUTO_CLOCK_INT=%d\n", autoClockInt);
    configFile.println(F("# Duration: How many seconds the clock is displayed"));
    configFile.printf("CLOCK_DURATION=%d\n", clockDuration);
    configFile.println(F("# Styles: 0=Solid, 1=Aurora, 2=Rainbow, 3=Gradient, 4=Fire, 5=Glitch"));
    configFile.printf("CLOCK_STYLE=%d\n", clockStyle);
    configFile.println(F("# Enable the clock-to-GIF transition with a particle explosion: 0=OFF, 1=ON"));
    configFile.printf("TRANSITION_ENABLE=%d\n", transitionEnable);
    configFile.println(F("# Clock color (0=White, 1=Red, 2=Green, 3=Blue, 4=Yellow, 5=Cyan, 6=Magenta, 7=Orange, 8=Pink)"));
    configFile.printf("CLOCK_COLOR=%d\n\n", clockColorIndex); ; 

    configFile.println(F("[WEATHER]"));
    configFile.println(F("# Enable weather: 0=OFF, 1=ON (Requires WiFi)"));
    configFile.printf("WEATHER_ENABLE=%d\n", weatherEnable);
    configFile.println(F("# Your city (No spaces, use '+' if needed: Madrid,ES or Buenos+Aires,AR)"));
    configFile.printf("CITY=%s\n", weatherCity);
    configFile.println(F("# Your free OpenWeatherMap API key"));
    configFile.printf("API_KEY=%s\n", weatherKey);
    configFile.println(F("# Weather update interval in MINUTES"));
    configFile.printf("WEATHER_INT=%d\n", weatherInterval);
    configFile.println(F("# Text displayed above the clock"));
    configFile.printf("WEATHER_MSG=%s\n\n", weatherCustomMsg);

    configFile.println(F("[LANGUAGE]"));
    configFile.println(F("# Specify the language (File name without .json: ES, EN, FR...)"));
    configFile.printf("LANGUAGE=%s\n\n", idiomaActivo);

    configFile.println(F("[IR_REMOTE]"));
    configFile.println(F("# IR remote HEX codes (DO NOT enter anything; Retro Pixel LED will save them automatically)"));
    configFile.printf("BTN_ON=%08X\n", ir_btn_on);
    configFile.printf("BTN_OFF=%08X\n", ir_btn_off);
    configFile.printf("BTN_BRILLO_UP=%08X\n", ir_btn_up);
    configFile.printf("BTN_BRILLO_DOWN=%08X\n", ir_btn_down);
    configFile.printf("BTN_MENU=%08X\n", ir_btn_menu);
    configFile.printf("BTN_OK=%08X\n", ir_btn_ok);
    configFile.printf("BTN_SUBIR=%08X\n", ir_btn_subir);
    configFile.printf("BTN_BAJAR=%08X\n\n", ir_btn_bajar);

    configFile.println(F("[REPLAY_OS]"));
    configFile.println(F("# IP address assigned to ReplayOS"));
    configFile.printf("IP=%s\n", replayOS_IP);
    configFile.println(F("# ReplayOS token: SYSTEM > INFORMATION > NET CONTROL CODE"));
    configFile.printf("TOKEN=%s\n\n", replayOS_Token);

    configFile.println(F("[END]"));
    configFile.close();
    
    Serial.println(F("[INI] Saved successfully."));
}

void mostrarPantallaConfigApp() {
    display->fillScreen(0);
    display->setTextSize(1);
    printMenuCentrado("APP CONFIG", 12, display->color565(0, 255, 255), offset);
    display->flipDMABuffer();
}

// ====================================================================
//                        LANGUAGE MANAGEMENT
// ====================================================================

// Function to automatically center text on 64px panels
void printMenuCentrado(const char* texto, int y, uint16_t color, int xOffset) {
    int numLetras = strlen(texto);
    // Each character is 6px wide
    int xCalculada = (PANEL_RES_X - (numLetras * 6/2));
    if (xCalculada < 0) xCalculada = 0; // Prevent going off the left edge
    display->setCursor(xOffset + xCalculada, y);
    display->setTextColor(color);
    display->print(texto);
}

const char* msg(const char* seccion, const char* clave) { 
    if (!idiomaCargado) return "---";
    
    const char* valor = idiomaDoc[seccion][clave];
    
    if (valor != nullptr) {
        return valor;
    }
    
    Serial.printf("[LANGUAGE] Not found: %s -> %s\n", seccion, clave);
    return "ErrorText"; 
}

void cargarIdiomaMenu() {
    if (!idiomaCargado) {
        char ruta[40];
        snprintf(ruta, sizeof(ruta), "/idioma/%s.json", idiomaActivo);
        File file = SD.open(ruta);
        
        if (!file) {
            Serial.print(F("[LANGUAGE] Error: Does not exist ")); 
            Serial.println(ruta);
            return;
        }

        DeserializationError error = deserializeJson(idiomaDoc, file);
        file.close();

        if (error) {
            Serial.print(F("[LANGUAGE] Deserialization error: "));
            Serial.println(error.c_str());
            
        } else {
            idiomaDoc.shrinkToFit(); // Free unused space from the reserved buffer
            idiomaCargado = true;
            Serial.println(F("[LANGUAGE] Loaded successfully"));
        }
    }
}

void liberarIdiomaMenu() {
    if (idiomaCargado) {
        idiomaDoc.clear(); // Free the internal memory
        idiomaCargado = false;
        Serial.println(F("[LANGUAGE] RAM freed."));
    }
}

// ====================================================================
//                      IR REMOTE FUNCTIONS
// ====================================================================

void chequearTimeoutMapeo() {
    // If we are waiting for an IR code (pasoMapeo != -1)
    if (pasoMapeo != -1) {
        if (millis() - tiempoInicioMapeo >= TIMEOUT_MAPEO) {
            // Panel drawing (OSD) only applies if mapping was started from the physical menu.
            // If it was started from the PWA, estadoActual does not change and we do not touch the display.
            if (estadoActual == ESTADO_SUBMENU_MAPEADO_IR) {
                display->fillScreen(0);
                printMenuCentrado("FAIL", 12, display->color565(255, 0, 0), offset);
                display->flipDMABuffer();
                delay(1000);
            }

            Serial.println(F("[IR] Timeout: Cancelling mapping due to inactivity."));
            pasoMapeo = -1;   // Cancel mapping mode

            if (estadoActual == ESTADO_SUBMENU_MAPEADO_IR) {
                dibujarMenuOSD();
            }
            
        }
    }
} 

void gestionarMapeoIR(uint32_t codigoHex) {
    // If no button is selected for mapping, ignore it
    if (pasoMapeo < 0 || pasoMapeo > 7) return;

    // Assign the received code to the corresponding global variable
    switch (pasoMapeo) {
        case 0: ir_btn_on    = codigoHex; break;
        case 1: ir_btn_off   = codigoHex; break;
        case 2: ir_btn_up    = codigoHex; break;
        case 3: ir_btn_down  = codigoHex; break;
        case 4: ir_btn_menu  = codigoHex; break;
        case 5: ir_btn_ok    = codigoHex; break;
        case 6: ir_btn_subir = codigoHex; break;
        case 7: ir_btn_bajar = codigoHex; break;
    }

    // Visual feedback: show a confirmation message
    display->fillScreen(0);
    printMenuCentrado("OK", 12, display->color565(0, 255, 0), offset);
    display->flipDMABuffer();
    
    Serial.printf("[IR] Mapped button %d with code: %08X\n", pasoMapeo, codigoHex);
    
    delay(1000); // Pause so the user can see the confirmation
    pasoMapeo = -1; // Return to navigation mode
    
    if (estadoActual == ESTADO_CONFIG_APP) {
        mostrarPantallaConfigApp();
    }else {
        dibujarMenuOSD();
    }
}

void procesarComandoIR(uint32_t codigoHex) {
    // Always capture when a button is selected for mapping (pasoMapeo != -1),
    // whether it comes from the OSD submenu or an /ir/learn request from the PWA.
    if (pasoMapeo != -1) {
        gestionarMapeoIR(codigoHex);
        return;
    }

    if (codigoHex == ir_btn_on) { 
        if (isSleeping) {
            toggleEnergia(false);
        }else {
            toggleEnergia(true);
        }
        manualOverride = true;
        }
    else if (codigoHex == ir_btn_off) { 
        if (!isSleeping) {
            toggleEnergia(true);
        }else {
            toggleEnergia(false);
        }
        manualOverride = true;
        }
    else if (codigoHex == ir_btn_up) { ajustarBrillo(1); }
    else if (codigoHex == ir_btn_down) { ajustarBrillo(-1); }
    else if (codigoHex == ir_btn_menu) {
        interrumpirReproduccion = true;
        cargarNombresPlaylists();
        cargarNombresIdiomas();
        cargarIdiomaMenu();
        estadoActual = ESTADO_MENU_PRINCIPAL;
        cursorPrincipal = 0;
    }
    else if (codigoHex == ir_btn_subir) { ejecutarAccionNavegar(-1); }
    else if (codigoHex == ir_btn_bajar) { ejecutarAccionNavegar(1); }
    else if (codigoHex == ir_btn_ok) { ejecutarAccionConfirmar(); }
}

void leerControlRemoto() {
    if (IrReceiver.decode()) {
        uint32_t codigoRecibido = IrReceiver.decodedIRData.decodedRawData;
        
        // Avoid processing empty codes or read errors (0x0)
        if (codigoRecibido != 0) {
            procesarComandoIR(codigoRecibido);
        }
        
        IrReceiver.resume(); // Prepare the receiver for the next code
    }
}

void ajustarBrillo(int direccion) {
    // 1. Convert the current value (0-255) to a percentage (0-100)
    int br = (brightness * 100) / 255;
    
    // 2. Apply a step of 5
    br += (direccion * 5); 
    
    // 3. Cyclic behavior, wrap around
    if (br > 100) br = 5; // If it exceeds 100, wrap to the minimum (5%)
    if (br < 5) br = 100; // If it goes below 5, wrap to the maximum (100%)
    
    // 4. Convert back to the 0-255 scale
    brightness = (br * 255) / 100;
    
    // 5. Apply to the panel
    display->setBrightness8(brightness);

    // 6. Redraw the menu
    menuNeedsRedraw = true;
    
    Serial.printf("[BRIGHTNESS] Set to: %d%% (%d/255)\n", br, brightness);
}

// ====================================================================
//              CONFIGURATION MENU FUNCTIONS
// ====================================================================
void cargarNombresIdiomas() {
    listaIdiomas.clear();
    
    File root = SD.open("/idioma");
    if (!root || !root.isDirectory()) {
        SD.mkdir("/idioma"); // If it does not exist, create it
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            String nombre = file.name();
            if (nombre.endsWith(".json")) {
                String limpio = nombre;
                limpio.replace("/idioma/", "");
                limpio.replace(".json", "");
                listaIdiomas.push_back(limpio);
            }
        }
        file = root.openNextFile();
    }
    root.close();
    listaIdiomas.shrink_to_fit();
}

void cargarNombresPlaylists() {
    listaPlaylists.clear();
    
    File root = SD.open("/playlists");
    if (!root || !root.isDirectory()) {
        SD.mkdir("/playlists"); // If it does not exist, create it
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            String nombre = file.name();
            if (nombre.endsWith(".txt")) {
                String limpio = nombre;
                limpio.replace("/playlists/", "");
                limpio.replace(".txt", "");
                listaPlaylists.push_back(limpio);
            }
        }
        file = root.openNextFile();
    }
    root.close();
    listaPlaylists.shrink_to_fit();
}

void gestionarBotonMenu() {
    bool lectura = (digitalRead(PIN_BOTON_MENU) == LOW);
    unsigned long ahora = millis();

    // 1. WHEN THE BUTTON IS RELEASED
    if (!lectura) {
        if (botonPresionado) {
            unsigned long duracionFinal = ahora - tiempoPresionado;
            
            // If sleep mode has NOT been activated (confirmadoExtraLargo)
            if (!confirmadoExtraLargo && !bloqueoPostSalida) {

                // SHORT PRESS (< 1 second)
                if (duracionFinal > 50 && duracionFinal < 1000) {
                    
                     // A. If sleeping, wake up
                    if (isSleeping) {
                        toggleEnergia(false); // Wake up
                        manualOverride = true;
                    }
                    // B. If in GIF or Arcade mode -> Enter the Main Menu
                    else if (estadoActual == ESTADO_GIFS || estadoActual == ESTADO_ARCADE) {
                        ejecutarAccionConfirmar(); 
                    } 
                    else {
                        // C. If INSIDE a menu -> Navigate through the options
                        ejecutarAccionNavegar(1);
                    }
                }
            }
            botonPresionado = false;
        }
        bloqueoPostSalida = false; 
        return;
    }

    // 2. BUTTON PRESS START
    if (bloqueoPostSalida) return; 

    if (!botonPresionado) {
        tiempoPresionado = ahora;
        botonPresionado = true;
        confirmadoLargo = false;
        confirmadoExtraLargo = false;
        return;
    }

    // 3. WHILE HELD (TIME-BASED ACTIONS)
    unsigned long duracionActual = ahora - tiempoPresionado;

    // --- SLEEP MODE (2 SECONDS) ---
    // Only from the GIF/Arcade screen
    if ((estadoActual == ESTADO_GIFS || estadoActual == ESTADO_ARCADE) && duracionActual >= 2000 && !confirmadoExtraLargo) {
        confirmadoExtraLargo = true;
        confirmadoLargo = true; 
        toggleEnergia(!isSleeping);
        manualOverride = true;
        return;
    }

    // --- CONFIRM WITHIN MENUS (1 SECOND) ---
    // Hold for 1 second to enter submenus, save, or exit.
    if (estadoActual != ESTADO_GIFS && estadoActual != ESTADO_ARCADE && duracionActual >= 1000 && !confirmadoLargo) {
        bool esCampoHora = (estadoActual == ESTADO_SUBMENU_TEMPORIZADOR && (cursorSubmenu == 1 || cursorSubmenu == 2));
        
        if (esCampoHora) {
            ejecutarAccionConfirmar(); // Subtracts 5 min
            confirmadoLargo = true;
        } else {
            confirmadoLargo = true;
            ejecutarAccionConfirmar();
            return;
        }
    }

    // --- AUTO-INCREMENT (ADD 5 MINUTES) ---
    // Activates after 2 seconds to avoid conflicting with the subtract confirmation
    if (estadoActual == ESTADO_SUBMENU_TEMPORIZADOR && (cursorSubmenu == 1 || cursorSubmenu == 2) && duracionActual > 2000) {
        if (ahora - ultimoIncremento > 200) { 
            if (cursorSubmenu == 1) { // Increment ON
                mOn += 5;
                if (mOn >= 60) { mOn = 0; hOn++; }
                if (hOn >= 24) hOn = 0;
            } 
            else if (cursorSubmenu == 2) { // Increment OFF
                mOff += 5;
                if (mOff >= 60) { mOff = 0; hOff++; }
                if (hOff >= 24) hOff = 0;
            }
            ultimoIncremento = ahora;
            dibujarMenuOSD(); 
        }
    }
}

void ejecutarAccionConfirmar() {

    switch (estadoActual) {
        case ESTADO_GIFS:
        case ESTADO_ARCADE:
            interrumpirReproduccion = true;
            cargarNombresPlaylists();
            cargarNombresIdiomas();
            cargarIdiomaMenu();
            estadoActual = ESTADO_MENU_PRINCIPAL;
            cursorPrincipal = 0;
            break;

        case ESTADO_MENU_PRINCIPAL:
            // Depending on the cursor position, enter a submenu or exit
            if (cursorPrincipal == 0) { estadoActual = ESTADO_SUBMENU_PLAYLIST; cursorSubmenu = 0; }
            else if (cursorPrincipal == 1) { estadoActual = ESTADO_SUBMENU_REPRODUCCION; cursorSubmenu = 0; }
            else if (cursorPrincipal == 2) { estadoActual = ESTADO_SUBMENU_BRILLO; }
            else if (cursorPrincipal == 3) { estadoActual = ESTADO_SUBMENU_WIFI; cursorSubmenu = 0; }
            else if (cursorPrincipal == 4) { estadoActual = ESTADO_SUBMENU_RELOJ; cursorSubmenu = 0;}
            else if (cursorPrincipal == 5) { estadoActual = ESTADO_SUBMENU_TIEMPO; cursorSubmenu = 0;}
            else if (cursorPrincipal == 6) { estadoActual = ESTADO_SUBMENU_TEMPORIZADOR; cursorSubmenu = 0;}
            else if (cursorPrincipal == 7) { estadoActual = ESTADO_SUBMENU_AVANZADO; cursorSubmenu = 0;}
            else if (cursorPrincipal == 8) { estadoActual = ESTADO_SUBMENU_ACTUALIZACION; cursorSubmenu = 0;}
            else if (cursorPrincipal == 9) { estadoActual = ESTADO_SUBMENU_FTP; cursorSubmenu = 0;}
            else if (cursorPrincipal == 10) { estadoActual = ESTADO_SUBMENU_IDIOMA; cursorSubmenu = 0;}
            else if (cursorPrincipal == 11) { // SAVE AND EXIT
                Serial.println(F("[MENU] Saving settings..."));
                guardarConfigIni(); 
                guardarAjustesTimer();
        
                if (requiereReinicio) {
                    display->fillScreen(0);
                    printMenuCentrado(msg("ESTADOS", "reinicio"), 12, display->color565(255, 255, 255), offset);
                    display->flipDMABuffer();
                    delay(1000);
                    ESP.restart();
                } else {
                    // Visual feedback: Confirmation blink
                    for(int i = 0; i < 3; i++) { 
                        display->fillScreen(0); 
                        display->flipDMABuffer(); 
                        delay(80); 
                        dibujarMenuOSD(); 
                        delay(80); 
                    }

                    while(digitalRead(PIN_BOTON_MENU) == LOW) { delay(10); }
                    liberarIdiomaMenu();
                    estadoActual = ESTADO_GIFS;
                    saliendoAGifs = true;
                }

            }else if (cursorPrincipal ==12) { // EXIT WITHOUT SAVING
                // Visual feedback: Confirmation blink
                for(int i = 0; i < 3; i++) { 
                    display->fillScreen(0); 
                    display->flipDMABuffer(); 
                    delay(80); 
                    dibujarMenuOSD(); 
                    delay(80); 
                }

                Serial.println(F("[MENU] Exiting without saving..."));

                while(digitalRead(PIN_BOTON_MENU) == LOW) { delay(10); }
                // Reload the original file from the SD card to discard changes in memory
                leerConfigIni();
                liberarIdiomaMenu();
                estadoActual = ESTADO_GIFS; 
                saliendoAGifs = true;       
            }

        break;

        case ESTADO_SUBMENU_PLAYLIST:
            // Check whether the cursor is on an actual playlist
            if (cursorSubmenu < listaPlaylists.size()) {       
                // 1. Build the path
                snprintf(playlistActiva, sizeof(playlistActiva),
                        "/playlists/%s.txt", listaPlaylists[cursorSubmenu].c_str());
        
                // 2. Save to persistent storage (Preferences)
                Preferences prefs;
                prefs.begin("retro-lite", false);
                prefs.putString("lastList", playlistActiva);
                prefs.end();
        
                // 3. Visual feedback: Confirmation blink
                for(int i = 0; i < 3; i++) { 
                    display->fillScreen(0); 
                    display->flipDMABuffer(); 
                    delay(80); 
                    dibujarMenuOSD(); 
                    delay(80); 
                }
        
                // 4. Reset indices and exit to playback
                while(digitalRead(PIN_BOTON_MENU) == LOW) { delay(10); }
                liberarIdiomaMenu();
                estadoActual = ESTADO_GIFS;
                saliendoAGifs = true;
                Serial.print(F("[PLAYLIST] Selected: ")); 
                Serial.println(playlistActiva);
            }else {
                
                estadoActual = ESTADO_MENU_PRINCIPAL;
                Serial.println(F("[PLAYLIST] Returning to main menu without changes."));
            }
        break;

        case ESTADO_SUBMENU_REPRODUCCION:
            if (cursorSubmenu == 0) {
                modoVisual = !modoVisual; // Switches between 0 (GIFs) and 1 (Clock)
            } else if (cursorSubmenu == 1) {
                randomMode = !randomMode; // Switches between 0 and 1
            } else if (cursorSubmenu == 2) {
                arcadeEnable++;
                if (arcadeEnable > 3) arcadeEnable = 0; // 0=OFF, 1=Batocera, 2=Recalbox, 3=ReplayOS
                requiereReinicio = true;
            } else if (cursorSubmenu == 3) {
                textEnable = !textEnable;
                requiereReinicio = true;
            } else {
                estadoActual = ESTADO_MENU_PRINCIPAL;
            }
            break;

        case ESTADO_SUBMENU_WIFI:
            if (cursorSubmenu == 0) {
                // If it was 0 and we change it to 1, enable the restart
                if (!wifiEnable) { 
                    requiereReinicio = true; 
                }
                wifiEnable = !wifiEnable;
            } else if (cursorSubmenu == 1) {
                if (!mostrarIP) { 
                    requiereReinicio = true; 
                }
                mostrarIP = !mostrarIP;
            } else if (cursorSubmenu == 2) {
    
            } else if (cursorSubmenu == 3) {
                confiAppEnable = !confiAppEnable;
                requiereReinicio = true;
            } else {
                estadoActual = ESTADO_MENU_PRINCIPAL;
            }
            break;     

        case ESTADO_SUBMENU_RELOJ:
            if (cursorSubmenu == 0) {
                clockEnable = !clockEnable;
            } else if (cursorSubmenu == 1) {
                autoClockInt += 2;
                if (autoClockInt > 10) autoClockInt = 2;
            } else if (cursorSubmenu == 2) {
                clockDuration += 5;
                if (clockDuration > 30) clockDuration = 5; // Wrap from 5s to 30s
            } else if (cursorSubmenu == 3) {
                clockStyle++;
                if (clockStyle > 5) clockStyle = 0;
            } else if (cursorSubmenu == 4) {
                clockColorIndex = (clockColorIndex + 1) % TOTAL_COLORES;
                clockColor = listaColores[clockColorIndex].colorRGB;
            } else if (cursorSubmenu == 5) {
               transitionEnable = !transitionEnable;
               requiereReinicio = true;
            } else if (cursorSubmenu == 6) {
                estadoActual = ESTADO_MENU_PRINCIPAL;
            }
            break;

        case ESTADO_SUBMENU_TIEMPO:
            if (cursorSubmenu == 0) {
                // If it was 0 and we change it to 1, enable the restart
                if (!weatherEnable) { 
                    requiereReinicio = true; 
                }
                weatherEnable = !weatherEnable;
            } else {
                estadoActual = ESTADO_MENU_PRINCIPAL;
            }
            break;

        case ESTADO_SUBMENU_TEMPORIZADOR:
            if (cursorSubmenu == 0) {
                timerEnable = !timerEnable;
            }else if (cursorSubmenu == 1) { // --- SUBTRACT 5 min FROM ON ---
                if (mOn == 0) {
                    mOn = 55; // Wrap to the last part of the previous hour
                    if (hOn == 0) hOn = 23;
                    else hOn--;
                } else {
                    mOn -= 5;
                }
            }else if (cursorSubmenu == 2) { // --- SUBTRACT 5 min FROM OFF ---
                if (mOff == 0) {
                    mOff = 55; // Wrap to the last part of the previous hour
                    if (hOff == 0) hOff = 23;
                    else hOff--;
                } else {
                    mOff -= 5;
                }
            }else if (cursorSubmenu == 3) {
                estadoActual = ESTADO_MENU_PRINCIPAL;
            }
            break;

        case ESTADO_SUBMENU_AVANZADO:
            if (cursorSubmenu == 0) {
                i2sSpeed++; 
                // If we reach 20MHz (3) with Double Buffer enabled, jump directly to 8MHz (0)
                if (i2sSpeed > 3 || (i2sSpeed == 3 && doubleBuff == true)) {
                i2sSpeed = 0; 
                if (doubleBuff) Serial.println(F("[SAFETY] Skipping 20MHz because Double Buffer is enabled."));
            }
            requiereReinicio = true;
            }else if (cursorSubmenu == 1) {
                refreshMin += 30; if (refreshMin > 120) refreshMin = 30;
                requiereReinicio = true;
            }else if (cursorSubmenu == 2) {
                doubleBuff = !doubleBuff;
                // SAFETY RULE: If we enable Double Buffer while the speed is 20MHz (3)
                if (doubleBuff == true && i2sSpeed == 3) {
                    i2sSpeed = 2; // Automatically lower to 16MHz
                    Serial.println(F("[SAFETY] Double Buffer enabled. Lowering I2S to 16MHz to prevent reboots."));
                }
                requiereReinicio = true;
            }else if (cursorSubmenu == 3) {
                latchBlank++; if (latchBlank > 4) latchBlank = 1;
                requiereReinicio = true;
            }else if (cursorSubmenu == 4) {
                estadoActual = ESTADO_SUBMENU_MAPEADO_IR;
                cursorSubmenu = 0;
            }else if (cursorSubmenu == 5) {
                Preferences prefs;
                prefs.begin("retro-lite", false);
                prefs.remove("lastList"); 
                prefs.end();
                ESP.restart(); // Immediate restart to clear
            }else if (cursorSubmenu == 6) {
                estadoActual = ESTADO_MENU_PRINCIPAL;
            }             
            break;
        
        case ESTADO_SUBMENU_MAPEADO_IR:
            if (cursorSubmenu == 0) {
                pasoMapeo = cursorSubmenu;
                tiempoInicioMapeo = millis();
                dibujarMenuOSD();
            } else if (cursorSubmenu == 1) {
                pasoMapeo = cursorSubmenu;
                tiempoInicioMapeo = millis();
                dibujarMenuOSD();
            } else if (cursorSubmenu == 2) {
                pasoMapeo = cursorSubmenu;
                tiempoInicioMapeo = millis();
                dibujarMenuOSD();
            } else if (cursorSubmenu == 3) {
                pasoMapeo = cursorSubmenu;
                tiempoInicioMapeo = millis();
                dibujarMenuOSD();
            } else if (cursorSubmenu == 4) {
                pasoMapeo = cursorSubmenu;
                tiempoInicioMapeo = millis();
                dibujarMenuOSD();
            } else if (cursorSubmenu == 5) {
                pasoMapeo = cursorSubmenu;
                tiempoInicioMapeo = millis();
                dibujarMenuOSD();
            } else if (cursorSubmenu == 6) {
                pasoMapeo = cursorSubmenu;
                tiempoInicioMapeo = millis();
                dibujarMenuOSD();
            } else if (cursorSubmenu == 7) {
                pasoMapeo = cursorSubmenu;
                tiempoInicioMapeo = millis();
                dibujarMenuOSD();                      
            } else if (cursorSubmenu == 8) {
                estadoActual = ESTADO_MENU_PRINCIPAL;
            }
            break;

        case ESTADO_SUBMENU_ACTUALIZACION:
            if (cursorSubmenu == 0) {
                display->fillScreen(0);
                printMenuCentrado(msg("OTA", "reset"), 8, display->color565(255, 255, 255), offset);
                printMenuCentrado(msg("OTA", "buscar"), 18, display->color565(255, 255, 255), offset);
                display->flipDMABuffer();
        
                delay(3000);
                triggerActualizacionOTA();
            } else if (cursorSubmenu == 1) {
                display->fillScreen(0);
                printMenuCentrado(msg("OTA", "reset"), 8, display->color565(255, 255, 255), offset);
                printMenuCentrado(msg("OTA", "buscar"), 18, display->color565(255, 255, 255), offset);
                display->flipDMABuffer();
        
                delay(3000);
                Preferences prefs;
                prefs.begin("sistema", false);
                prefs.remove("idiomas_res"); // Clear previous result
                prefs.remove("idiomas_ok");
                prefs.putBool("idiomas_pending", true);
                prefs.end();

                delay(300);
                ESP.restart();
            } else {
                estadoActual = ESTADO_MENU_PRINCIPAL;
            }
            break;
        
        case ESTADO_SUBMENU_FTP:
            if (cursorSubmenu == 0) {
                display->fillScreen(0);
                printMenuCentrado(msg("FTP", "reset"), 8, display->color565(255, 255, 255), offset);
                printMenuCentrado(msg("FTP", "iniciar"), 18, display->color565(255, 255, 255), offset);
                display->flipDMABuffer();

                Serial.println(F("[MENU] Enabling FTP Mode and restarting..."));

                Preferences prefs;
                prefs.begin("sistema", false);
                prefs.putBool("ftp_mode", true);
                prefs.end();

                delay(3000);
                ESP.restart();
            } else if (cursorSubmenu == 1) {
                estadoActual = ESTADO_MENU_PRINCIPAL;
            }
            break;

        case ESTADO_SUBMENU_IDIOMA:
            if (cursorSubmenu < listaIdiomas.size()) {
                strlcpy(idiomaActivo, listaIdiomas[cursorSubmenu].c_str(), sizeof(idiomaActivo));
        
                // Free the current JSON and load the new one so the menu changes immediately
                liberarIdiomaMenu();
                cargarIdiomaMenu();
        
                // Save to Preferences so it persists after reboot
                Preferences prefs;
                prefs.begin("retro-lite", false);
                prefs.putString("lang", idiomaActivo);
                prefs.end();
        
                // Visual feedback
                for(int i = 0; i < 3; i++) { 
                    display->fillScreen(0); display->flipDMABuffer(); delay(80); 
                    dibujarMenuOSD(); delay(80); 
                }
        
                estadoActual = ESTADO_MENU_PRINCIPAL;
            } else {
                estadoActual = ESTADO_MENU_PRINCIPAL;   
            }
            break;
        default:
            // In the other submenus, a long press returns to the main menu
            estadoActual = ESTADO_MENU_PRINCIPAL;
            break;
    }

    if (estadoActual != ESTADO_GIFS && estadoActual != ESTADO_ARCADE) {
        menuNeedsRedraw = true;
    }

}

void  ejecutarAccionNavegar(int paso) {

    switch (estadoActual) {
        case ESTADO_GIFS:
            // A short press while GIFs are playing also opens the menu
            interrumpirReproduccion = true;
            cargarNombresPlaylists();
            cargarNombresIdiomas();
            cargarIdiomaMenu();
            estadoActual = ESTADO_MENU_PRINCIPAL;
            cursorPrincipal = 0;
            break;

        case ESTADO_MENU_PRINCIPAL:
            cursorPrincipal += paso;
            if (cursorPrincipal > 12) cursorPrincipal = 0;
            if (cursorPrincipal < 0) cursorPrincipal = 12;
            break;

        case ESTADO_SUBMENU_PLAYLIST:
            cursorSubmenu += paso;
            if (cursorSubmenu > listaPlaylists.size()) cursorSubmenu = 0;
            if (cursorSubmenu < 0) cursorSubmenu = (int)listaPlaylists.size();
            break;

        case ESTADO_SUBMENU_REPRODUCCION:
            cursorSubmenu += paso;
            if (cursorSubmenu > 4) cursorSubmenu = 0;
            if (cursorSubmenu < 0) cursorSubmenu = 4;
            break;    

        case ESTADO_SUBMENU_BRILLO:
        {
            int br = (brightness * 100) / 255;
            // Brightness increases or decreases in steps of 5 depending on the button pressed
            br += (paso * 5); 
            // Cyclic behavior, wrap around
            if (br > 100) br = 5; // If it exceeds 100, wrap to the minimum
            if (br < 5) br = 100; // If it goes below 5, wrap to the maximum
            brightness = (br * 255) / 100;
            display->setBrightness8(brightness);
        }    
            break;

        case ESTADO_SUBMENU_WIFI:
            cursorSubmenu += paso;
            if (cursorSubmenu > 4) cursorSubmenu = 0;
            if (cursorSubmenu < 0) cursorSubmenu = 4;
            break;
                
        case ESTADO_SUBMENU_RELOJ:
            cursorSubmenu += paso;
            if (cursorSubmenu > 6) cursorSubmenu = 0;
            if (cursorSubmenu < 0) cursorSubmenu = 6;
            break;

        case ESTADO_SUBMENU_TIEMPO:
            cursorSubmenu += paso;
            if (cursorSubmenu > 1) cursorSubmenu = 0;
            if (cursorSubmenu < 0) cursorSubmenu = 1;
            break;

        case ESTADO_SUBMENU_TEMPORIZADOR:
            // If we try "Up" (step -1) while on the time rows (1 or 2)
            if (paso == -1 && (cursorSubmenu == 1 || cursorSubmenu == 2)) {
                if (cursorSubmenu == 1) { // ON row
                    mOn += 5;
                    if (mOn >= 60) { mOn = 0; hOn++; }
                    if (hOn >= 24) hOn = 0;
                } else { // OFF row
                    mOff += 5;
                    if (mOff >= 60) { mOff = 0; hOff++; }
                    if (hOff >= 24) hOff = 0;
                }
                // Do not move the cursor, only update the value
            } else {
                // In any other case
                cursorSubmenu += paso;
            if (cursorSubmenu > 3) cursorSubmenu = 0;
            if (cursorSubmenu < 0) cursorSubmenu = 3;
            }
            break;

        case ESTADO_SUBMENU_AVANZADO:
            cursorSubmenu += paso;
            if (cursorSubmenu > 6) cursorSubmenu = 0; 
            if (cursorSubmenu < 0) cursorSubmenu = 6;
            break;

        case ESTADO_SUBMENU_MAPEADO_IR:
            cursorSubmenu += paso;
            if (cursorSubmenu > 8) cursorSubmenu = 0; 
            if (cursorSubmenu < 0) cursorSubmenu = 8;
            break;

        case ESTADO_SUBMENU_ACTUALIZACION:
            cursorSubmenu += paso;
            if (cursorSubmenu > 2) cursorSubmenu = 0; 
            if (cursorSubmenu < 0) cursorSubmenu = 2; 
            break;

        case ESTADO_SUBMENU_FTP:
            cursorSubmenu += paso;
            if (cursorSubmenu > 1) cursorSubmenu = 0;
            if (cursorSubmenu < 0) cursorSubmenu = 1;
            break;

        case ESTADO_SUBMENU_IDIOMA:
            cursorSubmenu += paso;
            if (cursorSubmenu > (int)listaIdiomas.size()) cursorSubmenu = 0;
            if (cursorSubmenu < 0) cursorSubmenu = (int)listaIdiomas.size();
            break;
    }

    dibujarMenuOSD();
    
}

// ====================================================================
//                       OSD DRAWING ENGINE 
// ====================================================================
void dibujarMenuOSD() {

    // If we are waiting for an IR remote button press, show the waiting screen
    if (pasoMapeo != -1) {
        display->fillScreen(0);
        printMenuCentrado(msg("SUBMENU_MAPEADO_IR", "pulsar"), 12, display->color565(255, 255, 0), offset);
        display->flipDMABuffer();
        return; 
    }

    display->fillScreen(0); // Clean black background
    display->setTextSize(1); // Force text size to 1 in case we are coming from Text mode

    // Buffer for building dynamic titles with pagination
    char titleBuf[32]; 

    // ----------------------------------------------------------------
    // 1. DRAW MAIN MENU
    // ----------------------------------------------------------------
    if (estadoActual == ESTADO_MENU_PRINCIPAL) {
        
        // Pagination logic (3 items per page)
        int pagina = cursorPrincipal / 3; 
        int primerItem = pagina * 3;

        // Title with automatic centering. Uses the JSON text (e.g. "MENU PRINCIPAL ") and adds the page number
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/5", msg("MENU", "titulo"), pagina + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100)); // Separator

        // Draw the 3 items on the current page
        for (int i = 0; i < 3; i++) {
            int itemIndex = primerItem + i;
            if (itemIndex > 12) break; // There are 13 options (0 to 12)

            int yPos = 9 + (i * 8);

            // Draw the '>' cursor
            if (cursorPrincipal == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0)); // Yellow for the selection
                display->setCursor(offset + 2, yPos);
                display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // White for the selected text
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Gray for unselected items
            }

            display->setCursor(offset + 10, yPos);

            // Names and Quick Indicators
            switch (itemIndex) {
                case 0: display->print(msg("MENU", "playlists")); break;
                case 1: display->print(msg("MENU", "reproduccion")); break;
                case 2: 
                    // The JSON already includes "Brightness: "
                    display->printf("%s%d%%", msg("MENU", "brillo"), (brightness * 100) / 255); 
                    break;
                case 3: display->printf("%s[%s]", msg("MENU", "wifi"), wifiEnable ? msg("ESTADOS", "on") : msg("ESTADOS", "off")); break;
                case 4: display->printf("%s[%s]", msg("MENU", "reloj"), clockEnable ? msg("ESTADOS", "on") : msg("ESTADOS", "off")); break;
                case 5: display->printf("%s[%s]", msg("MENU", "clima"), weatherEnable ? msg("ESTADOS", "on") : msg("ESTADOS", "off")); break;
                case 6: display->printf("%s[%s]", msg("MENU", "timer"), timerEnable ? msg("ESTADOS", "on") : msg("ESTADOS", "off")); break;
                case 7: display->print(msg("MENU", "avanzado")); break;
                case 8: display->print(msg("MENU", "actualizacion")); break;
                case 9: display->print(msg("MENU", "ftp")); break;
                case 10: display->print(msg("MENU", "idioma")); break;
                case 11: display->print(msg("ESTADOS", "guardar")); break;
                case 12: display->print(msg("ESTADOS", "salir")); break;
            }
        }
    }
    // ----------------------------------------------------------------
    // 2. DRAW SUBMENU: PLAYLISTS 
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_PLAYLIST) {
        // Title with automatic centering
        printMenuCentrado(msg("SUBMENU_PLAYLIST", "titulo"), 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int totalOpciones = listaPlaylists.size() + 1; 
        int pag = cursorSubmenu / 3;
        int inicio = pag * 3;

        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex >= totalOpciones) break;

            int yPos = 9 + (i * 8);
            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0));
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255));
            } else {
                display->setTextColor(display->color565(150, 150, 150));
                display->setCursor(offset + 2, yPos); display->print(" ");
            }

            display->setCursor(offset + 10, yPos);
            if (itemIndex < listaPlaylists.size()) {
                display->print(listaPlaylists[itemIndex]);
            } else {
                display->print(msg("ESTADOS", "volver"));
            }
        }
    }
    // ----------------------------------------------------------------
    // 3. DRAW SUBMENU: PLAYBACK
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_REPRODUCCION) {
         int pagReproduccion = cursorSubmenu / 3; 

         // Title with automatic centering y paginado
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/3", msg("SUBMENU_REPRODUCCION", "titulo"), pagReproduccion + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int inicio = pagReproduccion * 3;
        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex > 4) break; 

            int yPos = 9 + (i * 8);

            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0)); // Yellow cursor
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // White text
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Gray text
                display->setCursor(offset + 2, yPos); display->print(" ");
            }

            display->setCursor(offset + 10, yPos);
            switch (itemIndex) {
                case 0: display->printf("%s%s", msg("SUBMENU_REPRODUCCION", "modo"), (modoVisual == 0 ? msg("ESTADOS", "modo_gifs") : msg("ESTADOS", "modo_reloj"))); break;
                case 1: display->printf("%s%s", msg("SUBMENU_REPRODUCCION", "aleatorio"), (randomMode ? msg("ESTADOS", "si") : msg("ESTADOS", "no"))); break;
                case 2: 
                    {
                        const char* nombresArcade[] = {"OFF", "Batocera", "Recalbox", "ReplayOS"};
                        // Ensure the index stays within the array bounds for safety
                        int indexArcade = (arcadeEnable >= 0 && arcadeEnable <= 3) ? arcadeEnable : 0;
                        display->printf("%s%s", msg("SUBMENU_REPRODUCCION", "arcade"), nombresArcade[indexArcade]);
                    }
                    break;
                case 3: display->printf("%s%s", msg("SUBMENU_REPRODUCCION", "texto"), (textEnable ? msg("ESTADOS","si") : msg("ESTADOS","no"))); break; 
                case 4: display->print(msg("ESTADOS", "volver")); break;
            }
        }
        
    }
    // ----------------------------------------------------------------
    // 4. DRAW SUBMENU: BRIGHTNESS
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_BRILLO) {
        // Title with automatic centering
        printMenuCentrado(msg("SUBMENU_BRILLO", "titulo"), 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int porcentaje = (brightness * 100) / 255;
        display->setTextColor(display->color565(255, 255, 255));
        display->setCursor(offset + 55, 10);
        display->printf("%d%%", porcentaje);

        // Draw the progress bar
        int anchoBarra = 108; // Total bar width in pixels
        int xBarra = offset + 10;
        int yBarra = 18;
        int hBarra = 4; // Bar height

        // Bar border (Dark gray)
        display->drawRect(xBarra, yBarra, anchoBarra, hBarra, display->color565(50, 50, 50));

        // Fill the bar according to the percentage
        int relleno = (porcentaje * (anchoBarra - 2)) / 100;
        display->fillRect(xBarra + 1, yBarra + 1, relleno, hBarra - 2, display->color565(0, 255, 0)); // Green
        
        // Information at the bottom
        display->setCursor(offset + 10, 25); 
        display->setTextColor(display->color565(150, 150, 150));
        display->print(msg("SUBMENU_BRILLO", "click_ok"));
    }
    // ----------------------------------------------------------------
    // 5. DRAW SUBMENU: WIFI
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_WIFI) {
        int pagWifi = cursorSubmenu / 3;

        // Title with automatic centering
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/2", msg("SUBMENU_WIFI", "titulo"), pagWifi + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int inicio = pagWifi * 3;
        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex > 4) break;
            int yPos = 9 + (i * 8);

            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0));  // Yellow cursor
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // White text
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Gray text
                display->setCursor(offset + 2, yPos); display->print(" ");
            }

            display->setCursor(offset + 10, yPos);
            switch (itemIndex) {
                case 0: display->printf("%s: %s", msg("ESTADOS", "activar"), (wifiEnable ? msg("ESTADOS", "si") : msg("ESTADOS", "no"))); break;
                case 1: display->printf("%s%s", msg("SUBMENU_WIFI", "mostrar_ip"), (mostrarIP ? msg("ESTADOS", "si") : msg("ESTADOS", "no"))); break;
                case 2: display->printf("IP: %s", WiFi.localIP().toString().c_str()); break;
                case 3: display->printf("%s%s", msg("SUBMENU_WIFI", "control_app"), (confiAppEnable ? msg("ESTADOS", "si") : msg("ESTADOS", "no"))); break;
                case 4: display->print(msg("ESTADOS", "volver")); break;
            } 
        }
     
    }
    // ----------------------------------------------------------------
    // 6. DRAW SUBMENU: CLOCK
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_RELOJ) {
        int pagReloj = cursorSubmenu / 3;

        // Title with automatic centering
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/2", msg("SUBMENU_RELOJ", "titulo"), pagReloj + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int inicio = pagReloj * 3;
        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex > 6) break; // Total of 7 options (0 to 6)
            int yPos = 9 + (i * 8);
            
            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0));  // Yellow cursor
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // White text
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Gray text
                display->setCursor(offset + 2, yPos); display->print(" ");
            }

            display->setCursor(offset + 10, yPos);
            switch (itemIndex) {
                case 0: display->printf("%s: %s", msg("ESTADOS", "activar"), (clockEnable ? msg("ESTADOS", "si") : msg("ESTADOS", "no"))); break;
                case 1: display->printf("%s%d GIF", msg("SUBMENU_RELOJ", "cada"), autoClockInt); break;
                case 2: display->printf("%s%ds", msg("SUBMENU_RELOJ", "ver"), clockDuration); break; 
                case 3:
                    {
                        const char* nombresEstilos[] = {"Solid", "Aurora", "Rainbow", "Gradient", "Fuego", "Glitch"};
                        display->printf("%s%s", msg("SUBMENU_RELOJ", "estilo"), nombresEstilos[clockStyle]); 
                    }
                    break;
                case 4: 
                    {
                        const char* llavesColores[] = {"blanco", "rojo", "verde", "azul", "amarillo", "cian", "magenta", "naranja", "rosa"};
                        display->printf("%s%s", msg("SUBMENU_RELOJ", "color"), msg("COLORES", llavesColores[clockColorIndex])); 
                    }
                    break;
                case 5: display->printf("%s%s", msg("SUBMENU_RELOJ", "transicion"), (transitionEnable ? msg("ESTADOS", "si") : msg("ESTADOS", "no"))); break;
                case 6: display->print(msg("ESTADOS", "volver")); break;
            }
        }
    }
    // ----------------------------------------------------------------
    // 7. DRAW SUBMENU: WEATHER
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_TIEMPO) {
        // Title with automatic centering
        printMenuCentrado(msg("SUBMENU_CLIMA", "titulo"), 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        // Option: Enable
        int y0 = 13;
        if (cursorSubmenu == 0) {
            display->setTextColor(display->color565(255, 255, 0)); // Yellow cursor
            display->setCursor(offset + 2, y0); display->print(">");
            display->setTextColor(display->color565(255, 255, 255)); // White text
        } else {
            display->setTextColor(display->color565(150, 150, 150)); // Gray text
            display->setCursor(offset + 2, y0); display->print(" ");
        }
        display->setCursor(offset + 10, y0);
        display->printf("%s: %s", msg("ESTADOS", "activar"), (weatherEnable ? msg("ESTADOS", "si") : msg("ESTADOS", "no")));

        // Option: Back
        int y1 = 23;
        if (cursorSubmenu == 1) {
            display->setTextColor(display->color565(255, 255, 0)); // Yellow cursor
            display->setCursor(offset + 2, y1); display->print(">");
            display->setTextColor(display->color565(255, 255, 255)); // White text
        } else {
            display->setTextColor(display->color565(150, 150, 150)); // Gray text
            display->setCursor(offset + 2, y1); display->print(" ");
        }
        display->setCursor(offset + 10, y1);
        display->print(msg("ESTADOS", "volver"));
    }
    // ----------------------------------------------------------------
    // 8. DRAW SUBMENU: TIMER
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_TEMPORIZADOR) {
        int pagTimer = cursorSubmenu / 3; 
        
        // Title with automatic centering y paginado
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/2", msg("SUBMENU_TEMPORIZADOR", "titulo"), pagTimer + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int inicio = pagTimer * 3;
        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex > 3) break; 

            int yPos = 9 + (i * 8);

            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0)); // Yellow cursor
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // White text
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Gray text
                display->setCursor(offset + 2, yPos); display->print(" ");
            }

            display->setCursor(offset + 10, yPos);
            switch (itemIndex) {
                case 0: display->printf("%s: %s", msg("ESTADOS", "activar"), (timerEnable ? msg("ESTADOS", "si") : msg("ESTADOS", "no"))); break;
                case 1: display->printf("ON:  %02d:%02d", hOn, mOn); break; // Fixed text is NOT in JSON
                case 2: display->printf("OFF: %02d:%02d", hOff, mOff); break; // Fixed text is NOT in JSON
                case 3: display->print(msg("ESTADOS", "volver")); break;
            }
        }
    }
    // ----------------------------------------------------------------
    // 9. DRAW SUBMENU: ADVANCED
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_AVANZADO) {
        int pagAv = cursorSubmenu / 3; 
        
        // Title with automatic centering y paginado
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/3", msg("SUBMENU_AVANZADO", "titulo"), pagAv + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int inicio = pagAv * 3;
        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex > 6) break; 

            int yPos = 9 + (i * 8);
            
            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0)); // Yellow cursor
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // White text
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Gray text
                display->setCursor(offset + 2, yPos); display->print(" ");
            }

            display->setCursor(offset + 10, yPos);
            switch (itemIndex) {
                case 0: {
                    const char* speeds[] = {"8MHz", "10MHz", "16MHz", "20MHz"};
                    display->printf("%s%s", msg("SUBMENU_AVANZADO", "Speed"), speeds[i2sSpeed]); 
                } break;
                case 1: display->printf("%s%dHz", msg("SUBMENU_AVANZADO", "refresco"), refreshMin); break;
                case 2: display->printf("%s%s", msg("SUBMENU_AVANZADO", "buffer"), doubleBuff ? msg("ESTADOS", "on") : msg("ESTADOS", "off")); break;
                case 3: display->printf("%s%d", msg("SUBMENU_AVANZADO", "antighos"), latchBlank); break;
                case 4: display->print(msg("SUBMENU_AVANZADO", "mapear")); break;
                case 5: display->print(msg("SUBMENU_AVANZADO", "reset")); break;
                case 6: display->print(msg("ESTADOS", "volver")); break;
            }
        }
    }
    // ----------------------------------------------------------------
    // 9.1.  DRAW SUBMENU: ADVANCED -> IR MAPPING
    // ----------------------------------------------------------------
    if (estadoActual == ESTADO_SUBMENU_MAPEADO_IR) {
        
        int pagAv = cursorSubmenu / 3; 
        
        // Title with automatic centering y paginado
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/3", msg("SUBMENU_MAPEADO_IR", "titulo"), pagAv + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int inicio = pagAv * 3;
        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex > 8) break; 

            int yPos = 9 + (i * 8);
            
            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0)); // Yellow cursor
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // White text
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Gray text
                display->setCursor(offset + 2, yPos); display->print(" ");
            }

            display->setCursor(offset + 10, yPos);
            switch (itemIndex) {
                case 0: display->printf("%s%d", msg("SUBMENU_MAPEADO_IR", "btn_on"), ir_btn_on); break;
                case 1: display->printf("%s%d", msg("SUBMENU_MAPEADO_IR", "btn_off"), ir_btn_off); break;
                case 2: display->printf("%s%d", msg("SUBMENU_MAPEADO_IR", "btn_brillo_up"), ir_btn_up); break;
                case 3: display->printf("%s%d", msg("SUBMENU_MAPEADO_IR", "btn_brillo_down"), ir_btn_down); break;
                case 4: display->printf("%s%d", msg("SUBMENU_MAPEADO_IR", "btn_menu"), ir_btn_menu); break;
                case 5: display->printf("%s%d", msg("SUBMENU_MAPEADO_IR", "btn_ok"), ir_btn_ok); break;
                case 6: display->printf("%s%d", msg("SUBMENU_MAPEADO_IR", "btn_subir"), ir_btn_subir); break;
                case 7: display->printf("%s%d", msg("SUBMENU_MAPEADO_IR", "btn_bajar"), ir_btn_bajar); break;
                case 8: display->print(msg("ESTADOS", "volver")); break;
            }
        }
    }
    // ----------------------------------------------------------------
    // 10. DRAW SUBMENU: UPDATE
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_ACTUALIZACION) {
        int pagOTA = cursorSubmenu / 3;

        // Title with automatic centering
        snprintf(titleBuf, sizeof(titleBuf), msg("SUBMENU_ACTUALIZACION", "titulo"));
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int inicio = pagOTA * 3;
        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex > 2) break;
            int yPos = 9 + (i * 8);

            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0));  // Yellow cursor
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // White text
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Gray text
                display->setCursor(offset + 2, yPos); display->print(" ");
            }

            display->setCursor(offset + 10, yPos);
            switch (itemIndex) {
                case 0: display->print(msg("SUBMENU_ACTUALIZACION", "buscar_ota")); break;
                case 1: display->print(msg("SUBMENU_ACTUALIZACION", "buscar_idioma")); break;
                case 2: display->print(msg("ESTADOS", "volver")); break;
            } 
        }
    }
    // ----------------------------------------------------------------
    // 11. DRAW SUBMENU: FTP
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_FTP) {
        // Title with automatic centering
        printMenuCentrado(msg("SUBMENU_FTP", "titulo"), 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        // Option: Status
        int y0 = 13;
        if (cursorSubmenu == 0) {
            display->setTextColor(display->color565(255, 255, 0)); // Yellow cursor
            display->setCursor(offset + 2, y0); display->print(">");
            display->setTextColor(display->color565(255, 255, 255)); // White text
        } else {
            display->setTextColor(display->color565(150, 150, 150)); // Gray text
            display->setCursor(offset + 2, y0); display->print(" "); 
        }
        display->setCursor(offset + 10, y0);
        display->printf(msg("SUBMENU_FTP", "activar"));

        // Option: Back
        int y1 = 23;
        if (cursorSubmenu == 1) {
            display->setTextColor(display->color565(255, 255, 0)); // Yellow cursor
            display->setCursor(offset + 2, y1); display->print(">"); // White text
            display->setTextColor(display->color565(255, 255, 255)); // Gray text
        } else {
            display->setTextColor(display->color565(150, 150, 150)); 
            display->setCursor(offset + 2, y1); display->print(" ");
        }
        display->setCursor(offset + 10, y1);
        display->print(msg("ESTADOS", "volver"));
    }
    // ----------------------------------------------------------------
    // 12. DRAW SUBMENU: LANGUAGE
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_IDIOMA) {
        // Title with automatic centering
        printMenuCentrado(msg("SUBMENU_IDIOMA", "titulo"), 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int totalOpciones = listaIdiomas.size() + 1; 
        int pag = cursorSubmenu / 3;
        int inicio = pag * 3;

        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex >= totalOpciones) break;

            int yPos = 9 + (i * 8);
            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0)); // Yellow cursor
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // White text
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Gray text
                display->setCursor(offset + 2, yPos); display->print(" ");
            }

            display->setCursor(offset + 10, yPos);
            if (itemIndex < listaIdiomas.size()) {
                display->print(listaIdiomas[itemIndex]);
            } else {
                display->print(msg("ESTADOS", "volver"));
            }
        }
    }

    display->flipDMABuffer(); // Send the buffer to the LEDs
}

// ====================================================================
//                     FUNCIONES CORE ANIMATED GIF
// ====================================================================
void GIFDraw(GIFDRAW *pDraw) {
    uint8_t *s;
    uint16_t *usPalette;
    static uint16_t usTemp[320];
    int x, y, iWidth, iCount;
    if (!display) return;
    int baseX = pDraw->iX + x_offset;
    iWidth = pDraw->iWidth;
    if (iWidth > offset) iWidth = offset; 
    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y + y_offset;
    s = pDraw->pPixels;

    if (pDraw->ucHasTransparency) { 
        iCount = 0;
        for (x = 0; x < iWidth; x++) {
            if (s[x] == pDraw->ucTransparent) {
                if (iCount) { 
                    for(int xOffset_ = 0; xOffset_ < iCount; xOffset_++ ){
                        display->drawPixel(baseX + x - iCount + xOffset_ + offset, y, usTemp[xOffset_]);
                    }
                    iCount = 0;
                }
            } else { usTemp[iCount++] = usPalette[s[x]]; }
        }
        if (iCount) {
            for(int xOffset_ = 0; xOffset_ < iCount; xOffset_++ ){
                display->drawPixel(baseX + x - iCount + xOffset_ + offset, y, usTemp[xOffset_]);
            }
        }
    } else { 
        s = pDraw->pPixels;
        for (x=0; x<iWidth; x++) display->drawPixel(baseX + x + offset, y, usPalette[*s++]);
    }
}

static void * GIFOpenFile(const char *fname, int32_t *pSize) {
    FSGifFile = SD.open(fname);
    if (FSGifFile) { *pSize = FSGifFile.size(); return (void *)&FSGifFile; }
    return NULL;
}

static void GIFCloseFile(void *pHandle) {
    File *f = static_cast<File *>(pHandle);
    if (f != NULL) f->close();
}

static int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen) {
    int32_t iBytesRead = iLen;
    File *f = static_cast<File *>(pFile->fHandle);
    if ((pFile->iSize - pFile->iPos) < iLen) iBytesRead = pFile->iSize - pFile->iPos - 1;
    if (iBytesRead <= 0) return 0;
    iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
    pFile->iPos = f->position();
    return iBytesRead;
}

static int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition) {
    File *f = static_cast<File *>(pFile->fHandle);
    f->seek(iPosition);
    pFile->iPos = (int32_t)f->position();
    return pFile->iPos;
}

// ====================================================================
//                     LITE CLOCK FUNCTIONS
// ====================================================================
uint16_t hsvTo565(uint16_t h, uint8_t s, uint8_t v) {
    float fH = h / 60.0; float fS = s / 255.0; float fV = v / 255.0;
    float c = fV * fS; float x = c * (1 - fabs(fmod(fH, 2.0) - 1));
    float m = fV - c; float r, g, b;
    if (fH < 1) { r = c; g = x; b = 0; }
    else if (fH < 2) { r = x; g = c; b = 0; }
    else if (fH < 3) { r = 0; g = c; b = x; }
    else if (fH < 4) { r = 0; g = x; b = c; }
    else if (fH < 5) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }
    return ((uint16_t)((r + m) * 31) << 11) | ((uint16_t)((g + m) * 63) << 5) | (uint16_t)((b + m) * 31);
}

void drawCustomChar(int x, int y, int index, uint16_t color, int scale) {
    for (int i = 0; i < 5; i++) {
        uint8_t line = font5x8[index][i];
        for (int j = 0; j < 8; j++) {
            if (line & (1 << j)) display->fillRect(x + (i * scale), y + (j * scale), scale, scale, color);
        }
    }
}

void mostrarRelojLite(bool conTransicion = false) {
    if(!display) return;

    if (arcadeEnable > 0 && estadoActual == ESTADO_ARCADE) return;

    interrumpirReproduccion = false;
    char lastTimeStr[9] = "";
    unsigned long lastColorMs = 0;

    // Only animated styles need to update faster than 1Hz
    // 1=Aurora 2=Rainbow, 4=Fuego, 5=Glitch
    const bool esAnimado = (clockStyle == 1 || clockStyle == 2 || clockStyle == 4 ||
                            clockStyle == 5);

    // ENTRANCE TRANSITION (GIF → CLOCK)
    if (conTransicion) transicionParticulasFormanRelojLite();

    unsigned long startTime = millis(); 
    while(millis() - startTime < (clockDuration * 1000UL) && !interrumpirReproduccion) {

        gestionarBotonMenu();
        leerControlRemoto();
        server.handleClient();
        verificarMarquesinaTCP();

        if (isSleeping || estadoActual != ESTADO_GIFS) {
            interrumpirReproduccion = true;
            break;
        }

        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) break;
        
        if (estadoActual == ESTADO_GIFS) {
            char fullTimeStr[9];
            strftime(fullTimeStr, sizeof(fullTimeStr), "%H:%M:%S", &timeinfo);

            const bool segundoCambio  = (strcmp(fullTimeStr, lastTimeStr) != 0);
            const bool colorCambio    = esAnimado && (millis() - lastColorMs >= 33); // ~30fps

            const int startY = (weatherEnable == 1 && weatherDataReady) ? 9 : 6;
            const int startX = offset;

            if (segundoCambio) {
                memcpy(lastTimeStr, fullTimeStr, sizeof(lastTimeStr));

                display->fillScreen(0);

                if (weatherEnable == 1) dibujarBarraNotificacionesLite();

                uint32_t ms = millis();
                for (int i = 0; i < 8; i++) {
                    int xPos = startX + (i * 16);
                    uint16_t color;
                    switch (clockStyle) {
                        case 0: color = display->color565((clockColor>>16)&0xFF,(clockColor>>8)&0xFF,clockColor&0xFF); break;
                        case 1: {
                            float onda = sinf((ms / 900.0f) + i * 0.6f);
                            int hue = ((int)(160 + onda * 50) + 360) % 360;
                            color = hsvTo565(hue, 200, 220);
                        } break;
                        case 2: color = hsvTo565((ms / 25 + xPos) % 360, 255, 255); break;
                        case 3: {
                            uint8_t r1=(clockColor>>16)&0xFF,g1=(clockColor>>8)&0xFF,b1=clockColor&0xFF;
                            float ratio = i / 8.75f;
                            color = display->color565(r1+(255-r1)*ratio,g1+(255-g1)*ratio,b1+(255-b1)*ratio);
                        } break;
                        case 4: {
                            uint8_t r = 200 + random(0, 56);
                            uint8_t g = 40 + random(0, 90);
                            color = display->color565(r, g, 0);
                        } break;
                        case 5: {
                            bool fallo = (random(0, 100) < 5);
                            if (fallo) {
                                uint8_t variante = random(0, 3);
                                color = (variante == 0) ? display->color565(255,0,60)
                                       : (variante == 1) ? display->color565(255,0,255)
                                                          : display->color565(255,255,255);
                            } else {
                                uint8_t verde = 150 + random(0, 100);
                                color = display->color565(0, verde, 40);
                            }
                        } break;
                        default: color = 0xFFFF; break;
                    }
                    if (fullTimeStr[i] >= '0' && fullTimeStr[i] <= '9') {
                        drawCustomChar(xPos, startY, fullTimeStr[i] - '0', color, 3);
                    } else if (fullTimeStr[i] == ':') {
                        if (timeinfo.tm_sec % 2 == 0) {
                            display->fillRect(xPos + 6, startY + 6,  3, 3, color);
                            display->fillRect(xPos + 6, startY + 15, 3, 3, color);
                        }
                    }
                }
                display->flipDMABuffer();
                lastColorMs = millis();

            } else if (colorCambio) {
                uint32_t ms = millis();
                for (int i = 0; i < 8; i++) {
                    if (fullTimeStr[i] < '0' || fullTimeStr[i] > '9') continue; // the colon does not change
                    int xPos = startX + (i * 16);
                    uint16_t color;
                    switch (clockStyle) {
                        case 1: {
                            float onda = sinf((ms / 900.0f) + i * 0.6f);
                            int hue = ((int)(160 + onda * 50) + 360) % 360;
                            color = hsvTo565(hue, 200, 220);
                        } break;
                        case 2: color = hsvTo565((ms / 25 + xPos) % 360, 255, 255); break;
                        case 4: {
                            uint8_t r = 200 + random(0, 56);
                            uint8_t g = 40 + random(0, 90);
                            color = display->color565(r, g, 0);
                        } break;
                        case 5: {
                            bool fallo = (random(0, 100) < 5);
                            if (fallo) {
                                uint8_t variante = random(0, 3);
                                color = (variante == 0) ? display->color565(255,0,60)
                                       : (variante == 1) ? display->color565(255,0,255)
                                                          : display->color565(255,255,255);
                            } else {
                                uint8_t verde = 150 + random(0, 100);
                                color = display->color565(0, verde, 40);
                            }
                        } break;
                        default: continue; // other styles are not animated — no redraw needed here
                    }
                    drawCustomChar(xPos, startY, fullTimeStr[i] - '0', color, 3);
                }
                display->flipDMABuffer();
                lastColorMs = millis();
            }
        }

        delay(esAnimado ? 16 : 50);
    }
    // EXIT TRANSITION (CLOCK → GIF)
    if (conTransicion && !interrumpirReproduccion) transicionRelojDispersaLite();
}

void dibujarBarraNotificacionesLite() {
    if (weatherEnable == 0 || !weatherDataReady) return;

    // 1. Select icon and color according to the OpenWeatherMap code
    uint16_t colorClima;
    const unsigned char* iconToDraw;

    if (weatherConditionCode >= 200 && weatherConditionCode < 300) {
        iconToDraw = icon_storm; colorClima = display->color565(200, 0, 200); // Storm
    } else if (weatherConditionCode >= 300 && weatherConditionCode < 600) {
        iconToDraw = icon_rain;  colorClima = display->color565(0, 100, 255); // Rain
    } else if (weatherConditionCode >= 600 && weatherConditionCode < 700) {
        iconToDraw = icon_snow;  colorClima = display->color565(255, 255, 255); // Snow
    } else if (weatherConditionCode >= 700 && weatherConditionCode < 800) {
        iconToDraw = icon_fog;   colorClima = display->color565(180, 180, 200); // Fog
    } else if (weatherConditionCode == 800) {
        if (isNight) {
            iconToDraw = icon_moon;   colorClima = display->color565(200, 200, 255); // Moon;
        } else {
        iconToDraw = icon_sun;   colorClima = display->color565(255, 255, 0); // Sun
        }
    } else {
        iconToDraw = icon_cloud; colorClima = display->color565(180, 180, 180); // Clouds
    }
    

    // 2. Draw the .ini message (Game Room) on the left
    display->setFont(NULL); // Standard 5x7 font
    display->setTextSize(1);
    display->setCursor(offset + 2, 0);
    display->setTextColor(display->color565(200, 200, 200)); // Light gray
    display->print(weatherCustomMsg); 

    // 3. Draw bitmap icon (at panel position 97)
    display->drawBitmap(offset + 97, 0, iconToDraw, 8, 8, colorClima);

    // 4. Draw temperature
    display->setTextColor(display->color565(200, 200, 200));
    display->setCursor(offset + 107, 0); 
    display->print((int)currentTemp); // (int) removes decimals to save space
    
    // Degree symbol (small 2x2 square) and the "C"
    display->drawRect(offset + 119, 0, 2, 2, display->color565(200, 200, 200));
    display->setCursor(offset + 122, 0);
    display->print("C");
}

void actualizarClimaLite() {
    if (WiFi.status() != WL_CONNECTED) return; 
    
    // 1. URL encoding
    String encodedCity = weatherCity;
    encodedCity.replace(" ", "%20");

    HTTPClient http;
    // 2. Preventive timeout for the Core 3-X
    http.setTimeout(5000); 

    String url = "http://api.openweathermap.org/data/2.5/weather?q=" + encodedCity + "&appid=" + weatherKey + "&units=metric";
    
    Serial.println(F("[WEATHER] Connecting to OpenWeatherMap..."));
    Serial.print(F("[WEATHER] URL: ")); Serial.println(url);

    if (http.begin(url)) {
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            StaticJsonDocument<48> filter;
            filter["main"]["temp"] = true;
            filter["weather"][0]["id"] = true;
            filter["weather"][0]["icon"] = true;

            StaticJsonDocument<192> doc;

            DeserializationError error = deserializeJson(
                doc, *http.getStreamPtr(),
                DeserializationOption::Filter(filter)
            );

            if (!error) {
                currentTemp = doc["main"]["temp"];
                weatherConditionCode = doc["weather"][0]["id"];

                const char* iconCode = doc["weather"][0]["icon"];
                isNight = (iconCode != nullptr && iconCode[strlen(iconCode) - 1] == 'n');

                weatherDataReady = true;
                Serial.printf(PSTR("[OK] %.1f°C, ID: %d, Night: %s\n"), currentTemp, weatherConditionCode, isNight ? "YES" : "NO");
            } else {
                Serial.print(F("[ERROR] JSON: "));
                Serial.println(error.c_str());
            }
        } else {
            Serial.printf("[ERROR] HTTP Code: %d\n", httpCode);
        }
        http.end();
    } else {
        Serial.println(F("[ERROR] Could not start HTTP connection"));
    }
}

void gestionarActualizacionClima() {
    // 1. If it is not time to update yet, exit
    if (weatherEnable == 0 || (millis() - lastWeatherUpdate < (unsigned long)weatherInterval * 60000UL)) return;

    Serial.println(F("\n[SYSTEM] Weather update window reached."));

    // 2. Strategy based on Double Buffer
    if (doubleBuff == 1) {
        Serial.println(F("[MEMORY] Double Buffer detected. Restarting for a clean update..."));
        delay(1000);
        ESP.restart();
    } 
    else {
        // 3. Standard connection attempt (Only if Double Buffer is disabled)
        WiFi.mode(WIFI_STA);
        WiFi.begin(wifi_ssid, wifi_pass);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            attempts++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            actualizarClimaLite();
            configTzTime(time_zone, ntpServer);
            lastWeatherUpdate = millis();
            Serial.println(F("[WEATHER] Data updated OK."));
        }

        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }
}

// ====================================================================
//               PARTICLE TRANSITION SYSTEM
// ====================================================================
#define TRANS_BUF_W  (PANEL_RES_X * panelChain)
#define TRANS_BUF_H  PANEL_RES_Y
#define MAX_PARTICULAS 800               

struct ParticulaLite {
    float x, y;
    float vx, vy;
    uint16_t color;
    bool activa;
};

static void renderRelojABuffer(uint16_t* buf) {
    memset(buf, 0, TRANS_BUF_W * TRANS_BUF_H * sizeof(uint16_t));

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return;

    char fullTimeStr[9];
    strftime(fullTimeStr, sizeof(fullTimeStr), "%H:%M:%S", &timeinfo);

    int startY = (weatherEnable == 1 && weatherDataReady) ? 9 : 6;
    int startX = offset;
    uint32_t ms = millis();

    for (int i = 0; i < 8; i++) {
        int xPos = startX + (i * 16);
        uint16_t color;
        switch (clockStyle) {
            case 0: color = display->color565((clockColor>>16)&0xFF,(clockColor>>8)&0xFF,clockColor&0xFF); break;
            case 1: {
                float onda = sinf((ms / 900.0f) + i * 0.6f);
                int hue = ((int)(160 + onda * 50) + 360) % 360;
                color = hsvTo565(hue, 200, 220);
            } break;
            case 2: color = hsvTo565((ms / 25 + xPos) % 360, 255, 255); break;
            case 3: {
                uint8_t r1=(clockColor>>16)&0xFF,g1=(clockColor>>8)&0xFF,b1=clockColor&0xFF;
                float ratio = i / 8.75f;
                color = display->color565(r1+(255-r1)*ratio,g1+(255-g1)*ratio,b1+(255-b1)*ratio);
            } break;
            case 4: {
                uint8_t r = 200 + random(0, 56);
                uint8_t g = 40 + random(0, 90);
                color = display->color565(r, g, 0);
            } break;
            case 5: {
                bool fallo = (random(0, 100) < 5);
                if (fallo) {
                    uint8_t variante = random(0, 3);
                    color = (variante == 0) ? display->color565(255,0,60)
                            : (variante == 1) ? display->color565(255,0,255)
                                                : display->color565(255,255,255);
                } else {
                    uint8_t verde = 150 + random(0, 100);
                    color = display->color565(0, verde, 40);
                }
            } break;
            default: color = 0xFFFF; break;
        }

        int fontIdx = -1;
        if (fullTimeStr[i] >= '0' && fullTimeStr[i] <= '9') fontIdx = fullTimeStr[i] - '0';
        else if (fullTimeStr[i] == ':') fontIdx = 10;
        if (fontIdx < 0) continue;

        for (int col = 0; col < 5; col++) {
            uint8_t lineBits = font5x8[fontIdx][col];
            for (int row = 0; row < 8; row++) {
                if (!(lineBits & (1 << row))) continue;
                for (int sy = 0; sy < 3; sy++) {
                    for (int sx = 0; sx < 3; sx++) {
                        int px = xPos + col * 3 + sx;
                        int py = startY + row * 3 + sy;
                        int bx = px - offset;
                        if (bx >= 0 && bx < TRANS_BUF_W && py >= 0 && py < TRANS_BUF_H)
                            buf[py * TRANS_BUF_W + bx] = color;
                    }
                }
            }
        }
    }
}

static void transicionParticulasFormanRelojLite() {
    size_t needed = TRANS_BUF_W * TRANS_BUF_H * sizeof(uint16_t)
                  + MAX_PARTICULAS * sizeof(ParticulaLite) + 2048;
    if (ESP.getMaxAllocHeap() < needed) {
        Serial.printf(PSTR("[TRANS] No contiguous block: %u/%u\n"), ESP.getMaxAllocHeap(), needed);
        return;
    }

    uint16_t* frameBuf = (uint16_t*)ps_malloc(TRANS_BUF_W * TRANS_BUF_H * sizeof(uint16_t));
    if (!frameBuf) frameBuf = (uint16_t*)malloc(TRANS_BUF_W * TRANS_BUF_H * sizeof(uint16_t));
    ParticulaLite* parts = (ParticulaLite*)malloc(MAX_PARTICULAS * sizeof(ParticulaLite));

    if (!frameBuf || !parts) {
        if (frameBuf) free(frameBuf);
        if (parts)    free(parts);
        Serial.println(F("[TRANS] Input malloc failed"));
        return;
    }

    renderRelojABuffer(frameBuf);

    int nParts = 0;
    float cx = offset + TRANS_BUF_W / 2.0f;
    float cy = TRANS_BUF_H / 2.0f;

    for (int by = 0; by < TRANS_BUF_H && nParts < MAX_PARTICULAS; by++) {
        for (int bx = 0; bx < TRANS_BUF_W && nParts < MAX_PARTICULAS; bx++) {
            uint16_t col = frameBuf[by * TRANS_BUF_W + bx];
            if (col == 0) continue;

            float tx = bx + offset;
            float ty = (float)by;

            float dx = tx - cx, dy = ty - cy;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < 1.0f) {
                float ang = random(0, 628) / 100.0f;
                dx = cosf(ang); dy = sinf(ang); dist = 1.0f;
            }

            float startDist = 70.0f + random(30);
            parts[nParts].x     = cx + (dx / dist) * startDist;
            parts[nParts].y     = cy + (dy / dist) * startDist;
            parts[nParts].vx    = tx;
            parts[nParts].vy    = ty;
            parts[nParts].color = col;
            parts[nParts].activa = true;
            nParts++;
        }
    }

    const int FRAMES = 55;
    for (int frame = 0; frame < FRAMES; frame++) {
        float t    = (float)(frame + 1) / (float)FRAMES;
        float ease = t * t * (3.0f - 2.0f * t);

        display->fillScreen(0);
        for (int i = 0; i < nParts; i++) {
            float curX = parts[i].x + (parts[i].vx - parts[i].x) * ease;
            float curY = parts[i].y + (parts[i].vy - parts[i].y) * ease;
            int ix = (int)(curX + 0.5f), iy = (int)(curY + 0.5f);
            if (ix >= 0 && ix < (offset + TRANS_BUF_W) && iy >= 0 && iy < TRANS_BUF_H)
                display->drawPixel(ix, iy, parts[i].color);
        }
        display->flipDMABuffer();
        delay(14);
    }

    for (int b = 0; b < 2; b++) {
        display->fillScreen(0);
        for (int by = 0; by < TRANS_BUF_H; by++)
            for (int bx = 0; bx < TRANS_BUF_W; bx++) {
                uint16_t col = frameBuf[by * TRANS_BUF_W + bx];
                if (col) display->drawPixel(bx + offset, by, col); 
            }
        display->flipDMABuffer();
    }

    free(frameBuf);
    free(parts);
}

static void transicionRelojDispersaLite() {
    size_t needed = TRANS_BUF_W * TRANS_BUF_H * sizeof(uint16_t)
                  + MAX_PARTICULAS * sizeof(ParticulaLite) + 2048;
    if (ESP.getMaxAllocHeap() < needed) {
        Serial.printf("[TRANS] No contiguous block: %u/%u\n", ESP.getMaxAllocHeap(), needed);
        return;
    }

    uint16_t* frameBuf = (uint16_t*)ps_malloc(TRANS_BUF_W * TRANS_BUF_H * sizeof(uint16_t));
    if (!frameBuf) frameBuf = (uint16_t*)malloc(TRANS_BUF_W * TRANS_BUF_H * sizeof(uint16_t));
    ParticulaLite* parts = (ParticulaLite*)malloc(MAX_PARTICULAS * sizeof(ParticulaLite));

    if (!frameBuf || !parts) {
        if (frameBuf) free(frameBuf);
        if (parts)    free(parts);
        Serial.println(F("[TRANS] Output malloc failed"));
        return;
    }

    renderRelojABuffer(frameBuf);

    int nParts = 0;
    float cx = offset + TRANS_BUF_W / 2.0f;
    float cy = TRANS_BUF_H / 2.0f;

    for (int by = 0; by < TRANS_BUF_H && nParts < MAX_PARTICULAS; by++) {
        for (int bx = 0; bx < TRANS_BUF_W && nParts < MAX_PARTICULAS; bx++) {
            uint16_t col = frameBuf[by * TRANS_BUF_W + bx];
            if (col == 0) continue;

            float tx = bx + offset;
            float ty = (float)by;
            float dx = tx - cx, dy = ty - cy;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < 1.0f) {
                float ang = random(0, 628) / 100.0f;
                dx = cosf(ang); dy = sinf(ang); dist = 1.0f;
            }

            float speed = 0.4f + random(100) / 160.0f;
            parts[nParts].x     = tx;
            parts[nParts].y     = ty;
            parts[nParts].vx    = (dx / dist) * speed + (random(-40, 41) / 100.0f);
            parts[nParts].vy    = (dy / dist) * speed + (random(-40, 41) / 100.0f);
            parts[nParts].color = col;
            parts[nParts].activa = true;
            nParts++;
        }
    }

    free(frameBuf);
    frameBuf = nullptr;

    const int FRAMES = 60;
    for (int frame = 0; frame < FRAMES; frame++) {
        display->fillScreen(0);
        int activos = 0;

        for (int i = 0; i < nParts; i++) {
            if (!parts[i].activa) continue;
            parts[i].x  += parts[i].vx;
            parts[i].y  += parts[i].vy;
            parts[i].vx *= 1.03f;
            parts[i].vy *= 1.03f;
            parts[i].vy += 0.01f;

            int ix = (int)parts[i].x, iy = (int)parts[i].y;
            if (ix < 0 || ix >= (offset + TRANS_BUF_W) || iy < 0 || iy >= TRANS_BUF_H) {
                parts[i].activa = false; continue;
            }
            display->drawPixel(ix, iy, parts[i].color);
            activos++;
        }

        display->flipDMABuffer();
        delay(16);
        if (activos == 0 && frame > 15) break;
    }

    free(parts);

    for (int b = 0; b < 2; b++) {
        display->fillScreen(0);
        display->flipDMABuffer();
    }
}

// ====================================================================
//                     LITE TEXT FUNCTIONS
// ====================================================================
String utf8ToExtended(const String& in) {
    String out;
    out.reserve(in.length());
    for (size_t i = 0; i < in.length();) {
        uint8_t c = (uint8_t)in[i];
        if (c < 0x80) {
            out += (char)c;
            i += 1;
        } else if ((c & 0xE0) == 0xC0 && i + 1 < in.length()) {
            uint8_t c2 = (uint8_t)in[i + 1];
            uint16_t codepoint = ((c & 0x1F) << 6) | (c2 & 0x3F);

            // Special remapping of Polish characters to the 0x80 - 0x8F range
            switch (codepoint) {
                case 0x0104: out += (char)0x80; break; // Ą
                case 0x0105: out += (char)0x81; break; // ą
                case 0x0106: out += (char)0x82; break; // Ć
                case 0x0107: out += (char)0x83; break; // ć
                case 0x0118: out += (char)0x84; break; // Ę
                case 0x0119: out += (char)0x85; break; // ę
                case 0x0141: out += (char)0x86; break; // Ł
                case 0x0142: out += (char)0x87; break; // ł
                case 0x0143: out += (char)0x88; break; // Ń
                case 0x0144: out += (char)0x89; break; // ń
                case 0x015A: out += (char)0x8A; break; // Ś
                case 0x015B: out += (char)0x8B; break; // ś
                case 0x0179: out += (char)0x8C; break; // Ź
                case 0x017A: out += (char)0x8D; break; // ź
                case 0x017B: out += (char)0x8E; break; // Ż
                case 0x017C: out += (char)0x8F; break; // ż
                default:
                    // If it is a character within Latin-1 (e.g. accented characters or ñ), keep it
                    if (codepoint <= 0xFF) {
                        out += (char)codepoint;
                    } else {
                        out += '?';
                    }
                    break;
            }
            i += 2;
        } else {
            // Fallback for emojis or 3-4-byte UTF-8
            out += '?';
            i += 1;
            while (i < in.length() && (((uint8_t)in[i]) & 0xC0) == 0x80) i++;
        }
    }
    return out;
}

void mostrarTextoScroll() {
    if (textoScrollMsg.length() == 0) return;

    unsigned long ahora = millis();
    if (ahora - textoScrollUltimoPaso < (unsigned long)textoScrollVelocidad) return;
    textoScrollUltimoPaso = ahora;

    // Only reconvert and remeasure the text when the message or font changes
    static String ultimoMensajeCacheado = "";
    static const GFXfont* ultimaFuenteCacheada = NULL;

    if (textoScrollMsg != ultimoMensajeCacheado || textoScrollFuente != ultimaFuenteCacheada) {
        textoScrollMsgProcesado = utf8ToExtended(textoScrollMsg);

        // Apply the font BEFORE measuring so the calculation is accurate
        display->setFont(textoScrollFuente);
        display->setTextSize(1);

        int16_t x1, y1;
        uint16_t altoTexto;
        display->getTextBounds(textoScrollMsgProcesado, 0, 0, &x1, &y1, &textoScrollAnchoCache, &altoTexto);

        ultimoMensajeCacheado = textoScrollMsg;
        ultimaFuenteCacheada = textoScrollFuente; // Update the cache
    }

    display->fillScreen(0);
    display->setTextColor(display->color565(
        (textoScrollColor >> 16) & 0xFF,
        (textoScrollColor >> 8) & 0xFF,
        textoScrollColor & 0xFF));
        
    // Apply the stored font for rendering
    display->setFont(textoScrollFuente);
    display->setTextSize(1);
    
    display->setCursor(textoScrollX, 22);
    display->print(textoScrollMsgProcesado);
    display->flipDMABuffer();

    textoScrollX--;
    if (textoScrollX < offset - (int)textoScrollAnchoCache) {
        textoScrollX = offset + PANEL_RES_X;
    }

    display->setFont(NULL);
}

// ====================================================================
//                 POWER AND TIMER FUNCTIONS
// ====================================================================
void cargarAjustesTimer() {
    Preferences prefs;
    prefs.begin("timer-lite", true); 
    timerEnable = prefs.getBool("t_act", false);
    hOn = prefs.getInt("h_on", 9);
    mOn = prefs.getInt("m_on", 0);
    hOff = prefs.getInt("h_off", 23);
    mOff = prefs.getInt("m_off", 0);
    prefs.end();
}

void guardarAjustesTimer() {
    Preferences prefs;
    prefs.begin("timer-lite", false);
    prefs.putBool("t_act", timerEnable);
    prefs.putInt("h_on", hOn);
    prefs.putInt("m_on", mOn);
    prefs.putInt("h_off", hOff);
    prefs.putInt("m_off", mOff);
    prefs.end();
}

void toggleEnergia(bool dormir) {
    if (dormir) {
        if (isSleeping) return; // Already sleeping
        Serial.println(F("[POWER] Entering Sleep Mode..."));
        
        mostrarAnimacionApagado();

        // Hardware shutdown
        gif.close(); // Releases the SD card
        display->fillScreen(0);
        display->flipDMABuffer();
        digitalWrite(OE_PIN, HIGH); // Turns off LEDs
        setCpuFrequencyMhz(80); // Lowers CPU to 80MHz
        isSleeping = true;
    } else {
        if (!isSleeping) return;
        Serial.println(F("[POWER] Waking system..."));
        setCpuFrequencyMhz(240);    // CPU at full speed
        delay(150);
        digitalWrite(OE_PIN, LOW);  // Turns on LEDs
        display->fillScreen(0);
        display->flipDMABuffer();
        isSleeping = false;
        interrumpirReproduccion = true;
        estadoActual = ESTADO_GIFS;
        saliendoAGifs = true; 
    }
}

void verificarTemporizador() {
    if (!timerEnable) return;

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return; // If there is no NTP time, do nothing

    int minutosAhora = timeinfo.tm_hour * 60 + timeinfo.tm_min;
    int minutosOn = hOn * 60 + mOn;
    int minutosOff = hOff * 60 + mOff;

    //1. OVERRIDE HANDLING
    static int ultimoMinuto = -1;
    if (minutosAhora != ultimoMinuto) {
        // When we reach exactly the scheduled power-on or power-off minute,
        // the timer automatically takes control back and overrides manual mode.
        if (minutosAhora == minutosOn || minutosAhora == minutosOff) {
            manualOverride = false; 
        }
        ultimoMinuto = minutosAhora;
    }

    // If the user has pressed a button, block state changes (until the next alarm)
    if (manualOverride) return;

    // 2. STATE LOGIC
    bool deberiaEstarEncendido = false;
    if (minutosOn < minutosOff) {
        deberiaEstarEncendido = (minutosAhora >= minutosOn && minutosAhora < minutosOff);
    } else { // Midnight crossover case
        deberiaEstarEncendido = (minutosAhora >= minutosOn || minutosAhora < minutosOff);
    }

    // 3. APPLY CHANGES
    if (deberiaEstarEncendido && isSleeping) {
        toggleEnergia(false); // Time to wake up
    } else if (!deberiaEstarEncendido && !isSleeping) {
        toggleEnergia(true); // Time to sleep
    }
}

void mostrarAnimacionApagado() {
    int centroX = offset + PANEL_RES_X;
    int centroY = PANEL_RES_Y / 2;
    uint16_t colorAmbar = display->color565(255, 255, 255);

    // "Standby" icon
    display->fillScreen(0);

    // Power symbol: circle with a notch at the top + vertical line
    display->drawCircle(centroX, centroY - 7, 7, colorAmbar);
    display->drawLine(centroX, centroY - 14, centroX, centroY - 7, colorAmbar);
    display->drawPixel(centroX - 1, centroY - 14, 0); // cover the notch
    display->drawPixel(centroX,     centroY - 14, 0);
    display->drawPixel(centroX + 1, centroY - 14, 0);

    display->setTextColor(colorAmbar);
    display->setCursor(centroX - 9, centroY + 7);
    display->print("OFF");

    display->flipDMABuffer();
    delay(800);

    // CRT-style collapse (the bar shrinks and turns off)
    int medioAncho = 128;
    for (int i = 0; i <= 10; i++) {
        int w = medioAncho - (medioAncho * i / 10);
        uint8_t brillo = 255 - (i * 22);
        //uint16_t colorLinea = display->color565(brillo, (uint8_t)(brillo * 0.65), 0);
        uint16_t colorLinea = display->color565(brillo, brillo, brillo);

        display->fillScreen(0);
        if (w > 0) {
            display->drawFastHLine(centroX - w, centroY, w * 2, colorLinea);
        } else {
            display->drawPixel(centroX, centroY, colorLinea);
        }
        display->flipDMABuffer();
        delay(30);
    }

    display->fillScreen(0);
    display->flipDMABuffer();
    delay(120);
}

// ====================================================================
//             WIFI RECONNECTION SYSTEM IN CASE OF FAILURE
// ====================================================================
void gestionarReconexionWiFi() {
    static bool avisado = false;
    static unsigned long ultimoIntento = 0;

    if (!wifiDebeEstarActivo) return; // WiFi intentionally disabled: do not retry

    if (WiFi.status() == WL_CONNECTED) {
        avisado = false;
        return;
    }

    if (!avisado) {
        Serial.println(F("[WIFI] Connection lost. Retrying every 10s..."));
        avisado = true;
    }

    // Non-blocking retry: WiFi.begin() is asynchronous, so we do not wait here
    if (millis() - ultimoIntento < 10000UL) return;
    ultimoIntento = millis();

    WiFi.disconnect();
    WiFi.begin(wifi_ssid, wifi_pass);
}

// ====================================================================
//                     OTA UPDATE SYSTEM
// ====================================================================
int resultadoOTA = -1; // -1: none, 0: up to date, 1: updated, 2: error

void triggerActualizacionOTA() {
    Preferences prefs;
    prefs.begin("sistema", false);
    prefs.putBool("ota_pending", true);
    prefs.end();
    
    Serial.println(F("[SYSTEM] OTA flag set. Restarting..."));
    delay(500);
    ESP.restart();
}

int buscarEInstalarOTA() {
    // 1. WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_pass);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() != WL_CONNECTED) return 2;

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    // 2. Get version
    int newVersion = 0;
    char binUrl[256] = "";
    if (http.begin(client, GITHUB_VERSION_URL)) {
        if (http.GET() == 200) {
            StaticJsonDocument<256> doc;
            deserializeJson(doc, http.getString());
            newVersion = doc["v"].as<int>();
            const char* binUrlTmp = doc["url"];
            if (binUrlTmp) strlcpy(binUrl, binUrlTmp, sizeof(binUrl));
        }
        http.end();
    }

    // 3. Installation
    if (newVersion > CURRENT_VERSION_NUM && binUrl[0] != '\0') {
        if (http.begin(client, binUrl)) {
            if (http.GET() == 200) {
                int contentLength = http.getSize();
                
                Update.abort(); // Cleanup just in case
                
                if (Update.begin(contentLength, U_FLASH)) {
                    Serial.println(F("[OTA] Writing binary..."));
                    WiFiClient* stream = http.getStreamPtr();
                    
                    // Use direct writing
                    size_t escrito = Update.writeStream(*stream);
                    
                    if (escrito == contentLength && Update.end(true)) {
                        Serial.println(F("[OTA] Update completed successfully!"));
                        
                        Preferences prefs;
                        prefs.begin("sistema", false);
                        prefs.putBool("ota_done", true); 
                        prefs.end();

                        delay(1000);
                        ESP.restart();
                        return 1; // This should never be reached because of the restart
                    } else {
                        Serial.printf("[OTA] Error: %s\n", Update.errorString());
                        return 2; // Write error
                    }
                }
            }
            http.end();
        }
        return 2; // Write error
    } else {
        // There is no new version
        Serial.println(F("[OTA] The system is already up to date."));
        return 0; // System is up to date
    }
}

void triggerDescargaIdiomas() {
    Preferences prefs;
    prefs.begin("sistema", false);
    prefs.putBool("idiomas_pending", true);
    prefs.end();

    Serial.println(F("[LANGUAGE] Download flag set. Restarting..."));
    delay(500);
    ESP.restart();
}

int descargarIdiomasGitHub(String &resumen) {
    Serial.printf("[LANGUAGE] Started. Free heap: %u bytes | Max contiguous block: %u bytes\n", 
                  ESP.getFreeHeap(), ESP.getMaxAllocHeap());

    if (WiFi.status() != WL_CONNECTED) { 
        resumen = "No WiFi connection"; 
        Serial.println("[LANGUAGE] Error: No WiFi connection.");
        return 0; 
    }

    // 1. Explicit DNS verification
    IPAddress ipGitHub;
    if (!WiFi.hostByName("raw.githubusercontent.com", ipGitHub)) {
        resumen = "DNS failure (raw.githubusercontent.com)";
        Serial.println("[LANGUAGE] ERROR: Could not resolve GitHub IP via DNS.");
        return 0;
    }
    Serial.printf("[LANGUAGE] DNS resolved successfully -> IP: %s\n", ipGitHub.toString().c_str());

    const char* carpetaSD = "/idioma";
    if (!SD.exists(carpetaSD)) {
        SD.mkdir(carpetaSD);
        Serial.println("[LANGUAGE] /idioma folder created on SD");
    }

    String contenidoLista = "";

    // 2. Download idiomas.txt using the full URL (automatically configures TLS SNI)
    {
        WiFiClientSecure client;
        client.setInsecure();

        HTTPClient http;
        String urlLista = String(GITHUB_RAW_BASE_URL) + "idiomas.txt";

        Serial.printf("[LANGUAGE] Connecting to: %s\n", urlLista.c_str());

        if (http.begin(client, urlLista)) {
            http.setUserAgent("RetroPixelLED-Lite");
            http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
            http.setTimeout(10000); // 10s timeout

            int code = http.GET();
            if (code == 200) {
                contenidoLista = http.getString();
                Serial.printf("[LANGUAGE] idiomas.txt retrieved successfully (%d bytes)\n", contenidoLista.length());
            } else {
                resumen = "idiomas.txt returned an error (" + String(code) + ")";
                Serial.printf("[LANGUAGE] HTTP error downloading idiomas.txt. Code: %d\n", code);
            }
            http.end();
        } else {
            resumen = "http.begin failed with URL";
            Serial.println("[LANGUAGE] http.begin() failed while processing URL");
        }
        client.stop();
    }

    if (contenidoLista.length() == 0) {
        if (resumen == "") resumen = "idiomas.txt is empty";
        return 0;
    }

    // 3. Iterate through the downloaded list and retrieve each .json file
    int descargados = 0, fallidos = 0;
    int posInicio = 0;

    while (posInicio < contenidoLista.length()) {
        int posFin = contenidoLista.indexOf('\n', posInicio);
        if (posFin == -1) posFin = contenidoLista.length();

        String linea = contenidoLista.substring(posInicio, posFin);
        linea.trim();
        posInicio = posFin + 1;

        if (linea.length() == 0 || !linea.endsWith(".json")) continue;

        Serial.printf("[LANGUAGE] Downloading '%s'... Free heap: %u | Max block: %u\n", 
                      linea.c_str(), ESP.getFreeHeap(), ESP.getMaxAllocHeap());

        WiFiClientSecure fileClient;
        fileClient.setInsecure();

        HTTPClient httpFile;
        String urlArchivo = String(GITHUB_RAW_BASE_URL) + linea;

        if (httpFile.begin(fileClient, urlArchivo)) {
            httpFile.setUserAgent("RetroPixelLED-Lite");
            httpFile.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
            httpFile.setTimeout(10000);

            int codeArchivo = httpFile.GET();
            if (codeArchivo == 200) {
                String rutaSD = String(carpetaSD) + "/" + linea;
                File f = SD.open(rutaSD, FILE_WRITE);
                if (f) {
                    httpFile.writeToStream(&f);
                    f.close();
                    descargados++;
                    Serial.printf("[LANGUAGE] '%s' saved to SD successfully\n", linea.c_str());
                } else {
                    fallidos++;
                    Serial.printf("[LANGUAGE] Error opening/writing SD path '%s'\n", rutaSD.c_str());
                }
            } else {
                fallidos++;
                Serial.printf("[LANGUAGE] HTTP error %d downloading '%s'\n", codeArchivo, linea.c_str());
            }
            httpFile.end();
        } else {
            fallidos++;
            Serial.printf("[LANGUAGE] httpFile.begin() failed for '%s'\n", linea.c_str());
        }
        fileClient.stop();
    }

    resumen = String(descargados) + " file(s) downloaded";
    if (fallidos > 0) resumen += ", " + String(fallidos) + " failed";

    Serial.printf("[LANGUAGE] Process complete. Downloaded: %d, Failed: %d. Final heap: %u\n", 
                  descargados, fallidos, ESP.getFreeHeap());

    return descargados;
}
// ====================================================================
//                        FTP FUNCTIONS
// ====================================================================
void ejecutarModoFTP() {
    // 1. Force WiFi
    if (wifiEnable == 1 && wifi_ssid[0] != '\0') {
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.mode(WIFI_STA);
            WiFi.begin(wifi_ssid, wifi_pass);
            int at = 0;
            while (WiFi.status() != WL_CONNECTED && at < 20) { delay(500); at++; }
        }
    }

    // 2. Start FTP
    ftpSrv.begin("admin", "admin"); 

    // 3. Static Visual Interface
    display->fillScreen(0);
    
    // 1. FOLDER (Main yellow body of the image)
    uint16_t colorCarpeta = display->color565(255, 230, 100); // Soft yellow
    display->fillRoundRect(offset + 4, 11, 24, 16, 2, colorCarpeta); 
    // Top folder tab
    display->fillRoundRect(offset + 4, 9, 10, 5, 1, colorCarpeta);

    // 2. ARROWS (Green upload, blue download as shown in the image)
    uint16_t colorVerde = display->color565(50, 200, 50); // Green brillante
    uint16_t colorAzul = display->color565(50, 150, 255);  // Light blue

    // Upload Arrow (Green)
    // Triangle (tip)
    display->fillTriangle(offset + 14, 1, offset + 18, 5, offset + 10, 5, colorVerde);
    // Rectangle (body)
    display->fillRect(offset + 13, 5, 3, 4, colorVerde);

    // Download Arrow (Blue)
    // Triangle (tip)
    display->fillTriangle(offset + 24, 9, offset + 28, 5, offset + 20, 5, colorAzul);
    // Rectangle (body)
    display->fillRect(offset + 23, 1, 3, 4, colorAzul);

    // 3. "FTP" TEXT INSIDE THE FOLDER
    display->setTextSize(1);
    display->setTextColor(0); // Black, for contrast with the yellow
    // Center "FTP" inside the folder rectangle
    // Coordinates calculated to center it in the 24x16 RoundRect
    display->setCursor(offset + 8, 16); 
    display->print("FTP");

    // IP address (To the right of the folder)
    display->setTextColor(display->color565(255, 255, 255)); // White
    display->setCursor(offset + 34, 16);
    display->print(WiFi.localIP().toString());

    display->flipDMABuffer();

    Serial.print(F("[FTP] Server ready at IP: "));
    Serial.println(WiFi.localIP().toString());

    // 4. Infinite maintenance loop
    while (true) {
        ftpSrv.handleFTP();

        // If the button is pressed (any duration), exit
        if (digitalRead(PIN_BOTON_MENU) == LOW) {
            Serial.println(F("[FTP] Closing FTP via physical button"));
            Preferences p;
            p.begin("sistema", false);
            p.putBool("ftp_mode", false);
            p.end();
            delay(1000);
            ESP.restart();
        }

        // If any IR button is pressed, exit
        if (IrReceiver.decode()) {
        //uint32_t codigoRecibido = IrReceiver.decodedIRData.command;
        uint32_t codigoRecibido = IrReceiver.decodedIRData.decodedRawData;
    
            // Only exit if the pressed button is exactly the OK/Confirm button
            if (codigoRecibido == ir_btn_ok) { 
                Serial.println(F("[FTP] OK button detected. Closing server and restarting..."));
                Preferences p;
                p.begin("sistema", false);
                p.putBool("ftp_mode", false);
                p.end();
                delay(1000);
                ESP.restart();
            }

            // Clear the buffer to allow the next read
            IrReceiver.resume();
        }
        
        yield();
        delay(2);
    }
}

// ====================================================================
//                REPLAYOS ARCADE PLAYBACK ENGINE
// ====================================================================
bool marqueeEsGif = false;              // true if the current marquee is a GIF, false if it is a BMP
std::vector<String> marqueeGifSecuencia; // paths of the .gif files to play in a loop (base + _01, _02...)
int marqueeGifIndiceSecuencia = 0;       // which element of the sequence is next

bool buscarSecuenciaGif(const char* rutaBase, std::vector<String>& out) {
    out.clear();
    char ruta[100];
    snprintf(ruta, sizeof(ruta), "%s.gif", rutaBase);
    if (!SD.exists(ruta)) return false;
    out.push_back(String(ruta));
    for (int i = 1; i < 100; i++) {
        snprintf(ruta, sizeof(ruta), "%s_%02d.gif", rutaBase, i);
        if (!SD.exists(ruta)) break;
        out.push_back(String(ruta));
    }
    return true;
}

bool buscarJuegoEnIndice_ReplayOS(const char* sistema, const char* juego) {
    char rutaIndice[80];
    snprintf(rutaIndice, sizeof(rutaIndice), "/Arcade/%s.txt", sistema);

    File archivo = SD.open(rutaIndice);
    if (!archivo) return false;

    long lo = 0;
    long hi = archivo.size();
    char linea[80];

    // Phase 1: binary jumps to narrow down a small window (~512 bytes).
    // We do not require perfect alignment on every jump (variable-length lines
    // can cause bugs): we only get close, then fine-tune
    // Phase 2 handles this.
    while (hi - lo > 512) {
        long mid = lo + (hi - lo) / 2;
        archivo.seek(mid);
        while (archivo.position() < hi && archivo.read() != '\n') {} // align to start of line

        long pos = archivo.position();
        if (pos >= hi) break; // no more useful line breaks: proceed to Phase 2

        int len = archivo.readBytesUntil('\n', linea, sizeof(linea) - 1);
        linea[len] = '\0';
        int l = strlen(linea);
        if (l > 0 && linea[l - 1] == '\r') linea[l - 1] = '\0';

        if (strcasecmp(linea, juego) < 0) lo = archivo.position();
        else hi = pos;
    }

    // Phase 2: linear scan of the narrowed window (always only a few lines)
    archivo.seek(lo);
    if (lo > 0) { while (archivo.position() < hi && archivo.read() != '\n') {} }

    bool encontrado = false;
    while (archivo.position() < hi && archivo.available()) {
        int len = archivo.readBytesUntil('\n', linea, sizeof(linea) - 1);
        linea[len] = '\0';
        int l = strlen(linea);
        if (l > 0 && linea[l - 1] == '\r') linea[l - 1] = '\0';
        if (strcasecmp(linea, juego) == 0) { encontrado = true; break; }
    }

    archivo.close();
    return encontrado;
}

void reproducirMarquesinaGIF_ReplayOS() {
    if (marqueeGifSecuencia.empty()) { marqueeEsGif = false; return; }
    interrumpirReproduccion = false; // clear the signal that brought us here

    String gifPath = marqueeGifSecuencia[marqueeGifIndiceSecuencia];
    if (gif.open(gifPath.c_str(), GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw)) {
        x_offset = (offset - gif.getCanvasWidth()) / 2;
        y_offset = (PANEL_RES_Y - gif.getCanvasHeight()) / 2;
        int delayMs;
        while (gif.playFrame(true, &delayMs)) {
            display->flipDMABuffer();
            verificarReplayOSLite(); // respects its own throttle (replayOSIntervaloActual)
            if (digitalRead(PIN_BOTON_MENU) == LOW || interrumpirReproduccion || estadoActual != ESTADO_ARCADE) {
                gif.close();
                return; // game change, end of game, or menu
            }
            delay(delayMs > 0 ? delayMs : 10);
        }
        gif.close();
    } else {
        Serial.printf("[ReplayOS] Error opening GIF: %s\n", gifPath.c_str());
    }
    marqueeGifIndiceSecuencia = (marqueeGifIndiceSecuencia + 1) % marqueeGifSecuencia.size();
}

void mostrarMarquesinaBMP(const char* path) {
    File bmpFile = SD.open(path);
    if (!bmpFile) {
        Serial.print(F("Error: Could not open BMP at "));
        Serial.println(path);
        return;
    }

    // Verify that this is a real BMP before trusting its offsets
    char firma[2];
    bmpFile.read((uint8_t*)firma, 2);
    if (firma[0] != 'B' || firma[1] != 'M') {
        Serial.println(F("[BMP] Invalid file (missing BM header)."));
        bmpFile.close();
        return;
    }

    // 1. Detect bit depth (stored at byte 28)
    bmpFile.seek(28);
    uint16_t bitsPerPixel = 0;
    bmpFile.read((uint8_t*)&bitsPerPixel, 2);

    // 2. Detect where the pixels start (stored at byte 10)
    bmpFile.seek(10);
    uint32_t dataOffset = 0;
    bmpFile.read((uint8_t*)&dataOffset, 4);

    // Calculate how many bytes each pixel occupies (3 or 4)
    int bytesPorPixel = bitsPerPixel / 8;
    if (bytesPorPixel != 3 && bytesPorPixel != 4) {
        Serial.printf(PSTR("[BMP] Unsupported bit depth: %d bits\n"), bitsPerPixel);
        bmpFile.close();
        return;
    }

    Serial.printf(PSTR("[BMP] Loading %dbits (%d bytes/px) from %s\n"), bitsPerPixel, bytesPorPixel, path);

    bmpFile.seek(dataOffset);

    // 3. Draw BMP row by row (1 SPI read per row, not per byte)
    const int ANCHO = 128;
    uint8_t filaBuffer[ANCHO * 4]; // enough for 24 or 32 bits

    for (int y = 31; y >= 0; y--) {
        int leidos = bmpFile.read(filaBuffer, ANCHO * bytesPorPixel);
        if (leidos < ANCHO * bytesPorPixel) break; // truncated file: stop cleanly

        for (int x = 0; x < ANCHO; x++) {
            uint8_t* px = filaBuffer + (x * bytesPorPixel);
            // BMP stores pixels in BGR order
            uint8_t b = px[0], g = px[1], r = px[2];
            display->drawPixel(x + offset, y, display->color565(r, g, b));
        }
    }

    bmpFile.close();
    Serial.println(F("Marquee drawn successfully."));
}

void verificarReplayOSLite() {
    if (arcadeEnable != 3) return;

    static unsigned long ultimaConsulta = 0;
    if (millis() - ultimaConsulta < replayOSIntervaloActual) return;
    ultimaConsulta = millis();

    static WiFiClient client;
    static HTTPClient http;

    char url[80];
    snprintf(url, sizeof(url), "http://%s:55356/api/v1/get_status", replayOS_IP);

    http.setConnectTimeout(800);
    http.setTimeout(1200);
    http.setReuse(true); 

    if (!http.begin(client, url)) {
        Serial.println(F("[ReplayOS] Could not start connection"));
        return;
    }
    http.addHeader("X-RePlay-Token", replayOS_Token);

    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK) {
        http.end();
        replayOSFallosConsecutivos++;
        if (replayOSFallosConsecutivos > 1) {
            replayOSIntervaloActual = min(3000UL * (1UL << min(replayOSFallosConsecutivos - 1, 3)), 30000UL);
        }
        if (httpCode > 0) {
            Serial.printf(PSTR("[ReplayOS] HTTP failure: %d (attempt %d, next in %lums)\n"),
                          httpCode, replayOSFallosConsecutivos, replayOSIntervaloActual);
        } else {
            Serial.printf(PSTR("[ReplayOS] No response (err %d, attempt %d, next in %lums)\n"),
                          httpCode, replayOSFallosConsecutivos, replayOSIntervaloActual);
        }
        return;
    }

    replayOSFallosConsecutivos = 0;
    replayOSIntervaloActual = 3000;

    StaticJsonDocument<64> filter;
    filter["system"]    = true;
    filter["game_file"] = true;
    filter["view_id"]   = true;

    StaticJsonDocument<320> doc;
    DeserializationError err = deserializeJson(doc, http.getStream(),
                                                DeserializationOption::Filter(filter));
    http.end();

    if (err) {
        Serial.printf(PSTR("[ReplayOS] JSON error: %s\n"), err.c_str());
        return;
    }

    int viewId = doc["view_id"] | 0;

    // CASE 1: NOT PLAYING
    if (viewId != 2) {
        if (ultimoJuegoCargado.length() > 0) {
            Serial.println(F("[ReplayOS] Out of game. Returning to normal mode."));
            ultimoJuegoCargado = "";
            marqueeEsGif = false;
            marqueeGifSecuencia.clear();
            estadoActual = ESTADO_GIFS;
            saliendoAGifs = true;
            interrumpirReproduccion = true;
        }
        return;
    }

    // CASE 2: PLAYING 
    const char* rawSystem   = doc["system"];
    const char* rawGameFile = doc["game_file"];
    if (!rawSystem || !rawGameFile) return;

    char juegoLimpio[64] = "";
    {
        const char* barra = strrchr(rawGameFile, '/');
        const char* base  = barra ? barra + 1 : rawGameFile;
        const char* punto = strrchr(base, '.');
        size_t len = punto ? (size_t)(punto - base) : strlen(base);
        if (len >= sizeof(juegoLimpio)) len = sizeof(juegoLimpio) - 1;
        memcpy(juegoLimpio, base, len);
        juegoLimpio[len] = '\0';
    }

    if (ultimoJuegoCargado == juegoLimpio) return;

    ultimoJuegoCargado = juegoLimpio;
    interrumpirReproduccion = true;

    Serial.printf(PSTR("[ReplayOS] New game detected: %s [%s]\n"), juegoLimpio, rawSystem);

    // --- GIF: (game -> system -> _default) ---
    char baseSubcarpeta[96], baseDirecta[80], baseSistema[80];
    snprintf(baseSubcarpeta, sizeof(baseSubcarpeta), "/arcade/%s/%s", rawSystem, juegoLimpio);
    snprintf(baseDirecta,    sizeof(baseDirecta),    "/arcade/%s", juegoLimpio);
    snprintf(baseSistema,    sizeof(baseSistema),    "/arcade/%s", rawSystem);
    const char* baseDefault = "/arcade/_default";

    std::vector<String> secuenciaGif;
    bool esGif = false;
    bool juegoIndexado = buscarJuegoEnIndice_ReplayOS(rawSystem, juegoLimpio);

    if (juegoIndexado) {
        esGif = buscarSecuenciaGif(baseSubcarpeta, secuenciaGif) ||
                buscarSecuenciaGif(baseDirecta, secuenciaGif);
    }
    if (!esGif) esGif = buscarSecuenciaGif(baseSistema, secuenciaGif);
    if (!esGif) esGif = buscarSecuenciaGif(baseDefault, secuenciaGif);

    if (esGif) {
        marqueeGifSecuencia = secuenciaGif;
        marqueeGifIndiceSecuencia = 0;
        marqueeEsGif = true;
        estadoActual = ESTADO_ARCADE;
        return;
    }
    marqueeEsGif = false;
    // --- FIN GIF ---

    // --- BMP: (game -> system -> _default) ---
    char rutaSubcarpeta[96], rutaDirecta[80], rutaSistema[80];
    snprintf(rutaSubcarpeta, sizeof(rutaSubcarpeta), "/arcade/%s/%s.bmp", rawSystem, juegoLimpio);
    snprintf(rutaDirecta,    sizeof(rutaDirecta),    "/arcade/%s.bmp", juegoLimpio);
    snprintf(rutaSistema,    sizeof(rutaSistema),    "/arcade/%s.bmp", rawSystem);
    const char* rutaDefault = "/arcade/_default.bmp";

    bool cargadoConExito = false;
    const char* rutaFinalBMP = nullptr;

    if (juegoIndexado) {
        if (SD.exists(rutaSubcarpeta)) {
            rutaFinalBMP = rutaSubcarpeta;
            cargadoConExito = true;
        } else if (SD.exists(rutaDirecta)) {
            rutaFinalBMP = rutaDirecta;
            cargadoConExito = true;
        }
    }

    if (!cargadoConExito && SD.exists(rutaSistema)) {
        rutaFinalBMP = rutaSistema;
        cargadoConExito = true;
    }

    if (!cargadoConExito && SD.exists(rutaDefault)) {
        rutaFinalBMP = rutaDefault;
        cargadoConExito = true;
    }

    if (cargadoConExito) {
        estadoActual = ESTADO_ARCADE;
        display->fillScreen(0);
        mostrarMarquesinaBMP(rutaFinalBMP);
        display->flipDMABuffer();
    } else {
        Serial.println(F("[ReplayOS] Logo and game missing. Falling back to GIF engine"));
        estadoActual = ESTADO_GIFS;
        saliendoAGifs = true;
    }
}
// ====================================================================
//          BATOCERA / RECALBOX ARCADE PLAYBACK ENGINE
// ====================================================================
// --- Persistent state for TCP marquee/animation streaming ---
static WiFiClient marqueeClient;
static uint8_t* marqueeBuffer = nullptr;
static int marqueeIndice = 0;
static unsigned long marqueeUltimoByte = 0;

// Only release network resources (buffer + socket). NEVER touch estadoActual:
// the image or last frame remains on screen until an explicit STOP is received.
void liberarRecursosMarquesina() {
    if (marqueeClient) marqueeClient.stop();
    if (marqueeBuffer) { free(marqueeBuffer); marqueeBuffer = nullptr; }
    marqueeIndice = 0;
}

void verificarMarquesinaTCP() {
    if (WiFi.status() != WL_CONNECTED || arcadeEnable == 0) return;

    // 1. Is there a new client? If one was already active, replace it
    //    (the most recently connected client takes over: useful when switching games quickly).
    WiFiClient nuevoClient = tcpServer.available();
    if (nuevoClient) {
        liberarRecursosMarquesina();
        marqueeClient = nuevoClient;
        marqueeBuffer = (uint8_t*)malloc(12288);
        if (marqueeBuffer == nullptr) {
            Serial.println(F("[ERROR] Not enough RAM for the TCP buffer"));
            marqueeClient.stop();
            return;
        }
        marqueeIndice = 0;
        marqueeUltimoByte = millis();
    }

    if (!marqueeClient || !marqueeClient.connected()) {
        if (marqueeBuffer) liberarRecursosMarquesina();
        return;
    }

    int disponibles = marqueeClient.available();
    if (disponibles == 0) {
        // 3s with no data: it may be a static frame that already closed on its own,
        // or a streamer that died/the network was interrupted mid-game.
        // In BOTH cases, only clear the socket — never touch estadoActual here.
        // Only the explicit STOP (below) can return us to playlist GIFs.
        if (millis() - marqueeUltimoByte > 3000) {
            liberarRecursosMarquesina();
        }
        return;
    }
    marqueeUltimoByte = millis();

    // 2. STOP detection: require all 4 exact bytes "STOP", not just the first,
    //    so a pixel with value 0x53 (='S') is not mistaken for a real command.
    if (marqueeIndice < 4) {
        int necesarios = 4 - marqueeIndice;
        int aLeerCabecera = min(disponibles, necesarios);
        int leidosCabecera = marqueeClient.read(marqueeBuffer + marqueeIndice, aLeerCabecera);
        if (leidosCabecera > 0) marqueeIndice += leidosCabecera;
        disponibles -= leidosCabecera;

        if (marqueeIndice < 4) return; // not enough bytes yet to decide

        if (memcmp(marqueeBuffer, "STOP", 4) == 0) {
            liberarRecursosMarquesina();
            estadoActual = ESTADO_GIFS;
            saliendoAGifs = true;
            interrumpirReproduccion = true;
            return;
        }
        // It was not STOP: those 4 accumulated bytes are valid frame pixels,
        // continue filling the rest normally below.
    }

    // 3. Fill the buffer with whatever is available NOW, without waiting for anything else
    int espacio = 12288 - marqueeIndice;
    int aLeer = min(disponibles, espacio);
    if (aLeer > 0) {
        int leidos = marqueeClient.read(marqueeBuffer + marqueeIndice, aLeer);
        if (leidos > 0) marqueeIndice += leidos;
    }

    // 4. Complete frame? Draw it and keep listening for the next one on the SAME socket
    if (marqueeIndice >= 12288) {
        interrumpirReproduccion = true;
        estadoActual = ESTADO_ARCADE;
        display->fillScreen(0);

        int idx = 0;
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 128; x++) {
                uint8_t r = marqueeBuffer[idx++];
                uint8_t g = marqueeBuffer[idx++];
                uint8_t b = marqueeBuffer[idx++];
                display->drawPixel(x + offset, y, display->color565(r, g, b));
            }
        }
        display->flipDMABuffer();

        marqueeIndice = 0; // do not close: ready to receive the next frame
    }
}

// ====================================================================
//                     GIF PLAYBACK ENGINE
// ====================================================================
String obtenerSiguienteGifSD() {
    // If no playlist is selected, do not try to open anything
    if (playlistActiva[0] == '\0') return "";

    File cacheFile = SD.open(playlistActiva, FILE_READ);
    if (!cacheFile) {
        Serial.print(F("Error: Could not open "));
        Serial.println(playlistActiva);
        return "";
    }

    // Random / Sequential logic
    if (randomMode == 1) {
        uint32_t fileSize = cacheFile.size();
        if (fileSize > 15) { 
            uint32_t randomPos = esp_random() % (fileSize - 10);
            cacheFile.seek(randomPos);
            if (randomPos != 0) {
                while (cacheFile.available()) {
                    if (cacheFile.read() == '\n') break; 
                }
            }
        }
    } else {
        if (gifCachePosition > 0) cacheFile.seek(gifCachePosition);
    }

    if (!cacheFile.available()) {
        cacheFile.seek(0);
        if (randomMode == 0) gifCachePosition = 0;
    }

    String gifPath = cacheFile.readStringUntil('\n');
    gifPath.trim();

    // Save position for sequential mode
    if (randomMode == 0) gifCachePosition = cacheFile.position();
    
    cacheFile.close();
    return gifPath;
}

void ejecutarModoGifLite() {
    if (saliendoAGifs) {
        interrumpirReproduccion = false;
        saliendoAGifs = false;
    }

    if (interrumpirReproduccion) return; // If something blocked it, do nothing

    String gifPath = obtenerSiguienteGifSD();
    if (gifPath == "" || interrumpirReproduccion) return; // If the flag was activated while searching the SD card, exit

    // Open the GIF file
    if (gif.open(gifPath.c_str(), GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw)) {
        
        // Adjust the offset to center on the second panel
        x_offset = (offset - gif.getCanvasWidth()) / 2; 
        y_offset = (PANEL_RES_Y - gif.getCanvasHeight()) / 2; 
        
        display->clearScreen(); 

        int delayMs;
        // GIF frame loop
        while (gif.playFrame(true, &delayMs)) {

            leerControlRemoto();

            // 1. Always handle the web server (PWA, Settings, Timer...)
             server.handleClient();
            if (arcadeEnable > 0) {
                verificarMarquesinaTCP(); // Batocera y Recalbox
                if (arcadeEnable == 3) verificarReplayOSLite(); // ReplayOS
            }

            // 2. Immediate exit if the state or button changes
            if (digitalRead(PIN_BOTON_MENU) == LOW || interrumpirReproduccion || (arcadeEnable > 0 && estadoActual == ESTADO_ARCADE)) {
                interrumpirReproduccion = true;
                break;
            }
            
            display->flipDMABuffer();

            // 3. Replace delay(delayMs) with an "attentive" loop
            unsigned long tiempoInicio = millis();
            while (millis() - tiempoInicio < (unsigned long)delayMs) {

                 leerControlRemoto();
                 server.handleClient();

                if (arcadeEnable > 0) { 
                    verificarMarquesinaTCP();
                    if (arcadeEnable == 3) verificarReplayOSLite();
                    if (estadoActual == ESTADO_ARCADE) {
                        interrumpirReproduccion = true;
                        break; 
                    }
                }
                if (digitalRead(PIN_BOTON_MENU) == LOW) break;
                yield(); // Keeps WiFi stable
            }
            if (interrumpirReproduccion) break;
        }
        
        gif.close();
        
        // Only count the GIF if it played completely
        if (!interrumpirReproduccion) {
            gifsPlayed++;
        }
        
    } else {
        Serial.printf("Error opening GIF: %s\n", gifPath.c_str());
    }
}

// ====================================================================
//                     SETUP AND MAIN LOOP
// ====================================================================
void setup() {
    Serial.begin(115200);
    Serial.println(F("\n=== RETRO PIXEL LED LITE v" FIRMWARE_VERSION " ==="));

    pinMode(PIN_BOTON_MENU, INPUT_PULLUP);

    // 0. Initialize the IR receiver
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); 
    Serial.println(F("IR receiver initialized on GPIO 34"));
    
    // 1. Initialize SD and Load Configuration
    bool sdOk = true;

    SPI.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, SD_CS_PIN);
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println(F("FATAL ERROR: SD card not detected."));
        sdOk = false;
    } else {
        leerConfigIni();
        cargarAjustesTimer();
    }

    // 2. Check whether to enter FTP Mode
    Preferences pFTP;
    pFTP.begin("sistema", true);
    bool entrarFTP = pFTP.getBool("ftp_mode", false);
    pFTP.end();

    // If the SD failed, ignore FTP mode for safety
    if (!sdOk) entrarFTP = false;

    if (sdOk) {
        // 3. Check for Updates
        Preferences prefsOTA;
        prefsOTA.begin("sistema", false);
    
        // A. Is an update pending?
        bool otaPendiente = prefsOTA.getBool("ota_pending", false);
        // B. Did we just complete an update successfully?
        bool otaRecienHecha = prefsOTA.getBool("ota_done", false);
        // C. Language download?
        bool idiomasPendiente = prefsOTA.getBool("idiomas_pending", false);

        if (otaPendiente) {
            prefsOTA.putBool("ota_pending", false);
            prefsOTA.end();
            resultadoOTA = buscarEInstalarOTA();

        } else if (otaRecienHecha) {
            prefsOTA.putBool("ota_done", false); // Clear the flag
            prefsOTA.end();
            resultadoOTA = 1; // Mark SUCCESS for the panel

        } else if (idiomasPendiente) {
            prefsOTA.putBool("idiomas_pending", false);
            prefsOTA.end();

            WiFi.mode(WIFI_STA);
            WiFi.begin(wifi_ssid, wifi_pass);
            int attempts = 0;
            while (WiFi.status() != WL_CONNECTED && attempts < 20) {
                delay(500);
                attempts++;
            }

            String resumenIdiomas;
            int n = 0;
            if (WiFi.status() == WL_CONNECTED) {
                n = descargarIdiomasGitHub(resumenIdiomas);
            } else {
                resumenIdiomas = "No WiFi connection tras reinicio";
            }

            Preferences prefsRes;
            prefsRes.begin("sistema", false);
            prefsRes.putString("idiomas_res", resumenIdiomas);
            prefsRes.putBool("idiomas_ok", n > 0);
            prefsRes.end();

        } else {
            prefsOTA.end();
        }

        // 4. WiFi Connection
        // First check whether WiFi is enabled by the user and whether we are entering FTP
        if (wifiEnable == 1 && !entrarFTP && wifi_ssid[0] != '\0') { 
            WiFi.mode(WIFI_STA);
            WiFi.setSleep(false);
            WiFi.begin(wifi_ssid, wifi_pass);
    
            Serial.print(F("Connecting to update data..."));
            int attempts = 0;
            while (WiFi.status() != WL_CONNECTED && attempts < 20) {
                delay(500);
                Serial.print(F("."));
                attempts++;
            }

            if(WiFi.status() == WL_CONNECTED) {
                Serial.println(F(" Connected!"));
                Serial.print(F("Local IP: ")); Serial.println(WiFi.localIP());
        
                // A. Synchronize Time (Only if the Clock is ON)
                if (clockEnable == 1 || timerEnable == true) {
                    configTzTime(time_zone, ntpServer);
                    // Wait for time synchronization
                    time_t now = time(nullptr);
                    attempts = 0;
                    while (now < 10000 && attempts < 10) { 
                        delay(500); 
                        now = time(nullptr); 
                        attempts++; 
                    }
                }   
                // B. Synchronize Weather (only if Time is ON)
                if (weatherEnable == 1) {
                    actualizarClimaLite();
                    lastWeatherUpdate = millis();
                }       
                // C. If Arcade is enabled, Text Mode is active, or APP configuration is enabled, keep WiFi connected
                if (arcadeEnable > 0 || textEnable || confiAppEnable) {
                    wifiDebeEstarActivo = true;
                    Serial.print(F("[WIFI] Keeping connection active because:"));
    
                    if (arcadeEnable > 0) {
                     const char* nombresArcade[] = {"OFF", "Batocera", "Recalbox", "ReplayOS"};
                        // Ensure the index is in range (1 to 3)
                        int indexArcade = (arcadeEnable >= 1 && arcadeEnable <= 3) ? arcadeEnable : 0;
                        Serial.printf(PSTR(" Arcade mode [%s]"), nombresArcade[indexArcade]);
                    }
    
                    if (textEnable) {
                        Serial.print(F(" Text mode"));
                    }

                    if (confiAppEnable) {
                        Serial.print(F(" App configuration"));
                    }
    
                    Serial.println(); // Final line break
                } else {
                    Serial.println(F("[WIFI] Arcade, Text and App configuration OFF: Disconnecting after update."));
                    WiFi.disconnect(true);
                    WiFi.mode(WIFI_OFF);
                    delay(500);

                    Serial.printf(PSTR("[MEMORIA] RAM libre tras WiFi: %d bytes\n"), ESP.getFreeHeap());
                }

            } else {
                Serial.println(F(" Connection failed."));
            }
        
        } else {
            Serial.println(F("WiFi disabled by user, SSID empty, or FTP Mode active. Skipping network to request data..."));
        }

    }

    // 5. Initialize PANEL
    const int FINAL_MATRIX_WIDTH = PANEL_RES_X * panelChain;

    // A. Dynamic mapping variables (initialized to RGB by default)
    int8_t pR1 = R1_PIN;
    int8_t pG1 = G1_PIN;
    int8_t pB1 = B1_PIN;
    int8_t pR2 = R2_PIN;
    int8_t pG2 = G2_PIN;
    int8_t pB2 = B2_PIN;

    // B. Apply swap logic according to config.ini
    if (strcmp(colorOrder, "RBG") == 0) {
        pG1 = B1_PIN;
        pB1 = G1_PIN;
        pG2 = B2_PIN;
        pB2 = G2_PIN;
        Serial.println(F("[PANEL] Color order set to RBG."));
    } else if (strcmp(colorOrder, "GBR") == 0) {
        pR1 = G1_PIN;
        pG1 = B1_PIN;
        pB1 = R1_PIN;
        pR2 = G2_PIN;
        pG2 = B2_PIN;
        pB2 = R2_PIN;
        Serial.println(F("[PANEL] Color order set to GBR."));
    } else {
        Serial.println(F("[PANEL] Default color order: RGB."));
    }

    // C. Configure pins using the dynamic variables and offset according to the number of panels
    HUB75_I2S_CFG::i2s_pins pin_config = { pR1, pG1, pB1, pR2, pG2, pB2, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN };

    offset = PANEL_RES_X * panelChain;

    // D. Initialize the matrix configuration
    HUB75_I2S_CFG matrix_config(FINAL_MATRIX_WIDTH, MATRIX_HEIGHT, panelChain, pin_config);
    
    if (i2sSpeed == 0) matrix_config.i2sspeed = HUB75_I2S_CFG::HZ_8M;
    else if (i2sSpeed == 1) matrix_config.i2sspeed = HUB75_I2S_CFG::HZ_10M;
    else if (i2sSpeed == 2) matrix_config.i2sspeed = HUB75_I2S_CFG::HZ_16M;
    else if (i2sSpeed == 3) matrix_config.i2sspeed = HUB75_I2S_CFG::HZ_20M;

    matrix_config.latch_blanking = latchBlank;
    matrix_config.min_refresh_rate = refreshMin;
    matrix_config.clkphase = false;
    
    // FTP, Arcade and Transition always with Double Buffer disabled
    bool usarDoubleBuff = (doubleBuff == 1) && !entrarFTP && (arcadeEnable == 0) && (transitionEnable != 1);
        matrix_config.double_buff = usarDoubleBuff;

    Serial.printf(PSTR("[PANEL] Intentando arrancar — DoubleBuff: %s "),usarDoubleBuff ? "ON" : "OFF");    
   
    // First attempt (with the requested configuration)
    display = new MatrixPanel_I2S_DMA(matrix_config);
    if (display) { 
        if (!display || !display->begin()) {
            // Automatic fallback: if double_buff failed, retry with single
            if (usarDoubleBuff) {
                Serial.println(F("[PANEL] Double Buffer insufficient — retrying with Single Buffer..."));
                if (display) { delete display; display = nullptr; }
                matrix_config.double_buff = false;
                display = new MatrixPanel_I2S_DMA(matrix_config);

                if (!display || !display->begin()) {
                    Serial.println(F("[FATAL ERROR] Display cannot start with Single Buffer."));
                    delay(500);
                    ESP.restart();
                }
                Serial.println(F("[PANEL] Single Buffer active (insufficient RAM for Double Buffer)."));
            } else {
                Serial.println(F("[FATAL ERROR] Display cannot start."));
                delay(500);
                ESP.restart();
            }
        } else {
            Serial.println(F("[PANEL] Display OK"));
        }
        display->setTextWrap(false);
        display->setBrightness8(brightness);
        display->fillScreen(0);

        // --- SD FAILURE NOTICE ---
        if (!sdOk) {
            display->setTextColor(display->color565(255, 0, 0));
            display->setCursor(offset + 31, 12);
            display->print("SD ERROR");
            display->flipDMABuffer();
            
            // Complete lock: the system stays here until it is restarted
            while(true) { delay(1000); } 
        }

        // If we enter FTP, jump to the dedicated function and setup ends here
        if (entrarFTP) {
            ejecutarModoFTP(); 
        }

        // --- UPDATE STATUS NOTICE ---
        if (resultadoOTA == 0) { // System is up to date
            display->setTextColor(display->color565(255, 255, 255));
            display->setCursor(offset + 7, 8);
            display->print("You have the latest");
            display->setCursor(offset + 1, 18);
            display->print("No update needed");
            display->flipDMABuffer();
            delay(3000);
            display->fillScreen(0);
        } 
        else if (resultadoOTA == 1) { // UPDATED SUCCESSFULLY!
            display->setTextColor(display->color565(255, 255, 255));
            display->setCursor(offset + 25, 8);
            display->print("Update");
            display->setCursor(offset + 4, 18);
            display->print("Completed Successfully");
            display->flipDMABuffer();
            delay(3000);
            display->fillScreen(0);
        } 
        else if (resultadoOTA == 2) { // Error
            display->setTextColor(display->color565(255, 0, 0));
            display->setCursor(offset + 10, 12);
            display->print("UPDATE ERROR");
            display->flipDMABuffer();
            delay(3000);
            display->fillScreen(0);
        }

        // --- DRAW "RETRO PIXEL" LOGO ---
        display->setTextSize(1);
        display->setTextColor(display->color565(150, 150, 150)); 
        display->setCursor(offset + 30, 3);
        display->print("RETRO PIXEL");

        // --- DRAW "LED" (Colored) ---
        // L in Red
        display->setTextColor(display->color565(255, 0, 0));
        display->setCursor(offset + 40, 15);
        display->print("L");
        
        // E in Green
        display->setTextColor(display->color565(0, 255, 0));
        display->setCursor(offset + 48, 15);
        display->print("E");
        
        // D in Blue
        display->setTextColor(display->color565(0, 0, 255));
        display->setCursor(offset + 56, 15);
        display->print("D");

        // --- DRAW "lite" ---
        display->setTextColor(display->color565(200, 200, 200));
        display->setCursor(offset + 65, 15);
        display->print("lite");

        // --- OUTLINE LINES ---
        uint16_t borderCol = display->color565(80, 80, 80);
        display->drawRect(offset + 28, 1, 72, 11, borderCol); 
        display->drawRect(offset + 33, 13, 62, 11, borderCol); 

        // --- SHOW FIRMWARE VERSION ---
        display->setCursor(offset + 46, 25);
        display->setTextColor(display->color565(100, 100, 100)); // Gris suave
        display->print("v");
        display->print(FIRMWARE_VERSION);

        display->flipDMABuffer();
        delay(1500);
    }
    
    gif.begin(LITTLE_ENDIAN_PIXELS);

    // 5. Start the server
    // Only if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
        registrarRutasWeb();
    }

    if (mostrarIP) {
        // 1. DRAW MOBILE ICON
    uint16_t colorMovil = display->color565(70, 70, 70);       // Dark gray for the outer casing
    uint16_t colorPantalla = display->color565(20, 180, 255);  // Sky blue for the screen
    uint16_t colorDetalle = display->color565(255, 255, 255);  // White for speaker and button

    display->fillScreen(0);

    // Mobile casing (x, y, width, height, radius, color)
    display->fillRoundRect(offset + 8, 4, 14, 24, 2, colorMovil); 

    // Inner screen
    display->fillRect(offset + 10, 7, 10, 16, colorPantalla);

    // Speaker (small horizontal line in the upper frame)
    display->drawLine(offset + 13, 5, offset + 16, 5, colorDetalle);

    // Home button (two pixels in the lower frame)
    display->drawPixel(offset + 14, 25, colorDetalle);
    display->drawPixel(offset + 15, 25, colorDetalle);

    // 2. IP ADDRESS TEXT
    display->setTextSize(1);
    display->setTextColor(display->color565(255, 255, 255)); // White text
    
    // Align the text to the right of the mobile icon and centered on the Y axis
    display->setCursor(offset + 32, 13); 
    display->print(WiFi.localIP().toString());

    display->flipDMABuffer();

    delay(5000);
    display->fillScreen(0);
    }

    // Try to restore the last saved playlist
    Preferences prefs;
    prefs.begin("retro-lite", true); // Read mode
    String tmp = prefs.getString("lastList", "");
    strlcpy(playlistActiva, tmp.c_str(), sizeof(playlistActiva));
    prefs.end();

    // --- FALLBACK LOGIC (PLUG & PLAY) ---
    if (playlistActiva[0] == '\0' || !SD.exists(playlistActiva)) {
        Serial.println(F("[SYSTEM] No active playlist or file not found."));
        
        // 1. Scan the /playlists folder to see what is available
        cargarNombresPlaylists(); 
        
        if (listaPlaylists.size() > 0) {
            // 2. Playlists found! Use the first one that exists on the SD alphabetically
            snprintf(playlistActiva, sizeof(playlistActiva),
             "/playlists/%s.txt", listaPlaylists[0].c_str());
            Serial.print(F("[INFO] Auto-assigning first playlist found: "));
            Serial.println(playlistActiva);
            
            // Save this playlist in memory so it starts faster next time
            prefs.begin("retro-lite", false);
            prefs.putString("lastList", playlistActiva);
            prefs.end();
            
            // Mandamos a reproducir directamente
            interrumpirReproduccion = false;
            estadoActual = ESTADO_GIFS;
        } else {
            // 3. Critical error: There is not a single .txt file in the folder
            Serial.println(F("[CRITICAL] The /playlists folder is empty."));
            estadoActual = ESTADO_MENU_PRINCIPAL;
            interrumpirReproduccion = true;
        }
    } else {
        // The saved playlist exists and is valid
        Serial.print(F("[SYSTEM] Loading playlist: "));
        Serial.println(playlistActiva);
        interrumpirReproduccion = false;
        estadoActual = ESTADO_GIFS;
    }
}

void loop() {
    verificarTemporizador();
    gestionarBotonMenu();
    leerControlRemoto();
    chequearTimeoutMapeo();

    gestionarReconexionWiFi();

    if (WiFi.status() == WL_CONNECTED) {
        server.handleClient();
    }

    // If sleeping, do not process GIFs or menus
    if (isSleeping) {
        delay(100); 
        return;
    }

    if (WiFi.status() == WL_CONNECTED) {
        if (arcadeEnable > 0) 
        verificarMarquesinaTCP();
        if (arcadeEnable == 3) 
        verificarReplayOSLite(); 
    }

    if (estadoActual == ESTADO_ARCADE) {
        if (marqueeEsGif) reproducirMarquesinaGIF_ReplayOS();
        else delay(50);
        return;  
    }

    if (estadoActual == ESTADO_TEXTO) {
    mostrarTextoScroll();
    delay(5);
    return;
    }

    if (estadoActual == ESTADO_CONFIG_APP) {
    delay(50);
    return;   // only serve HTTP requests; do not touch GIFs/clock
    }

    // Decide what to draw on the panel
    if (estadoActual == ESTADO_GIFS) {

        if (saliendoAGifs) {
            // Full reset when returning to GIFs
            gifCachePosition = 0; 
            gifsPlayed = 0;
            interrumpirReproduccion = false;        
            botonPresionado = false;
            confirmadoLargo = false;      
            confirmadoExtraLargo = false; 

            if (digitalRead(PIN_BOTON_MENU) == LOW) {
                bloqueoPostSalida = true;        
            } else {
                bloqueoPostSalida = false;
            }
            
            tiempoPresionado = millis();
            saliendoAGifs = false; 
            Serial.println(F("[SYSTEM] Returning to GIFs: Button flags reset."));
        }

        if (modoVisual == 1) { 
            // --- MODE: CLOCK ONLY ---
            mostrarRelojLite(false);
            gestionarActualizacionClima(); // Handles the weather data
            
        } else {
            // --- GIF MODE ---
            // Is it time to show the clock?
            if (clockEnable == 1 && autoClockInt > 0 && gifsPlayed >= autoClockInt) {
        
                // A. Show the clock on screen
                if (transitionEnable == 1) {
                    mostrarRelojLite(true); // With particle transition 
                }else
                    mostrarRelojLite(false);// Without particle transition
        
                // B. Handle data updates (only when the interval is due)
                gestionarActualizacionClima(); 

                // C. Reset counter to return to GIFs
                gifsPlayed = 0;

            } else {
                ejecutarModoGifLite();
            }
        }

    }else {
    // --- MENU MODE ---
    static EstadoSistema prevEstado    = ESTADO_GIFS;
    static int           prevCursorP   = -1;
    static int           prevCursorS   = -1;
    static int           prevPasoMapeo = -2;

    bool hayCambio = menuNeedsRedraw                       ||
                     (estadoActual    != prevEstado)       ||
                     (cursorPrincipal != prevCursorP)      ||
                     (cursorSubmenu   != prevCursorS)      ||
                     (pasoMapeo       != prevPasoMapeo);

    if (hayCambio) {
        menuNeedsRedraw = false;
        prevEstado    = estadoActual;
        prevCursorP   = cursorPrincipal;
        prevCursorS   = cursorSubmenu;
        prevPasoMapeo = pasoMapeo;
        dibujarMenuOSD();
    }
    delay(30);

    }
}

