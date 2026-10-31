// ====================================================================
//  PortalWiFi.ino - Portal WiFi propio (configurar la red sin editar config.ini)
//
//  Se abre desde el menu OSD (WiFi > Anadir red) o, automaticamente, si el
//  WiFi esta activado y no hay SSID. El flujo es:
//
//    1. portalEscanearRedes()   escanea antes de levantar el AP
//    2. portalIniciarAP()       AP con clave aleatoria + DNS "captive portal"
//    3. portalIniciarWeb()      servidor temporal con las rutas del portal
//    4. bucle:                  atiende peticiones, prueba la red recibida y
//                               comprueba si hay que salir (web, mando IR o boton)
//    5. portalLimpiar()         libera todo; reinicia o continua sin WiFi
//
//  IDIOMA: los textos del portal (panel y web) estan en PortalTextos.h
//  (ES, EN y FR) y se eligen segun LANGUAGE de config.ini; cualquier otro idioma
//  muestra ingles. No se usan los archivos /idioma/*.json (solo para los menus OSD).
//  Las piezas fijas del HTML estan en PortalPaginas.h.
//
// ====================================================================
#include <DNSServer.h>
#include "PortalPaginas.h"
#include "PortalTextos.h"

#define PORTAL_AP_SSID     "RetroPixel-LED"
#define PORTAL_TIMEOUT_MS  300000UL    // 5 min abierto como maximo
#define PORTAL_PRUEBA_MS   15000UL     // tiempo maximo probando una red
#define PORTAL_MAX_REDES   12

// --------------------------------------------------------------------
// Estado del portal (solo se usa mientras esta abierto)
// --------------------------------------------------------------------
struct RedWiFi { String ssid; int rssi; bool abierta; };

static std::vector<RedWiFi> portalRedes;
static WebServer*           portalWeb = nullptr;
static DNSServer            portalDns;

static char portalClave[9];
static char portalLinea2[24], portalLinea3[24], portalLinea4[24];
static char portalSsidNuevo[33];
static char portalPassNueva[64];
static int  portalIdioma = PORTAL_IDIOMA_EN;
static bool portalPendiente = false;    // el usuario envio el formulario
static bool portalCancelado = false;    // el usuario pulso "Cancelar" en la web

// ====================================================================
//  UTILIDADES
// ====================================================================

// Elige el idioma del portal segun LANGUAGE de config.ini: ES, FR o ingles por defecto
static void portalElegirIdioma() {
    if (strcasecmp(idiomaActivo, "ES") == 0)      portalIdioma = PORTAL_IDIOMA_ES;
    else if (strcasecmp(idiomaActivo, "FR") == 0) portalIdioma = PORTAL_IDIOMA_FR;
    else                                          portalIdioma = PORTAL_IDIOMA_EN;   // EN y cualquier otro
    Serial.printf("[PORTAL] Idioma del portal: %s (config: %s)\n", PORTAL_LANG[portalIdioma], idiomaActivo);
}

// Texto del portal en el idioma elegido (id = valor del enum PortalTexto)
static const char* portalT(int id) {
    const char* t = PORTAL_TXT[portalIdioma][id];
    return t ? t : PORTAL_TXT[PORTAL_IDIOMA_EN][id];     // por si falta una traduccion
}

static String escHtml(const String& s) {
    String o;
    o.reserve(s.length() + 8);
    for (size_t i = 0; i < s.length(); i++) {
        char c = s[i];
        if (c == '&') o += F("&amp;");
        else if (c == '<') o += F("&lt;");
        else if (c == '>') o += F("&gt;");
        else if (c == '"') o += F("&quot;");
        else o += c;
    }
    return o;
}

// Cuatro lineas de texto en el panel (fuente por defecto, solo ASCII, max. 21 caracteres por linea)
static void portalPantalla(const char* l1, const char* l2, const char* l3, const char* l4) {
    display->fillScreen(0);
    display->setFont(nullptr);
    display->setTextSize(1);
    display->setTextWrap(false);
    printMenuCentrado(l1, 0, display->color565(0, 255, 255), offset);
    display->drawLine(offset, 7, offset + offset, 7, display->color565(100, 100, 100));
    display->setTextColor(display->color565(255, 255, 255));
    display->setCursor(offset + 2, 9);    display->print(l2);
    display->setCursor(offset + 2, 17);   display->print(l3);
    display->setCursor(offset + 2, 25);   display->print(l4);
    display->flipDMABuffer();
}

static void portalPantallaPrincipal() {
    portalPantalla(portalT(PT_P_TITULO), portalLinea2, portalLinea3, portalLinea4);
}

