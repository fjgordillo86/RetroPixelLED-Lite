// ============================================================
// WebRoutes.ino
// Registro de todos los endpoints HTTP del servidor de la PWA.
// ============================================================

void registrarRutasWeb() {
    registrarRutasEstado();
    registrarRutasControl();
    registrarRutasTemporizador();
    registrarRutasIdiomas();
    registrarRutasConfig();
    registrarRutasPlaylist();
    registrarRutasOTA();
    registrarRutasTexto();
    registrarRutasIR();
    registrarRutasDiag();
    registrarRutasReloj();

    server.begin();
    tcpServer.begin();
    Serial.println(F("[HTTP] Servidor iniciado y Panel listo."));
}

// --- ESTADO LIGERO (no cambia nada, solo informa) ---
void registrarRutasEstado() {
    server.on("/dynfaces", HTTP_GET, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        String s = "[";
        for (int i = 0; i < RD_NUM_CARAS; i++) {
            if (i) s += ",";
            s += "\""; s += RD_CARAS[i].nombre; s += "\"";
        }
        s += "]";
        server.send(200, "application/json", s);
    });
    
    server.on("/status", HTTP_GET, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        StaticJsonDocument<448> doc;
        doc["brightness"]      = brightness;
        doc["playMode"]        = modoVisual;
        doc["randomMode"]      = randomMode;
        doc["clockStyle"]      = clockStyle;
        doc["clockColorIndex"] = clockColorIndex;
        doc["dynClock"]        = relojDinamico;
        doc["dynMode"]         = dynModo;
        doc["dynRot"]          = dynRotMin;
        doc["dynCount"]        = RD_NUM_CARAS;
        char hexColor[8];
        snprintf(hexColor, sizeof(hexColor), "#%06X", (unsigned)(clockColor & 0xFFFFFF));
        doc["clockColorHex"]   = hexColor;
        doc["textEnable"]      = textEnable ? 1 : 0;
        doc["textoActivo"]     = (estadoActual == ESTADO_TEXTO) ? 1 : 0;
        doc["isSleeping"]      = isSleeping ? 1 : 0;
        doc["fw"]    = FIRMWARE_VERSION;
        doc["fwNum"] = CURRENT_VERSION_NUM;
        doc["ip"] = WiFi.localIP().toString();

        String activa = String(playlistActiva);
        activa.replace("/playlists/", ""); activa.replace(".txt", "");
        doc["activePlaylist"] = activa;

        String salida; serializeJson(doc, salida);
        server.send(200, "application/json", salida);
    });
}

// --- CONTROL EN VIVO (aplica y persiste, SIN reiniciar) ---
void registrarRutasControl() {
    server.on("/control", HTTP_OPTIONS, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(204);
    });

    server.on("/control", HTTP_POST, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        if (!server.hasArg("plain")) { server.send(400, "text/plain", "Body vacio"); return; }

        StaticJsonDocument<384> doc;
        if (deserializeJson(doc, server.arg("plain"))) { server.send(400, "text/plain", "JSON invalido"); return; }

        // El fondo se valida ANTES de aplicar nada: solo "" (sin fondo) o un .bmp existente en /relojes/imagenes
        if (doc.containsKey("clockBg") && !nombreFondoValido(doc["clockBg"] | "")) {
            server.send(400, "text/plain", "Fondo no valido");
            return;
        }

        // Color RGB libre del reloj: "#RRGGBB" (o "RRGGBB")
        if (doc.containsKey("clockColorHex")) {
            const char* h = doc["clockColorHex"] | "";
            if (h[0] == '#') h++;
            if (strlen(h) != 6 || strspn(h, "0123456789abcdefABCDEF") != 6) {
                server.send(400, "text/plain", "Color no valido");
                return;
            }
        }

        if (doc.containsKey("brightness")) {
            brightness = doc["brightness"];
            display->setBrightness8(brightness);
        }
        if (doc.containsKey("playMode")) {
            const int nuevoModo = doc["playMode"];
            const bool cambia = (nuevoModo != modoVisual);
            modoVisual = nuevoModo;
            if (cambia && estadoActual == ESTADO_GIFS) {
                // Corta el GIF o el reloj en curso: el bucle principal arranca el modo nuevo al instante
                interrumpirReproduccion = true;
                cambioModoPendiente = true;
            }
        }
        if (doc.containsKey("randomMode")) randomMode = doc["randomMode"];
        if (doc.containsKey("clockStyle")) clockStyle = doc["clockStyle"];
        if (doc.containsKey("dynMode")) { int v = doc["dynMode"]; dynModo = (v < 0 || v > 2) ? 0 : v; }
        if (doc.containsKey("dynRot"))  { int v = doc["dynRot"];  dynRotMin = (v < 1 || v > 240) ? 15 : v; }
        if (doc.containsKey("dynClock")) {
                    int v = doc["dynClock"];
                    relojDinamico = (v < 0 || v > RD_NUM_CARAS) ? 0 : v;
                }
        if (doc.containsKey("clockColorIndex")) {
            clockColorIndex = doc["clockColorIndex"];
            if (clockColorIndex < 0 || clockColorIndex >= TOTAL_COLORES) clockColorIndex = 0;
            clockColor = listaColores[clockColorIndex].colorRGB;
        }

        if (doc.containsKey("clockColorHex")) {
            const char* h = doc["clockColorHex"] | "000000";
            if (h[0] == '#') h++;
            clockColor = strtoul(h, nullptr, 16) & 0xFFFFFF;
            clockColorIndex = -1;                     // -1 = color personalizado (ninguna paleta)
        }

        bool relojCambiado = false;
        if (doc.containsKey("clockBg")) {
            strlcpy(clockBg, doc["clockBg"] | "", sizeof(clockBg));
            relojCambiado = true;
        }
        if (doc.containsKey("clockPos")) {
            clockPos = constrain(doc["clockPos"].as<int>(), 0, 2);
            relojCambiado = true;
        }
        if (relojCambiado) aplicarCambioFondoReloj();

        guardarConfigIni();
        server.send(200, "text/plain", "OK");
    });
}

