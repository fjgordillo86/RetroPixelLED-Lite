// ====================================================================
//  PortalTextos.h - Traducciones del portal WiFi (ES, EN, FR) dentro del firmware
//
//  El idioma se elige segun LANGUAGE de config.ini (idiomaActivo):
//     ES -> espanol,  FR -> frances,  cualquier otro valor (EN incluido) -> ingles
//  No dependen de los archivos /idioma/*.json: el portal funciona aunque la SD
//  no tenga idiomas descargados. El JSON queda solo para los menus OSD.
//
//  REGLAS
//   - PT_P_*  (panel):  solo ASCII (sin acentos) y maximo 21 caracteres por linea.
//                       Los que se usan como titulo (PT_P_TITULO, PT_P_NO_CONECTA,
//                       PT_P_PROBANDO, PT_P_OK) maximo 19.
//   - PT_W_*  (web):    UTF-8 libre (acentos, ñ, etc.). No admite HTML.
//   - El ORDEN de cada columna debe coincidir con el del enum PortalTexto.
//
//  PARA ANADIR UN IDIOMA: nueva constante PORTAL_IDIOMA_xx, subir PORTAL_NUM_IDIOMAS,
//  una entrada mas en PORTAL_LANG, una fila nueva en PORTAL_TXT y un caso nuevo en
//  portalElegirIdioma() (PortalWiFi.ino).
// ====================================================================
#pragma once

#define PORTAL_IDIOMA_ES   0
#define PORTAL_IDIOMA_EN   1
#define PORTAL_IDIOMA_FR   2
#define PORTAL_NUM_IDIOMAS 3

// Para <html lang="">
static const char* const PORTAL_LANG[PORTAL_NUM_IDIOMAS] = { "es", "en", "fr" };

enum PortalTexto {
    // --- Panel ---
    PT_P_TITULO,
    PT_P_PROBANDO,
    PT_P_OK,
    PT_P_NO_CONECTA,
    PT_P_REVISA1,
    PT_P_REVISA2,
    PT_P_REINTENTA,
    PT_P_REINICIANDO,
    // --- Web ---
    PT_W_INSTRUCCION,
    PT_W_REDES,
    PT_W_SIN_REDES,
    PT_W_RED,
    PT_W_PASS,
    PT_W_CONECTAR,
    PT_W_AVISO_24,
    PT_W_CANCELAR,
    PT_W_ESPERA_TITULO,
    PT_W_ESPERA_1,
    PT_W_ESPERA_2,
    PT_W_ERROR_TITULO,
    PT_W_ERROR_TXT,
    PT_W_VOLVER,
    PT_W_CANCELADO_TITULO,
    PT_W_CANCELADO_TXT,
    PORTAL_NUM_TEXTOS
};

static const char* const PORTAL_TXT[PORTAL_NUM_IDIOMAS][PORTAL_NUM_TEXTOS] = {
    // ------------------------------------------------ ES
    {
        "CONFIGURACION WIFI",                       // PT_P_TITULO
        "PROBANDO RED...",                          // PT_P_PROBANDO
        "WIFI OK",                                  // PT_P_OK
        "NO CONECTA",                               // PT_P_NO_CONECTA
        "Revisa la clave y",                        // PT_P_REVISA1
        "el nombre de la red",                      // PT_P_REVISA2
        "Reintenta",                                // PT_P_REINTENTA
        "Reiniciando...",                           // PT_P_REINICIANDO
        "Toca tu red WiFi (o escríbela a mano) e introduce su contraseña.",   // PT_W_INSTRUCCION
        "Redes encontradas",                        // PT_W_REDES
        "No se han encontrado redes. Escribe el nombre a mano.",              // PT_W_SIN_REDES
        "Red WiFi",                                 // PT_W_RED
        "Contraseña",                               // PT_W_PASS
        "CONECTAR",                                 // PT_W_CONECTAR
        "Retro Pixel LED solo funciona con redes de 2,4 GHz.",                // PT_W_AVISO_24
        "Cancelar",                                 // PT_W_CANCELAR
        "Comprobando la red...",                    // PT_W_ESPERA_TITULO
        "El panel está probando la conexión. Mira su pantalla: si conecta, mostrará su IP y se reiniciará.",  // PT_W_ESPERA_1
        "Si falla, vuelve a abrir esta página y revisa el nombre de la red y la contraseña.",                 // PT_W_ESPERA_2
        "Datos no válidos",                         // PT_W_ERROR_TITULO
        "El nombre de la red debe tener entre 1 y 32 caracteres, y la contraseña, entre 8 y 63 (o estar vacía si la red es abierta).",  // PT_W_ERROR_TXT
        "Volver",                                   // PT_W_VOLVER
        "Cancelado",                                // PT_W_CANCELADO_TITULO
        "El panel se reinicia con la configuración anterior."                 // PT_W_CANCELADO_TXT
    },
    // ------------------------------------------------ EN
    {
        "WIFI SETUP",
        "TESTING NETWORK...",
        "WIFI OK",
        "CANNOT CONNECT",
        "Check the password",
        "and the network name",
        "Try again",
        "Restarting...",
        "Tap your WiFi network (or type it in) and enter its password.",
        "Networks found",
        "No networks found. Type the name manually.",
        "WiFi network",
        "Password",
        "CONNECT",
        "Retro Pixel LED only works with 2.4 GHz networks.",
        "Cancel",
        "Checking the network...",
        "The panel is testing the connection. Look at its screen: if it connects, it will show its IP and restart.",
        "If it fails, open this page again and check the network name and password.",
        "Invalid data",
        "The network name must be 1 to 32 characters long, and the password 8 to 63 characters (or empty for an open network).",
        "Back",
        "Cancelled",
        "The panel is restarting with the previous settings."
    },
    // ------------------------------------------------ FR
    {
        "CONFIGURATION WIFI",
        "TEST DU RESEAU...",
        "WIFI OK",
        "ECHEC CONNEXION",
        "Verifiez le mot de",
        "passe et le nom WiFi",
        "Reessayez",
        "Redemarrage...",
        "Touchez votre réseau WiFi (ou saisissez-le à la main) et entrez son mot de passe.",
        "Réseaux trouvés",
        "Aucun réseau trouvé. Saisissez le nom à la main.",
        "Réseau WiFi",
        "Mot de passe",
        "SE CONNECTER",
        "Retro Pixel LED ne fonctionne qu'avec des réseaux 2,4 GHz.",
        "Annuler",
        "Vérification du réseau...",
        "Le panneau teste la connexion. Regardez son écran : en cas de succès, il affichera son IP et redémarrera.",
        "En cas d'échec, rouvrez cette page et vérifiez le nom du réseau et le mot de passe.",
        "Données invalides",
        "Le nom du réseau doit contenir de 1 à 32 caractères, et le mot de passe de 8 à 63 (ou être vide si le réseau est ouvert).",
        "Retour",
        "Annulé",
        "Le panneau redémarre avec la configuration précédente."
    }
};