// ====================================================================
//  1. ESCANEO DE REDES
// ====================================================================
static void portalEscanearRedes() {
    portalRedes.clear();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(200);

    int n = WiFi.scanNetworks();
    if (n <= 0) { delay(500); n = WiFi.scanNetworks(); }     // segundo intento si el primero falla
    Serial.printf("[PORTAL] Redes encontradas: %d\n", n);

    for (int i = 0; i < n; i++) {
        String s = WiFi.SSID(i);
        if (s.length() == 0) continue;                        // redes ocultas: se escriben a mano
        int rssi = WiFi.RSSI(i);
        bool abierta = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN);

        bool repetida = false;
        for (RedWiFi& r : portalRedes) {
            if (r.ssid == s) { repetida = true; if (rssi > r.rssi) r.rssi = rssi; break; }
        }
        if (!repetida) portalRedes.push_back({s, rssi, abierta});
    }
    WiFi.scanDelete();

    // De mas a menos senal (insercion: la lista es corta)
    for (size_t i = 1; i < portalRedes.size(); i++) {
        RedWiFi x = portalRedes[i];
        int j = (int)i - 1;
        while (j >= 0 && portalRedes[j].rssi < x.rssi) { portalRedes[j + 1] = portalRedes[j]; j--; }
        portalRedes[j + 1] = x;
    }
    if (portalRedes.size() > PORTAL_MAX_REDES) portalRedes.resize(PORTAL_MAX_REDES);

    for (const RedWiFi& r : portalRedes) Serial.printf("[PORTAL]   %s (%d dBm)\n", r.ssid.c_str(), r.rssi);
}

// ====================================================================
//  2. PUNTO DE ACCESO + DNS
// ====================================================================
static void portalIniciarAP() {
    WiFi.mode(WIFI_AP_STA);

    // Clave aleatoria que solo se ve en el panel
    snprintf(portalClave, sizeof(portalClave), "%08lu", (unsigned long)(esp_random() % 100000000UL));
    WiFi.softAP(PORTAL_AP_SSID, portalClave, 1, 0, 2);        // canal 1, visible, max. 2 clientes
    delay(200);

    const IPAddress ip = WiFi.softAPIP();
    portalDns.start(53, "*", ip);   // "captive portal": cualquier nombre apunta al panel

    snprintf(portalLinea2, sizeof(portalLinea2), "WiFi: %s", PORTAL_AP_SSID);
    snprintf(portalLinea3, sizeof(portalLinea3), "Pass: %s", portalClave);
    snprintf(portalLinea4, sizeof(portalLinea4), "IP: %s", ip.toString().c_str());

    Serial.printf("[PORTAL] AP %s, IP %s, heap libre %u, mayor bloque %u\n",
                  PORTAL_AP_SSID, ip.toString().c_str(), ESP.getFreeHeap(), ESP.getMaxAllocHeap());
}

// ====================================================================
//  3. SERVIDOR WEB TEMPORAL
// ====================================================================

// Cabecera de la respuesta: <html lang>, estilos y <body>. Envia por trozos (sin String grandes).
static void portalInicioRespuesta(int codigo, bool formulario, bool titularRojo) {
    portalWeb->setContentLength(CONTENT_LENGTH_UNKNOWN);
    portalWeb->send(codigo, "text/html; charset=utf-8", "");
    portalWeb->sendContent_P(PORTAL_HTML_A);
    portalWeb->sendContent(PORTAL_LANG[portalIdioma]);
    portalWeb->sendContent_P(PORTAL_HTML_B);
    portalWeb->sendContent_P(PORTAL_CSS_BASE);
    if (formulario)  portalWeb->sendContent_P(PORTAL_CSS_FORM);
    if (titularRojo) portalWeb->sendContent_P(PORTAL_CSS_ROJO);
    portalWeb->sendContent_P(PORTAL_FIN_CABECERA);
}

static void portalFinRespuesta() {
    portalWeb->sendContent("");
}

// <etiqueta>texto traducido</etiqueta>
static void portalElemento(const char* etiqueta, int id) {
    String h = String("<") + etiqueta + ">" + escHtml(portalT(id)) + "</" + etiqueta + ">";
    portalWeb->sendContent(h);
}