// --- TEMPORIZADOR (horario + encendido/apagado manual) ---
void registrarRutasTemporizador() {
    server.on("/timer", HTTP_GET, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        StaticJsonDocument<192> doc;
        doc["enable"]     = timerEnable ? 1 : 0;
        doc["hOn"]        = hOn;
        doc["mOn"]        = mOn;
        doc["hOff"]       = hOff;
        doc["mOff"]       = mOff;
        doc["isSleeping"] = isSleeping ? 1 : 0;
        String salida; serializeJson(doc, salida);
        server.send(200, "application/json", salida);
    });

    server.on("/timer", HTTP_OPTIONS, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(204);
    });

    server.on("/timer", HTTP_POST, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        if (!server.hasArg("plain")) { server.send(400, "text/plain", "Body vacio"); return; }

        StaticJsonDocument<192> doc;
        if (deserializeJson(doc, server.arg("plain"))) { server.send(400, "text/plain", "JSON invalido"); return; }

        if (doc.containsKey("enable")) timerEnable = doc["enable"].as<int>() != 0;
        if (doc.containsKey("hOn"))    hOn  = doc["hOn"];
        if (doc.containsKey("mOn"))    mOn  = doc["mOn"];
        if (doc.containsKey("hOff"))   hOff = doc["hOff"];
        if (doc.containsKey("mOff"))   mOff = doc["mOff"];

        guardarAjustesTimer();
        server.send(200, "text/plain", "OK");
    });

    server.on("/timer/toggle", HTTP_POST, [&]() {
    server.sendHeader("Access-Control-Allow-Origin", "*");

    bool dormir = !isSleeping; // comportamiento actual: alternar (lo sigue usando la PWA)
    if (server.hasArg("plain")) {
        StaticJsonDocument<64> doc;
        if (!deserializeJson(doc, server.arg("plain")) && doc.containsKey("sleep")) {
            dormir = doc["sleep"].as<bool>(); // Home Assistant sí especifica dirección exacta
        }
    }

    toggleEnergia(dormir);
    manualOverride = true;

    StaticJsonDocument<64> respuesta;
    respuesta["isSleeping"] = isSleeping ? 1 : 0;
    String salida; serializeJson(respuesta, salida);
    server.send(200, "application/json", salida);
    });
}

// --- IDIOMAS: descarga de .json desde GitHub ---
void registrarRutasIdiomas() {
    server.on("/idiomas/actualizar", HTTP_POST, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", "{\"ok\":true,\"resumen\":\"Reiniciando para liberar RAM y descargar...\"}");

        Preferences prefs;
        prefs.begin("sistema", false);
        prefs.remove("idiomas_res"); // limpiamos resultado anterior
        prefs.remove("idiomas_ok");
        prefs.putBool("idiomas_pending", true);
        prefs.end();

        delay(300);
        ESP.restart();
    });

    server.on("/idiomas/estado", HTTP_GET, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");

        Preferences prefs;
        prefs.begin("sistema", true);
        bool hay = prefs.isKey("idiomas_res");
        String resumen = prefs.getString("idiomas_res", "");
        bool ok = prefs.getBool("idiomas_ok", false);
        prefs.end();

        StaticJsonDocument<256> doc;
        doc["hay"] = hay;
        doc["ok"] = ok;
        doc["resumen"] = resumen;
        String salida; serializeJson(doc, salida);
        server.send(200, "application/json", salida);
    });
}

