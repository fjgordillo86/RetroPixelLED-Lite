// ====================================================================
//  RELOJ DINÁMICO — bucle (pestaña nueva del sketch: RelojDinamico.ino)
//
//  Sigue el mismo "contrato" que mostrarRelojLite(): se queda en el bucle
//  clockDuration segundos atendiendo botón, IR, servidor web y marquesina
//  TCP, y sale si se abre el menú, entra el temporizador de sueño o alguien
//  pide interrumpir. Así se reutilizan tal cual todos los sitios que ya
//  llaman al reloj (modo solo reloj, reloj cada N GIFs, etc.).
// ====================================================================

static unsigned long rdSalidaMs  = 0;   // cuándo salimos la última vez
static int           rdCaraVista = -1;  // qué cara se estaba mostrando
static int           rdRotIdx    = -1;  // cara actual en modo "rotar"
static unsigned long rdRotDesde  = 0;   // cuándo se eligió

// relojDinamico: 0 = desactivado (reloj Lite), 1..RD_NUM_CARAS = cara + 1
void mostrarRelojDinamico(bool conTransicion) {
    (void)conTransicion;   // la transición de partículas es del reloj Lite
    if (!display) return;
    if (arcadeEnable > 0 && estadoActual == ESTADO_ARCADE) return;

    int idx = relojDinamico - 1;
    const int modoEntrada = dynModo;
    if (dynModo != 0 && RD_NUM_CARAS > 1) {
        const bool reentrada = (rdCaraVista >= 0) && (millis() - rdSalidaMs < 1500UL);
        if (dynModo == 1) {                                  // aleatoria: una distinta cada vez que aparece el reloj
            if (reentrada) idx = rdCaraVista;
            else { int nueva; do { nueva = random(RD_NUM_CARAS); } while (nueva == rdCaraVista); idx = nueva; }
        } else {                                             // rotar: la siguiente cara cada dynRotMin minutos
            if (rdRotIdx < 0) { rdRotIdx = idx; rdRotDesde = millis(); }
            if (millis() - rdRotDesde >= (unsigned long)dynRotMin * 60000UL) { rdRotIdx = (rdRotIdx + 1) % RD_NUM_CARAS; rdRotDesde = millis(); }
            idx = rdRotIdx;
        }
    }
    if (idx < 0 || idx >= RD_NUM_CARAS) idx = 0;
    interrumpirReproduccion = false;

    struct tm t;
    if (!getLocalTime(&t)) return;

    // Con DOUBLE_BUFF=1 el motor repinta la escena completa (2 frames tras
    // cada cambio). Misma condición que usa setup() para activar el buffer doble.
    const bool doble = (doubleBuff == 1) && (arcadeEnable == 0) && (transitionEnable != 1);

    // En modo "solo reloj" el bucle principal vuelve a llamarnos cada
    // clockDuration segundos: si es una re-entrada inmediata y la pantalla
    // sigue siendo nuestra, NO reiniciamos la escena (el fontanero no salta).
    const bool continuar = (rdCaraVista == idx) && (millis() - rdSalidaMs < 1500UL);
    if (!continuar) {
        rdIniciar(idx, t.tm_hour, t.tm_min, t.tm_sec, offset, PANEL_RES_X * panelChain, doble);
        display->flipDMABuffer();
        rdCaraVista = idx;
    }

    unsigned long startTime = millis();
    while (millis() - startTime < (clockDuration * 1000UL) && !interrumpirReproduccion) {

        gestionarBotonMenu();
        leerControlRemoto();
        server.handleClient();
        verificarMarquesinaTCP();

        if (isSleeping || estadoActual != ESTADO_GIFS) {
            interrumpirReproduccion = true;
            rdCaraVista = -1;               // el menú/otro modo pisa la pantalla
            break;
        }

        // Si desde la PWA/OSD cambian de cara (o la desactivan), salimos ya
        // y el bucle principal volverá a entrar con la nueva configuración.
        if (relojDinamico == 0 || dynModo != modoEntrada || (dynModo == 0 && relojDinamico != idx + 1)) { rdCaraVista = -1; break; }
        if (dynModo == 2 && (millis() - rdRotDesde) >= (unsigned long)dynRotMin * 60000UL) break;   // toca rotar a la siguiente cara

        if (!getLocalTime(&t)) break;

        if (rdActualizar(t.tm_hour, t.tm_min, t.tm_sec, millis()))
            display->flipDMABuffer();

        delay(10);
    }
    rdSalidaMs = millis();
}
