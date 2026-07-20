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


// ====================================================================
//                     CONSTANTES & FIRMWARE LITE
// ====================================================================
#define FIRMWARE_VERSION "3.0.9" // Nueva función Texto Scroll con APP web e implementación de la función Arcade 2.0
#define CURRENT_VERSION_NUM 309 // Versión numérica para comparar (2.1.0 -> 210)
#define GITHUB_VERSION_URL "https://github.com/fjgordillo86/RetroPixelLED-Lite/raw/refs/heads/main/docs/version.json"
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

// --- CONFIGURACIÓN DE LAS DIMENSIONES DEL PANEL ---
const int PANEL_RES_X = 64; // Ancho de un solo panel
const int PANEL_RES_Y = 32; // Alto de un solo panel
#define MATRIX_HEIGHT PANEL_RES_Y 

// --- CONFIGURACIÓN DEL BOTÓN DE MENÚ ---
#define PIN_BOTON_MENU 21

// --- CONFIGURACIÓN DEL IR ---
#define IR_RECEIVE_PIN 34


// ====================================================================
//                     VARIABLES GLOBALES LITE
// ====================================================================
AnimatedGIF gif;
MatrixPanel_I2S_DMA *display = nullptr; 
File FSGifFile;

// Variables de Configuración (Valores por defecto seguros)
bool wifiEnable = false;
char wifi_ssid[64] = "";
char wifi_pass[64] = "";
char time_zone[64] = "CET-1CEST,M3.5.0,M10.5.0/3";

int modoVisual = 0; // 0 = GIFs, 1 = Solo Reloj
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
uint32_t textoScrollColor = 0xFFFFFF;
int textoScrollVelocidad = 30;
int textoScrollX = 0;
int textoScrollAncho = 0;
unsigned long textoScrollUltimoPaso = 0;

bool timerEnable = false;
int hOn = 9, mOn = 0;    // Hora de encendido (por defecto 09:00)
int hOff = 23, mOff = 0; // Hora de apagado (por defecto 23:00)
bool isSleeping = false;
bool manualOverride = false; // Controla si se despertó con el botón

unsigned long gifCachePosition = 0;
int gifsPlayed = 0;
int x_offset = 0;
int y_offset = 0;
const char* ntpServer = "pool.ntp.org";
WebServer server(80);
WiFiServer tcpServer(8888);