// --- CONFIG: lectura/escritura de config.ini ---
void registrarRutasConfig() {
    server.on("/config", HTTP_OPTIONS, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(204);
    });

    server.on("/config/exit", HTTP_OPTIONS, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
        server.send(204);
    });

    server.on("/config", HTTP_GET, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");

        estadoActual = ESTADO_CONFIG_APP;
        interrumpirReproduccion = true;
        mostrarPantallaConfigApp();

        StaticJsonDocument<1024> doc;
        doc["WIFI_ENABLE"]      = wifiEnable;
        doc["SSID"]             = wifi_ssid;
        doc["PASS"]             = wifi_pass;
        doc["MOSTRAR_IP"]       = mostrarIP ? 1 : 0;
        doc["TZ"]               = time_zone;
        doc["PANEL_CHAIN"]      = panelChain;
        doc["COLOR_ORDER"]      = colorOrder;
        doc["BRIGHTNESS"]       = brightness;
        doc["I2S_SPEED"]        = i2sSpeed;
        doc["REFRESH_MIN"]      = refreshMin;
        doc["DOUBLE_BUFF"]      = doubleBuff;
        doc["LATCH_BLANK"]      = latchBlank;
        doc["SD_SPEED"]         = sdSpeed;
        doc["PLAY_MODE"]        = modoVisual;
        doc["ARCADE_ENABLE"]    = arcadeEnable;
        doc["TEXT_ENABLE"]      = textEnable ? 1 : 0;
        doc["CLOCK_ENABLE"]     = clockEnable;
        doc["RANDOM_MODE"]      = randomMode;
        doc["AUTO_CLOCK_INT"]   = autoClockInt;
        doc["CLOCK_DURATION"]   = clockDuration;
        doc["CLOCK_STYLE"]      = clockStyle;
        doc["DYN_MODE"]         = dynModo;
        doc["DYN_ROT"]          = dynRotMin;
        doc["DYN_CLOCK"]        = relojDinamico;
        doc["TRANSITION_ENABLE"]= transitionEnable;
        doc["CLOCK_COLOR"]      = clockColorIndex;
        doc["WEATHER_ENABLE"]   = weatherEnable;
        doc["CITY"]             = weatherCity;
        doc["API_KEY"]          = weatherKey;
        doc["WEATHER_INT"]      = weatherInterval;
        doc["WEATHER_MSG"]      = weatherCustomMsg;
        doc["LANGUAGE"]         = idiomaActivo;
        doc["IP"]               = replayOS_IP;
        doc["TOKEN"]            = replayOS_Token;

        String salida;
        serializeJson(doc, salida);
        server.send(200, "application/json", salida);
    });

    server.on("/config/exit", HTTP_POST, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        pasoMapeo = -1;
        leerConfigIni();
        if (estadoActual == ESTADO_CONFIG_APP) {
            estadoActual = ESTADO_GIFS;
            saliendoAGifs = true;
            interrumpirReproduccion = true;
        }
        server.send(200, "text/plain", "OK");
    });

    server.on("/config", HTTP_POST, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");

        if (!server.hasArg("plain")) {
            server.send(400, "text/plain", "Body vacio");
            return;
        }

        StaticJsonDocument<1664> doc;
        if (deserializeJson(doc, server.arg("plain"))) {
            server.send(400, "text/plain", "JSON invalido");
            return;
        }

        // Valores anteriores: solo algunos cambios obligan a reiniciar (mismas reglas que el menú OSD)
        const auto oArcade = arcadeEnable;     const auto oTexto = textEnable;       const auto oTrans = transitionEnable;
        const auto oI2s = i2sSpeed;            const auto oRefresco = refreshMin;    const auto oDbuff = doubleBuff;
        const auto oLatch = latchBlank;        const auto oSd = sdSpeed;             const auto oChain = panelChain;
        const auto oWifi = wifiEnable;         const auto oMostrarIP = mostrarIP;    const auto oClima = weatherEnable;
        const auto oIntClima = weatherInterval;
        char oOrder[sizeof(colorOrder)], oSsid[sizeof(wifi_ssid)], oPass[sizeof(wifi_pass)], oTz[sizeof(time_zone)];
        char oCiudad[sizeof(weatherCity)], oKey[sizeof(weatherKey)], oIdioma[sizeof(idiomaActivo)];
        strlcpy(oOrder, colorOrder, sizeof(oOrder));        strlcpy(oSsid, wifi_ssid, sizeof(oSsid));
        strlcpy(oPass, wifi_pass, sizeof(oPass));           strlcpy(oTz, time_zone, sizeof(oTz));
        strlcpy(oCiudad, weatherCity, sizeof(oCiudad));     strlcpy(oKey, weatherKey, sizeof(oKey));
        strlcpy(oIdioma, idiomaActivo, sizeof(oIdioma));

        // Aplicamos los valores recibidos a las variables globales
        if (doc.containsKey("WIFI_ENABLE"))   wifiEnable = doc["WIFI_ENABLE"];
        if (doc.containsKey("SSID"))          strlcpy(wifi_ssid, doc["SSID"] | "", sizeof(wifi_ssid));
        if (doc.containsKey("PASS"))          strlcpy(wifi_pass, doc["PASS"] | "", sizeof(wifi_pass));
        if (doc.containsKey("MOSTRAR_IP"))    mostrarIP = doc["MOSTRAR_IP"].as<int>() != 0;
        if (doc.containsKey("TZ"))            strlcpy(time_zone, doc["TZ"] | "", sizeof(time_zone));

        if (doc.containsKey("PANEL_CHAIN"))   panelChain = doc["PANEL_CHAIN"];
        if (doc.containsKey("COLOR_ORDER"))   strlcpy(colorOrder, doc["COLOR_ORDER"] | "RGB", sizeof(colorOrder));
        if (doc.containsKey("BRIGHTNESS"))    brightness = doc["BRIGHTNESS"];
        if (doc.containsKey("I2S_SPEED"))     i2sSpeed = doc["I2S_SPEED"];
        if (doc.containsKey("REFRESH_MIN"))   refreshMin = doc["REFRESH_MIN"];
        if (doc.containsKey("DOUBLE_BUFF"))   doubleBuff = doc["DOUBLE_BUFF"];
        if (doc.containsKey("LATCH_BLANK"))   latchBlank = doc["LATCH_BLANK"];
        if (doc.containsKey("SD_SPEED")) {
            int v = doc["SD_SPEED"].as<int>();
            if (v == 4 || v == 10 || v == 20 || v == 25) sdSpeed = v;   // ignora valores no válidos
        }

        if (doc.containsKey("PLAY_MODE"))     modoVisual = doc["PLAY_MODE"];
        if (doc.containsKey("ARCADE_ENABLE")) arcadeEnable = doc["ARCADE_ENABLE"];
        if (doc.containsKey("TEXT_ENABLE"))   textEnable = doc["TEXT_ENABLE"].as<int>() != 0;
        if (doc.containsKey("CLOCK_ENABLE"))  clockEnable = doc["CLOCK_ENABLE"];
        if (doc.containsKey("RANDOM_MODE"))   randomMode = doc["RANDOM_MODE"];
        if (doc.containsKey("AUTO_CLOCK_INT"))autoClockInt = doc["AUTO_CLOCK_INT"];
        if (doc.containsKey("CLOCK_DURATION"))clockDuration = doc["CLOCK_DURATION"];
        if (doc.containsKey("CLOCK_STYLE"))   clockStyle = doc["CLOCK_STYLE"];
        if (doc.containsKey("DYN_MODE")) { int v = doc["DYN_MODE"]; dynModo = (v < 0 || v > 2) ? 0 : v; }
        if (doc.containsKey("DYN_ROT"))  { int v = doc["DYN_ROT"];  dynRotMin = (v < 1 || v > 240) ? 15 : v; }
        if (doc.containsKey("DYN_CLOCK")) {
            int v = doc["DYN_CLOCK"];
            relojDinamico = (v < 0 || v > RD_NUM_CARAS) ? 0 : v;
        }
        if (doc.containsKey("TRANSITION_ENABLE")) transitionEnable = doc["TRANSITION_ENABLE"];
        if (doc.containsKey("CLOCK_COLOR")) {
            clockColorIndex = doc["CLOCK_COLOR"];
            if (clockColorIndex < 0 || clockColorIndex >= TOTAL_COLORES) clockColorIndex = 0;
            clockColor = listaColores[clockColorIndex].colorRGB;
        }

        if (doc.containsKey("WEATHER_ENABLE"))weatherEnable = doc["WEATHER_ENABLE"];
        if (doc.containsKey("CITY"))          strlcpy(weatherCity, doc["CITY"] | "", sizeof(weatherCity));
        if (doc.containsKey("API_KEY"))       strlcpy(weatherKey, doc["API_KEY"] | "", sizeof(weatherKey));
        if (doc.containsKey("WEATHER_INT"))   weatherInterval = doc["WEATHER_INT"];
        if (doc.containsKey("WEATHER_MSG"))   strlcpy(weatherCustomMsg, doc["WEATHER_MSG"] | "", sizeof(weatherCustomMsg));

        if (doc.containsKey("LANGUAGE"))      strlcpy(idiomaActivo, doc["LANGUAGE"] | "ES", sizeof(idiomaActivo));

        if (doc.containsKey("IP"))            strlcpy(replayOS_IP, doc["IP"] | "", sizeof(replayOS_IP));
        if (doc.containsKey("TOKEN"))         strlcpy(replayOS_Token, doc["TOKEN"] | "", sizeof(replayOS_Token));

        // ¿Hace falta reiniciar? Mismas reglas que "Guardar y salir" del menú OSD:
        //  - cualquier cambio en: arcade, texto scroll, transición, velocidad I2S, refresco, doble buffer,
        //    anti-ghosting y velocidad SD (y, por cómo arranca el panel: nº de paneles, orden de color y red WiFi)
        //  - solo al ACTIVAR (0 -> 1): WiFi, mostrar IP y clima
        bool reinicio = false;
        if (arcadeEnable != oArcade || textEnable != oTexto || transitionEnable != oTrans) reinicio = true;
        if (i2sSpeed != oI2s || refreshMin != oRefresco || doubleBuff != oDbuff || latchBlank != oLatch || sdSpeed != oSd) reinicio = true;
        if (panelChain != oChain || strcmp(colorOrder, oOrder) != 0) reinicio = true;
        if (strcmp(wifi_ssid, oSsid) != 0 || strcmp(wifi_pass, oPass) != 0) reinicio = true;
        if ((!oWifi && wifiEnable) || (!oMostrarIP && mostrarIP) || (!oClima && weatherEnable)) reinicio = true;

        guardarConfigIni();

        if (reinicio) {
            server.send(200, "text/plain", "OK - Reiniciando");
            Serial.println(F("[CONFIG-APP] Configuracion guardada desde la PWA. Este cambio requiere reiniciar..."));
            delay(400);
            ESP.restart();
            return;
        }

        // Sin reinicio: los cambios se aplican en caliente
        if (display) display->setBrightness8(brightness);
        if (strcmp(time_zone, oTz) != 0) configTzTime(time_zone, ntpServer);
        if (strcmp(idiomaActivo, oIdioma) != 0) liberarIdiomaMenu();          // el menú carga el idioma nuevo al abrirse
        if (weatherEnable && !doubleBuff &&
            (strcmp(weatherCity, oCiudad) != 0 || strcmp(weatherKey, oKey) != 0 || weatherInterval != oIntClima)) {
            // Pide actualizar el clima en cuanto se salga de la pantalla de ajustes
            // (con doble buffer el clima ya se actualiza reiniciando, por diseño, en su próxima ventana)
            lastWeatherUpdate = millis() - (unsigned long)weatherInterval * 60000UL;
        }

        // Se sale de la pantalla "Configuración APP" como en /config/exit, pero conservando lo recién guardado
        pasoMapeo = -1;
        if (estadoActual == ESTADO_CONFIG_APP) {
            estadoActual = ESTADO_GIFS;
            saliendoAGifs = true;
            interrumpirReproduccion = true;
        }
        server.send(200, "text/plain", "OK");
        Serial.println(F("[CONFIG-APP] Configuracion guardada desde la PWA (sin reinicio)."));
    });
}

