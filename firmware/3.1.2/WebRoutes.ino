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

    server.begin();
    tcpServer.begin();
    Serial.println(F("[HTTP] Servidor iniciado y Panel listo."));
}

// --- ESTADO LIGERO (no cambia nada, solo informa) ---
void registrarRutasEstado() {
    server.on("/status", HTTP_GET, [&]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        StaticJsonDocument<256> doc;
        doc["brightness"]      = brightness;
        doc["playMode"]        = modoVisual;
        doc["randomMode"]      = randomMode;
        doc["clockStyle"]      = clockStyle;
        doc["clockColorIndex"] = clockColorIndex;
        doc["textEnable"]      = textEnable ? 1 : 0;
        doc["textoActivo"]     = (estadoActual == ESTADO_TEXTO) ? 1 : 0;
        doc["isSleeping"]      = isSleeping ? 1 : 0;

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

        StaticJsonDocument<256> doc;
        if (deserializeJson(doc, server.arg("plain"))) { server.send(400, "text/plain", "JSON invalido"); return; }

        if (doc.containsKey("brightness")) {
            brightness = doc["brightness"];
            display->setBrightness8(brightness);
        }
        if (doc.containsKey("playMode"))   modoVisual = doc["playMode"];
        if (doc.containsKey("randomMode")) randomMode = doc["randomMode"];
        if (doc.containsKey("clockStyle")) clockStyle = doc["clockStyle"];
        if (doc.containsKey("clockColorIndex")) {
            clockColorIndex = doc["clockColorIndex"];
            if (clockColorIndex < 0 || clockColorIndex >= TOTAL_COLORES) clockColorIndex = 0;
            clockColor = listaColores[clockColorIndex].colorRGB;
        }

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
        doc["PLAY_MODE"]        = modoVisual;
        doc["ARCADE_ENABLE"]    = arcadeEnable;
        doc["TEXT_ENABLE"]      = textEnable ? 1 : 0;
        doc["CLOCK_ENABLE"]     = clockEnable;
        doc["RANDOM_MODE"]      = randomMode;
        doc["AUTO_CLOCK_INT"]   = autoClockInt;
        doc["CLOCK_DURATION"]   = clockDuration;
        doc["CLOCK_STYLE"]      = clockStyle;
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

        StaticJsonDocument<1024> doc;
        if (deserializeJson(doc, server.arg("plain"))) {
            server.send(400, "text/plain", "JSON invalido");
            return;
        }

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

        if (doc.containsKey("PLAY_MODE"))     modoVisual = doc["PLAY_MODE"];
        if (doc.containsKey("ARCADE_ENABLE")) arcadeEnable = doc["ARCADE_ENABLE"];
        if (doc.containsKey("TEXT_ENABLE"))   textEnable = doc["TEXT_ENABLE"].as<int>() != 0;
        if (doc.containsKey("CLOCK_ENABLE"))  clockEnable = doc["CLOCK_ENABLE"];
        if (doc.containsKey("RANDOM_MODE"))   randomMode = doc["RANDOM_MODE"];
        if (doc.containsKey("AUTO_CLOCK_INT"))autoClockInt = doc["AUTO_CLOCK_INT"];
        if (doc.containsKey("CLOCK_DURATION"))clockDuration = doc["CLOCK_DURATION"];
        if (doc.containsKey("CLOCK_STYLE"))   clockStyle = doc["CLOCK_STYLE"];
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

        guardarConfigIni();

        server.send(200, "text/plain", "OK - Reiniciando");
        Serial.println(F("[CONFIG-APP] Configuracion guardada desde la PWA. Reiniciando..."));
        delay(400);
        ESP.restart();
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