// Variables de estado y Navegación OSD
enum EstadoSistema {
    ESTADO_GIFS,
    ESTADO_ARCADE,
    ESTADO_TEXTO,
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


// Variables de Navegación
int cursorPrincipal = 0; // Posición de la flecha en el menú principal
int cursorSubmenu = 0;   // Posición de la flecha dentro de los submenús
bool requiereReinicio = false; // Se activará si tocamos ajustes Avanzados
bool confirmadoLargo = false;      // Disparo a 1s
bool confirmadoExtraLargo = false; // Disparo a 4s
bool botonPresionado = false;
bool bloqueoPostSalida = false;
unsigned long tiempoPresionado = 0;
unsigned long ultimoIncremento = 0;
bool saliendoAGifs = false;
bool menuNeedsRedraw = false;

// Variables del sistema de guardado en memoria
char playlistActiva[128] = "";
bool interrumpirReproduccion = false;
bool modoMantenimiento = false;

// Variables de idioma
JsonDocument idiomaDoc; 
bool idiomaCargado = false;

// Variables FTP
FtpServer ftpSrv;

// Variables de lectura dinámica
std::vector<String> listaPlaylists;
std::vector<String> listaIdiomas;

// Variables de Clima
int weatherEnable = 0;
char weatherCustomMsg[32] = "";
char weatherCity[64] = "";
char weatherKey[48] = "";
int weatherInterval = 60; // En minutos
float currentTemp = 0.0;
int weatherConditionCode = 0;
bool isNight = false;
unsigned long lastWeatherUpdate = 0;
bool weatherDataReady = false;

// Iconos 8x8 píxeles
const uint8_t icon_sun[] PROGMEM = {0x00, 0x3c, 0x7e, 0x7e, 0x7e, 0x7e, 0x3c, 0x00};
const uint8_t icon_cloud[] PROGMEM = {0x00, 0x00, 0x1c, 0x3f, 0x7f, 0x7f, 0x00, 0x00};
const uint8_t icon_rain[] PROGMEM = {0x1c, 0x3f, 0x7f, 0x7f, 0x22, 0x44, 0x22, 0x00};
const uint8_t icon_snow[] PROGMEM = {0x24, 0x00, 0xbd, 0x3c, 0x3c, 0xbd, 0x00, 0x24};
const uint8_t icon_storm[] PROGMEM = {0x1c, 0x3f, 0x7f, 0x1c, 0x1c, 0x08, 0x10, 0x00};
const uint8_t icon_fog[] PROGMEM = {0x00, 0x3e, 0x00, 0x7f, 0x00, 0x1c, 0x3e, 0x00};
const uint8_t icon_moon[] PROGMEM = {0x1c, 0x38, 0x70, 0x70, 0x70, 0x38, 0x1c, 0x00};

// --- CONFIGURACIÓN RELOJ ---
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

// Variables de control de color para el Reloj
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

// --- VARIABLES DINÁMICAS DEL CONTROL REMOTO IR ---
uint32_t ir_btn_on     = 0xF20DFF00;
uint32_t ir_btn_off    = 0xE01FFF00;
uint32_t ir_btn_up     = 0xF609FF00;
uint32_t ir_btn_down   = 0xE21DFF00;
uint32_t ir_btn_menu   = 0xEA15FF00;
uint32_t ir_btn_ok     = 0xED12FF00;
uint32_t ir_btn_subir  = 0xE41BFF00;
uint32_t ir_btn_bajar  = 0xB34CFF00;

// Variable para el estado de mapeo
int pasoMapeo = -1; // -1 significa que estamos navegando, >= 0 significa que estamos capturando
unsigned long tiempoInicioMapeo = 0;
const unsigned long TIMEOUT_MAPEO = 10000; // 10 segundos


// ====================================================================
//                     GESTOR DE CONFIG.INI
// ====================================================================
void leerConfigIni() {
    File configFile = SD.open(CONFIG_FILE, FILE_READ);
    if (!configFile) {
        Serial.println(F("[INI] Error: No se encuentra config.ini. Usando valores por defecto."));
        return;
    }

    Serial.println(F("[INI] Cargando configuración..."));

    while (configFile.available()) {
        String linea = configFile.readStringUntil('\n');
        linea.trim();
        
        // Ignorar comentarios, cabeceras de sección o líneas vacías
        if (linea.startsWith("#") || linea.startsWith("[") || linea.length() == 0) continue;

        // Buscar el separador '='
        int separatorIndex = linea.indexOf('=');
        if (separatorIndex == -1) continue;

        String clave = linea.substring(0, separatorIndex);
        String valor = linea.substring(separatorIndex + 1);
        
        // Limpiar espacios extra
        clave.trim();
        valor.trim();

        // Eliminar comentarios inline si los hay (ej: BRIGHTNESS=40 # Brillo)
        int commentIdx = valor.indexOf('#');
        if (commentIdx != -1) {
            valor = valor.substring(0, commentIdx);
            valor.trim();
        }

        // --- MAPEO DIRECTO A VARIABLES GLOBALES ---
        // [WIFI_NTP]
        if (clave == "WIFI_ENABLE") wifiEnable = valor.toInt();
        else if (clave == "SSID") strlcpy(wifi_ssid,  valor.c_str(), sizeof(wifi_ssid));
        else if (clave == "PASS") strlcpy(wifi_pass,  valor.c_str(), sizeof(wifi_pass));
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


    // VALIDACIÓN DE SEGURIDAD POST-LECTURA
    if (colorOrder[0] == '\0') strlcpy(colorOrder, "RGB", sizeof(colorOrder));
    if (weatherCustomMsg[0]== '\0') strlcpy(weatherCustomMsg,"Game Room", sizeof(weatherCustomMsg));
    if (idiomaActivo[0] == '\0') strlcpy(idiomaActivo, "ES", sizeof(idiomaActivo));

    if (doubleBuff && i2sSpeed == 3) {
        i2sSpeed = 2; // Forzamos 16MHz si el Double Buffer está activo
        Serial.println(F("[SEGURIDAD] Configuración incompatible detectada en SD: Bajando I2S a 16MHz."));
    }
    
    Serial.println(F("[INI] Carga completada."));
}

void guardarConfigIni() {
    File configFile = SD.open(CONFIG_FILE, FILE_WRITE);
    if (!configFile) {
        Serial.println(F("[ERROR] No se pudo abrir config.ini para escribir"));
        return;
    }

    Serial.println(F("[INI] Guardando nuevos ajustes en SD..."));

    // Cabecera Principal
    configFile.println(F("# ============================================================"));
    configFile.printf(F("# 🕹️ RETRO PIXEL LED LITE - CONFIGURACION v%s\n"), FIRMWARE_VERSION);
    configFile.println(F("# ============================================================"));
    configFile.println(F("# Nota: No dejes espacios alrededor del símbolo '='."));
    configFile.println(F("# Ejemplo correcto: BRIGHTNESS=40\n"));

    configFile.println(F("[WIFI_NTP]"));
    configFile.println(F("# Configura tu red WiFi"));
    configFile.printf("WIFI_ENABLE=%d\n", wifiEnable);
    configFile.printf("SSID=%s\n", wifi_ssid);
    configFile.printf("PASS=%s\n", wifi_pass);
    configFile.println(F("# Configura tu zona horaria"));
    configFile.printf("TZ=%s\n\n", time_zone);

    configFile.println(F("[HARDWARE]"));
    configFile.println(F("# Numero de paneles"));
    configFile.printf("PANEL_CHAIN=%d\n", panelChain);
    configFile.println(F("# Orden de colores del Panel: RGB, RBG o GBR"));
    configFile.printf("COLOR_ORDER=%s\n", colorOrder);
    configFile.println(F("# Brillo (0 a 255)"));
    configFile.printf("BRIGHTNESS=%d\n", brightness);
    configFile.println(F("# Velocidad I2S: 0=8MHz, 1=10MHz, 2=16MHz, 3=20MHz (Turbo)"));
    configFile.printf("I2S_SPEED=%d\n", i2sSpeed);
    configFile.println(F("# Refresco Minimo (Hz): 30 a 120"));
    configFile.printf("REFRESH_MIN=%d\n", refreshMin);
    configFile.println(F("# Doble Buffer: 0=OFF, 1=ON (Elimina parpadeos)"));
    configFile.printf("DOUBLE_BUFF=%d\n", doubleBuff);
    configFile.println(F("# Anti-Ghosting: 1 a 4 (Sube si ves brillo fantasma)"));
    configFile.printf("LATCH_BLANK=%d\n\n", latchBlank);

    configFile.println(F("[LOGIC]"));
    configFile.println(F("# Modo de visualizacion: 0=GIFs, 1=Solo Reloj"));
    configFile.printf("PLAY_MODE=%d\n", modoVisual);
    configFile.println(F("# Selecciona tu sistema Arcade: 0=OFF, 1=Batocera, 2=Recalbox, 3=ReplayOS"));
    configFile.printf("ARCADE_ENABLE=%d\n", arcadeEnable);
    configFile.printf("TEXT_ENABLE=%d\n", textEnable ? 1 : 0);
    configFile.println(F("# Activa o desactiva el reloj: 0=OFF, 1=ON (Requiere WiFi)"));
    configFile.printf("CLOCK_ENABLE=%d\n", clockEnable);
    configFile.println(F("# Modo de reproduccion: 0=Secuencial, 1=Aleatorio"));
    configFile.printf("RANDOM_MODE=%d\n", randomMode);
    configFile.println(F("# Intervalo: Cada cuantos GIFs aparece el reloj"));
    configFile.printf("AUTO_CLOCK_INT=%d\n", autoClockInt);
    configFile.println(F("# Duracion: Cuantos segundos se muestra el reloj"));
    configFile.printf("CLOCK_DURATION=%d\n", clockDuration);
    configFile.println(F("# Estilos: 0=Matrix, 1=Solid, 2=Rainbow, 3=Pulse, 4=Gradient"));
    configFile.printf("CLOCK_STYLE=%d\n", clockStyle);
    configFile.println(F("# Activa la transicion del reloj a GIFs con una explosion de particulas : 0=OFF, 1=ON"));
    configFile.printf("TRANSITION_ENABLE=%d\n", transitionEnable);
    configFile.println(F("# Color del reloj (0= Blanco, 1=Rojo, 2=Verde, 3=Azul, 4=Amarillo, 5=Cian, 6=Magenta, 7=Naranja, 8=Rosa)"));
    configFile.printf("CLOCK_COLOR=%d\n\n", clockColorIndex); ; 

    configFile.println(F("[WEATHER]"));
    configFile.println(F("# Activa el clima: 0=OFF, 1=ON (Requiere WiFi)"));
    configFile.printf("WEATHER_ENABLE=%d\n", weatherEnable);
    configFile.println(F("# Tu ciudad (Sin espacios, usa '+' si es necesario: Madrid,ES o Buenos+Aires,AR)"));
    configFile.printf("CITY=%s\n", weatherCity);
    configFile.println(F("# Tu API Key gratuita de OpenWeatherMap"));
    configFile.printf("API_KEY=%s\n", weatherKey);
    configFile.println(F("# Intervalo de actualizacion del clima en MINUTOS"));
    configFile.printf("WEATHER_INT=%d\n", weatherInterval);
    configFile.println(F("# Texto que se muestra encima del reloj"));
    configFile.printf("WEATHER_MSG=%s\n\n", weatherCustomMsg);

    configFile.println(F("[LANGUAGE]"));
    configFile.println(F("# Indica el Idioma (Nombre del archivo sin .json: ES, EN, FR...)"));
    configFile.printf("LANGUAGE=%s\n\n", idiomaActivo);

    configFile.println(F("[IR_REMOTE]"));
    configFile.println(F("# Códigos HEX del mando IR (NO hay que indicar nada los guardará automaticamente Retro Pixel LED)"));
    configFile.printf("BTN_ON=%08X\n", ir_btn_on);
    configFile.printf("BTN_OFF=%08X\n", ir_btn_off);
    configFile.printf("BTN_BRILLO_UP=%08X\n", ir_btn_up);
    configFile.printf("BTN_BRILLO_DOWN=%08X\n", ir_btn_down);
    configFile.printf("BTN_MENU=%08X\n", ir_btn_menu);
    configFile.printf("BTN_OK=%08X\n", ir_btn_ok);
    configFile.printf("BTN_SUBIR=%08X\n", ir_btn_subir);
    configFile.printf("BTN_BAJAR=%08X\n\n", ir_btn_bajar);

    configFile.println(F("[REPLAY_OS]"));
    configFile.println(F("# IP que tiene asignada ReplayOS"));
    configFile.printf("IP=%s\n", replayOS_IP);
    configFile.println(F("# Token ReplayOS: SYSTEM > INFORMATION > NET CONTROL CODE"));
    configFile.printf("TOKEN=%s\n\n", replayOS_Token);

    configFile.println(F("[END]"));
    configFile.close();
    
    Serial.println(F("[INI] Guardado exitoso."));
}

// ====================================================================
//                        GESTIÓN DE IDIOMAS
// ====================================================================

// Función para centrar texto automáticamente en paneles de 64px
void printMenuCentrado(const char* texto, int y, uint16_t color, int xOffset) {
    int numLetras = strlen(texto);
    // Cada letra mide 6px de ancho
    int xCalculada = (PANEL_RES_X - (numLetras * 6/2));
    if (xCalculada < 0) xCalculada = 0; // Evitar salirse por la izquierda
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
    
    Serial.printf("[IDIOMA] No encontrada: %s -> %s\n", seccion, clave);
    return "ErrorText"; 
}

void cargarIdiomaMenu() {
    if (!idiomaCargado) {
        char ruta[40];
        snprintf(ruta, sizeof(ruta), "/idioma/%s.json", idiomaActivo);
        File file = SD.open(ruta);
        
        if (!file) {
            Serial.print(F("[IDIOMA] Error: No existe ")); 
            Serial.println(ruta);
            return;
        }

        DeserializationError error = deserializeJson(idiomaDoc, file);
        file.close();

        if (error) {
            Serial.print(F("[IDIOMA] Error deserializando: "));
            Serial.println(error.c_str());
            
        } else {
            idiomaDoc.shrinkToFit(); // Libera lo que no se usa del buffer reservado
            idiomaCargado = true;
            Serial.println(F("[IDIOMA] Cargado exitosamente"));
        }
    }
}

void liberarIdiomaMenu() {
    if (idiomaCargado) {
        idiomaDoc.clear(); // Libera la memoria interna
        idiomaCargado = false;
        Serial.println(F("[IDIOMA] RAM liberada."));
    }
}

// ====================================================================
//                      FUNCIONES PARA MANDO IR
// ====================================================================

void chequearTimeoutMapeo() {
    // Si estamos esperando un código IR (pasoMapeo != -1)
    if (pasoMapeo != -1) {
        if (millis() - tiempoInicioMapeo >= TIMEOUT_MAPEO) {
            // Feedback visual: mostramos un mensaje de confirmación
            display->fillScreen(0);
            printMenuCentrado("FAIL", 12, display->color565(255, 0, 0), offset);
            display->flipDMABuffer();

            delay(1000); 
            Serial.println(F("[IR] Timeout: Cancelando mapeo por inactividad."));
            pasoMapeo = -1;   // Cancelamos el modo mapeo
            dibujarMenuOSD(); 
        }
    }
}

void gestionarMapeoIR(uint32_t codigoHex) {
    // Si no hay ningún botón seleccionado para mapear, ignoramos
    if (pasoMapeo < 0 || pasoMapeo > 7) return;

    // Asignamos el código recibido a la variable global correspondiente
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

    // Feedback visual: mostramos un mensaje de confirmación
    display->fillScreen(0);
    printMenuCentrado("OK", 12, display->color565(0, 255, 0), offset);
    display->flipDMABuffer();
    
    Serial.printf("[IR] Mapeado botón %d con código: %08X\n", pasoMapeo, codigoHex);
    
    delay(1000); // Pausa para que el usuario vea la confirmación
    pasoMapeo = -1; // Volvemos al modo navegación
    dibujarMenuOSD();
}

void procesarComandoIR(uint32_t codigoHex) {
    //Solo capturamos si estamos en el submenú Y ya hemos seleccionado un botón para mapear (pasoMapeo != -1)
    if (estadoActual == ESTADO_SUBMENU_MAPEADO_IR && pasoMapeo != -1) {
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
        
        // Evitar procesar códigos vacíos o errores de lectura (0x0)
        if (codigoRecibido != 0) {
            procesarComandoIR(codigoRecibido);
        }
        
        IrReceiver.resume(); // Preparar el receptor para el siguiente código
    }
}

void ajustarBrillo(int direccion) {
    // 1. Convertimos el valor actual (0-255) a porcentaje (0-100)
    int br = (brightness * 100) / 255;
    
    // 2. Aplicamos el paso de 5 en 5
    br += (direccion * 5); 
    
    // 3. Comportamiento cíclico, da la vuelta completa
    if (br > 100) br = 5; // Si pasa de 100, salta al mínimo (5%)
    if (br < 5) br = 100; // Si baja de 5, salta al máximo (100%)
    
    // 4. Convertimos de vuelta a escala 0-255
    brightness = (br * 255) / 100;
    
    // 5. Aplicamos al panel
    display->setBrightness8(brightness);

    // 6. Recargamos el menú
    menuNeedsRedraw = true;
    
    Serial.printf("[BRILLO] Ajustado a: %d%% (%d/255)\n", br, brightness);
}

// ====================================================================
//              FUNCIONES PARA EL MENÚ CONFIGURACIÓN
// ====================================================================
void cargarNombresIdiomas() {
    listaIdiomas.clear();
    
    File root = SD.open("/idioma");
    if (!root || !root.isDirectory()) {
        SD.mkdir("/idioma"); // Si no existe, la creamos
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
        SD.mkdir("/playlists"); // Si no existe, la creamos
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

    // 1. SI EL BOTÓN SE SUELTA
    if (!lectura) {
        if (botonPresionado) {
            unsigned long duracionFinal = ahora - tiempoPresionado;
            
            // Si NO hemos llegado a activar el modo sueño (confirmadoExtraLargo)
            if (!confirmadoExtraLargo && !bloqueoPostSalida) {

                // PULSACIÓN CORTA (< 1 segundo)
                if (duracionFinal > 50 && duracionFinal < 1000) {
                    
                     // A. Si estamos en sueño despertamos
                    if (isSleeping) {
                        toggleEnergia(false); // Despertar
                        manualOverride = true;
                    }
                    // B. Si estamos en GIF o Arcade -> Entrar al Menú Principal
                    else if (estadoActual == ESTADO_GIFS || estadoActual == ESTADO_ARCADE) {
                        ejecutarAccionConfirmar(); 
                    } 
                    else {
                        // C. Si estamos DENTRO de un menú -> Navegar por las opciones
                        ejecutarAccionNavegar(1);
                    }
                }
            }
            botonPresionado = false;
        }
        bloqueoPostSalida = false; 
        return;
    }

    // 2. INICIO DE PULSACIÓN
    if (bloqueoPostSalida) return; 

    if (!botonPresionado) {
        tiempoPresionado = ahora;
        botonPresionado = true;
        confirmadoLargo = false;
        confirmadoExtraLargo = false;
        return;
    }

    // 3. MIENTRAS ESTÁ PULSADO (ACCIONES POR TIEMPO)
    unsigned long duracionActual = ahora - tiempoPresionado;

    // --- MODO SUEÑO (2 SEGUNDOS) ---
    // Solo desde la pantalla de GIFs/Arcade
    if ((estadoActual == ESTADO_GIFS || estadoActual == ESTADO_ARCADE) && duracionActual >= 2000 && !confirmadoExtraLargo) {
        confirmadoExtraLargo = true;
        confirmadoLargo = true; 
        toggleEnergia(!isSleeping);
        manualOverride = true;
        return;
    }

    // --- CONFIRMAR DENTRO DE MENÚS (1 SEGUNDO) ---
    // Mantenemos pulsado 1 segundo para entrar a submenús, guardar o salir.
    if (estadoActual != ESTADO_GIFS && estadoActual != ESTADO_ARCADE && duracionActual >= 1000 && !confirmadoLargo) {
        bool esCampoHora = (estadoActual == ESTADO_SUBMENU_TEMPORIZADOR && (cursorSubmenu == 1 || cursorSubmenu == 2));
        
        if (esCampoHora) {
            ejecutarAccionConfirmar(); // Resta 5 min
            confirmadoLargo = true;
        } else {
            confirmadoLargo = true;
            ejecutarAccionConfirmar();
            return;
        }
    }

    // --- AUTO-INCREMENTO (SUMAR 5 MINUTOS) ---
    // Se activa después de 2 segundos para no chocar con la confirmación de restar
    if (estadoActual == ESTADO_SUBMENU_TEMPORIZADOR && (cursorSubmenu == 1 || cursorSubmenu == 2) && duracionActual > 2000) {
        if (ahora - ultimoIncremento > 200) { 
            if (cursorSubmenu == 1) { // Incrementar ON
                mOn += 5;
                if (mOn >= 60) { mOn = 0; hOn++; }
                if (hOn >= 24) hOn = 0;
            } 
            else if (cursorSubmenu == 2) { // Incrementar OFF
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
            // Según donde esté el cursor, entramos a un submenú o salimos
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
            else if (cursorPrincipal == 11) { // GUARDAR Y SALIR
                Serial.println(F("[MENU] Guardando ajustes..."));
                guardarConfigIni(); 
                guardarAjustesTimer();
        
                if (requiereReinicio) {
                    display->fillScreen(0);
                    printMenuCentrado(msg("ESTADOS", "reinicio"), 12, display->color565(255, 255, 255), offset);
                    display->flipDMABuffer();
                    delay(1000);
                    ESP.restart();
                } else {
                    // Feedback visual: Parpadeo de confirmación
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

            }else if (cursorPrincipal ==12) { // SALIR SIN GUARDAR
                // Feedback visual: Parpadeo de confirmación
                for(int i = 0; i < 3; i++) { 
                    display->fillScreen(0); 
                    display->flipDMABuffer(); 
                    delay(80); 
                    dibujarMenuOSD(); 
                    delay(80); 
                }

                Serial.println(F("[MENU] Saliendo sin guardar..."));

                while(digitalRead(PIN_BOTON_MENU) == LOW) { delay(10); }
                // Recargamos el archivo original de la SD para descartar cambios en memoria
                leerConfigIni();
                liberarIdiomaMenu();
                estadoActual = ESTADO_GIFS; 
                saliendoAGifs = true;       
            }

        break;

        case ESTADO_SUBMENU_PLAYLIST:
            // Comprobamos si el cursor está sobre una Playlist real
            if (cursorSubmenu < listaPlaylists.size()) {       
                // 1. Construir la ruta
                snprintf(playlistActiva, sizeof(playlistActiva),
                        "/playlists/%s.txt", listaPlaylists[cursorSubmenu].c_str());
        
                // 2. Guardar en memoria permanente (Preferences)
                Preferences prefs;
                prefs.begin("retro-lite", false);
                prefs.putString("lastList", playlistActiva);
                prefs.end();
        
                // 3. Feedback visual: Parpadeo de confirmación
                for(int i = 0; i < 3; i++) { 
                    display->fillScreen(0); 
                    display->flipDMABuffer(); 
                    delay(80); 
                    dibujarMenuOSD(); 
                    delay(80); 
                }
        
                // 4. Resetear índices y salir a reproducir
                while(digitalRead(PIN_BOTON_MENU) == LOW) { delay(10); }
                liberarIdiomaMenu();
                estadoActual = ESTADO_GIFS;
                saliendoAGifs = true;
                Serial.print(F("[PLAYLIST] Seleccionada: ")); 
                Serial.println(playlistActiva);
            }else {
                
                estadoActual = ESTADO_MENU_PRINCIPAL;
                Serial.println(F("[PLAYLIST] Volviendo al menú principal sin cambios."));
            }
        break;

        case ESTADO_SUBMENU_REPRODUCCION:
            if (cursorSubmenu == 0) {
                modoVisual = !modoVisual; // Cambia entre 0 (GIFs) y 1 (Reloj)
            } else if (cursorSubmenu == 1) {
                randomMode = !randomMode; // Cambia entre 0 y 1
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
                // Si estaba en 0 y lo pasamos a 1, activamos el reinicio
                if (!wifiEnable) { 
                    requiereReinicio = true; 
                }
                wifiEnable = !wifiEnable;
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
                if (clockDuration > 30) clockDuration = 5; // Salto de 5s a 30s
            } else if (cursorSubmenu == 3) {
                clockStyle++;
                if (clockStyle > 4) clockStyle = 0; // Matrix, Solid, Rainb, Pulse, Grad
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
                // Si estaba en 0 y lo pasamos a 1, activamos el reinicio
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
            }else if (cursorSubmenu == 1) { // --- RESTAR 5 min a ON ---
                if (mOn == 0) {
                    mOn = 55; // Salta al último tramo de la hora anterior
                    if (hOn == 0) hOn = 23;
                    else hOn--;
                } else {
                    mOn -= 5;
                }
            }else if (cursorSubmenu == 2) { // --- RESTAR 5 min a OFF ---
                if (mOff == 0) {
                    mOff = 55; // Salta al último tramo de la hora anterior
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
                // Si llegamos a 20MHz (3) con Double Buffer, saltamos directamente a 8MHz (0)
                if (i2sSpeed > 3 || (i2sSpeed == 3 && doubleBuff == true)) {
                i2sSpeed = 0; 
                if (doubleBuff) Serial.println(F("[SEGURIDAD] Saltando 20MHz por Double Buffer activo."));
            }
            requiereReinicio = true;
            }else if (cursorSubmenu == 1) {
                refreshMin += 30; if (refreshMin > 120) refreshMin = 30;
                requiereReinicio = true;
            }else if (cursorSubmenu == 2) {
                doubleBuff = !doubleBuff;
                // REGLA DE SEGURIDAD: Si activamos Double Buffer pero la velocidad es 20MHz (3)
                if (doubleBuff == true && i2sSpeed == 3) {
                    i2sSpeed = 2; // Bajamos automáticamente a 16MHz
                    Serial.println(F("[SEGURIDAD] Doble Buffer activado. Bajando I2S a 16MHz para evitar reinicios."));
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
                ESP.restart(); // Reinicio inmediato para limpiar
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
                estadoActual = ESTADO_MENU_PRINCIPAL;
            }
            break;
        
        case ESTADO_SUBMENU_FTP:
            if (cursorSubmenu == 0) {
                display->fillScreen(0);
                printMenuCentrado(msg("FTP", "reset"), 8, display->color565(255, 255, 255), offset);
                printMenuCentrado(msg("FTP", "iniciar"), 18, display->color565(255, 255, 255), offset);
                display->flipDMABuffer();

                Serial.println(F("[MENU] Activando Modo FTP y reiniciando..."));

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
        
                // Liberamos el JSON actual y cargamos el nuevo para que el menú cambie al instante
                liberarIdiomaMenu();
                cargarIdiomaMenu();
        
                // Guardar en Preferences para que persista al reiniciar
                Preferences prefs;
                prefs.begin("retro-lite", false);
                prefs.putString("lang", idiomaActivo);
                prefs.end();
        
                // Feedback visual
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
            // En los demás submenús, una pulsación larga vuelve al menú principal
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
            // Un toque corto mientras hay GIFs también abre el menú
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
            // Aquí el brillo sube o baja de 5 en 5 según el botón pulsado
            br += (paso * 5); 
            // Comportamiento ciclico, da la vuelta completa
            if (br > 100) br = 5; // Si pasa de 100, vuelve al mínimo
            if (br < 5) br = 100; // Si baja de 5, salta al máximo
            brightness = (br * 255) / 100;
            display->setBrightness8(brightness);
        }    
            break;

        case ESTADO_SUBMENU_WIFI:
            cursorSubmenu += paso;
            if (cursorSubmenu > 1) cursorSubmenu = 0;
            if (cursorSubmenu < 0) cursorSubmenu = 1;
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
            // Si intentamos "Subir" (paso -1) estando en las filas de tiempo (1 o 2)
            if (paso == -1 && (cursorSubmenu == 1 || cursorSubmenu == 2)) {
                if (cursorSubmenu == 1) { // Fila ON
                    mOn += 5;
                    if (mOn >= 60) { mOn = 0; hOn++; }
                    if (hOn >= 24) hOn = 0;
                } else { // Fila OFF
                    mOff += 5;
                    if (mOff >= 60) { mOff = 0; hOff++; }
                    if (hOff >= 24) hOff = 0;
                }
                // No movemos el cursor, solo actualizamos el valor
            } else {
                // En cualquier otro caso
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
            if (cursorSubmenu > 1) cursorSubmenu = 0; 
            if (cursorSubmenu < 0) cursorSubmenu = 1; 
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
//                       MOTOR DE DIBUJO OSD 
// ====================================================================
void dibujarMenuOSD() {

    // Si estamos esperando una pulsación del mando, mostramos pantalla de espera
    if (pasoMapeo != -1) {
        display->fillScreen(0);
        printMenuCentrado(msg("SUBMENU_MAPEADO_IR", "pulsar"), 12, display->color565(255, 255, 0), offset);
        display->flipDMABuffer();
        return; 
    }

    display->fillScreen(0); // Fondo negro limpio
    display->setTextSize(1); // forzamos tamaño del texto a 1 por si venimos del modo Texto

    // Buffer para construir títulos dinámicos con paginación
    char titleBuf[32]; 

    // ----------------------------------------------------------------
    // 1. DIBUJO DEL MENÚ PRINCIPAL
    // ----------------------------------------------------------------
    if (estadoActual == ESTADO_MENU_PRINCIPAL) {
        
        // Lógica de Paginación (3 ítems por página)
        int pagina = cursorPrincipal / 3; 
        int primerItem = pagina * 3;

        // Título con centrado automático. Usa el espacio del JSON (ej: "MENU PRINCIPAL ") y añade la página
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/5", msg("MENU", "titulo"), pagina + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100)); // Separador

        // Dibujar los 3 ítems de la página actual
        for (int i = 0; i < 3; i++) {
            int itemIndex = primerItem + i;
            if (itemIndex > 12) break; // Tenemos 13 opciones (0 a 12)

            int yPos = 9 + (i * 8);

            // Dibujar el Cursor '>'
            if (cursorPrincipal == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0)); // Amarillo para la selección
                display->setCursor(offset + 2, yPos);
                display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // Blanco para el texto seleccionado
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Gris para los no seleccionados
            }

            display->setCursor(offset + 10, yPos);

            // Nombres e Indicadores Rápidos
            switch (itemIndex) {
                case 0: display->print(msg("MENU", "playlists")); break;
                case 1: display->print(msg("MENU", "reproduccion")); break;
                case 2: 
                    // El JSON ya incluye "Brillo: "
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
    // 2. DIBUJO SUBMENÚ: PLAYLISTS 
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_PLAYLIST) {
        // Título con centrado automático
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
    // 3. DIBUJO SUBMENÚ: REPRODUCCIÓN
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_REPRODUCCION) {
         int pagReproduccion = cursorSubmenu / 3; 

         // Título con centrado automático y paginado
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/3", msg("SUBMENU_REPRODUCCION", "titulo"), pagReproduccion + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int inicio = pagReproduccion * 3;
        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex > 4) break; 

            int yPos = 9 + (i * 8);

            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0)); // Cursor Amarillo
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // Texto Blanco
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Texto Gris
                display->setCursor(offset + 2, yPos); display->print(" ");
            }

            display->setCursor(offset + 10, yPos);
            switch (itemIndex) {
                case 0: display->printf("%s%s", msg("SUBMENU_REPRODUCCION", "modo"), (modoVisual == 0 ? msg("ESTADOS", "modo_gifs") : msg("ESTADOS", "modo_reloj"))); break;
                case 1: display->printf("%s%s", msg("SUBMENU_REPRODUCCION", "aleatorio"), (randomMode ? msg("ESTADOS", "si") : msg("ESTADOS", "no"))); break;
                case 2: 
                    {
                        const char* nombresArcade[] = {"OFF", "Batocera", "Recalbox", "ReplayOS"};
                        // Aseguramos que el índice no se salga del array por seguridad
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
    // 4. DIBUJO SUBMENÚ: BRILLO
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_BRILLO) {
        // Título con centrado automático
        printMenuCentrado(msg("SUBMENU_BRILLO", "titulo"), 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int porcentaje = (brightness * 100) / 255;
        display->setTextColor(display->color565(255, 255, 255));
        display->setCursor(offset + 55, 10);
        display->printf("%d%%", porcentaje);

        // Dibujo de la Barra de Progreso
        int anchoBarra = 108; // Ancho total de la barra en píxeles
        int xBarra = offset + 10;
        int yBarra = 18;
        int hBarra = 4; // Alto de la barra

        // Borde de la barra (Gris oscuro)
        display->drawRect(xBarra, yBarra, anchoBarra, hBarra, display->color565(50, 50, 50));

        // Relleno de la barra según porcentaje
        int relleno = (porcentaje * (anchoBarra - 2)) / 100;
        display->fillRect(xBarra + 1, yBarra + 1, relleno, hBarra - 2, display->color565(0, 255, 0)); // Verde
        
        // Info en la parte inferior
        display->setCursor(offset + 10, 25); 
        display->setTextColor(display->color565(150, 150, 150));
        display->print(msg("SUBMENU_BRILLO", "click_ok"));
    }
    // ----------------------------------------------------------------
    // 5. DIBUJO SUBMENÚ: WIFI
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_WIFI) {
        // Título con centrado automático
        printMenuCentrado(msg("SUBMENU_WIFI", "titulo"), 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        // Opción: Estado
        int y0 = 13;
        if (cursorSubmenu == 0) {
            display->setTextColor(display->color565(255, 255, 0)); // Cursor Amarillo
            display->setCursor(offset + 2, y0); display->print(">");
            display->setTextColor(display->color565(255, 255, 255)); // Texto Blanco
        } else {
            display->setTextColor(display->color565(150, 150, 150)); // Texto Gris
            display->setCursor(offset + 2, y0); display->print(" "); 
        }
        display->setCursor(offset + 10, y0);
        display->printf("%s: %s", msg("ESTADOS", "activar"), (wifiEnable ? msg("ESTADOS", "si") : msg("ESTADOS", "no")));

        // Opción: Volver
        int y1 = 23;
        if (cursorSubmenu == 1) {
            display->setTextColor(display->color565(255, 255, 0)); // Cursor Amarillo
            display->setCursor(offset + 2, y1); display->print(">"); // Texto Blanco
            display->setTextColor(display->color565(255, 255, 255)); // Texto Gris
        } else {
            display->setTextColor(display->color565(150, 150, 150)); 
            display->setCursor(offset + 2, y1); display->print(" ");
        }
        display->setCursor(offset + 10, y1);
        display->print(msg("ESTADOS", "volver"));
    }
    // ----------------------------------------------------------------
    // 6. DIBUJO SUBMENÚ: RELOJ
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_RELOJ) {
        int pagReloj = cursorSubmenu / 3;

        // Título con centrado automático
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/2", msg("SUBMENU_RELOJ", "titulo"), pagReloj + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int inicio = pagReloj * 3;
        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex > 6) break; // Total 7 opciones (0 a 6)
            int yPos = 9 + (i * 8);
            
            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0));  // Cursor Amarillo
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // Texto Blanco
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Texto Gris
                display->setCursor(offset + 2, yPos); display->print(" ");
            }

            display->setCursor(offset + 10, yPos);
            switch (itemIndex) {
                case 0: display->printf("%s: %s", msg("ESTADOS", "activar"), (clockEnable ? msg("ESTADOS", "si") : msg("ESTADOS", "no"))); break;
                case 1: display->printf("%s%d GIF", msg("SUBMENU_RELOJ", "cada"), autoClockInt); break;
                case 2: display->printf("%s%ds", msg("SUBMENU_RELOJ", "ver"), clockDuration); break; 
                case 3:
                    {
                        const char* nombresEstilos[] = {"Matrix", "Solid", "Rainbow", "Pulse", "Gradient"};
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
    // 7. DIBUJO SUBMENÚ: TIEMPO
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_TIEMPO) {
        // Título con centrado automático
        printMenuCentrado(msg("SUBMENU_CLIMA", "titulo"), 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        // Opción: Activar
        int y0 = 13;
        if (cursorSubmenu == 0) {
            display->setTextColor(display->color565(255, 255, 0)); // Cursor Amarillo
            display->setCursor(offset + 2, y0); display->print(">");
            display->setTextColor(display->color565(255, 255, 255)); // Texto Blanco
        } else {
            display->setTextColor(display->color565(150, 150, 150)); // Texto Gris
            display->setCursor(offset + 2, y0); display->print(" ");
        }
        display->setCursor(offset + 10, y0);
        display->printf("%s: %s", msg("ESTADOS", "activar"), (weatherEnable ? msg("ESTADOS", "si") : msg("ESTADOS", "no")));

        // Opción: Volver
        int y1 = 23;
        if (cursorSubmenu == 1) {
            display->setTextColor(display->color565(255, 255, 0)); // Cursor Amarillo
            display->setCursor(offset + 2, y1); display->print(">");
            display->setTextColor(display->color565(255, 255, 255)); // Texto Blanco
        } else {
            display->setTextColor(display->color565(150, 150, 150)); // Texto Gris
            display->setCursor(offset + 2, y1); display->print(" ");
        }
        display->setCursor(offset + 10, y1);
        display->print(msg("ESTADOS", "volver"));
    }
    // ----------------------------------------------------------------
    // 8. DIBUJO SUBMENÚ: TEMPORIZADOR
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_TEMPORIZADOR) {
        int pagTimer = cursorSubmenu / 3; 
        
        // Título con centrado automático y paginado
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/2", msg("SUBMENU_TEMPORIZADOR", "titulo"), pagTimer + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int inicio = pagTimer * 3;
        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex > 3) break; 

            int yPos = 9 + (i * 8);

            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0)); // Cursor Amarillo
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // Texto Blanco
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Texto Gris
                display->setCursor(offset + 2, yPos); display->print(" ");
            }

            display->setCursor(offset + 10, yPos);
            switch (itemIndex) {
                case 0: display->printf("%s: %s", msg("ESTADOS", "activar"), (timerEnable ? msg("ESTADOS", "si") : msg("ESTADOS", "no"))); break;
                case 1: display->printf("ON:  %02d:%02d", hOn, mOn); break; // Texto fijo NO esta en JSON
                case 2: display->printf("OFF: %02d:%02d", hOff, mOff); break; // Texto fijo NO esta en JSON
                case 3: display->print(msg("ESTADOS", "volver")); break;
            }
        }
    }
    // ----------------------------------------------------------------
    // 9. DIBUJO SUBMENÚ: AVANZADO
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_AVANZADO) {
        int pagAv = cursorSubmenu / 3; 
        
        // Título con centrado automático y paginado
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/3", msg("SUBMENU_AVANZADO", "titulo"), pagAv + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int inicio = pagAv * 3;
        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex > 6) break; 

            int yPos = 9 + (i * 8);
            
            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0)); // Cursor Amarillo
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // Texto Blanco
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Texto Gris
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
    // 9.1.  DIBUJO SUBMENÚ: AVANZADO -> MAPEADO IR
    // ----------------------------------------------------------------
    if (estadoActual == ESTADO_SUBMENU_MAPEADO_IR) {
        
        int pagAv = cursorSubmenu / 3; 
        
        // Título con centrado automático y paginado
        snprintf(titleBuf, sizeof(titleBuf), "%s%d/3", msg("SUBMENU_MAPEADO_IR", "titulo"), pagAv + 1);
        printMenuCentrado(titleBuf, 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        int inicio = pagAv * 3;
        for (int i = 0; i < 3; i++) {
            int itemIndex = inicio + i;
            if (itemIndex > 8) break; 

            int yPos = 9 + (i * 8);
            
            if (cursorSubmenu == itemIndex) {
                display->setTextColor(display->color565(255, 255, 0)); // Cursor Amarillo
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // Texto Blanco
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Texto Gris
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
    // 10. DIBUJO SUBMENÚ: ACTUALIZACIÓN
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_ACTUALIZACION) {
        // Título con centrado automático
        printMenuCentrado(msg("SUBMENU_ACTUALIZACION", "titulo"), 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        for (int i = 0; i < 2; i++) {
            int yPos = 13 + (i * 10);
            if (cursorSubmenu == i) {
                display->setTextColor(display->color565(255, 255, 0)); // Cursor Amarillo
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // Texto Blanco
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Texto Gris
                display->setCursor(offset + 2, yPos); display->print(" ");
            }
            display->setCursor(offset + 10, yPos);
            if (i == 0) display->print(msg("SUBMENU_ACTUALIZACION", "buscar_ota"));
            else display->print(msg("ESTADOS", "volver"));
        }
    }
    // ----------------------------------------------------------------
    // 11. DIBUJO SUBMENÚ: FTP
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_FTP) {
        // Título con centrado automático
        printMenuCentrado(msg("SUBMENU_FTP", "titulo"), 0, display->color565(0, 255, 255), offset);
        display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));

        // Opción: Estado
        int y0 = 13;
        if (cursorSubmenu == 0) {
            display->setTextColor(display->color565(255, 255, 0)); // Cursor Amarillo
            display->setCursor(offset + 2, y0); display->print(">");
            display->setTextColor(display->color565(255, 255, 255)); // Texto Blanco
        } else {
            display->setTextColor(display->color565(150, 150, 150)); // Texto Gris
            display->setCursor(offset + 2, y0); display->print(" "); 
        }
        display->setCursor(offset + 10, y0);
        display->printf(msg("SUBMENU_FTP", "activar"));

        // Opción: Volver
        int y1 = 23;
        if (cursorSubmenu == 1) {
            display->setTextColor(display->color565(255, 255, 0)); // Cursor Amarillo
            display->setCursor(offset + 2, y1); display->print(">"); // Texto Blanco
            display->setTextColor(display->color565(255, 255, 255)); // Texto Gris
        } else {
            display->setTextColor(display->color565(150, 150, 150)); 
            display->setCursor(offset + 2, y1); display->print(" ");
        }
        display->setCursor(offset + 10, y1);
        display->print(msg("ESTADOS", "volver"));
    }
    // ----------------------------------------------------------------
    // 12. DIBUJO SUBMENÚ: IDIOMA
    // ----------------------------------------------------------------
    else if (estadoActual == ESTADO_SUBMENU_IDIOMA) {
        // Título con centrado automático
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
                display->setTextColor(display->color565(255, 255, 0)); // Cursor Amarillo
                display->setCursor(offset + 2, yPos); display->print(">");
                display->setTextColor(display->color565(255, 255, 255)); // Texto Blanco
            } else {
                display->setTextColor(display->color565(150, 150, 150)); // Texto Gris
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

    display->flipDMABuffer(); // Volcamos a los LEDs
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
//                     FUNCIONES RELOJ LITE
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

    // Solo los estilos animados necesitan actualizarse más de 1Hz
    const bool esAnimado = (clockStyle == 0 || clockStyle == 2 || clockStyle == 3);

    // TRANSICIÓN DE ENTRADA (GIF → RELOJ)
    if (conTransicion) transicionParticulasFormanRelojLite();

    unsigned long startTime = millis(); 
    while(millis() - startTime < (clockDuration * 1000UL) && !interrumpirReproduccion) {

        gestionarBotonMenu();
        leerControlRemoto();

        if (arcadeEnable > 0) 
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
                        case 0: color = display->color565(0, 255, 0); break;
                        case 1: color = display->color565((clockColor>>16)&0xFF,(clockColor>>8)&0xFF,clockColor&0xFF); break;
                        case 2: color = hsvTo565((ms / 25 + xPos) % 360, 255, 255); break;
                        case 3: color = hsvTo565((int)(5 + sin(ms / 500.0) * 10) % 360, 255, 200); break;
                        case 4: {
                            uint8_t r1=(clockColor>>16)&0xFF,g1=(clockColor>>8)&0xFF,b1=clockColor&0xFF;
                            float ratio = i / 8.75f;
                            color = display->color565(r1+(255-r1)*ratio,g1+(255-g1)*ratio,b1+(255-b1)*ratio);
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
                    if (fullTimeStr[i] < '0' || fullTimeStr[i] > '9') continue; // el dos puntos no varía
                    int xPos = startX + (i * 16);
                    uint16_t color;
                    switch (clockStyle) {
                        case 2: color = hsvTo565((ms / 25 + xPos) % 360, 255, 255); break;
                        case 3: color = hsvTo565((int)(5 + sin(ms / 500.0) * 10) % 360, 255, 200); break;
                        default: continue; // otros estilos no animan — no hace falta redibuja aquí
                    }
                    drawCustomChar(xPos, startY, fullTimeStr[i] - '0', color, 3);
                }
                display->flipDMABuffer();
                lastColorMs = millis();
            }
        }

        delay(esAnimado ? 16 : 50);
    }
    // TRANSICIÓN DE SALIDA (RELOJ → GIF)
    if (conTransicion && !interrumpirReproduccion) transicionRelojDispersaLite();
}

