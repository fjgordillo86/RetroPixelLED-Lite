# ✨ Retro Pixel LED Lite

<p align="center">
  <img alt="Versión" src="https://img.shields.io/badge/versión-3.1.5-blue">
  <img alt="Plataforma" src="https://img.shields.io/badge/plataforma-ESP32-informational">
  <img alt="Licencia" src="https://img.shields.io/badge/licencia-MIT-green">
  <img alt="Estado" src="https://img.shields.io/badge/estado-activo-success">
</p>

<p align="center">
  <a href="https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README.md">🇪🇸 Español</a> ·
  <a href="https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/Frances/README.md">🇫🇷 Français</a> ·
  <a href="https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/Ingles/README.md">🇬🇧 English</a> ·
  <a href="https://t.me/RetroPixelLed">✈️ Grupo de Telegram</a>
</p>

<p align="center">
  <a href="https://paypal.me/fjgordillo"><img alt="Donar con PayPal" src="https://img.shields.io/badge/☕_Invítame_a_un_café-PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white"></a>
</p>

> 💛 Si Retro Pixel LED te está alegrando el rincón retro de casa, con el botón de arriba puedes invitarme a un café. ¡Todo ayuda a seguir sacando cosas nuevas!

---

## 💡 Descripción del proyecto

**Retro Pixel LED Lite** es la versión de alto rendimiento de Retro Pixel LED, pensada para quienes buscan estabilidad absoluta, velocidad instantánea y un sistema libre de mantenimiento. A diferencia de la versión estándar, el firmware **LITE** se quita de encima el servidor web y la conectividad permanente para dedicar el 100% de la potencia del ESP32 a hacer una sola cosa muy bien: renderizar GIFs.

Si la rama 2.x.x trajo el Menú OSD, la **v3.0.0** fue el salto definitivo hacia la independencia del hardware: el panel se convirtió en un dispositivo inteligente autónomo, sin necesidad de conectarlo al ordenador para mantenimiento ni configuración. `config.ini` y las playlists se editan directamente desde el Explorador de Windows o un cliente FTP, convirtiendo la tarjeta SD en una unidad de red inalámbrica. También trae soporte nativo para mando a distancia: navega por el Menú OSD, ajusta el brillo y enciende/apaga desde el sofá.

**Desde la v3.1.0 controlas el panel desde una app (PWA)**, y desde la **v3.1.2** también desde **Home Assistant**. 🏠

**Desde la v3.1.5 ya no hace falta editar ningún archivo para conectarlo a tu WiFi:** el panel abre su propio portal de configuración desde el móvil, y después lo encuentras siempre en `retropixel.local`. 📶

---

## 📑 Índice

