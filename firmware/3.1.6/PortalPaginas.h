// ====================================================================
//  PortalPaginas.h - Piezas fijas del HTML del portal WiFi (en flash)
// ====================================================================
#pragma once

// Cabecera: <html lang="xx"> se completa en vivo con el idioma activo
static const char PORTAL_HTML_A[] PROGMEM = R"HTML(<!DOCTYPE html><html lang=")HTML";
static const char PORTAL_HTML_B[] PROGMEM = R"HTML("><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>RetroPixel LED</title>)HTML";

// Estilos (CSS_BASE abre <style>; FIN_CABECERA lo cierra y abre <body>)
static const char PORTAL_CSS_BASE[] PROGMEM = R"HTML(<style>body{margin:0 auto;padding:24px 18px;max-width:420px;background:#0b0b10;color:#e8e8e8;font-family:system-ui,sans-serif}h1{font-size:16px;letter-spacing:1px;color:#FFB000;text-transform:uppercase}p{font-size:12px;color:#999;line-height:1.5}a{color:#FFB000})HTML";
static const char PORTAL_CSS_FORM[] PROGMEM = R"HTML(label{display:block;font-size:11px;color:#999;margin:16px 0 6px;text-transform:uppercase;letter-spacing:.5px}input{width:100%;box-sizing:border-box;padding:12px;border-radius:8px;border:1px solid #333;background:#15151c;color:#fff;font-size:16px}button{width:100%;margin-top:22px;padding:13px;border-radius:8px;border:2px solid #FFB000;background:transparent;color:#FFB000;font-size:14px;letter-spacing:1px}button.red{display:block;margin:0 0 8px;padding:12px;text-align:left;letter-spacing:0;font-size:15px;color:#fff;background:#15151c;border:1px solid #333})HTML";
static const char PORTAL_CSS_ROJO[] PROGMEM = R"HTML(h1{color:#FF3B30})HTML";
static const char PORTAL_FIN_CABECERA[] PROGMEM = R"HTML(</style></head><body>)HTML";

// Formulario
static const char PORTAL_H1[]         PROGMEM = R"HTML(<h1>Retro Pixel LED &middot; WiFi</h1>)HTML";
static const char PORTAL_FORM_INICIO[] PROGMEM = R"HTML(<form method="POST" action="/guardar">)HTML";
static const char PORTAL_INPUT_SSID[] PROGMEM = R"HTML(<input id="ssid" name="ssid" maxlength="32" required autocapitalize="none" autocorrect="off">)HTML";
static const char PORTAL_INPUT_PASS[] PROGMEM = R"HTML(<input id="pass" name="pass" type="password" maxlength="63" autocapitalize="none" autocorrect="off">)HTML";

// Cierre de la pagina del formulario: al tocar una red se rellena el nombre
static const char PORTAL_SCRIPT_FIN[] PROGMEM = R"HTML(<script>
document.querySelectorAll('button.red').forEach(function(b){
  b.onclick=function(){
    document.getElementById('ssid').value=b.dataset.s;
    document.getElementById('pass').focus();
  };
});
</script>
</body></html>)HTML";

static const char PORTAL_FIN_BODY[] PROGMEM = R"HTML(</body></html>)HTML";