void dibujarBarraNotificacionesLite() {
    if (weatherEnable == 0 || !weatherDataReady) return;

    // 1. Selección de Icono y Color según el código de OpenWeatherMap
    uint16_t colorClima;
    const unsigned char* iconToDraw;

    if (weatherConditionCode >= 200 && weatherConditionCode < 300) {
        iconToDraw = icon_storm; colorClima = display->color565(200, 0, 200); // Tormenta
    } else if (weatherConditionCode >= 300 && weatherConditionCode < 600) {
        iconToDraw = icon_rain;  colorClima = display->color565(0, 100, 255); // Lluvia
    } else if (weatherConditionCode >= 600 && weatherConditionCode < 700) {
        iconToDraw = icon_snow;  colorClima = display->color565(255, 255, 255); // Nieve
    } else if (weatherConditionCode >= 700 && weatherConditionCode < 800) {
        iconToDraw = icon_fog;   colorClima = display->color565(180, 180, 200); // Niebla
    } else if (weatherConditionCode == 800) {
        if (isNight) {
            iconToDraw = icon_moon;   colorClima = display->color565(200, 200, 255); // Luna;
        } else {
        iconToDraw = icon_sun;   colorClima = display->color565(255, 255, 0); // Sol
        }
    } else {
        iconToDraw = icon_cloud; colorClima = display->color565(180, 180, 180); // Nubes
    }
    

    // 2. Dibujar el Mensaje del .ini (Game Room) a la izquierda
    display->setFont(NULL); // Fuente estándar 5x7
    display->setTextSize(1);
    display->setCursor(offset + 2, 0);
    display->setTextColor(display->color565(200, 200, 200)); // Gris claro
    display->print(weatherCustomMsg); 

    // 3. Dibujar Icono Bitmap (en la posición 97 del panel)
    display->drawBitmap(offset + 97, 0, iconToDraw, 8, 8, colorClima);

    // 4. Dibujar Temperatura
    display->setTextColor(display->color565(200, 200, 200));
    display->setCursor(offset + 107, 0); 
    display->print((int)currentTemp); // (int) para quitar decimales y ahorrar espacio
    
    // Símbolo de grado (pequeño cuadrado 2x2) y la "C"
    display->drawRect(offset + 119, 0, 2, 2, display->color565(200, 200, 200));
    display->setCursor(offset + 122, 0);
    display->print("C");
}