// --- PLAYLISTS ---
void registrarRutasPlaylist() {
    server.on("/playlists", HTTP_GET, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");

        cargarNombresPlaylists();

        DynamicJsonDocument doc(2048);
        JsonArray items = doc.createNestedArray("items");
        for (auto &p : listaPlaylists) items.add(p);

        String activa = String(playlistActiva);
        activa.replace("/playlists/", "");
        activa.replace(".txt", "");
        doc["active"] = activa;

        String salida;
        serializeJson(doc, salida);
        server.send(200, "application/json", salida);
    });

    server.on("/playlist", HTTP_OPTIONS, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(204);
    });

    server.on("/playlist", HTTP_POST, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");

        if (!server.hasArg("plain")) { server.send(400, "text/plain", "Body vacio"); return; }

        StaticJsonDocument<256> doc;
        if (deserializeJson(doc, server.arg("plain"))) { server.send(400, "text/plain", "JSON invalido"); return; }

        String nombre = doc["name"] | "";
        if (nombre.length() == 0) { server.send(400, "text/plain", "Falta 'name'"); return; }

        cargarNombresPlaylists();
        bool existe = false;
        for (auto &p : listaPlaylists) if (p == nombre) { existe = true; break; }
        if (!existe) { server.send(404, "text/plain", "Playlist no encontrada"); return; }

        snprintf(playlistActiva, sizeof(playlistActiva), "/playlists/%s.txt", nombre.c_str());

        Preferences prefs;
        prefs.begin("retro-lite", false);
        prefs.putString("lastList", playlistActiva);
        prefs.end();

        estadoActual = ESTADO_GIFS;
        interrumpirReproduccion = true;
        saliendoAGifs = true;

        server.send(200, "text/plain", "OK");
        Serial.print(F("[PLAYLIST-APP] Seleccionada desde PWA: "));
        Serial.println(playlistActiva);
    });
}