1. [🆕 Novedades de la versión actual](#-novedades-de-la-versión-v315-lite)
2. [🚀 Guía rápida](#-guía-rápida-primeros-pasos)
3. [🎛️ Funciones principales](#️-funciones-principales)
   - [🖥️ Menú OSD](#️-menú-osd-navegación-inteligente)
   - [📱 PWA — App de control remoto](#-pwa--app-de-control-remoto)
   - [🏠 Home Assistant](#-home-assistant)
   - [🕹️ Modo Arcade](#️-modo-arcade-batocera-recalbox-retrobat--replayos)
   - [🕒 Reloj y Clima](#-reloj-y-clima)
   - [⏰ Temporizador](#-temporizador)
   - [🌐 Multi-idioma](#-multi-idioma)
   - [📂 Servidor FTP](#-servidor-ftp)
   - [🔄 Actualización OTA](#-actualización-ota)
   - [📶 Portal WiFi y acceso por nombre](#-portal-wifi-y-acceso-por-nombre-mdns)
4. [⚙️ Instalación y configuración](#️-instalación-y-configuración)
   - [1. Programar el ESP32](#1--programar-el-esp32-web-installer)
   - [2. Preparar la tarjeta SD](#2--preparación-de-la-tarjeta-sd)
   - [3. El archivo `config.ini`](#3--configuración-vía-configini)
   - [4. Zona horaria (TZ)](#4--configuración-de-zona-horaria-tz)
   - [5. API key del clima](#5-%EF%B8%8F-c%C3%B3mo-obtener-tu-api-key-de-clima)
5. [📖 Generador de playlists (Windows)](#-generador-de-playlists-windows)
6. [🕹️ Integración con Arcade](#️-integración-con-arcade-batocera-recalbox-retrobat-o-replayos)
7. [🏠 Integración con Home Assistant](#-integración-con-home-assistant-guía-completa)
8. [🧠 Arquitectura interna / Core Lite](#-arquitectura-interna--core-lite)
9. [📜 Historial de cambios detallado](#-historial-de-cambios-detallado-v300--v315)
10. [🛒 Lista de materiales](#-lista-de-materiales)
11. [🔌 Conexiones (Pinout)](#-conexiones)
12. [🛠️ Roadmap](#️-roadmap)
13. [⚖️ Licencia y agradecimientos](#️-licencia-y-agradecimientos)

---

## 🆕 Novedades de la versión v3.1.5 Lite

- **📶 Portal WiFi integrado:** conecta el panel a tu red desde el móvil, sin editar `config.ini`. Se abre desde `Menú OSD → WiFi → Añadir red`, o solo en el primer arranque si no hay red configurada. El panel muestra su red y una clave aleatoria, eliges tu WiFi de una lista y él mismo comprueba la conexión antes de guardarla. Textos en español, inglés y francés. [Ver cómo funciona](#-portal-wifi-y-acceso-por-nombre-mdns).
- **🏷️ Acceso por nombre (mDNS):** el panel responde en `retropixel.local`, sin depender de la IP. La PWA recuerda la última IP y te ofrece volver a ella si el nombre no se resuelve en tu dispositivo.
- **🔔 Aviso de actualización en la PWA:** un punto ámbar sobre el icono 🔄 te avisa cuando hay una versión nueva, con su número y las novedades.
- **ℹ️ Nueva página "Acerca de" en la PWA:** versiones de la PWA y del firmware, creador, licencia y **diagnóstico en vivo** del panel (memoria, último reinicio, WiFi, SD...), con botón para copiar el informe.
- **🩺 Diagnóstico `/diag`:** consulta rápida del estado interno desde el navegador (`http://retropixel.local/diag`).
- **🔐 Contraseñas WiFi con `#`:** corregido un fallo de lectura de `config.ini` que cortaba el SSID o la contraseña a partir de un `#`.
- **🛡️ PWA más fiable al actualizarse:** el service worker lleva ahora una versión única, instala aunque falle algún archivo y deja de interceptar las peticiones al panel.

> ⚠️ **Al actualizar desde versiones anteriores a la v3.1.4:** desde esa versión se usa el esquema de particiones *Minimal SPIFFS* (1,9 MB para la app con OTA) y el firmware ya no cabe en el espacio antiguo. No se puede instalar por OTA: reinstálalo desde el instalador web. Se borran los datos guardados en la memoria interna (playlist activa, ajustes del temporizador); la configuración y los GIFs de la SD se conservan. Si ya tienes la v3.1.4, puedes pasar a la v3.1.5 por OTA.

Para el detalle de versiones anteriores (marquesinas GIF en Arcade, reconexión WiFi, IP en el menú OSD...) consulta el [Historial de cambios](#-historial-de-cambios-detallado-v300--v315).

---

## 🚀 Guía rápida (primeros pasos)

Si es la primera vez que instalas Retro Pixel LED Lite, este es el camino más corto:

1. **Flashea el firmware** con el [instalador web](#1--programar-el-esp32-web-installer) — solo necesitas Chrome o Edge, nada que instalar en el PC.
2. **Prepara la MicroSD** en FAT32 con el [contenido de la carpeta `Contenido SD`](#2--preparación-de-la-tarjeta-sd).
3. **Revisa `config.ini`** con tus preferencias (zona horaria, ciudad del clima...) — [referencia completa](#3--configuración-vía-configini). El WiFi puedes dejarlo vacío: el panel te lo pedirá desde el móvil en el primer arranque.
4. **Enciende el panel.** Si no tiene red configurada, abre el [portal WiFi](#-portal-wifi-y-acceso-por-nombre-mdns): conecta el móvil a la red que muestra la pantalla y elige tu WiFi. Después sincronizará la hora, cargará tus GIFs y estará listo. ✨
5. *(Opcional)* Instala la [PWA](#-pwa--app-de-control-remoto) para controlarlo desde el móvil, o [intégralo con Home Assistant](#-integración-con-home-assistant-guía-completa) si usas domótica.
6. *(Opcional)* Si tienes Batocera, Recalbox, RetroBat o ReplayOS, sigue la [guía de integración Arcade](#️-integración-con-arcade-batocera-recalbox-retrobat-o-replayos) para marquesinas dinámicas.

El resto de este documento es la referencia detallada de cada función — no hace falta leerlo entero para empezar. 🙂

---

## 🎛️ Funciones principales

Esta sección resume **qué hace** cada parte del sistema. Para el paso a paso de instalación, ve a [⚙️ Instalación y configuración](#️-instalación-y-configuración).

### 🖥️ Menú OSD (navegación inteligente)

El sistema se controla mediante un **único botón** (o el mando IR), con una lógica de pulsación que se adapta según el menú:

- **Pulsación rápida:**
  - **En menús:** mover el cursor / navegar hacia abajo.
  - **En modo sueño:** despierta el panel de forma inmediata (wake-up).
- **Pulsación mantenida:**
  - **Acción general:** entrar en submenús o confirmar selección.
  - **En configuración de tiempo (temporizador):** resta **-5 minutos** al valor actual.
- **Pulsación extra larga (> 4 seg):**
  - **Manual override:** fuerza el apagado (modo sueño), bloqueando el temporizador hasta el próximo ciclo.
- **Mantener pulsación continua:**
  - **En configuración de tiempo (temporizador):** incrementa automáticamente **+5 minutos** en bucle mientras mantienes pulsado.

```text
🏠 MENÚ PRINCIPAL
├── 📂 Playlists
│   ├── 📄 Favoritos
│   ├── 📄 Arcade
│   ├── 📄 ...
│   └── 🔙 Volver
├── 📂 Reproducción
│   ├── 🖼️ Modo: [GIFs / Reloj]
│   ├── 🔀 Aleatorio: [SI / NO]
│   ├── 🕹️ Arcade: [OFF / Batocera / Recalbox / ReplayOS/ RetroBat]
│   ├── 💬 Texto: [SI / NO]
│   └── 🔙 Volver
├── ☀️ Brillo
│   └── Brillo: [5% - 100%]
├── 📶 WiFi: [ON / OFF]
│   ├── 🔄 Activar: [SI / NO]
│   ├── ➕ Añadir red Wifi
│   ├── 📱 Control APP: [SI / NO]
│   ├── 🏷️ IP: [192.168.1.117]
│   ├── 🔎 Mostrar IP: [SI / NO]
│   └── 🔙 Volver
├── 🕒 Reloj: [ON / OFF]
│   ├── 🔄 Activar: [SI / NO]
│   ├── 🖼️ Cada: [1...20] GIFs
│   ├── ⏳ Ver: [5...30] seg
│   ├── 🎨 Estilo Reloj: [Matrix, Solid, Rainbow, Pulse, Gradient]
│   ├── 🎨 Color: [Blanco, Rojo, Verde, Azul, Amarillo, Cian, Magenta, Naranja, Rosa]
│   ├── 🔄 Transición: [SI / NO]
│   └── 🔙 Volver
├── 🌡️ Clima: [ON / OFF]
│   ├── 🔄 Activar: [SI / NO]
│   └── 🔙 Volver
├── 🕒 Temporizador: [ON / OFF]
│   ├── 🔄 Activar: [SI / NO]
│   ├── ⏳ ON: [00:00 a 24:00]
│   ├── ⏳ OFF: [00:00 a 24:00]
│   └── 🔙 Volver
├── ⚙️ Ajustes avanzados
│   ├── ⚡ I2S Speed: [8, 10, 16, 20MHz]
│   ├── 🔄 Refresco: [30, 60, 90, 120Hz]
│   ├── 🖼️ Buffer: [SI / NO]
│   ├── 👻 AntiGhost: [1, 2, 3, 4]
│   ├── 🚀 Lectura SD: [4, 10, 20, 25MHz]
│   ├── 🎮 Mapeado mando IR: [On, Off, Menu, Validar, Subir, Bajar, Brillo+, Brillo-]
│   ├── ⚠️ Reset
│   └── 🔙 Volver
├── 🚀 Actualización
│   ├── 🔄 Buscar OTA
│   ├── 🔤 Descargar idiomas
│   └── 🔙 Volver
├── 📂 Explorador SD
│   ├── 🔄 Iniciar FTP
│   └── 🔙 Volver
├── 🌐 Idioma
│   ├── [ES] Español
│   ├── [EN] English
│   ├── [FR] Français
│   ├── ...
│   └── 🔙 Volver
├── 💾 Guardar
└── 🔙 Salir
```

---

### 📱 PWA — App de control remoto

**[👉 Instalar o probar Retro Pixel LED Control](https://fjgordillo86.github.io/RetroPixelLED-Lite/control/)**

App web moderna, instalable en cualquier dispositivo (móvil, tablet, ordenador) conectado a la misma red que el panel. No requiere servidor externo, funciona en la red local y es accesible offline una vez instalada. 📴

https://github.com/user-attachments/assets/f5231448-7862-4476-901e-ac25ac7f4248

La interfaz se divide en **6 secciones**:

**1️⃣ Página principal (Home)**
- **☀️ Control de brillo:** slider 0-100% con aplicación instantánea, sin reinicio.
- **🎛️ Selector de modo:** GIF, Reloj o Texto en tiempo real.
  - **Modo GIF:** playlist activa + toggle de reproducción aleatoria.
  - **Modo Reloj:** 5 estilos (Matrix, Solid, Rainbow, Pulse, Gradient) y 9 colores.
  - **Modo Texto:** vista previa en vivo de la matriz mientras escribes, con color, fuente y velocidad de scroll.
- **🔌 Estado de conexión:** indicador visual (verde/rojo) del WiFi.

**2️⃣ Temporizador ⏰**
- Activar/desactivar con un toggle.
- Hora de encendido y de apagado (formato 24h).
- Botón de encendido/apagado manual inmediato (override).
- Estado actual: encendido (✓ verde) o dormido (● gris).

**3️⃣ Modo texto**
- Vista previa de matriz 26×7 en tiempo real mientras escribes.
- Selector de color: paleta de 9 colores + selector hex personalizado.
- **Selector de fuente:** `Bold`, `SemiBold`, `Regular`, `Light` — *(nuevo en v3.1.2)*.
- Control de velocidad: slider 5-200ms/paso con preview en vivo.
- Botones "▶ Enviar" y "■ Stop".

**4️⃣ Actualización 🔄**
- **OTA de firmware:** comprueba GitHub y actualiza automáticamente si hay versión nueva.
- **Aviso de nueva versión:** un punto ámbar sobre el icono 🔄 indica que hay una versión nueva; al abrir el modal ves cuál es y sus novedades. *(nuevo en v3.1.5)*
- **Descarga de idiomas:** trae los `.json` desde GitHub a `/idiomas` de la SD.

**5️⃣ Ajustes 🛠**

Edición remota de `config.ini`, organizada en 7 secciones: WiFi (SSID, contraseña, mostrar IP al iniciar, zona horaria), Hardware (nº de paneles, orden de color RGB/RBG/GBR, brillo, velocidad I2S, refresco, buffering, anti-ghosting), Arcade (Batocera, Recalbox, ReplayOS o ninguno), Texto deslizante, Reloj (activo, transición con partículas, intervalo, duración, estilo, color), Clima (activar, ciudad, API key, intervalo, texto sobre el reloj) e Idioma (ES, EN, FR...).

Reinicio automático tras guardar, solo si el cambio lo requiere.

**6️⃣ Acerca de ℹ️** *(nuevo en v3.1.5)*

Se abre desde el enlace "ℹ Acerca de" al pie de la pantalla principal.
- **Versiones:** de la PWA y del firmware del panel, con aviso si hay una versión nueva.
- **Créditos:** creador (`fjgordillo86`), licencia MIT y enlace al código.
- **Diagnóstico en vivo:** memoria libre, último reinicio, señal WiFi, SD, CPU y uso de la partición de firmware, con botón para **copiar el informe** y adjuntarlo al reportar un problema.

#### ⚙️ Instalación y configuración de la PWA

1. Abre <https://fjgordillo86.github.io/RetroPixelLED-Lite/control/> desde tu dispositivo.
2. Toca el icono de conexión (⚙) y escribe el nombre del panel (`retropixel.local`) o su IP local (ej. `192.168.1.117`). Si el nombre no se encuentra en tu dispositivo, la PWA te ofrece usar la última IP conocida.
3. *(Opcional)* Instálala como app: Chrome/Edge suele ofrecerlo solo; si no, menú (⋮) → "Instalar aplicación".
4. Listo — aparece como una app normal, sin escribir URLs cada vez. 🎉

#### 📝 Requisitos

- El panel debe estar en la misma red WiFi que tu dispositivo.
- `CONFI_APP_ENABLE=1` en el panel, para poder controlarlo desde la app.
- `TEXT_ENABLE=1` para que funcione el envío de mensajes.
- Conexión a internet en el panel para OTA e idiomas (salida a GitHub).
- Los idiomas se descargan una vez; luego funcionan sin conexión.

---

### 🏠 Home Assistant

Control completo desde tu dashboard de Home Assistant, vía **API REST local** — sin nube, sin depender de internet. Permite:
- 🟢 **Encender / apagar** el panel con un `switch`.
- 📊 **Consultar el estado actual** (modo activo, playlist en reproducción, etc.).
- 🔄 **Cambiar de modo** (Reloj / GIF) y de **playlist** al instante.
- 💬 **Enviar texto en scroll** eligiendo color, velocidad y **fuente** desde el dashboard.

Ver la [guía completa de integración con Home Assistant](#-integración-con-home-assistant-guía-completa) más abajo, con el archivo `.yaml` listo para copiar.

---

### 🕹️ Modo Arcade (Batocera, Recalbox, RetroBat & ReplayOS)

Convierte el panel en una marquesina dinámica que reacciona a lo que estás jugando, con dos vías de sincronización: scripts locales (**Batocera / Recalbox / RetroBat**) o monitorización nativa por red (**ReplayOS**).

Muestra automáticamente:
1. **Marquesina del juego:** imagen `.bmp` de 24 bits, o un **GIF animado** si existe uno para ese juego — incluyendo secuencias de varios GIFs reproducidos uno tras otro en bucle.
2. **Logo del sistema:** imagen `.bmp` mientras navegas por los sistemas.

Se activa desde `Menú → Reproducción → Arcade`. La guía de instalación completa está en la [sección de integración con Arcade](#️-integración-con-arcade-batocera-recalbox-o-replayos).

---

### 🕒 Reloj y Clima

- **Reloj:** 5 estilos (Matrix, Solid, Rainbow, Pulse, Gradient) y 9 colores, con transición de partículas al aparecer/desaparecer. Se interrumpe la galería de GIFs cada *x* GIFs para mostrarlo *x* segundos (configurable), retomando la reproducción justo donde se quedó.
- **Clima:** con una API key gratuita de OpenWeatherMap, muestra temperatura e icono del tiempo sobre el reloj, junto a un mensaje personalizable (`WEATHER_MSG`). Ver [cómo obtener tu API key](#5--cómo-obtener-tu-api-key-de-clima).

---

### ⏰ Temporizador

Encendido/apagado programado por horario, con override manual desde el botón físico o la PWA. Un botón anula el automatismo hasta el próximo ciclo programado.

---

### 🌐 Multi-idioma

Sistema de **diccionarios dinámicos**: el idioma NO reside en RAM constantemente, solo se carga al entrar en el menú y se libera al salir — el motor de GIFs conserva toda la memoria disponible.

- **Ubicación:** `/idioma/` en la SD. El nombre del archivo (sin extensión) es lo que aparece en el menú: `/idioma/ES.json` → "ES".
- **Estructura del JSON:** bloques `MENU`, `SUBMENU_XXX`, `ESTADOS`, `CONFIG_INI`.
- **Reglas críticas:**
  - 🚫 Sin acentos ni Ñ (usa `n` en vez de `ñ`, sin tildes).
  - 📏 Máximo 21 caracteres en etiquetas de submenú, para el centrado en 128px.
  - 🔡 Incluye los dos puntos y el espacio si quieres que aparezcan (ej: `"modo": "Modo: "`).
  - 💾 Guarda en UTF-8 sin BOM.
- **Descarga remota:** desde la PWA o el menú OSD (`Actualización → Descargar idiomas`), directo desde GitHub, sin SD extraíble.
- **Portal WiFi:** sus textos (panel y web) van dentro del firmware en ES, EN y FR, según `LANGUAGE`; cualquier otro idioma muestra inglés. No usan estos archivos.
- **Claves nuevas en v3.1.5:** el bloque `SUBMENU_WIFI` incluye `anadir_red`, `portal_abriendo` y `portal_reinicio`. Si mantienes un idioma propio, añádelas; mientras falten, el menú muestra el texto en español.

---

### 📂 Servidor FTP

Servidor de archivos inalámbrico para mantenimiento sin extraer la MicroSD.

> [!IMPORTANT]
> Pensado para **`config.ini`**, archivos de idioma (`.json`) y **playlists** (`.txt`) — no para transferir colecciones enteras de GIFs, sería muy lento frente a un lector de tarjetas.

**Activación:** `Menú OSD → Explorador SD → Iniciar FTP`. El panel detiene los GIFs y muestra su IP.

**Cliente recomendado — FileZilla:**

| Parámetro | Valor |
| :--- | :--- |
| Protocolo | FTP plano (sin cifrado) |
| Servidor/Host | IP que muestra el panel |
| Usuario / Contraseña | `admin` / `admin` |
| Puerto | `21` |
| Conexiones simultáneas | 1 (limitar activado) |
| Límite de descarga/carga | 20 KiB/s |

También puedes montarlo como unidad de red en el Explorador de Windows (`ftp://<IP>`, usuario `admin`) — aunque en pruebas ha dado algún fallo de archivos incompletos, se recomienda FileZilla.

**Notas de seguridad:** el panel no reproduce GIFs mientras el FTP está activo (toda la CPU va a la transferencia). Para salir, botón físico o tecla "Validar" del mando. No desconectes la alimentación mientras editas un archivo por FTP.

Ver el [paso a paso completo con capturas](#-explorador-sd-ftp--detalle) más abajo.

---

### 🔄 Actualización OTA

Sin necesidad de conectar el panel al PC:
1. WiFi configurado y activo (en `config.ini` o con el [portal WiFi](#-portal-wifi-y-acceso-por-nombre-mdns)).
2. La PWA te avisa cuando hay una versión nueva: punto ámbar sobre 🔄, con la versión disponible y sus novedades.
3. `Menú OSD → Actualización → Buscar OTA` (o el botón de la PWA).
4. El sistema descarga el firmware desde GitHub y se reinicia solo. 🔃

> [!WARNING]
> No desconectes la alimentación durante la actualización.

---

### 📶 Portal WiFi y acceso por nombre (mDNS)

Conecta el panel a tu red **sin editar `config.ini`** ni sacar la MicroSD, y encuéntralo siempre en `retropixel.local`.

**Cuándo se abre el portal:**
- `Menú OSD → WiFi → Añadir red` — para cambiar de router o de contraseña cuando quieras.
- **Automáticamente** al encender, si `WIFI_ENABLE=1` y `SSID` está vacío en `config.ini` (primera instalación).

**Paso a paso:**
1. El panel muestra su propia red: `WiFi: RetroPixel-LED`, `Pass:` (8 dígitos aleatorios, distintos cada vez) e `IP: 192.168.4.1`.
2. Conecta el móvil a esa red. La página de configuración se abre sola; si no, entra en `192.168.4.1`.
3. Toca tu red en la lista (o escribe el nombre si está oculta), introduce la contraseña y pulsa **Conectar**.
4. El panel prueba la conexión (hasta 15 s). Si conecta, la guarda en `config.ini`, muestra su IP y `retropixel.local`, y se reinicia. Si falla, muestra `NO CONECTA` y puedes intentarlo de nuevo.

**Para salir sin guardar:** botón **Cancelar** de la página, tecla **OK** del mando IR o el botón físico. Si nadie lo usa, el portal se cierra solo a los 5 minutos.

> [!NOTE]
> - Necesita la tarjeta SD, porque la red se guarda en `config.ini`.
> - El ESP32 solo funciona con redes de **2,4 GHz**.
> - Los textos salen en español o francés si `LANGUAGE` es `ES` o `FR`; con cualquier otro idioma, en inglés.
> - Para ahorrar memoria, el panel arranca sin *Double Buffer* mientras el portal está abierto. Tu ajuste se conserva.

**Acceso por nombre (mDNS):** con el WiFi activo, el panel responde en **`http://retropixel.local`**. En la PWA escribe `retropixel.local` en lugar de la IP; si tu dispositivo no lo resuelve, la PWA te ofrece volver a la última IP conocida.

> [!TIP]
> `.local` funciona en Windows 10/11, macOS, iOS y Linux. Algunos Android y ciertas redes (invitados, VLAN, routers que filtran multicast) no lo resuelven: en ese caso usa la IP.

**Diagnóstico:** `http://retropixel.local/diag` devuelve un JSON con la memoria libre y mínima, el motivo del último reinicio, la señal WiFi, la SD y el uso de la partición de firmware. Es lo mismo que muestra la página **Acerca de** de la PWA.

---

## ⚙️ Instalación y configuración

### 1. 🚀 Programar el ESP32 (Web Installer)

Instala esta versión sin instalar nada en tu PC:

**[👉 Abrir el instalador web de Retro Pixel LED Lite](https://fjgordillo86.github.io/RetroPixelLED-Lite/)**

1. Usa un navegador compatible (**Google Chrome** o **Microsoft Edge**).
2. Conecta tu ESP32 por USB.
3. Pulsa **"Install"** y selecciona el puerto COM.
4. **Importante:** marca **"Erase device"** para una limpieza completa de memoria y evitar errores de fragmentación.

> 💡 **¿No reconoce tu ESP32?** Si no aparece ningún puerto COM, instala los drivers del chip USB de tu placa:
> - **Chip CP2102:** [drivers Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
> - **Chip CH340/CH341:** [drivers SparkFun](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all)

### 2. 📂 Preparación de la tarjeta SD

Formatea tu MicroSD en **FAT32** y añade todo el contenido de la carpeta [Contenido SD](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD) en la raíz:

```text
/ (Raíz de la SD)
├── gifs/                        <-- Tus carpetas con GIFs (Arcade, Consolas, etc.)
├── idioma/                      <-- Archivos .json con los textos traducidos.
│   ├── ES.json
│   ├── EN.json
│   └── FR.json
├── playlists/                   <-- Listas generadas por el script "Generador de Playlists".
│   ├── Arcade.txt
│   ├── Computers.txt
│   ├── Consolas.txt
│   └── Todos.txt
├── config.ini                   <-- Configuración de WiFi y panel.
└── Generador de Playlists.bat   <-- Script para generar las playlists.
```

> [!IMPORTANT]
> Si añades, borras o mueves GIFs dentro de `/gifs/`, ejecuta de nuevo **Generador de Playlists.bat** para actualizar el índice.

### 3. 📝 Configuración vía `config.ini`

El archivo `config.ini` está en la carpeta [Contenido SD](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD) — cópialo a la raíz de la SD y ajústalo a tu gusto:

```ini
# ============================================================
# 🕹️ RETRO PIXEL LED LITE v3.1.5 - ARCHIVO DE CONFIGURACIÓN
# ============================================================
# Nota: No dejes espacios alrededor del símbolo '='.
# Ejemplo correcto: BRIGHTNESS=40

[WIFI_NTP]
# Configura tu red WiFi. Déjala vacía (SSID= y PASS=) para elegirla desde el móvil con el portal WiFi del panel
# Si tu contraseña contiene '#', escríbela tal cual (solo es comentario un '#' precedido de un espacio)
WIFI_ENABLE=1
SSID=
PASS=
# Mostrar IP al iniciar
MOSTRAR_IP=0
# Configura tu zona horaria
TZ=CET-1CEST,M3.5.0,M10.5.0/3

[HARDWARE]
# Numero de paneles
PANEL_CHAIN=2
# Orden de colores del Panel: RGB, RBG o GBR
COLOR_ORDER=RGB
# Brillo (0 a 255)
BRIGHTNESS=38
# Velocidad I2S: 0=8MHz, 1=10MHz, 2=16MHz, 3=20MHz (Turbo)
I2S_SPEED=2
# Refresco Minimo (Hz): 30 a 120
REFRESH_MIN=120
# Doble Buffer: 0=OFF, 1=ON (Elimina parpadeos)
DOUBLE_BUFF=0
# Anti-Ghosting: 1 a 4 (Sube si ves brillo fantasma)
LATCH_BLANK=1
# Velocidad SD (MHz): 4, 10, 20 o 25. Baja si hay cortes o errores de lectura
SD_SPEED=10

[LOGIC]
# Modo de visualizacion: 0=GIFs, 1=Solo Reloj
PLAY_MODE=0
# Activa o desactiva la configuracion mediante la APP: 0=OFF, 1=ON (Requiere WiFi)
CONFI_APP_ENABLE=1
# Selecciona tu sistema Arcade: 0=OFF, 1=Batocera, 2=Recalbox, 3=ReplayOS, 4=RetroBat
ARCADE_ENABLE=0
# Activa o desactiva el texto en scroll: 0=OFF, 1=ON (Requiere WiFi)
TEXT_ENABLE=1
# Activa o desactiva el reloj: 0=OFF, 1=ON (Requiere WiFi)
CLOCK_ENABLE=1
# Modo de reproduccion: 0=Secuencial, 1=Aleatorio
RANDOM_MODE=1
# Intervalo: Cada cuantos GIFs aparece el reloj
AUTO_CLOCK_INT=6
# Duracion: Cuantos segundos se muestra el reloj
CLOCK_DURATION=10
# Estilos: 0=Solid, 1=Aurora, 2=Rainbow, 3=Gradient, 4= Fuego, 5=Glitch
CLOCK_STYLE=2
# Activa la transicion del reloj a GIFs con una explosion de particulas : 0=OFF, 1=ON
TRANSITION_ENABLE=1
# Color del reloj (0= Blanco, 1=Rojo, 2=Verde, 3=Azul, 4=Amarillo, 5=Cian, 6=Magenta, 7=Naranja, 8=Rosa)
CLOCK_COLOR=4

[WEATHER]
# Activa el clima: 0=OFF, 1=ON (Requiere WiFi)
WEATHER_ENABLE=1
# Tu ciudad (Sin espacios, usa '+' si es necesario: Madrid,ES o Buenos+Aires,AR)
CITY=Navalmoral+de+la+Mata,ES
# Tu API Key gratuita de OpenWeatherMap
API_KEY=xxxxxxxxxxxxxxxxxxxxxxx
# Intervalo de actualizacion del clima en MINUTOS
WEATHER_INT=60
# Texto que se muestra encima del reloj
WEATHER_MSG=Game Room

[LANGUAGE]
# Indica el Idioma (Nombre del archivo sin .json: ES, EN, FR...)
LANGUAGE=ES

[IR_REMOTE]
# Códigos HEX del mando IR (NO hay que indicar nada los guardará automaticamente Retro Pixel LED)
BTN_ON=F20DFF00
BTN_OFF=E01FFF00
BTN_BRILLO_UP=F609FF00
BTN_BRILLO_DOWN=E21DFF00
BTN_MENU=EA15FF00
BTN_OK=ED12FF00
BTN_SUBIR=E41BFF00
BTN_BAJAR=B34CFF00

[REPLAY_OS]
# IP que tiene asignada ReplayOS
IP=192.168.1.101
# Token ReplayOS: SYSTEM > INFORMATION > NET CONTROL CODE
TOKEN=xxxxxx

[END]
```

### 4. 🌍 Configuración de zona horaria (TZ)

Para que el **reloj** y el **temporizador** funcionen correctamente, `TZ` debe seguir el formato POSIX.

- **España (Península y Baleares) / Francia / Italia:** `TZ=CET-1CEST,M3.5.0,M10.5.0/3`
- **Canarias / Portugal / Reino Unido:** `TZ=WET0WEST,M3.5.0/1,M10.5.0`

👉 **[ESP32 TZ Tool / Database](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)** para tu código exacto si vives en otra región.

**Formato:** `CET-1CEST` (zona y desfase UTC+1) · `M3.5.0` (cambio a verano: marzo, semana 5, domingo) · `M10.5.0/3` (cambio a invierno: octubre, semana 5, domingo a las 03:00).

### 5. ☁️ Cómo obtener tu API key de clima

1. Ve a [OpenWeatherMap.org](https://openweathermap.org/) y crea una cuenta gratuita.
2. En tu perfil, **"My API Keys"**.
3. Genera una nueva key (ej. "RetroPixel").
4. **Importante:** puede tardar entre 30 minutos y 2 horas en activarse. Si el panel muestra "0.0C", espera un poco. ⏳
5. Cópiala en `API_KEY=` de tu `config.ini`.

**🔍 Comprobar que tu ciudad es correcta:** pega en el navegador `http://api.openweathermap.org/data/2.5/weather?q=TU_CIUDAD&appid=TU_API_KEY` (sustituyendo ambos valores). JSON con datos = todo bien; error 401/404 = revisa la key (tarda en activarse) o el nombre de la ciudad.

---

## 📖 Generador de playlists (Windows)

El script `Generador de Playlist v1.0.1.bat` crea colecciones personalizadas sin tocar código. Está en la carpeta [Contenido SD](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD).

1. **Preparación:** coloca el `.bat` en la raíz de tu SD, junto a la carpeta `gifs`.
2. **Ejecución:** doble clic — se abre una ventana de comandos.
3. **Selección:** el script lista las subcarpetas de `/gifs`. Introduce los números separados por comas (ej: `3,4,10`) o escribe `TODO`.
4. **Nombre:** el que quieras para tu lista (ej: `MisFavoritos`).
5. **Resultado:** crea `playlists/MisFavoritos.txt` con las rutas listas para el ESP32.
6. **Carga:** inserta la SD; reproducirá la primera playlist que encuentre. Para cambiar, `Menú OSD → Playlists`. 🎞️

<img width="514" height="565" alt="Script PlayList" src="https://github.com/user-attachments/assets/3c600615-5539-4430-af7b-26cd219fc7fe" />

---

## 🕹️ Integración con Arcade (Batocera, Recalbox, RetroBat o ReplayOS)

Activa `Menú → Reproducción → Arcade` para que el panel muestre las marquesinas del juego o sistema en el que estás:

```text
🏠 MENÚ PRINCIPAL
├── 📂 Reproducción
│   ├── 🖼️ Modo: [GIFs / Reloj]
│   ├── 🔀 Aleatorio: [SI / NO]
│   ├── 🕹️ Arcade: [OFF / Batocera / Recalbox / ReplayOS / RetroBat]   <-- AQUÍ
│   └── 🔙 Volver
```

> [!IMPORTANT]
> Para sincronizar tus ROMs, usar el script de PC e instalar los scripts de comunicación, consulta la guía detallada de cada sistema:
>
> **[👉 Instrucciones de Batocera](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_BATOCERA.md)**
>
> **[👉 Instrucciones de Recalbox](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_RECALBOX.md)**
>
> **[👉 Instrucciones de RetroBat](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_RETROBAT.md)**
>
> **[👉 Instrucciones de ReplayOS](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_REPLAYOS.md)**

---

## 🏠 Integración con Home Assistant (guía completa)

Puedes integrar y controlar completamente **Retro Pixel LED Lite** desde **Home Assistant** a través de la API REST local, sin depender de la nube.

Esta integración te permite:
- 🟢 **Encender / apagar** el panel mediante un interruptor (*switch*).
- 📊 **Consultar el estado actual** (modo activo, playlist en reproducción, etc.).
- 🔄 **Cambiar de modo** (Reloj / GIF) y de **playlist** de forma instantánea.
- 💬 **Enviar mensajes de texto con desplazamiento** eligiendo color, velocidad y fuente desde el dashboard.

### 📦 Añadir la configuración a Home Assistant

Si utilizas la estructura de carpetas por paquetes (`packages`), guarda el archivo `retropixel.yaml` que está **[aquí](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Home%20Asisstant)** dentro de `/config/packages/`. Si no usas `packages`, pega el contenido en tu `configuration.yaml`.

> ⚠️ **Importante:** reemplaza la IP `192.168.31.210` por la de tu ESP32, y los nombres de playlist por los tuyos.

También hay una tarjeta `entities.yaml` lista para tu dashboard.

<img width="822" height="1005" alt="Captura HA" src="https://github.com/user-attachments/assets/9294b479-f428-4c68-9fcc-c871ad2e88e4" />

---

## 🧠 Arquitectura interna / Core Lite

Detalles técnicos de cómo funciona el sistema por dentro:

- **📡 Control IR y mapeado dinámico:** mandos infrarrojos con mapeado de funciones desde el menú OSD (brillo, navegación, power toggle, confirmación).
- **📂 Servidor FTP de mantenimiento:** gestión inalámbrica de `config.ini` y playlists sin extraer la MicroSD.
- **Anti-Panic RAM Management:** vigilancia del *heap* — si el DMA no puede asignar memoria tras usar WiFi, conmuta a Single Buffer para garantizar estabilidad.
- **Motor de búsqueda binaria (Arcade):** localiza marquesinas entre miles de archivos en milisegundos, saltando directamente a la posición en la SD gracias a índices ordenados alfabéticamente — no "escanea" carpetas.
- **Memoria adaptativa (Single/Double Buffer):** Double Buffer para fluidez total en GIFs; conmuta a Single Buffer en modo Arcade al cargar bitmaps de alta definición.
- **API HTTP en tiempo real:** receptor de comandos para sincronización con Batocera, Recalbox, ReplayOS y Home Assistant.
- **📶 Portal WiFi integrado:** punto de acceso temporal con DNS de portal cautivo y un servidor web propio que se destruye al terminar, sin dejar rutas abiertas en la red. Escanea las redes *antes* de levantar el punto de acceso, genera una clave aleatoria que solo se ve en el panel y arranca sin Double Buffer para ahorrar RAM (se restaura el valor del usuario al guardar).
- **🏷️ mDNS:** el panel se anuncia como `retropixel.local` mientras hay WiFi; la PWA guarda la última IP como alternativa.
- **🩺 Diagnóstico `/diag`:** JSON de solo lectura, sin acceso a la SD, con memoria, motivo de reinicio, WiFi y uso de la partición OTA.
- **🧩 Código modular:** las rutas HTTP viven en `WebRoutes.ino` y el portal en `PortalWiFi.ino` (con `PortalPaginas.h` y `PortalTextos.h`).
- **Smart Text Centering:** alinea automáticamente menús y estados al centro de la matriz (`offset + 64px`), calculando el ancho de cada cadena.
- **WiFi Stealth Mode:** el WiFi solo se activa brevemente para sincronizar hora y clima; el resto del tiempo el sistema va **100% offline**, con **0 lag** en la reproducción.
- **Barra de notificaciones dinámica:** con el clima activo, el reloj baja su posición (`startY=9`) para mostrar `WEATHER_MSG`, el icono del tiempo y la temperatura.
- **Iconografía avanzada (día/noche):** iconos de 8×8 px dibujados a mano (Sol, Luna, Nubes, Lluvia, Nieve, Tormenta, Niebla), adaptados según la hora.
- **Sistema de playlists dinámicas:** múltiples `.txt` en `/playlists/`, saltando entre colecciones temáticas desde el menú OSD.
- **Reloj auto-interrupción:** interrumpe la galería cada *x* GIFs para mostrar la hora *x* segundos, retomando la reproducción donde se quedó.
- **Resiliencia offline:** sin WiFi, ignora la sincronización y reproduce GIFs de inmediato con el reloj interno del chip.
- **Motor de renderizado Double Buffer:** aprovecha el DMA del ESP32 para dibujar frames de forma invisible, sin parpadeo.

---

## 📜 Historial de cambios detallado (v3.0.0 → v3.1.5)

| Característica | Detalle técnico | Beneficio |
| :--- | :--- | :--- |
| **📶 Portal WiFi integrado** | Punto de acceso temporal (`RetroPixel-LED`) con clave aleatoria en pantalla, DNS de portal cautivo y página web propia con la lista de redes ordenada por señal. Prueba la conexión antes de guardarla en `config.ini`. Se abre desde `Menú OSD → WiFi → Añadir red` o solo, con `WIFI_ENABLE=1` y `SSID` vacío. Textos en ES/EN/FR dentro del firmware. | **Conecta el panel sin editar archivos.** Cambiar de router ya no obliga a sacar la SD. |
| **🏷️ Acceso por nombre (mDNS)** | El panel se anuncia como `retropixel.local`. La PWA acepta nombre o IP (`GET /status` incluye ahora `ip`), recuerda la última IP conocida y ofrece usarla si el nombre no se resuelve. | **Sin depender de la IP**, que puede cambiar con el router. |
| **🔔 Aviso de actualización** | La PWA compara la versión del firmware (`fw` y `fwNum` en `GET /status`) con `version.json` en GitHub Pages. Punto ámbar sobre 🔄, versión disponible y novedades (`notes`). | **Te enteras de las versiones nuevas** sin tener que buscarlas. |
| **ℹ️ Página "Acerca de" (PWA)** | Versión de la PWA (leída de la caché del service worker) y del firmware, creador, licencia y diagnóstico en vivo con botón de copiar informe. | **Informes de errores más fáciles** y estado del panel a la vista. |
| **🩺 Endpoint `/diag`** | `GET /diag`: uptime, heap libre y mínimo, mayor bloque libre, motivo del último reinicio, RSSI, tamaño y velocidad de la SD, CPU y uso de la partición OTA. | **Diagnóstico de memoria y estabilidad** sin cable USB. |
| **🔐 Contraseñas WiFi con `#`** | En `config.ini`, para `SSID` y `PASS` solo se considera comentario un `#` precedido de un espacio. | **Cualquier contraseña funciona**, también las que llevan `#`. |
| **🛡️ Service worker de la PWA** | Versión única en `sw.js` (nombre de la caché), instalación tolerante a fallos, coincidencia exacta de rutas (no intercepta las peticiones al panel) y borrado solo de sus propias cachés. | **Actualizaciones de la PWA más fiables.** |
| **🕹️ Soporte RetroBat** | Integración con el frontend RetroBat para la detección y reproducción automática de GIFs/marquesinas asociadas a los juegos. | **Experiencia arcade dinámica.** Muestra el marquee del juego activo al cambiar de título en RetroBat. |
| **💾 Velocidad de lectura SD configurable** | Parámetro `SD_SPEED` (4, 10, 20 o 25 MHz; 10 por defecto) editable desde la PWA, el menú OSD (Ajustes avanzados) y `config.ini`. Arranque en dos fases con *fallback* automático a una velocidad inferior si la tarjeta no responde. | **Carga de GIFs más rápida** y ajustable a la calidad de cada tarjeta, sin recompilar. |
| **🎞️ Fix de velocidad de reproducción de GIFs** | Eliminada la doble espera entre frames (`playFrame` sincronizado + retardo propio). El tiempo de decodificación ahora se descuenta de la espera. | **Reproducción más fluida** y fiel al tiempo real de cada frame del GIF. |
| **🧩 Particiones Minimal SPIFFS** | Esquema *Minimal SPIFFS* (1,9 MB de app con OTA) en lugar del esquema por defecto de 1,25 MB. | **Más espacio de firmware** para nuevas funciones. Requiere reinstalar por web desde versiones anteriores. |
| **🕹️ Soporte RePlayOS** | Integración con el frontend RePlayOS para detección y reproducción automática de GIFs/marquesinas asociadas a los juegos. | **Experiencia arcade dinámica.** Muestra el marquee del juego activo al cambiar de título en el sistema RePlayOS. |
| **⏰ Nuevos estilos de reloj** | Añadidos nuevos modos de renderizado y diseños visuales seleccionables para la hora. | **Mayor personalización** estética adaptada a distintos gustos y espacios. |
| **📡 Mapeo IR en PWA** | Interfaz en la PWA para capturar, asignar y guardar códigos de botones del mando a distancia en `config.ini`. | **Configuración intuitiva del mando IR** ahora tambien disponible en la PWA. |
| **🏠 Integración Home Assistant** | Exposición de endpoints REST API (`GET /status`, `POST /control`, `/playlist`, `/texto`, `/timer/toggle`) y paquete YAML completo. | **Automatización y control domótico.** Enciende, apaga, cambia modos, playlists y envía texto desde HA. |
| **🔤 Selección de fuente en texto** | 4 tipografías seleccionables por parámetro (`Bold`, `SemiBold`, `Regular`, `Light`) en endpoints HTTP, PWA e integración REST. | **Personalización visual** del texto en scroll. |
| **🎛️ PWA Control Panel** | Interfaz web progresiva con 5 módulos de control y edición remota de `config.ini`. | **Control total desde cualquier dispositivo**, sin servidor externo. |
| **☀️ Control de brillo** | Slider en tiempo real (0-100%), aplicación instantánea. | **Ajuste fluido** a la iluminación ambiental. |
| **🔤 Texto scroll con UTF-8** | Decodificador UTF-8→Latin-1 en tiempo real, soporte para caracteres polacos y acentuados. | Mensajes con ñ, á, ł, ą sin limitaciones. |
| **🎨 Modo GIF / Reloj / Texto** | Selector de modo en la home de la PWA, sin reinicio. | Cambios visibles al instante. |
| **🎞️ Playlists dinámicas** | Cambio de playlist en tiempo real desde la PWA. | Alterna colecciones sin interrumpir la reproducción. |
| **⏰ Temporizador inteligente** | Horario programado, override manual por botón o PWA. | Automatización completa de encendido/apagado. |
| **🔄 Actualización remota (OTA + idiomas)** | Descarga de firmware e idiomas desde GitHub sin SD. | Mantenimiento completamente inalámbrico. |
| **⚙️ Ajustes completos remotos** | Edición remota de `config.ini`: WiFi, hardware, reproducción, clima, idioma. | Configuración inalámbrica total. |
| **💬 Control de texto scroll** | Endpoints HTTP POST + PWA para mensajes, color y velocidad. | Avisos al vuelo sin reprogramar. |
| **💥 Transición de partículas** | Motor de partículas para la entrada/salida del reloj. | Fluidez visual, sin cortes estáticos. |
| **🎨 Selección de color OSD** | Menú interactivo mapeado con IR y memoria EEPROM/SD. | Cambia el color del reloj sin editar `config.ini`. |
| **⚡ Reloj sin parpadeos** | Renderizado en modo *Single Buffer* optimizado. | Cero *flicker* al actualizar datos rápidos. |
| **🧠 Optimización de RAM** | `String` → `char[]`, uso masivo de `PSTR()`/`F()`. | Cero fragmentación; más heap para el Double Buffer. |
| **🛡️ Anti-Panic System** | Verificación de `display->begin()` con fallback a Single Buffer. | Evita cuelgues (`StoreProhibited`) por fragmentación tras el WiFi. |
| **🖱️ Confirmación segura** | Detección por tiempo de pulsación (*Long Press*). | Evita entradas accidentales en menús. |
| **📂 Servidor FTP integrado** | Transferencia inalámbrica directa a la SD. | Gestiona playlists e `.ini`/`.json` sin extraer la MicroSD. |
| **📡 Control remoto IR** | Mapeado dinámico de funciones y navegación. | Brillo, encendido y menú desde el mando. |
| **🎨 Configuración de color** | `colorOrder` (RGB/RBG/GBR) dinámico desde `config.ini`. | Compatible con cualquier panel HUB75 sin reprogramar. |

---

## 🛒 Lista de materiales

Componentes probados durante el desarrollo:

- **Microcontrolador:** [ESP32 DevKit V1 (30 pines) - AliExpress](https://es.aliexpress.com/item/1005005704190069.html)
- **Panel LED Matrix (HUB75):** [P2.5 / P4 RGB Matrix Panel - AliExpress](https://es.aliexpress.com/item/1005008479388445.html)
- **Lector de tarjetas:** [Módulo adaptador Micro SD (SPI) - AliExpress](https://es.aliexpress.com/item/1005005591145849.html)
- **Placa conexión ESP32-Panel LED:** [DMDos Board V3 - Mortaca](https://www.mortaca.com/) *(opcional, sin soldar, con lector SD incorporado)*
- **Receptor de IR:** [Sensor de receptor de infrarrojos universal - AliExpress](https://es.aliexpress.com/item/1005005343424296.html)
- **Pulsador:** [Interruptor momentáneo DS-316 - AliExpress](https://es.aliexpress.com/item/4000888761296.html)
- **Alimentación:** fuente de 5V (mínimo 2A recomendado para paneles de 64×32).

---

## 🔌 Conexiones

Si utilizas **DMDos Board V3** esta parte ya la tienes hecha — salta al siguiente punto.

#### 📂 Lector de tarjeta Micro SD (Interfaz SPI)
| Pin SD | Pin ESP32 | Función |
| :--- | :--- | :--- |
| **CS** | GPIO 5 | Chip Select |
| **CLK** | GPIO 18 | Clock |
| **MOSI** | GPIO 23 | Master Out Slave In |
| **MISO** | GPIO 19 | Master In Slave Out |
| **VCC** | 3.3V | Alimentación |
| **GND** | GND | GND |

#### 🖼️ Panel LED RGB (Interfaz HUB75)
| Pin Panel | Pin ESP32 | Función |
| :--- | :--- | :--- |
| **R1** | GPIO 25 | Datos Rojo (Superior) |
| **G1** | GPIO 26 | Datos Verde (Superior) |
| **B1** | GPIO 27 | Datos Azul (Superior) |
| **R2** | GPIO 14 | Datos Rojo (Inferior) |
| **G2** | GPIO 12 | Datos Verde (Inferior) |
| **B2** | GPIO 13 | Datos Azul (Inferior) |
| **A** | GPIO 33 | Selección de Fila A |
| **B** | GPIO 32 | Selección de Fila B |
| **C** | GPIO 22 | Selección de Fila C |
| **D** | GPIO 17 | Selección de Fila D |
| **E** | GND | GND |
| **CLK** | GPIO 16 | Clock |
| **LAT** | GPIO 4 | Latch |
| **OE** | GPIO 15 | Output Enable (Brillo) |

#### 🕹️ Control de usuario (físico e infrarrojo)
| Componente | Pin ESP32 | Función |
| :--- | :--- | :--- |
| **Botón (PIN)** | GPIO 21 | **Multifunción:** Click (Navegar) / Long Press (Confirmar - Power Toggle). |
| **Botón (GND)** | GND | Referencia de tierra. |
| **Receptor IR (Data)** | GPIO 34 | Entrada de señal (Protocolo NEC/etc). |
| **Receptor IR (VCC)** | 3.3V | Alimentación del sensor. |
| **Receptor IR (GND)** | GND | Referencia de tierra. |

<img width="769" height="716" alt="image" src="https://github.com/user-attachments/assets/11fef006-59f3-405f-b00a-a32c9bba7bc5" />

---

### 📂 Explorador SD (FTP) — Detalle

Esta función activa un servidor de archivos inalámbrico. Su objetivo es facilitar el mantenimiento sin extraer la MicroSD.

> [!IMPORTANT]
> **Uso recomendado:** `config.ini`, archivos de idioma (`.json`), playlists (`.txt`) y archivos pequeños. Por las limitaciones de ancho de banda del ESP32, **no se recomienda para colecciones de GIFs completas** — sería muy lento frente a un lector de tarjetas convencional.

**Activar el servidor FTP:**
1. `Menú OSD → Explorador SD`.
2. Selecciona **Iniciar FTP**.
3. El panel detiene los GIFs y muestra su **dirección IP** (ej. `192.168.1.109`).

**Configuración de conexión (FileZilla):**
- **Protocolo:** FTP plano (sin cifrado).
- **Servidor/Host:** la IP que muestra el panel.
- **Modo de acceso:** Normal.
- **Usuario / Contraseña:** `admin` / `admin`.
- **Puerto:** `21`.
- **Conexiones simultáneas:** limitado a 1.

<img width="545" height="227" alt="image" src="https://github.com/user-attachments/assets/1b537615-3e39-48ba-9eb0-48b03931c5f9" />
<img width="544" height="193" alt="image" src="https://github.com/user-attachments/assets/ba4c85bc-920a-48c9-83d8-99b96ecbc57f" />

**En Edición → Opciones → Transferencias:**
- Máximo de transferencias simultáneas: 1.
- Límites de velocidad activados: 20 KiB/s descarga y carga.

<img width="841" height="522" alt="image" src="https://github.com/user-attachments/assets/e90d3e84-9c93-45c0-b942-8b601db40041" />

**Alternativa sin FileZilla (Explorador de Windows)** *(no recomendado — en pruebas algunos archivos no se cargaron completos)*:
1. **Este equipo** → clic derecho → **"Agregar una ubicación de red"**.
2. Dirección: `ftp://<IP_del_panel>` (ej. `ftp://192.168.1.109`).
3. Desmarca "Iniciar sesión de forma anónima", usuario `admin`.
4. Ponle un nombre descriptivo a la unidad.

**Notas de seguridad:**
- El panel no reproduce GIFs mientras el FTP está activo.
- Para salir: botón físico o tecla "Validar" del mando IR.
- No desconectes la alimentación mientras editas un archivo por FTP — podría corromperse.

---

## 🛠️ Roadmap

### ⚡ Optimización y funcionalidad

- 📂 Gestor de archivos en la PWA: subir GIFs y crear playlists por WiFi (solo si resulta estable y rápido).

### 🎨 Estética y conectividad

- 🎮 Mini-juegos (Snake, Pong) controlados con el mando IR.

---

## ⚖️ Licencia y agradecimientos

Este proyecto se publica bajo la **Licencia MIT**.

Agradecimientos especiales a los desarrolladores de las librerías base:
- **Bitbank2** por la excelente librería `AnimatedGIF`.
- **Mrfaptastic** por el motor DMA de alto rendimiento para matrices.
- **Comunidad Telegram DMDos**, donde ver de lo que era capaz DMDos me animó a desarrollar **Retro Pixel LED**.
- **RpiTe@m** por compartir el pack de 600 GIFs **gratis** y su recopilación de 11000 GIFs, disponible [aquí](https://www.neo-arcadia.com/forum/viewtopic.php?t=67065).
- **shan-aya** por la traducción al francés y su magnífico software para crear [GIFs](https://github.com/shan-aya/DMD_GIF_converter).
- **joseAveleira** por el efecto de partículas en el reloj. [GitHub](https://github.com/joseAveleira/RelojPixel/tree/main)

---

<p align="center">Hecho con ❤️ y muchos GIFs retro. Si te sirve, ¡una ⭐ en el repo también ayuda mucho!</p>