void actualizarClimaLite() {
    if (WiFi.status() != WL_CONNECTED) return; 
    
    // 1. Codificación para URL
    String encodedCity = weatherCity;
    encodedCity.replace(" ", "%20");

    HTTPClient http;
    // 2. Timeout preventivo para el Core 3-X
    http.setTimeout(5000); 

    String url = "http://api.openweathermap.org/data/2.5/weather?q=" + encodedCity + "&appid=" + weatherKey + "&units=metric";
    
    Serial.println(F("[CLIMA] Conectando a OpenWeatherMap..."));
    Serial.print(F("[CLIMA] URL: ")); Serial.println(url);

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
                Serial.printf(PSTR("[OK] %.1f°C, ID: %d, Noche: %s\n"), currentTemp, weatherConditionCode, isNight ? "SI" : "NO");
            } else {
                Serial.print(F("[ERROR] JSON: "));
                Serial.println(error.c_str());
            }
        } else {
            Serial.printf("[ERROR] HTTP Code: %d\n", httpCode);
        }
        http.end();
    } else {
        Serial.println(F("[ERROR] No se pudo iniciar la conexión HTTP"));
    }
}

void gestionarActualizacionClima() {
    // 1. Si no toca actualizar por tiempo, salimos
    if (weatherEnable == 0 || (millis() - lastWeatherUpdate < (unsigned long)weatherInterval * 60000UL)) return;

    Serial.println(F("\n[SISTEMA] Ventana de actualización de clima alcanzada."));

    // 2. Estrategia según Double Buffer
    if (doubleBuff == 1) {
        Serial.println(F("[MEMORIA] Double Buffer detectado. Reiniciando para actualizar limpio..."));
        delay(1000);
        ESP.restart();
    } 
    else {
        // 3. Intento de conexión estándar (Solo si no hay Double Buffer)
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
            Serial.println(F("[CLIMA] Datos actualizados OK."));
        }

        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }
}