// --- OTA (firmware) ---
void registrarRutasOTA() {
    server.on("/ota", HTTP_POST, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "text/plain", "OK - Buscando actualizacion");
        delay(300);
        triggerActualizacionOTA();
    });
}

// --- TEXTO SCROLL ---
void registrarRutasTexto() {
    if (!textEnable) return;

    server.on("/texto", HTTP_OPTIONS, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(204);
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
                display->setFont(NULL);
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
        velocidad = constrain(velocidad, 5, 200);

        // Leemos la fuente del JSON
        String tipoFuente = doc["font"] | "bold"; // "bold" por defecto
        // tipoFuente.toLowerCase(); // Normalizamos a minúsculas por seguridad

        if (tipoFuente == "light") {
            textoScrollFuente = &fuente8pt7b_Light;
        } else if (tipoFuente == "regular") {
            textoScrollFuente = &fuente8pt7b_Regular;
        } else if (tipoFuente == "semibold") {
            textoScrollFuente = &fuente8pt7b_SemiBold;
        } else {
            textoScrollFuente = &fuente8pt7b_Bold; // Fallback a Bold
        }

        textoScrollMsg = nuevoTexto;
        textoScrollColor = parseHexColor(colorHex);
        textoScrollVelocidad = velocidad;

        Serial.printf(PSTR("[TEXTO] Recibido: '%s' | Color: %s | Velocidad: %d ms | Fuente: %s\n"),
              textoScrollMsg.c_str(), colorHex.c_str(), velocidad, tipoFuente.c_str());

        textoScrollX = offset + PANEL_RES_X;
        textoScrollUltimoPaso = millis();

        interrumpirReproduccion = true;
        estadoActual = ESTADO_TEXTO;

        server.send(200, "text/plain", "OK");
    });
}