static void portalRutaInicio() {
    portalInicioRespuesta(200, true, false);
    portalWeb->sendContent_P(PORTAL_H1);
    portalElemento("p", PT_W_INSTRUCCION);

    portalWeb->sendContent_P(PORTAL_FORM_INICIO);
    portalElemento("label", PT_W_REDES);
    portalWeb->sendContent("<div id=\"lista\">");
    if (portalRedes.empty()) {
        portalElemento("p", PT_W_SIN_REDES);
    }
    for (const RedWiFi& r : portalRedes) {
        portalWeb->sendContent("<button type=\"button\" class=\"red\" data-s=\"" + escHtml(r.ssid) + "\">" +
                               escHtml(r.ssid) + (r.abierta ? "" : " &#128274;") + "</button>");
    }
    portalWeb->sendContent("</div>");

    portalElemento("label", PT_W_RED);
    portalWeb->sendContent_P(PORTAL_INPUT_SSID);
    portalElemento("label", PT_W_PASS);
    portalWeb->sendContent_P(PORTAL_INPUT_PASS);
    portalWeb->sendContent("<button type=\"submit\">" + escHtml(portalT(PT_W_CONECTAR)) + "</button></form>");

    portalElemento("p", PT_W_AVISO_24);
    portalWeb->sendContent("<p><a href=\"/cancelar\">" + escHtml(portalT(PT_W_CANCELAR)) + "</a></p>");
    portalWeb->sendContent_P(PORTAL_SCRIPT_FIN);
    portalFinRespuesta();
}

static void portalPaginaEspera() {
    portalInicioRespuesta(200, false, false);
    portalElemento("h1", PT_W_ESPERA_TITULO);
    portalElemento("p", PT_W_ESPERA_1);
    portalElemento("p", PT_W_ESPERA_2);
    portalWeb->sendContent_P(PORTAL_FIN_BODY);
    portalFinRespuesta();
}

static void portalPaginaError() {
    portalInicioRespuesta(400, false, true);
    portalElemento("h1", PT_W_ERROR_TITULO);
    portalElemento("p", PT_W_ERROR_TXT);
    portalWeb->sendContent("<p><a href=\"/\">" + escHtml(portalT(PT_W_VOLVER)) + "</a></p>");
    portalWeb->sendContent_P(PORTAL_FIN_BODY);
    portalFinRespuesta();
}

static void portalPaginaCancelado() {
    portalInicioRespuesta(200, false, false);
    portalElemento("h1", PT_W_CANCELADO_TITULO);
    portalElemento("p", PT_W_CANCELADO_TXT);
    portalWeb->sendContent_P(PORTAL_FIN_BODY);
    portalFinRespuesta();
}

static void portalRutaGuardar() {
    String s = portalWeb->arg("ssid");
    String p = portalWeb->arg("pass");
    bool ok = (s.length() >= 1 && s.length() <= 32) &&
              (p.length() == 0 || (p.length() >= 8 && p.length() <= 63));
    if (!ok) {
        portalPaginaError();
        return;
    }
    strlcpy(portalSsidNuevo, s.c_str(), sizeof(portalSsidNuevo));
    strlcpy(portalPassNueva, p.c_str(), sizeof(portalPassNueva));
    portalPendiente = true;
    portalPaginaEspera();
}

static void portalRutaCancelar() {
    portalCancelado = true;
    portalPaginaCancelado();
}

// Cualquier otra ruta (sondas de "portal cautivo" de Android, iOS y Windows) redirige al formulario
static void portalRutaNoEncontrada() {
    portalWeb->sendHeader("Location", String("http://") + WiFi.softAPIP().toString() + "/", true);
    portalWeb->send(302, "text/plain", "");
}

static void portalIniciarWeb() {
    portalWeb = new WebServer(80);
    portalWeb->on("/", HTTP_GET, portalRutaInicio);
    portalWeb->on("/guardar", HTTP_POST, portalRutaGuardar);
    portalWeb->on("/cancelar", HTTP_GET, portalRutaCancelar);
    portalWeb->onNotFound(portalRutaNoEncontrada);
    portalWeb->begin();
}

// ====================================================================
//  4. BUCLE: PROBAR LA RED Y SALIR
// ====================================================================

// Prueba la red recibida; si conecta, la guarda en config.ini. Devuelve true si se guardo.
static bool portalProbarYGuardar(int doubleBuffOriginal) {
    portalPantalla(portalT(PT_P_PROBANDO), portalSsidNuevo, "", "");

    WiFi.begin(portalSsidNuevo, portalPassNueva);
    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < PORTAL_PRUEBA_MS) delay(250);

    if (WiFi.status() != WL_CONNECTED) {
        WiFi.disconnect(false);
        portalPantalla(portalT(PT_P_NO_CONECTA), portalT(PT_P_REVISA1), portalT(PT_P_REVISA2), portalT(PT_P_REINTENTA));
        delay(4000);
        portalPantallaPrincipal();
        return false;
    }

    wifiEnable = true;
    strlcpy(wifi_ssid, portalSsidNuevo, sizeof(wifi_ssid));
    strlcpy(wifi_pass, portalPassNueva, sizeof(wifi_pass));
    doubleBuff = doubleBuffOriginal;      // no cambiamos el ajuste del usuario
    guardarConfigIni();

    char lIP[24];
    snprintf(lIP, sizeof(lIP), "IP: %s", WiFi.localIP().toString().c_str());