// ====================================================================
//               SISTEMA DE TRANSICIÓN DE PARTÍCULAS
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
            case 0: color = display->color565(0, 255, 0); break;
            case 1: color = display->color565((clockColor>>16)&0xFF,(clockColor>>8)&0xFF,clockColor&0xFF); break;
            case 2: color = hsvTo565((ms / 25 + xPos) % 360, 255, 255); break;
            case 3: color = hsvTo565((int)(5 + sin(ms / 500.0) * 10) % 360, 255, 200); break;
            case 4: {
                uint8_t r1=(clockColor>>16)&0xFF, g1=(clockColor>>8)&0xFF, b1=clockColor&0xFF;
                float ratio = i / 8.75f;
                color = display->color565(r1+(255-r1)*ratio, g1+(255-g1)*ratio, b1+(255-b1)*ratio);
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
        Serial.printf(PSTR("[TRANS] Sin bloque contiguo: %u/%u\n"), ESP.getMaxAllocHeap(), needed);
        return;
    }

    uint16_t* frameBuf = (uint16_t*)ps_malloc(TRANS_BUF_W * TRANS_BUF_H * sizeof(uint16_t));
    if (!frameBuf) frameBuf = (uint16_t*)malloc(TRANS_BUF_W * TRANS_BUF_H * sizeof(uint16_t));
    ParticulaLite* parts = (ParticulaLite*)malloc(MAX_PARTICULAS * sizeof(ParticulaLite));

    if (!frameBuf || !parts) {
        if (frameBuf) free(frameBuf);
        if (parts)    free(parts);
        Serial.println(F("[TRANS] Fallo malloc entrada"));
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
        Serial.printf("[TRANS] Sin bloque contiguo: %u/%u\n", ESP.getMaxAllocHeap(), needed);
        return;
    }

    uint16_t* frameBuf = (uint16_t*)ps_malloc(TRANS_BUF_W * TRANS_BUF_H * sizeof(uint16_t));
    if (!frameBuf) frameBuf = (uint16_t*)malloc(TRANS_BUF_W * TRANS_BUF_H * sizeof(uint16_t));
    ParticulaLite* parts = (ParticulaLite*)malloc(MAX_PARTICULAS * sizeof(ParticulaLite));

    if (!frameBuf || !parts) {
        if (frameBuf) free(frameBuf);
        if (parts)    free(parts);
        Serial.println(F("[TRANS] Fallo malloc salida"));
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
//                     FUNCIONES TEXTO LITE
// ====================================================================
void mostrarTextoScroll() {
    if (textoScrollMsg.length() == 0) return;

    unsigned long ahora = millis();
    if (ahora - textoScrollUltimoPaso < (unsigned long)textoScrollVelocidad) return;
    textoScrollUltimoPaso = ahora;

    int textSize = 2;

    int anchoTexto = textoScrollMsg.length() * 6 * textSize;

    display->fillScreen(0);
    display->setTextSize(textSize);
    display->setTextColor(display->color565(
        (textoScrollColor >> 16) & 0xFF,
        (textoScrollColor >> 8) & 0xFF,
        textoScrollColor & 0xFF));
    display->setCursor(textoScrollX, 8);
    display->print(textoScrollMsg);
    display->flipDMABuffer();

    textoScrollX--;
    if (textoScrollX < offset - anchoTexto) {
        textoScrollX = offset + PANEL_RES_X; // vuelve a entrar por la derecha (loop infinito)
    }
}

// ====================================================================
//                 FUNCIONES DE ENERGÍA Y TEMPORIZADOR
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
        if (isSleeping) return; // Ya está dormido
        Serial.println(F("[ENERGÍA] Entrando en Modo Sueño..."));
        
        // Feedback visual
        display->fillScreen(0);

        int centroX = offset + PANEL_RES_X + 10; // El centro exacto de tus dos paneles

        // 1. Cara del Emoji (Círculo Amarillo)
        // La desplazamos un poco a la izquierda del centro para que las Zzz equilibren el bloque
        uint16_t colorCara = display->color565(255, 200, 0);
        display->fillCircle(centroX - 10, 16, 10, colorCara); 

        // 2. Ojos y Boca (Relativos a la posición de la cara)
        uint16_t colorDetalle = display->color565(50, 30, 0);
        display->drawLine(centroX - 15, 14, centroX - 12, 14, colorDetalle); 
        display->drawLine(centroX - 8, 14, centroX - 5, 14, colorDetalle);
        display->drawCircle(centroX - 10, 20, 2, colorDetalle);

        // 3. Las "Zzz" (A la derecha del centro)
        uint16_t colorZ = display->color565(0, 150, 255);
        display->setCursor(centroX + 4, 12); display->print("Z");
        display->setCursor(centroX + 10, 7); display->print("z");
        display->setCursor(centroX + 15, 3); display->print("z");

        display->flipDMABuffer();
        delay(2000); 

        display->fillScreen(0);
        display->flipDMABuffer();
        
        // Cierre de hardware
        gif.close(); // Libera la tarjeta SD
        display->fillScreen(0);
        display->flipDMABuffer();
        digitalWrite(OE_PIN, HIGH); // Apaga LEDs
        setCpuFrequencyMhz(80);     // Baja CPU a 80MHz
        isSleeping = true;
    } else {
        if (!isSleeping) return;
        Serial.println(F("[ENERGÍA] Despertando sistema..."));
        setCpuFrequencyMhz(240);    // CPU a tope
        delay(150);
        digitalWrite(OE_PIN, LOW);  // Enciende LEDs
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
    if (!getLocalTime(&timeinfo)) return; // Si no hay hora NTP, no hace nada

    int minutosAhora = timeinfo.tm_hour * 60 + timeinfo.tm_min;
    int minutosOn = hOn * 60 + mOn;
    int minutosOff = hOff * 60 + mOff;

    //1. GESTIÓN DEL OVERRIDE
    static int ultimoMinuto = -1;
    if (minutosAhora != ultimoMinuto) {
        // Cuando entramos exactamente en el minuto de encendido o de apagado programado,
        // el temporizador recupera el control automáticamente anulando el modo manual.
        if (minutosAhora == minutosOn || minutosAhora == minutosOff) {
            manualOverride = false; 
        }
        ultimoMinuto = minutosAhora;
    }

    // Si el usuario ha tocado un botón, bloqueamos los cambios de estado (hasta la próxima alarma)
    if (manualOverride) return;

    // 2. LÓGICA DE ESTADO
    bool deberiaEstarEncendido = false;
    if (minutosOn < minutosOff) {
        deberiaEstarEncendido = (minutosAhora >= minutosOn && minutosAhora < minutosOff);
    } else { // Caso cruce de medianoche
        deberiaEstarEncendido = (minutosAhora >= minutosOn || minutosAhora < minutosOff);
    }

    // 3. APLICAR CAMBIOS
    if (deberiaEstarEncendido && isSleeping) {
        toggleEnergia(false); // Es hora de despertar
    } else if (!deberiaEstarEncendido && !isSleeping) {
        toggleEnergia(true); // Es hora de dormir
    }
}

// ====================================================================
//                     SISTEMA DE ACTUALIZACIÓN OTA
// ====================================================================
int resultadoOTA = -1; // -1: nada, 0: al día, 1: actualizado, 2: error

void triggerActualizacionOTA() {
    Preferences prefs;
    prefs.begin("sistema", false);
    prefs.putBool("ota_pending", true);
    prefs.end();
    
    Serial.println(F("[SISTEMA] Bandera OTA marcada. Reiniciando..."));
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

    // 2. Obtener versión
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

    // 3. Instalación
    if (newVersion > CURRENT_VERSION_NUM && binUrl[0] != '\0') {
        if (http.begin(client, binUrl)) {
            if (http.GET() == 200) {
                int contentLength = http.getSize();
                
                Update.abort(); // Limpieza por si acaso
                
                if (Update.begin(contentLength, U_FLASH)) {
                    Serial.println(F("[OTA] Escribiendo binario..."));
                    WiFiClient* stream = http.getStreamPtr();
                    
                    // Usamos la escritura directa
                    size_t escrito = Update.writeStream(*stream);
                    
                    if (escrito == contentLength && Update.end(true)) {
                        Serial.println(F("[OTA] ¡Actualización Realizada con Éxito!"));
                        
                        Preferences prefs;
                        prefs.begin("sistema", false);
                        prefs.putBool("ota_done", true); 
                        prefs.end();

                        delay(1000);
                        ESP.restart();
                        return 1; // Realmente no llegará aquí por el restart
                    } else {
                        Serial.printf("[OTA] Error: %s\n", Update.errorString());
                        return 2; // Error al escribir
                    }
                }
            }
            http.end();
        }
        return 2; // Error al escribir
    } else {
        // No hay una nueva versión
        Serial.println(F("[OTA] El sistema ya está actualizado."));
        return 0; // Sistema al día
    }
}