// --- MANDO IR: mapeo de botones desde la PWA (reutiliza gestionarMapeoIR/chequearTimeoutMapeo) ---
void registrarRutasIR() {
    server.on("/ir/learn", HTTP_OPTIONS, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(204);
    });

    server.on("/ir/learn", HTTP_POST, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        if (!server.hasArg("plain")) { server.send(400, "text/plain", "Body vacio"); return; }

        StaticJsonDocument<64> doc;
        if (deserializeJson(doc, server.arg("plain"))) { server.send(400, "text/plain", "JSON invalido"); return; }

        int idx = doc["idx"] | -1;
        if (idx < 0 || idx > 7) { server.send(400, "text/plain", "idx invalido (0-7)"); return; }

        pasoMapeo = idx;
        tiempoInicioMapeo = millis();

        Serial.printf("[IR-APP] Esperando código para el boton %d (desde PWA)\n", idx);
        server.send(200, "text/plain", "OK");
    });

    // Estado de la captura + códigos actuales (en RAM), para que la PWA pueda hacer polling
    server.on("/ir/estado", HTTP_GET, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");

        StaticJsonDocument<256> doc;
        doc["pasoMapeo"] = pasoMapeo;

        char buf[9];
        snprintf(buf, sizeof(buf), "%08X", ir_btn_on);    doc["on"]    = buf;
        snprintf(buf, sizeof(buf), "%08X", ir_btn_off);   doc["off"]   = buf;
        snprintf(buf, sizeof(buf), "%08X", ir_btn_up);    doc["up"]    = buf;
        snprintf(buf, sizeof(buf), "%08X", ir_btn_down);  doc["down"]  = buf;
        snprintf(buf, sizeof(buf), "%08X", ir_btn_menu);  doc["menu"]  = buf;
        snprintf(buf, sizeof(buf), "%08X", ir_btn_ok);    doc["ok"]    = buf;
        snprintf(buf, sizeof(buf), "%08X", ir_btn_subir); doc["subir"] = buf;
        snprintf(buf, sizeof(buf), "%08X", ir_btn_bajar); doc["bajar"] = buf;

        String salida; serializeJson(doc, salida);
        server.send(200, "application/json", salida);
    });

    server.on("/ir/cancelar", HTTP_OPTIONS, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
        server.send(204);
    });

}