#ifdef MDNS_HOSTNAME
    portalPantalla(portalT(PT_P_OK), lIP, MDNS_HOSTNAME ".local", portalT(PT_P_REINICIANDO));
#else
    portalPantalla(portalT(PT_P_OK), lIP, "", portalT(PT_P_REINICIANDO));
#endif
    delay(6000);
    return true;
}

// true si se pulso la tecla OK del mando IR (ignora los codigos de repeticion al mantener pulsado)
static bool portalTeclaSalirIR() {
    if (!IrReceiver.decode()) return false;
    bool salir = !(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) &&
                 IrReceiver.decodedIRData.decodedRawData == ir_btn_ok;
    IrReceiver.resume();
    if (salir) Serial.println(F("[PORTAL] Tecla OK del mando: saliendo del portal"));
    return salir;
}

// true si hay que cerrar el portal sin guardar: boton "Cancelar" de la web, mando IR o boton fisico
static bool portalDebeSalir() {
    if (portalCancelado) {
        delay(500);                       // deja que el navegador reciba la respuesta
        return true;
    }
    if (portalTeclaSalirIR()) return true;
    if (digitalRead(PIN_BOTON_MENU) == LOW) {
        while (digitalRead(PIN_BOTON_MENU) == LOW) delay(10);
        return true;
    }
    return false;
}

// ====================================================================
//  5. LIMPIEZA
// ====================================================================
static void portalLimpiar() {
    if (portalWeb) {                      // servidor temporal: no deja rutas abiertas en la red normal
        portalWeb->stop();
        delete portalWeb;
        portalWeb = nullptr;
    }
    portalDns.stop();
    WiFi.softAPdisconnect(true);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(200);

    portalRedes.clear();
    portalRedes.shrink_to_fit();
}

// ====================================================================
//  PUNTOS DE ENTRADA
// ====================================================================

// Llamar desde setup(): consume la bandera que deja el menu OSD y decide si arranca el portal.
// Se abre si el menu lo pidio, o si el WiFi esta activado pero no hay red configurada.
bool portalWiFiNecesario(bool sdOk, bool entrarFTP) {
    Preferences p;
    p.begin("sistema", false);
    bool pedido = p.getBool("portal_wifi", false);
    if (pedido) p.putBool("portal_wifi", false);              // se consume: un solo intento
    p.end();

    return sdOk && !entrarFTP && (pedido || (wifiEnable == 1 && wifi_ssid[0] == '\0'));
}

// Llamar desde el menu OSD: pide abrir el portal en el proximo arranque (mismo patron que el modo FTP).
// Estos dos mensajes son del menu OSD, asi que siguen viniendo del JSON de idioma (SUBMENU_WIFI).
void abrirPortalWiFiDesdeMenu() {
    display->fillScreen(0);
    printMenuCentrado(msgOr("SUBMENU_WIFI", "portal_abriendo", "Abriendo portal"), 8, display->color565(255, 255, 255), offset);
    printMenuCentrado(msgOr("SUBMENU_WIFI", "portal_reinicio", "Reiniciando..."), 18, display->color565(255, 255, 255), offset);
    display->flipDMABuffer();

    Serial.println(F("[MENU] Portal WiFi solicitado: reiniciando..."));
    Preferences p;
    p.begin("sistema", false);
    p.putBool("portal_wifi", true);
    p.end();

    delay(2000);
    ESP.restart();
}

// Llamar desde setup() tras iniciar el panel.
// doubleBuffOriginal: valor de DOUBLE_BUFF antes de desactivarlo para este arranque (se restaura antes de guardar)
void ejecutarPortalWiFi(int doubleBuffOriginal) {
    Serial.println(F("[PORTAL] Abriendo portal de configuración WiFi"));

    portalPendiente = false;
    portalCancelado = false;
    portalElegirIdioma();

    portalEscanearRedes();        // antes de levantar el AP, para no mover el canal con el movil conectado
    portalIniciarAP();
    portalIniciarWeb();
    portalPantallaPrincipal();

    bool guardado = false;
    const unsigned long inicio = millis();
    while (!guardado && millis() - inicio < PORTAL_TIMEOUT_MS) {
        portalDns.processNextRequest();
        portalWeb->handleClient();

        if (portalPendiente) {
            portalPendiente = false;
            guardado = portalProbarYGuardar(doubleBuffOriginal);
        }

        if (portalDebeSalir()) break;
        delay(2);
    }

    portalLimpiar();

    // Con red nueva guardada, o con una red ya configurada: reinicio limpio
    if (guardado || wifi_ssid[0] != '\0') {
        ESP.restart();
    }

    // Sin red configurada: se continua el arranque sin WiFi
    doubleBuff = doubleBuffOriginal;
    display->fillScreen(0);
    display->flipDMABuffer();
}