// ====================================================================
//                        FUNCIONES DE FTP
// ====================================================================
void ejecutarModoFTP() {
    // 1. Forzar WiFi
    if (wifiEnable == 1 && wifi_ssid[0] != '\0') {
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.mode(WIFI_STA);
            WiFi.begin(wifi_ssid, wifi_pass);
            int at = 0;
            while (WiFi.status() != WL_CONNECTED && at < 20) { delay(500); at++; }
        }
    }

    // 2. Iniciar FTP
    ftpSrv.begin("admin", "admin"); 

    // 3. Interfaz Visual Estática
    display->fillScreen(0);
    
    // 1. CARPETA (Cuerpo principal amarillo de la imagen)
    uint16_t colorCarpeta = display->color565(255, 230, 100); // Amarillo suave
    display->fillRoundRect(offset + 4, 11, 24, 16, 2, colorCarpeta); 
    // Pestaña superior de la carpeta
    display->fillRoundRect(offset + 4, 9, 10, 5, 1, colorCarpeta);

    // 2. FLECHAS (Verde subida, Azul bajada como en la imagen)
    uint16_t colorVerde = display->color565(50, 200, 50); // Verde brillante
    uint16_t colorAzul = display->color565(50, 150, 255);  // Azul claro

    // Flecha de Subida (Verde)
    // Triángulo (punta)
    display->fillTriangle(offset + 14, 1, offset + 18, 5, offset + 10, 5, colorVerde);
    // Rectángulo (cuerpo)
    display->fillRect(offset + 13, 5, 3, 4, colorVerde);

    // Flecha de Bajada (Azul)
    // Triángulo (punta)
    display->fillTriangle(offset + 24, 9, offset + 28, 5, offset + 20, 5, colorAzul);
    // Rectángulo (cuerpo)
    display->fillRect(offset + 23, 1, 3, 4, colorAzul);

    // 3. TEXTO "FTP" DENTRO DE LA CARPETA
    display->setTextSize(1);
    display->setTextColor(0); // Negro, para contrastar con el amarillo
    // Centramos "FTP" dentro del rectángulo de la carpeta
    // Coordenadas calculadas para que quede centrado en el RoundRect de 24x16
    display->setCursor(offset + 8, 16); 
    display->print("FTP");

    // Dirección IP (A la derecha de la carpeta)
    display->setTextColor(display->color565(255, 255, 255)); // Blanco
    display->setCursor(offset + 34, 16);
    display->print(WiFi.localIP().toString());

    display->flipDMABuffer();

    Serial.print(F("[FTP] Servidor listo en IP: "));
    Serial.println(WiFi.localIP().toString());

    // 4. Bucle infinito de mantenimiento
    while (true) {
        ftpSrv.handleFTP();

        // Si pulsas el botón (cualquier duración), salimos
        if (digitalRead(PIN_BOTON_MENU) == LOW) {
            Serial.println(F("[FTP] Cerrando FTP por Botón físico"));
            Preferences p;
            p.begin("sistema", false);
            p.putBool("ftp_mode", false);
            p.end();
            delay(1000);
            ESP.restart();
        }

        // Si pulsas cualquier botón IR, salimos
        if (IrReceiver.decode()) {
        //uint32_t codigoRecibido = IrReceiver.decodedIRData.command;
        uint32_t codigoRecibido = IrReceiver.decodedIRData.decodedRawData;
    
            // Solo salimos si el botón pulsado es exactamente el de OK/Validar
            if (codigoRecibido == ir_btn_ok) { 
                Serial.println(F("[FTP] Boton OK detectado. Cerrando servidor y reiniciando..."));
                Preferences p;
                p.begin("sistema", false);
                p.putBool("ftp_mode", false);
                p.end();
                delay(1000);
                ESP.restart();
            }

            //Limpiar el buffer para permitir la siguiente lectura
            IrReceiver.resume();
        }
        
        yield();
        delay(2);
    }
}

// ====================================================================
//                     MOTOR DE REPRODUCCIÓN ARCADE
// ====================================================================
bool buscarJuegoEnIndice(String sistema, String juego) {
    char ruta[80];
    snprintf(ruta, sizeof(ruta), "/arcade/%s.txt", sistema.c_str());
    File f = SD.open(ruta);
    
    if (!f) {
        Serial.print(F("[ERROR] No se pudo abrir: "));
        Serial.println(ruta);
        return false;
    }

    String l;
    while (f.available()) {
        l = f.readStringUntil('\n');
        l.trim(); 

        if (l.equalsIgnoreCase(juego)) { 
            f.close(); 
            return true; 
        }
    }
    f.close();
    Serial.print(F("[INFO] Fin del archivo y no se encontró: "));
    Serial.println(juego);
    return false;
}

bool buscarJuegoEnIndice_ReplayOS(const char* sistema, const char* juego) {
    char rutaIndice[80];
    snprintf(rutaIndice, sizeof(rutaIndice), "/Arcade/%s.txt", sistema);

    if (!SD.exists(rutaIndice)) return false;

    File archivo = SD.open(rutaIndice);
    if (!archivo) return false;

    char juegoBusca[64];
    strlcpy(juegoBusca, juego, sizeof(juegoBusca));
    for (char* p = juegoBusca; *p; p++) *p = tolower((unsigned char)*p);

    char linea[80];
    bool encontrado = false;

    while (archivo.available()) {
        int len = archivo.readBytesUntil('\n', linea, sizeof(linea) - 1);
        linea[len] = '\0';

        int l = strlen(linea);
        if (l > 0 && linea[l - 1] == '\r') linea[l - 1] = '\0';

        if (strcasecmp(linea, juegoBusca) == 0) {
            encontrado = true;
            break;
        }
    }

    archivo.close();
    return encontrado;
}

void mostrarMarquesinaBMP(const char* path) {
    File bmpFile = SD.open(path);
    if (!bmpFile) {
       Serial.print(F("Error: No se pudo abrir el BMP en "));
       Serial.println(path);
        return;
    }

    // 1. Detectar profundidad de bits (está en el byte 28)
    bmpFile.seek(28); 
    uint16_t bitsPerPixel = 0;
    bmpFile.read((uint8_t*)&bitsPerPixel, 2);

    // 2. Detectar dónde empiezan los píxeles (está en el byte 10)
    bmpFile.seek(10);
    uint32_t dataOffset = 0;
    bmpFile.read((uint8_t*)&dataOffset, 4);
    
    bmpFile.seek(dataOffset);

    // Calculamos cuántos bytes ocupa cada píxel (3 o 4)
    int bytesPorPixel = bitsPerPixel / 8;
    
    Serial.printf(PSTR("[BMP] Cargando %dbits (%d bytes/px) desde %s\n"), bitsPerPixel, bytesPorPixel, path);

    // 3. Dibujamos BMP
    for (int y = 31; y >= 0; y--) {
        for (int x = 0; x < 128; x++) {
            if (bmpFile.available() >= bytesPorPixel) {
                // Leemos los colores (BMP guarda en orden BGR)
                uint8_t b = bmpFile.read();
                uint8_t g = bmpFile.read();
                uint8_t r = bmpFile.read();
                
                // Si el archivo es de 32 bits, leemos el 4º byte y lo ignoramos
                if (bytesPorPixel == 4) {
                    bmpFile.read(); 
                }

                display->drawPixel(x + offset, y, display->color565(r, g, b));
            }
        }
        
    }

    bmpFile.close();
    Serial.println(F("Marquesina dibujada con éxito."));
}

void verificarReplayOSLite() {
    if (arcadeEnable != 3) return;

    static unsigned long ultimaConsulta = 0;
    if (millis() - ultimaConsulta < replayOSIntervaloActual) return;
    ultimaConsulta = millis();

    HTTPClient http;
    WiFiClient client;

    char url[80];
    snprintf(url, sizeof(url), "http://%s:55356/api/v1/get_status", replayOS_IP);

    http.setConnectTimeout(800);
    http.setTimeout(1200);
    http.setReuse(true); 

    if (!http.begin(client, url)) {
        Serial.println(F("[ReplayOS] No se pudo iniciar conexión"));
        return;
    }
    http.addHeader("X-RePlay-Token", replayOS_Token);

    int httpCode = http.GET();

    // Gestión de fallos
    if (httpCode != HTTP_CODE_OK) {
        http.end();
        replayOSFallosConsecutivos++;

        // Backoff progresivo: 3s -> 6s -> 12s -> máx 30s
        // Evita martillear la red si ReplayOS está apagado o reiniciando
        if (replayOSFallosConsecutivos > 1) {
            replayOSIntervaloActual = min(3000UL * (1UL << min(replayOSFallosConsecutivos - 1, 3)), 30000UL);
        }

        if (httpCode > 0) {
            Serial.printf(PSTR("[ReplayOS] Fallo HTTP: %d (intento %d, próximo en %lums)\n"),
                          httpCode, replayOSFallosConsecutivos, replayOSIntervaloActual);
        } else {
            Serial.printf(PSTR("[ReplayOS] Sin respuesta (err %d, intento %d, próximo en %lums)\n"),
                          httpCode, replayOSFallosConsecutivos, replayOSIntervaloActual);
        }
        return;
    }

    // Éxito: resetear backoff a la cadencia normal
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
        Serial.printf(PSTR("[ReplayOS] Error JSON: %s\n"), err.c_str());
        return;
    }

    int viewId = doc["view_id"] | 0;

    // CASO 1: NO ESTAMOS JUGANDO
    if (viewId != 2) {
        if (ultimoJuegoCargado.length() > 0) {
            Serial.println(F("[ReplayOS] Fuera de juego. Restableciendo a modo normal."));
            ultimoJuegoCargado = "";
            estadoActual = ESTADO_GIFS;
            saliendoAGifs = true;
            interrumpirReproduccion = true;
        }
        return;
    }

    // CASO 2: SI ESTAMOS JUGANDO 
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

    Serial.printf(PSTR("[ReplayOS] Nuevo juego detectado: %s [%s]\n"), juegoLimpio, rawSystem);

    char rutaSubcarpeta[96], rutaDirecta[80], rutaSistema[80];
    snprintf(rutaSubcarpeta, sizeof(rutaSubcarpeta), "/arcade/%s/%s.bmp", rawSystem, juegoLimpio);
    snprintf(rutaDirecta,    sizeof(rutaDirecta),    "/arcade/%s.bmp", juegoLimpio);
    snprintf(rutaSistema,    sizeof(rutaSistema),    "/arcade/%s.bmp", rawSystem);

    bool cargadoConExito = false;
    const char* rutaFinalBMP = nullptr;

    // A. Buscar el juego indexado
    if (buscarJuegoEnIndice_ReplayOS(rawSystem, juegoLimpio)) {
        if (SD.exists(rutaSubcarpeta)) {
            rutaFinalBMP = rutaSubcarpeta;
            cargadoConExito = true;
        } else if (SD.exists(rutaDirecta)) {
            rutaFinalBMP = rutaDirecta;
            cargadoConExito = true;
        }
    }

    // B. FALLBACK: logo del sistema
    if (!cargadoConExito && SD.exists(rutaSistema)) {
        rutaFinalBMP = rutaSistema;
        cargadoConExito = true;
    }

    // C. Aplicar estado
    if (cargadoConExito) {
        estadoActual = ESTADO_ARCADE;
        display->fillScreen(0);
        mostrarMarquesinaBMP(rutaFinalBMP);
        display->flipDMABuffer();
    } else {
        Serial.println(F("[ReplayOS] Faltan logo y juego. Derivando al motor de GIFs"));
        estadoActual = ESTADO_GIFS;
        saliendoAGifs = true;
    }
}

// ====================================================================
//                   MOTOR DE REPRODUCCIÓN ARCADE 2.0
// ====================================================================
void verificarMarquesinaTCP() {
    // Solo actuamos si el WiFi está conectado y el Arcade Mode está activo
    if (WiFi.status() != WL_CONNECTED || arcadeEnable == 0) return;
    
    WiFiClient client = tcpServer.available();
    if (client) {
        unsigned long timeout = millis();
        int indice = 0;
        
        // Leemos el primer byte para ver si es un comando "STOP"
        while(client.available() == 0 && millis() - timeout < 1000) { delay(1); }
        if (client.available() == 0) { client.stop(); return; }
        
        uint8_t primerByte = client.read();
        if (primerByte == 'S') { 
            // Si empieza por 'S', asumimos que es "STOP". Salimos a GIFs.
            estadoActual = ESTADO_GIFS;
            saliendoAGifs = true;
            interrumpirReproduccion = true;
            client.stop();
            return;
        }
        
        uint8_t* buffer = (uint8_t*)malloc(12288);
        if (buffer == NULL) {
            Serial.println(F("[ERROR] No hay memoria RAM suficiente para el buffer TCP"));
            client.stop();
            return;
        }
        
        buffer[indice++] = primerByte;
        timeout = millis();
        
        // Leemos el resto de los bytes (12288 bytes en total)
        while (client.connected() && indice < 12288 && (millis() - timeout < 3000)) {
            while (client.available() > 0 && indice < 12288) {
                buffer[indice++] = client.read();
                timeout = millis(); // Refresco del timeout
            }
        }
        client.stop();
        
        // Si llegaron los 12KB completos, los volcamos al panel
        if (indice == 12288) {
            interrumpirReproduccion = true;
            estadoActual = ESTADO_ARCADE;
            display->fillScreen(0);
            
            int idx = 0;
            for (int y = 0; y < 32; y++) {
                for (int x = 0; x < 128; x++) {
                    uint8_t r = buffer[idx++];
                    uint8_t g = buffer[idx++];
                    uint8_t b = buffer[idx++];
                    display->drawPixel(x + offset, y, display->color565(r, g, b));
                }
            }
            display->flipDMABuffer();
        }
        
        // Liberamos la memoria para evitar fugas (Memory Leaks)
        free(buffer);
    }
}