// --- DIAGNÓSTICO (solo lectura) ---
void registrarRutasDiag() {
    server.on("/diag", HTTP_GET, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Cache-Control", "no-store");

        // Mínimo observado del mayor bloque libre (se actualiza cada vez que se consulta)
        static uint32_t bloqueMin = UINT32_MAX;
        uint32_t bloque = ESP.getMaxAllocHeap();
        if (bloque < bloqueMin) bloqueMin = bloque;

        StaticJsonDocument<512> doc;
        doc["fw"]           = FIRMWARE_VERSION;
        doc["fwNum"]        = CURRENT_VERSION_NUM;
        doc["uptime"]       = millis() / 1000;
        doc["heapTotal"]    = ESP.getHeapSize();
        doc["heapFree"]     = ESP.getFreeHeap();
        doc["heapMin"]      = ESP.getMinFreeHeap();   // mínimo desde el arranque
        doc["heapBlock"]    = bloque;                 // mayor bloque contiguo libre ahora
        doc["heapBlockMin"] = bloqueMin;              // mínimo observado de ese bloque
        doc["rst"]          = (int)esp_reset_reason();
        doc["rssi"]         = WiFi.RSSI();
        doc["sdMB"]         = (uint32_t)(SD.cardSize() / (1024ULL * 1024ULL));
        doc["sdSpeed"]      = sdSpeed;
        doc["cpuMHz"]       = ESP.getCpuFreqMHz();
        doc["sketchKB"]     = ESP.getSketchSize() / 1024;
        doc["slotKB"]       = ESP.getFreeSketchSpace() / 1024;  // tamaño de la partición OTA destino

        char buf[512];                                  // en la pila: no fragmenta el heap
        serializeJson(doc, buf, sizeof(buf));
        server.send(200, "application/json", buf);
    });
}

// La PWA envía un BMP de 128x32 y 24 bits ya convertido (12.342 bytes). Aquí se escribe en un temporal,
// se valida la cabecera y solo entonces se renombra al nombre final.
#define FONDO_TMP        "/relojes/imagenes/.subida.tmp"
#define FONDO_MAX_BYTES  20480

static File   fondoUpFile;
static int    fondoUpCodigo   = 400;                // 0 = subida en curso o terminada bien
static String fondoUpMensaje  = "Sin archivo";
static char   fondoUpNombre[32] = "";
static size_t fondoUpBytes    = 0;

static void fondoUpError(int codigo, const char* mensaje) {
    fondoUpCodigo  = codigo;
    fondoUpMensaje = mensaje;
    if (fondoUpFile) fondoUpFile.close();
    SD.remove(FONDO_TMP);
}

// Nombre aceptable para SUBIR: letras, números, "_" y "-" más ".bmp" (la PWA ya lo normaliza)
static bool nombreSubidaValido(const String& n) {
    size_t len = n.length();
    if (len < 5 || len > 31 || !n.endsWith(".bmp")) return false;
    for (size_t i = 0; i < len - 4; i++) {
        char c = n[i];
        if (!(isalnum((unsigned char)c) || c == '_' || c == '-')) return false;
    }
    return true;
}