// ====================================================================
//                     MOTOR DE REPRODUCCIÓN GIFs
// ====================================================================
String obtenerSiguienteGifSD() {
    // Si no hay playlist seleccionada, no intentamos abrir nada
    if (playlistActiva[0] == '\0') return "";

    File cacheFile = SD.open(playlistActiva, FILE_READ);
    if (!cacheFile) {
        Serial.print(F("Error: No se pudo abrir "));
        Serial.println(playlistActiva);
        return "";
    }

    // Lógica de Random / Secuencial
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

    // Guardamos posición para el modo secuencial
    if (randomMode == 0) gifCachePosition = cacheFile.position();
    
    cacheFile.close();
    return gifPath;
}

void ejecutarModoGifLite() {
    if (saliendoAGifs) {
        interrumpirReproduccion = false;
        saliendoAGifs = false;
    }

    if (interrumpirReproduccion) return; // Si algo lo bloqueó, no hacemos nada

    String gifPath = obtenerSiguienteGifSD();
    if (gifPath == "" || interrumpirReproduccion) return; // Si se activó la bandera buscando en la SD, salimos

    // Abrimos el archivo GIF
    if (gif.open(gifPath.c_str(), GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw)) {
        
        // Ajustamos el offset para centrar en el segundo panel
        x_offset = (offset - gif.getCanvasWidth()) / 2; 
        y_offset = (PANEL_RES_Y - gif.getCanvasHeight()) / 2; 
        
        display->clearScreen(); 

        int delayMs;
        // Bucle de frames del GIF
        while (gif.playFrame(true, &delayMs)) {

            leerControlRemoto();

            // 1. Escuchar al servidor WiFi solo si Arcade está activo
            if (arcadeEnable > 0) {
                server.handleClient(); // Batocera y Recalbox
                verificarMarquesinaTCP();
                if (arcadeEnable == 3) verificarReplayOSLite(); // ReplayOS
            }

            // 2. Salida inmediata si hay cambio de estado o botón
            if (digitalRead(PIN_BOTON_MENU) == LOW || interrumpirReproduccion || (arcadeEnable > 0 && estadoActual == ESTADO_ARCADE)) {
                interrumpirReproduccion = true;
                break;
            }
            
            display->flipDMABuffer();

            // 3. Reemplazamos delay(delayMs) por un bucle "atento"
            unsigned long tiempoInicio = millis();
            while (millis() - tiempoInicio < (unsigned long)delayMs) {

                 leerControlRemoto();

                if (arcadeEnable > 0) {
                    server.handleClient(); // Sigue escuchando durante la espera del frame
                    verificarMarquesinaTCP();
                    if (arcadeEnable == 3) verificarReplayOSLite();
                    if (estadoActual == ESTADO_ARCADE) {
                        interrumpirReproduccion = true;
                        break; 
                    }
                }
                if (digitalRead(PIN_BOTON_MENU) == LOW) break;
                yield(); // Mantiene estable el WiFi
            }
            if (interrumpirReproduccion) break;
        }
        
        gif.close();
        
        // Solo contamos el GIF si se reprodujo entero
        if (!interrumpirReproduccion) {
            gifsPlayed++;
        }
        
    } else {
        Serial.printf("Error abriendo GIF: %s\n", gifPath.c_str());
    }
}

// ====================================================================
//                     SETUP Y LOOP PRINCIPAL
// ====================================================================
void setup() {
    Serial.begin(115200);
    Serial.println(F("\n=== RETRO PIXEL LED LITE v" FIRMWARE_VERSION " ==="));

    pinMode(PIN_BOTON_MENU, INPUT_PULLUP);

    // 0. Inicializar el receptor IR
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); 
    Serial.println(F("Receptor IR inicializado en GPIO 34"));
    
    // 1. Iniciar SD y Cargar Configuración
    bool sdOk = true;

    SPI.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, SD_CS_PIN);
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println(F("Error FATAL: No se detecta SD."));
        sdOk = false;
    } else {
        leerConfigIni();
        cargarAjustesTimer();
    }

    // 2. Comprobacion para entrar en Modo FTP
    Preferences pFTP;
    pFTP.begin("sistema", true);
    bool entrarFTP = pFTP.getBool("ftp_mode", false);
    pFTP.end();

    // Si la SD falló, ignoramos el modo FTP por seguridad
    if (!sdOk) entrarFTP = false;

    if (sdOk) {
        // 3. Comprobacion de Actualización
        Preferences prefsOTA;
        prefsOTA.begin("sistema", false);
    
        // A. ¿Hay actualización pendiente?
        bool otaPendiente = prefsOTA.getBool("ota_pending", false);
        // B. ¿Venimos de actualizar con éxito?
        bool otaRecienHecha = prefsOTA.getBool("ota_done", false);

        if (otaPendiente) {
            prefsOTA.putBool("ota_pending", false);
            prefsOTA.end();

            resultadoOTA = buscarEInstalarOTA();

        } else if (otaRecienHecha) {
            prefsOTA.putBool("ota_done", false); // Limpiamos la bandera
            prefsOTA.end();
            resultadoOTA = 1; // Marcamos ÉXITO para el panel
        }else {
            prefsOTA.end();
        }

        // 4. Conexión WiFi
        // Comprobamos primero si el WiFi está activado por el usuario y si vamos a entrar en FTP
        if (wifiEnable == 1 && !entrarFTP && wifi_ssid[0] != '\0') { 
            WiFi.mode(WIFI_STA);
            WiFi.setSleep(false);
            WiFi.begin(wifi_ssid, wifi_pass);
    
            Serial.print(F("Conectando para actualizar datos..."));
            int attempts = 0;
            while (WiFi.status() != WL_CONNECTED && attempts < 20) {
                delay(500);
                Serial.print(F("."));
                attempts++;
            }

            if(WiFi.status() == WL_CONNECTED) {
                Serial.println(F(" ¡Conectado!"));
                Serial.print(F("IP local: ")); Serial.println(WiFi.localIP());
        
                // A. Sincronizar Hora (Solo si el Reloj está ON)
                if (clockEnable == 1 || timerEnable == true) {
                    configTzTime(time_zone, ntpServer);
                    // Esperar sincronización de hora
                    time_t now = time(nullptr);
                    attempts = 0;
                    while (now < 10000 && attempts < 10) { 
                        delay(500); 
                        now = time(nullptr); 
                        attempts++; 
                    }
                }   
                // B. Sincronizar Clima (Solo si el Tiempo está ON)
                if (weatherEnable == 1) {
                    actualizarClimaLite();
                    lastWeatherUpdate = millis();
                }       
                // C. Si Arcade está habilitado o el Modo Texto está activo, mantenemos WiFi conectado
                if (arcadeEnable > 0 || textEnable) {
                    Serial.print(F("[WIFI] Manteniendo conexión activa por:"));
    
                    if (arcadeEnable > 0) {
                     const char* nombresArcade[] = {"OFF", "Batocera", "Recalbox", "ReplayOS"};
                        // Aseguramos que el índice esté en rango (1 a 3)
                        int indexArcade = (arcadeEnable >= 1 && arcadeEnable <= 3) ? arcadeEnable : 0;
                        Serial.printf(PSTR(" Modo Arcade [%s]"), nombresArcade[indexArcade]);
                    }
    
                    if (textEnable) {
                        Serial.print(F(" Modo Texto"));
                    }
    
                    Serial.println(); // Salto de línea final
                } else {
                    Serial.println(F("[WIFI] Modo Arcade y Texto OFF: Desconectando tras actualización."));
                    WiFi.disconnect(true);
                    WiFi.mode(WIFI_OFF);
                    delay(500);

                    Serial.printf(PSTR("[MEMORIA] RAM libre tras WiFi: %d bytes\n"), ESP.getFreeHeap());
                }

            } else {
                Serial.println(F(" Falló la conexión."));
            }
        
        } else {
            Serial.println(F("WiFi desactivado por el usuario, SSID vacío o Modo FTP Activo. Saltando red para solicitar datos..."));
        }

    }

    // 5. Inicializar PANEL
    const int FINAL_MATRIX_WIDTH = PANEL_RES_X * panelChain;

    // A. Variables de mapeo dinámico (Inicializadas por defecto en RGB)
    int8_t pR1 = R1_PIN;
    int8_t pG1 = G1_PIN;
    int8_t pB1 = B1_PIN;
    int8_t pR2 = R2_PIN;
    int8_t pG2 = G2_PIN;
    int8_t pB2 = B2_PIN;

    // B. Aplicar lógica de intercambio según el config.ini
    if (strcmp(colorOrder, "RBG") == 0) {
        pG1 = B1_PIN;
        pB1 = G1_PIN;
        pG2 = B2_PIN;
        pB2 = G2_PIN;
        Serial.println(F("[PANEL] Orden de colores ajustado a RBG."));
    } else if (strcmp(colorOrder, "GBR") == 0) {
        pR1 = G1_PIN;
        pG1 = B1_PIN;
        pB1 = R1_PIN;
        pR2 = G2_PIN;
        pG2 = B2_PIN;
        pB2 = R2_PIN;
        Serial.println(F("[PANEL] Orden de colores ajustado a GBR."));
    } else {
        Serial.println(F("[PANEL] Orden de colores por defecto: RGB."));
    }

    // C. Configuración de pines usando las variables dinámicas y Offset segun numero de paneles
    HUB75_I2S_CFG::i2s_pins pin_config = { pR1, pG1, pB1, pR2, pG2, pB2, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN };

    offset = PANEL_RES_X * panelChain;

    // D. Inicialización de la configuración de la matriz
    HUB75_I2S_CFG matrix_config(FINAL_MATRIX_WIDTH, MATRIX_HEIGHT, panelChain, pin_config);
    
    if (i2sSpeed == 0) matrix_config.i2sspeed = HUB75_I2S_CFG::HZ_8M;
    else if (i2sSpeed == 1) matrix_config.i2sspeed = HUB75_I2S_CFG::HZ_10M;
    else if (i2sSpeed == 2) matrix_config.i2sspeed = HUB75_I2S_CFG::HZ_16M;
    else if (i2sSpeed == 3) matrix_config.i2sspeed = HUB75_I2S_CFG::HZ_20M;

    matrix_config.latch_blanking = latchBlank;
    matrix_config.min_refresh_rate = refreshMin;
    matrix_config.clkphase = false;
    
    // FTP, Arcade y Transició siempre con Doble Buff desactivado
    bool usarDoubleBuff = (doubleBuff == 1) && !entrarFTP && (arcadeEnable == 0) && (transitionEnable != 1);
        matrix_config.double_buff = usarDoubleBuff;

    Serial.printf(PSTR("[PANEL] Intentando arrancar — DoubleBuff: %s "),usarDoubleBuff ? "ON" : "OFF");    
   
    // Primer intento (con la configuración solicitada)
    display = new MatrixPanel_I2S_DMA(matrix_config);
    if (display) { 
        if (!display || !display->begin()) {
            // Fallback automático: si double_buff falló, reintenta con single
            if (usarDoubleBuff) {
                Serial.println(F("[PANEL] Double Buffer insuficiente — reintentando con Single Buffer..."));
                if (display) { delete display; display = nullptr; }
                matrix_config.double_buff = false;
                display = new MatrixPanel_I2S_DMA(matrix_config);

                if (!display || !display->begin()) {
                    Serial.println(F("[ERROR FATAL] El display no puede arrancar en Single Buff."));
                    delay(500);
                    ESP.restart();
                }
                Serial.println(F("[PANEL] Single Buffer activo (RAM insuficiente para Double Buffer)."));
            } else {
                Serial.println(F("[ERROR FATAL] El display no puede arrancar."));
                delay(500);
                ESP.restart();
            }
        } else {
            Serial.println(F("[PANEL] Display OK"));
        }
        display->setTextWrap(false);
        display->setBrightness8(brightness);
        display->fillScreen(0);

        // --- AVISO DE FALLO SD ---
        if (!sdOk) {
            display->setTextColor(display->color565(255, 0, 0));
            display->setCursor(offset + 31, 12);
            display->print("ERROR NO SD");
            display->flipDMABuffer();
            
            // Bloqueo total: el sistema se queda aquí hasta que se reinicie
            while(true) { delay(1000); } 
        }

        // Si entramos en FTP, saltamos a la función dedicada y el setup muere aquí
        if (entrarFTP) {
            ejecutarModoFTP(); 
        }

        // --- AVISO DE ESTADO DE ACTUALIZACIÓN ---
        if (resultadoOTA == 0) { // Sistema al día
            display->setTextColor(display->color565(255, 255, 255));
            display->setCursor(offset + 7, 8);
            display->print("Ya tienes la ultima");
            display->setCursor(offset + 1, 18);
            display->print("No hay que actualizar");
            display->flipDMABuffer();
            delay(3000);
            display->fillScreen(0);
        } 
        else if (resultadoOTA == 1) { // ¡ACTUALIZADO CON ÉXITO!
            display->setTextColor(display->color565(255, 255, 255));
            display->setCursor(offset + 25, 8);
            display->print("Actualizacion");
            display->setCursor(offset + 4, 18);
            display->print("Completada con Exito");
            display->flipDMABuffer();
            delay(3000);
            display->fillScreen(0);
        } 
        else if (resultadoOTA == 2) { // Error
            display->setTextColor(display->color565(255, 0, 0));
            display->setCursor(offset + 10, 12);
            display->print("ERROR ACTUALIZANDO");
            display->flipDMABuffer();
            delay(3000);
            display->fillScreen(0);
        }

        // --- DIBUJO DEL LOGO "RETRO PIXEL" ---
        display->setTextSize(1);
        display->setTextColor(display->color565(150, 150, 150)); 
        display->setCursor(offset + 30, 3);
        display->print("RETRO PIXEL");

        // --- DIBUJO DE "LED" (Coloreado) ---
        // L en Rojo
        display->setTextColor(display->color565(255, 0, 0));
        display->setCursor(offset + 40, 15);
        display->print("L");
        
        // E en Verde
        display->setTextColor(display->color565(0, 255, 0));
        display->setCursor(offset + 48, 15);
        display->print("E");
        
        // D en Azul
        display->setTextColor(display->color565(0, 0, 255));
        display->setCursor(offset + 56, 15);
        display->print("D");

        // --- DIBUJO DE "lite" ---
        display->setTextColor(display->color565(200, 200, 200));
        display->setCursor(offset + 65, 15);
        display->print("lite");

        // --- LÍNEAS DE CONTORNO ---
        uint16_t borderCol = display->color565(80, 80, 80);
        display->drawRect(offset + 28, 1, 72, 11, borderCol); 
        display->drawRect(offset + 33, 13, 62, 11, borderCol); 

        // --- MOSTRAR VERSIÓN DEL FIRMWARE ---
        display->setCursor(offset + 46, 25);
        display->setTextColor(display->color565(100, 100, 100)); // Gris suave
        display->print("v");
        display->print(FIRMWARE_VERSION);

        display->flipDMABuffer();
        delay(1500);
    }
    
    gif.begin(LITTLE_ENDIAN_PIXELS);

    // 5. Definimos la ruta de Batocera y arrancamos el servidor
    // Solo si el WiFi está conectado y estamos en modo Batocera, Recalbox (1 o 2) o Texto
    if (WiFi.status() == WL_CONNECTED && (arcadeEnable == 1 || arcadeEnable == 2 || textEnable)) {
    server.on("/batocera", HTTP_GET, [&]() {
        if (server.hasArg("s") && server.hasArg("g")) {
            String s = server.arg("s");
            String g = server.arg("g");
            s.trim(); g.trim();

            if (g == "STOP" || g == "OFF" || g == "") {
                Serial.println(F("[BATOCERA] Comando STOP recibido. Volviendo a GIFs."));
                estadoActual = ESTADO_GIFS; 
                interrumpirReproduccion = false;
            } else {
                char rutaFinal[164] = ""; 
                char rutaTemp[164];

                // 1. Intentar buscar el juego específico
                snprintf(rutaTemp, sizeof(rutaTemp), "/Arcade/%s/%s.bmp", s.c_str(), g.c_str());
                if (buscarJuegoEnIndice(s, g) && SD.exists(rutaTemp)) {
                    strncpy(rutaFinal, rutaTemp, sizeof(rutaFinal));
                    Serial.printf(PSTR("[BATOCERA] Mostrando Juego: %s\n"), g.c_str());
                } 
                else {
                    // 2. Si no está el juego, buscar el logo del sistema
                    snprintf(rutaTemp, sizeof(rutaTemp), "/Arcade/%s/logo.bmp", s.c_str());
                    if (SD.exists(rutaTemp)) {
                        strncpy(rutaFinal, rutaTemp, sizeof(rutaFinal));
                        Serial.printf(PSTR("[BATOCERA] Logo Sistema: %s\n"), s.c_str());
                    } 
                    else {
                        // 3. Si no hay nada, usar el default
                        if (SD.exists(PSTR("/Arcade/default.bmp"))) {
                            strncpy(rutaFinal, "/Arcade/default.bmp", sizeof(rutaFinal));
                            Serial.println(F("[BATOCERA] Mostrando Default."));
                        }
                    }
                }

                // Si hemos encontrado alguna opción (rutaFinal no está vacía)
                if (rutaFinal[0] != '\0') {
                    estadoActual = ESTADO_ARCADE; 
                    interrumpirReproduccion = true;
                    display->fillScreen(0);
                    mostrarMarquesinaBMP(rutaFinal);
                    display->flipDMABuffer(); 
                } else {
                    Serial.println(F("[BATOCERA] ERROR: No se encontro ni Juego, ni Logo, ni Default."));
                }
            }
            server.send(200, "text/plain", "OK");
        }
    });

    if (textEnable) {
        // Manejador manual para la petición preflight (OPTIONS)
        server.on("/texto", HTTP_OPTIONS, []() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
            server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
            server.send(204); // 204 indica al navegador que la petición es correcta y puede proceder
        });

        server.on("/texto", HTTP_POST, [&]() {

            server.sendHeader("Access-Control-Allow-Origin", "*");

            if (!server.hasArg("plain")) {
                server.send(400, "text/plain", "Body vacio");
                return;
            }
            String body = server.arg("plain");

            StaticJsonDocument<512> doc;
            DeserializationError err = deserializeJson(doc, body);
            if (err) {
                server.send(400, "text/plain", "JSON invalido");
                return;
            }

            String cmd = doc["cmd"] | "";
            if (cmd == "STOP") {
                if (estadoActual == ESTADO_TEXTO) {
                    estadoActual = ESTADO_GIFS;
                    saliendoAGifs = true;
                    interrumpirReproduccion = true;
                }
                server.send(200, "text/plain", "OK - STOP");
                return;
            }

            String nuevoTexto = doc["text"] | "";
            if (nuevoTexto.length() == 0) {
                server.send(400, "text/plain", "Falta 'text'");
                return;
            }

            String colorHex = doc["color"] | "#FFFFFF";
            int velocidad = doc["speed"] | 30;
            velocidad = constrain(velocidad, 5, 200); // ms/paso, límites de seguridad

            textoScrollMsg = nuevoTexto;
            textoScrollColor = parseHexColor(colorHex);
            textoScrollVelocidad = velocidad;

            Serial.printf(PSTR("[TEXTO] Recibido: '%s' | Color: %s | Velocidad: %d ms\n"), 
                  textoScrollMsg.c_str(), colorHex.c_str(), velocidad);

            // Arrancamos/reiniciamos la animación desde el borde derecho
            textoScrollX = offset + PANEL_RES_X;
            textoScrollUltimoPaso = millis();

            interrumpirReproduccion = true;
            estadoActual = ESTADO_TEXTO;

            server.send(200, "text/plain", "OK");
        });
        }

        server.begin();
        tcpServer.begin();
        Serial.println(F("[HTTP] Servidor iniciado y Panel listo."));
    }

    // Intentamos recuperar la última playlist guardada
    Preferences prefs;
    prefs.begin("retro-lite", true); // Modo lectura
    String tmp = prefs.getString("lastList", "");
    strlcpy(playlistActiva, tmp.c_str(), sizeof(playlistActiva));
    prefs.end();

    // --- LÓGICA DE FALLBACK (PLUG & PLAY) ---
    if (playlistActiva[0] == '\0' || !SD.exists(playlistActiva)) {
        Serial.println(F("[SISTEMA] Sin playlist activa o archivo no encontrado."));
        
        // 1. Escaneamos la carpeta /playlists para ver qué hay
        cargarNombresPlaylists(); 
        
        if (listaPlaylists.size() > 0) {
            // 2. ¡Hay listas! Cogemos la primera que exista en la SD alfabéticamente
            snprintf(playlistActiva, sizeof(playlistActiva),
             "/playlists/%s.txt", listaPlaylists[0].c_str());
            Serial.print(F("[INFO] Auto-asignando primera lista encontrada: "));
            Serial.println(playlistActiva);
            
            // Guardamos esta lista en memoria para que arranque más rápido la próxima vez
            prefs.begin("retro-lite", false);
            prefs.putString("lastList", playlistActiva);
            prefs.end();
            
            // Mandamos a reproducir directamente
            interrumpirReproduccion = false;
            estadoActual = ESTADO_GIFS;
        } else {
            // 3. Error Crítico: No hay ni un solo archivo .txt en la carpeta
            Serial.println(F("[CRÍTICO] La carpeta /playlists está vacía."));
            estadoActual = ESTADO_MENU_PRINCIPAL;
            interrumpirReproduccion = true;
        }
    } else {
        // La lista guardada existe y está bien
        Serial.print(F("[SISTEMA] Cargando playlist: "));
        Serial.println(playlistActiva);
        interrumpirReproduccion = false;
        estadoActual = ESTADO_GIFS;
    }
}

void loop() {
    verificarTemporizador();

    // Siempre vigilamos el botón (en cualquier modo)
    gestionarBotonMenu();
    leerControlRemoto();

    // Temporizador Mapeo IR
    chequearTimeoutMapeo();

    // Si está dormido, no procesamos GIFs ni menús
    if (isSleeping) {
        delay(100); 
        return;
    }

    if (WiFi.status() == WL_CONNECTED) {
        if (arcadeEnable > 0 || textEnable) 
        server.handleClient();
        verificarMarquesinaTCP();
        if (arcadeEnable == 3) 
        verificarReplayOSLite(); 
    }

    if (estadoActual == ESTADO_ARCADE) {
        delay(50); 
        return;  
    }

    if (estadoActual == ESTADO_TEXTO) {
    mostrarTextoScroll();
    delay(5);
    return;
    }

    // Decidimos qué dibujar en el panel
    if (estadoActual == ESTADO_GIFS) {

        if (saliendoAGifs) {
            // Reseteo total al volver a los GIFs
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
            Serial.println(F("[SISTEMA] Regreso a GIFs: Banderas de botón reseteadas."));
        }

        if (modoVisual == 1) { 
            // --- MODO: SOLO RELOJ ---
            mostrarRelojLite(false);
            gestionarActualizacionClima(); // Se encarga del tiempo
            
        } else {
            // --- MODO GIFS ---
            // ¿Toca mostrar el reloj?
            if (clockEnable == 1 && autoClockInt > 0 && gifsPlayed >= autoClockInt) {
        
                // A. Mostrar el reloj en pantalla
                if (transitionEnable == 1) {
                    mostrarRelojLite(true); // Con transición de particulas 
                }else
                    mostrarRelojLite(false);// Sin transición de particulas
        
                // B. Gestionar la actualización de datos (Solo si toca por tiempo)
                gestionarActualizacionClima(); 

                // C. Reiniciamos contador para volver a los GIFs
                gifsPlayed = 0;

            } else {
                ejecutarModoGifLite();
            }
        }

    }else {
    // --- MODO MENÚ ---
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