static void manejarSubidaFondo() {
    HTTPUpload& up = server.upload();

    if (up.status == UPLOAD_FILE_START) {
        fondoUpCodigo = 0; fondoUpMensaje = ""; fondoUpBytes = 0; fondoUpNombre[0] = '\0';
        if (fondoUpFile) fondoUpFile.close();

        if (!nombreSubidaValido(up.filename)) { fondoUpError(400, "Nombre no valido"); return; }
        strlcpy(fondoUpNombre, up.filename.c_str(), sizeof(fondoUpNombre));

        SD.mkdir("/relojes");
        SD.mkdir("/relojes/imagenes");

        char destino[64];
        snprintf(destino, sizeof(destino), "/relojes/imagenes/%s", fondoUpNombre);
        if (!SD.exists(destino)) {                    // un nombre nuevo cuenta para el máximo; sustituir uno existente no
            cargarNombresFondos();
            if (listaFondos.size() >= MAX_FONDOS) { fondoUpError(409, "Maximo de fondos alcanzado"); return; }
        }

        SD.remove(FONDO_TMP);
        fondoUpFile = SD.open(FONDO_TMP, FILE_WRITE);
        if (!fondoUpFile) { fondoUpError(500, "No se pudo escribir en la SD"); return; }

    } else if (up.status == UPLOAD_FILE_WRITE) {
        if (fondoUpCodigo != 0 || !fondoUpFile) return;
        fondoUpBytes += up.currentSize;
        if (fondoUpBytes > FONDO_MAX_BYTES) { fondoUpError(413, "Archivo demasiado grande"); return; }
        if (fondoUpFile.write(up.buf, up.currentSize) != up.currentSize) fondoUpError(500, "Error de escritura en la SD");

    } else if (up.status == UPLOAD_FILE_END) {
        if (fondoUpCodigo != 0 || !fondoUpFile) return;
        fondoUpFile.close();

        bool valido = false;
        File f = SD.open(FONDO_TMP, FILE_READ);
        if (f) {
            uint32_t off; int bpx; bool td;
            valido = leerCabeceraFondo(f, off, bpx, td) && f.size() >= (size_t)off + (size_t)(128 * 32 * bpx);
            f.close();
        }
        if (!valido) { fondoUpError(400, "BMP no valido (debe ser 128x32, 24 bits)"); return; }

        char destino[64];
        snprintf(destino, sizeof(destino), "/relojes/imagenes/%s", fondoUpNombre);
        SD.remove(destino);
        if (!SD.rename(FONDO_TMP, destino)) { fondoUpError(500, "No se pudo guardar el fondo"); return; }
        Serial.printf(PSTR("[FONDOS] Subido %s (%u bytes)\n"), fondoUpNombre, (unsigned)fondoUpBytes);

    } else if (up.status == UPLOAD_FILE_ABORTED) {
        fondoUpError(400, "Subida cancelada");
    }
}

static void responderPreflightFondos() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(204);
}

void registrarRutasReloj() {
    server.on("/reloj/fondos", HTTP_OPTIONS, responderPreflightFondos);
    server.on("/reloj/fondos/borrar", HTTP_OPTIONS, responderPreflightFondos);

    // Lista de fondos + selección actual. Se elige con POST /control {"clockBg":"x.bmp"} / {"clockPos":0..2}
    server.on("/reloj/fondos", HTTP_GET, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Cache-Control", "no-store");

        cargarNombresFondos();

        DynamicJsonDocument doc(2048);
        JsonArray items = doc.createNestedArray("items");
        for (auto &n : listaFondos) items.add(n);
        doc["active"] = clockBg;
        doc["pos"]    = clockPos;
        doc["max"]    = MAX_FONDOS;

        String salida;
        serializeJson(doc, salida);
        server.send(200, "application/json", salida);
    });

    // Subida: la respuesta se envía cuando ya se ha recibido el archivo (el trabajo lo hace manejarSubidaFondo)
    server.on("/reloj/fondos", HTTP_POST, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        if (fondoUpCodigo == 0) {
            if (strcmp(clockBg, fondoUpNombre) == 0) aplicarCambioFondoReloj();   // se ha sustituido el fondo en uso
            server.send(200, "text/plain", "OK");
        } else {
            server.send(fondoUpCodigo, "text/plain", fondoUpMensaje);
        }
        fondoUpCodigo = 400; fondoUpMensaje = "Sin archivo"; fondoUpNombre[0] = '\0';   // estado inicial para la siguiente petición
    }, manejarSubidaFondo);

    // Borrado: POST /reloj/fondos/borrar {"name":"x.bmp"}
    server.on("/reloj/fondos/borrar", HTTP_POST, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");

        StaticJsonDocument<128> doc;
        if (deserializeJson(doc, server.arg("plain"))) { server.send(400, "text/plain", "JSON invalido"); return; }

        const char* nombre = doc["name"] | "";
        if (nombre[0] == '\0' || !nombreFondoValido(nombre)) {      // exige un .bmp existente en /relojes/imagenes, sin rutas
            server.send(404, "text/plain", "Fondo no encontrado");
            return;
        }

        char ruta[64];
        snprintf(ruta, sizeof(ruta), "/relojes/imagenes/%s", nombre);
        if (!SD.remove(ruta)) { server.send(500, "text/plain", "No se pudo borrar"); return; }

        if (strcmp(clockBg, nombre) == 0) {                         // era el fondo en uso: el reloj pasa a "ninguno"
            clockBg[0] = '\0';
            guardarConfigIni();
            aplicarCambioFondoReloj();
        }
        Serial.printf(PSTR("[FONDOS] Borrado %s\n"), nombre);
        server.send(200, "text/plain", "OK");
    });
}