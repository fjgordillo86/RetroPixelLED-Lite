# ✨ Retro Pixel LED Lite v3.0.1
**[🇪🇸 Español](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README.md) | [🇫🇷 Français](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_FR.md)**

### **[✈️ Unirse al Grupo de Telegram: Retro Pixel LED para estár al día de las actualizaciones](https://t.me/RetroPixelLed)**

## 💡 Descripción del Proyecto

**Retro Pixel LED Lite** es la versión de alto rendimiento diseñada para quienes buscan estabilidad absoluta, velocidad instantánea y un sistema libre de mantenimiento. A diferencia de la versión estándar, el firmware LITE elimina la carga del servidor web y la conectividad permanente para dedicar el 100% de la potencia del ESP32 al renderizado de GIFs. 

Si la rama 2.x.x introdujo el Menú OSD, la nueva **v3.0.0** supone el salto definitivo hacia la independencia del hardware. Esta versión transforma el panel LED en un dispositivo inteligente autónomo, eliminando por completo la necesidad de conectar el ESP32 al ordenador para tareas de mantenimiento o configuración.
Por primera vez, el sistema permite la edición de archivos de configuración (`config.ini`) y la gestión de librerías de playlists directamente desde el Explorador de Windows o clientes FTP, convirtiendo la tarjeta SD en una unidad de red inalámbrica.
Se integra soporte nativo para mandos a distancia, permitiendo navegar por el Menú OSD, ajustar el brillo dinámico y controlar el encendido/apagado desde el sofá.

> [!TIP]
> **🚀 Filosofía Lite:** Menos es más. Al apagar el WiFi después de sincronizar la hora y el tiempo, el sistema elimina el lag, reduce el calor del chip y evita cuelgues por saturación de red, permitiendo reproducciones fluidas de colecciones masivas.

Si quieres probar la versión estandar aquí tienes el enlace al **[GitHub.](https://github.com/fjgordillo86/RetroPixelLED)**

¿Quieres hacer tus propios GIFs?  Aquí tienes dos herramientas magínificas.
- [DMD GIF converter](https://github.com/shan-aya/DMD_GIF_converter) creada por **shan-aya**.
- [Video a GIF](https://p4blogc.github.io/dmdos-converter/) creada por **p4bloGC**.

---
## 🆕 Novedades de la Versión v3.0.1 Lite

| Característica | Detalle Técnico | Beneficio |
| :--- | :--- | :--- |
| **🧠 Optimización de RAM** | Refactorización de objetos `String` a `char[]` y uso masivo de `PSTR()` / `F()`. | **Cero fragmentación.** Los textos se almacenan en la Flash, liberando el Heap para el Double Buffer. |
| **🛡️ Anti-Panic System** | Verificación de `display->begin()` con cambio a single Buffer en caso de fallo de asignación de RAM. | **Estabilidad total.** Evita cuelgues (`StoreProhibited`) si la memoria se fragmenta tras usar el WiFi. |
| **🖱️ Confirmación Segura** | Lógica de detección basada en tiempo de pulsación (*Long Press*) para el botón físico. | **Navegación Precisa.** Evita entradas accidentales en menús; ahora confirmas manteniendo presionado. |
| **📂 Servidor FTP Integrado** | Protocolo de transferencia de archivos inalámbrico directo a la tarjeta SD del ESP32. | **Comodidad.** Gestiona tus playlists, archivos `.ini` y `.json` sin necesidad de extraer la MicroSD. |
| **📡 Control Remoto IR** | Mapeado dinámico de funciones y navegación de menús mediante receptor infrarrojo. | **Control a distancia.** Maneja el brillo, apaga o enciende el panel y navega por el menú cómodamente desde un mando. |
| **🎨 Configuración de Color** | Parámetro `colorOrder` (RGB/RBG/GBR) procesado dinámicamente desde el `config.ini`. | **Versatilidad.** Compatibilidad con cualquier panel HUB75 del mercado sin necesidad de reprogramar. |

---

## 🕹️ Integración Especial: Modo Arcade (Batocera)

Esta versión Lite introduce un soporte avanzado para sistemas de retrogaming. Mediante una jerarquía de archivos inteligente, el panel puede mostrar:
1. **Marquesina del Juego:** Scrapeada directamente desde tu colección de Batocera.
2. **Logo del Sistema:** Imagen de respaldo si el juego no dispone de arte específico.
3. **Reserva Maestro:** Imagen por defecto si el sistema no está indexado.

> [!IMPORTANT]
> Se incluye una herramienta en `/Batocera/tools` para automatizar el redimensionado a 128x32, la conversión a BMP de 24 bits y la generación de índices para una respuesta instantánea del ESP32. Consulta el [README específico de Batocera](/README_BATOCERA.md) para más detalles.

---

## 📜 Historial de Cambios Detallado (v3.0.0 -> v3.0.1)

| Tipo | Componente | Descripción del Cambio |
| :--- | :--- | :--- |
| **🧠 Optimización** | **RAM**. | **Cero fragmentación.** Los textos se almacenan en la Flash, liberando el Heap para el Double Buffer. |
| **🛡️ Estabilidad** | **Núcleo (DMA)** | **Anti-Panic System:** Implementada detección de fragmentación de RAM, cambia a single Buffer automaticamente. |
---
### 🖥️ Estructura del Menú OSD (Navegación Inteligente)

El sistema se controla mediante un **único botón**. Utiliza una lógica de pulsación avanzada que se adapta según el menú donde te encuentres:
* **Pulsación Rápida:**
    * **En Menús:** Mover el cursor / Navegar hacia abajo.
    * **En Modo Sueño:** Despierta el panel de forma inmediata (Wake-up).
* **Pulsación mantenida:**
    * **Acción General:** Entrar en submenús o confirmar selección.
    * **En Configuración de Tiempo (Temporizador):** Resta **-5 minutos** al valor actual para un ajuste rápido hacia atrás.
* **Pulsación Extra Larga (> 4 seg):**
    * **Manual Override:** Fuerza el apagado (Modo Sueño), bloqueando el automatismo del temporizador hasta el próximo ciclo.
* **Mantener Pulsación Continua:**
    * **En Configuración de Tiempo (Temporizador):** Incrementa automáticamente **+5 minutos** de forma cíclica mientras mantengas pulsado.

```text
🏠 MENÚ PRINCIPAL
├── 📂 Playlists
│   ├── 📄 Favoritos
│   ├── 📄 Arcade
│   ├── 📄 ...
│   └── 🔙 Volver
├── 📂 Reproducción
│   └── 🖼️ Modo: [GIFs / Reloj]
│   └── 🔀 Aleatorio: [SI / NO]
│   └── 🕹️ Arcade: [SI / NO]
│   └── 🔙 Volver
├── ☀️ Brillo
│   └──   Brillo: [5% - 100%]
├── 📶 WiFi: [ON / OFF]
│   ├── 🔄 Activar: [SI / NO]
│   └── 🔙 Volver
├── 🕒 Reloj: [ON / OFF]
│   ├── 🔄 Activar: [SI / NO]
│   ├── 🖼️ Cada: [1...20] GIFs
│   ├── ⏳ Ver: [5...30] seg
│   └── 🎨 Estilo Reloj: [Matrix, Solid, Rainbow, Pulse, Gradient]
│   └── 🔙 Volver
├── 🌡️ Clima: [ON / OFF]
│   └── 🔄 Activar: [SI / NO]
│   └── 🔙 Volver
├── 🕒 Temporizador: [ON / OFF]
│   ├── 🔄 Activar: [SI / NO]
│   ├── ⏳ ON: [00:00 a 24:00]
│   ├── ⏳ OFF: [00:00 a 24:00]
│   └── 🔙 Volver
├── ⚙️ Ajustes Avanzados
│   ├── ⚡ I2sSeep: [8, 10, 16, 20MHz]
│   ├── 🔄 Refresco: [30, 60, 90, 120Hz]
│   ├── 🖼️ Buffer: [SI / NO]
│   ├── 👻 AntiGhot: [1, 2, 3, 4]
│   ├── 🎮 Mapeado mando IR: [On, Off, ,Menu, Validar, Subir, Bajar, Brillo+, Brillo-]
│   ├── ⚠️ Reset:
│   └── 🔙 Volver
├── 🚀 Actualización
│   └── 🔄 Buscar OTA
│   └── 🔙 Volver
├── 📂 Explorardor SD
│   └── 🔄 Iniciar FTP
│   └── 🔙 Volver
├── 🌐 Idioma
│   └── [ES] Español
│   ├── [EN] English
│   ├── [FR] Francais
│   ├── ...
│   └── 🔙 Volver
├── 💾 Guardar
└── 🔙 Salir
```
### ⏰ Funcionamiento del Submenú Temporizador

Para facilitar la configuración de las horas de encendido (`hon/mon`) y apagado (`hoff/moff`), se ha implementado una lógica de saltos de 5 minutos:

1.  **¿Quieres avanzar rápido?** Mantén pulsado el botón. El tiempo subirá de 5 en 5 minutos sin que tengas que soltar.
2.  **¿Te has pasado de hora?** Realiza una pulsación larga (un segundo) y el tiempo retrocederá 5 minutos.

   
## 🛠️ Herramientas Exclusivas Lite

### 📖 Cómo usar el Script Generador de Playlists (Windows)

El script `Generador de Playlist v1.0.1.bat` facilita la creación de colecciones personalizadas sin tocar una sola línea de código. Lo encontrarás en la carpeta "Contenido SD" [aquí](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD).

1. **Preparación:** Coloca el archivo `.bat` en la **raíz de tu tarjeta SD**, justo al lado de la carpeta `gifs`.
2. **Ejecución:** Haz doble clic en el archivo. Se abrirá una ventana de comandos.
3. **Selección:** - El script listará todas las subcarpetas dentro de `/gifs`.
   - Introduce los números de las carpetas que quieras incluir en la lista separados por comas (ej: `3,4,10`) o escribe `TODO`.
4. **Nombre:** Escribe el nombre que quieras para tu lista (ej: `MisFavoritos`). 
5. **Resultado:** El script creará automáticamente una carpeta llamada `playlists` y guardará dentro el archivo `MisFavoritos.txt` con las rutas corregidas para el ESP32.
6. **Carga:** Inserta la SD en tu Retro Pixel LED, reproducirá la primera playlist que encuentre en la carpeta. Si quieres cambiar de playlist entra en el menú OSD y seleccionala en "Playlists".
<img width="514" height="565" alt="Script PlayList" src="https://github.com/user-attachments/assets/3c600615-5539-4430-af7b-26cd219fc7fe" />

### ⚙️ Archivo de Configuración (config.ini)
Sustituye por completo la interfaz web de la versión estándar. Permite ajustar el comportamiento del hardware de forma persistente.
* **Ubicación en el repo:** `/Contenido SD/`
* **Destino:** El config.ini debe copiarse en la **raíz de la Micro SD**.
* **Función:** Define las credenciales WiFi para la sincronización horaria, el brillo de los LEDs, el estilo del reloj y la frecuencia con la que se interrumpe la galería para mostrar la hora.
---

## ⚙️ Instalación y Configuración

### 1. 🚀 Programar el ESP32 (Web Installer)
Puedes instalar esta versión sin instalar nada en tu PC usando nuestro instalador basado en Chrome/Edge:

### **[👉 Abrir Instalador Web Retro Pixel LED Lite](https://fjgordillo86.github.io/RetroPixelLED-Lite/)**

**Pasos para la instalación:**
1. Utiliza un navegador compatible (**Google Chrome** o **Microsoft Edge**).
2. Conecta tu ESP32 al puerto USB del ordenador.
3. Haz clic en el botón **"Install"** de la web y selecciona el puerto COM correspondiente.
4. **IMPORTANTE:** Asegúrate de marcar la casilla **"Erase device"** en el asistente para realizar una limpieza completa de la memoria y evitar errores de fragmentación.

> 💡 **¿No reconoce tu ESP32?**
> Si al pulsar "Install" no aparece ningún puerto COM, es probable que necesites instalar los drivers del chip USB de tu placa:
> * **Chip CP2102:** [Descargar Drivers Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
> * **Chip CH340/CH341:** [Descargar Drivers SparkFun](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all)

### 2. 📂 Preparación de la Tarjeta SD
Formatea tu MicroSD en **FAT32** añade todo el contenido de la carpeta [Contenido SD](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD) en la raiz. La Micro SD tendrá la siguiente estructura:

```text
/ (Raíz de la SD)
├── gifs/                        <-- Tus carpetas con GIFs (Arcade, Consolas, etc.)
├── idioma/                      <-- Aquí estarán los archivos .json con los textos traducidos.
│   ├── ES.json                  <-- Dicccionadio ES.json.
│   ├── EN.json                  <-- Dicccionadio EN.json.
│   └── FR.json                  <-- Dicccionadio FR.json.
├── playlists/                   <-- Aquí estarán las listas generadas por el script "Generador de Playlists".
│   ├── Arcade.txt               <-- Lista .txt.
│   ├── Computers.txt            <-- Lista .txt.
│   ├── Consolas.txt             <-- Lista .txt.
│   └── Todos.txt                <-- Lista .txt.
├── arcade/                      <-- Integración con Batocera Marquesinas. (Solo Si haces uso de Modo Arcade)
│   ├── neogeo.txt               <-- Lista marquesinas.txt. (Solo Si haces uso de Modo Arcade)
│   ├── mame.txt                 <-- Lista marquesinas.txt. (Solo Si haces uso de Modo Arcade)
│   ├── neogeo/                  <-- Carpetas con marquesinas para sistema neogeo. (Solo Si haces uso de Modo Arcade)
│   │   ├── mslug.bmp            <-- Imagen de marquesina. (Solo Si haces uso de Modo Arcade)
│   │   └── kof98.bmp            <-- Imagen de marquesina. (Solo Si haces uso de Modo Arcade)
│   ├── mame/                    <-- Carpetas con marquesinas para sistema mame. (Solo Si haces uso de Modo Arcade)
│   │   ├── logo.bmp             <-- Imagen de marquesina. (Solo Si haces uso de Modo Arcade)
│   └── └── pacman.bmp           <-- Imagen de marquesina. (Solo Si haces uso de Modo Arcade)
├── config.ini                   <-- Configuración de WiFi y Panel.
└── Generador de Playlists.bat   <-- Script para generar las Playlist.
```
`La carpeta arcade solo es necesaria si se utiliza el "Modo Arcade" y la genera automáticamente el script Marquesinas. Para mas info lee el punto 9.`

>[!IMPORTANT]
>Si añades, borras o mueves GIFs dentro de la carpeta /gifs/, asegúrate de ejecutar el script **Generador de Playlists.bat** de nuevo para actualizar el índice.

### 3. 📝 Configuración via `config.ini`
El archivo llamado `config.ini` que lo encontrarás en la carpeta "Contenido SD" [aquí](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD) tienes que añadirlo en la raíz de la SD y modificarlo para dejar Retro Pixel LED Lite a tu gusto:

```ini
# ============================================================
# 🕹️ RETRO PIXEL LED LITE v3.0.0 - ARCHIVO DE CONFIGURACIÓN
# ============================================================
# Nota: No dejes espacios alrededor del símbolo '='.
# Ejemplo correcto: BRIGHTNESS=40

[WIFI_NTP]
# Configura tu red WiFi
WIFI_ENABLE=1
SSID=Nombre_De_Tu_Red
PASS=Password_De_Tu_Red
# Configura tu zona horaria
TZ=CET-1CEST,M3.5.0,M10.5.0/3

[HARDWARE]
# Numero de paneles
PANEL_CHAIN=2
# Orden de colores del Panel: RGB, RBG o GBR
COLOR_ORDER=RGB
# Brillo (0 a 255)
BRIGHTNESS=43
# Velocidad I2S: 0=8MHz, 1=10MHz, 2=16MHz, 3=20MHz (Turbo)
I2S_SPEED=2
# Refresco Minimo (Hz): 30 a 120
REFRESH_MIN=120
# Doble Buffer: 0=OFF, 1=ON (Elimina parpadeos)
DOUBLE_BUFF=1
# Anti-Ghosting: 1 a 4 (Sube si ves brillo fantasma)
LATCH_BLANK=1

[LOGIC]
# Modo de visualizacion: 0=GIFs, 1=Solo Reloj
PLAY_MODE=0
# Activa la recepcion de marquesinas desde Batocera: 0=OFF, 1=ON
ARCADE_ENABLE=0
# Activa o desactiva el reloj: 0=OFF, 1=ON (Requiere WiFi)
CLOCK_ENABLE=1
# Modo de reproduccion: 0=Secuencial, 1=Aleatorio
RANDOM_MODE=1
# Intervalo: Cada cuantos GIFs aparece el reloj
AUTO_CLOCK_INT=6
# Duracion: Cuantos segundos se muestra el reloj
CLOCK_DURATION=10
# Estilos: 0=Matrix, 1=Solid, 2=Rainbow, 3=Pulse, 4=Gradient
CLOCK_STYLE=2
# Color del Reloj (Formato HEX)
CLOCK_COLOR=#FF0055

[WEATHER]
# Activa el clima: 0=OFF, 1=ON (Requiere CLOCK_ENABLE=1)
WEATHER_ENABLE=1
# Tu ciudad (Sin espacios, usa '+' si es necesario: Madrid,ES o Buenos+Aires,AR)
CITY=Navalmoral+de+la+Mata,ES
# Tu API Key gratuita de OpenWeatherMap
API_KEY=xxxxxxxxxxxxxxxxxxxxxxx
# Intervalo de actualización en MINUTOS
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

[END]
```
---

### 4. 🌍 Configuración de Zona Horaria (TZ)

Para que el **Reloj** y el **Temporizador** funcionen correctamente, el parámetro `timezone` en el archivo `config.ini` debe seguir el formato POSIX. 

Ejemplo para **España (Península y Baleares) / Francia / Italia**:
`timezone=CET-1CEST,M3.5.0,M10.5.0/3`
Ejemplo para **Canarias / Portugal / Reino Unido**:
`timezone=WET0WEST,M3.5.0/1,M10.5.0`

### ¿Cómo obtener tu código TZ?
Si vives en otra región, puedes obtener el código exacto de tu ciudad aquí:
👉 **[ESP32 TZ Tool / Database](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)**

### Explicación del formato:
* **CET-1CEST**: Nombre de la zona (Central European Time) y desfase base (UTC+1).
* **M3.5.0**: Cambio de horario de verano (Marzo, semana 5, Domingo).
* **M10.5.0/3**: Cambio de horario de invierno (Octubre, semana 5, Domingo a las 03:00).
  
### 5. ☁️ Cómo obtener tu API KEY de Clima

Para que la barra de notificaciones muestre la temperatura y el icono del tiempo, necesitas una llave gratuita de **OpenWeatherMap**:

1. Ve a [OpenWeatherMap.org](https://openweathermap.org/) y crea una cuenta gratuita.
2. Una vez logueado, ve a tu perfil y haz clic en **"My API Keys"**.
3. Genera una nueva Key (puedes llamarla "RetroPixel").
4. **IMPORTANTE:** La Key puede tardar entre **30 minutos y 2 horas** en activarse desde que se crea. Si el panel muestra "0.0C", simplemente espera un poco.
5. Copia esa clave en el apartado `API_KEY=` de tu archivo `config.ini`.

### 🔍 ¿Cómo comprobar si el código de ciudad es correcto?

Si quieres estar 100% seguro de que **OpenWeatherMap** reconoce tu ciudad antes de guardar el archivo en la Micro SD, puedes realizar esta prueba rápida en tu navegador:

1. Copia la siguiente dirección en la barra de tu navegador.
2. Sustituye las `Navalmoral de la Mata` por tu **Ciudad** real.
3. Sustituye las `XXXXX` por tu **API Key** real.

`http://api.openweathermap.org/data/2.5/weather?q=Navalmoral de la Mata,ES&appid=XXXXX`

* **Si el resultado es un texto con datos (JSON):** ¡El nombre es perfecto y el ESP32 lo leerá sin problemas!
* **Si el resultado es un error (401 o 404):** Revisa que tu API Key esté activa (recuerda que tarda hasta 2 horas en activarse) o que el nombre de la ciudad no tenga errores tipográficos.

### 6. ☁️ Actualización del Sistema (OTA)
Ya no es necesario conectar el panel al PC para actualizarlo. Si hay una nueva versión disponible en el repositorio:

1. Verifica que el WiFi esté configurado y activo en tu `config.ini`.
2. Accede al menú OSD del panel.
3. Navega hasta **Actualizacion > Buscar OTA**.
4. El sistema descargará el nuevo firmware desde GitHub y se reiniciará solo.

> [!WARNING]
> No desconectes la alimentación del panel durante el proceso de actualización.

### 7. 🌐 Guía del Sistema Multidioma (Archivos .json)

La versión v2.1.0 utiliza un sistema de **Diccionarios Dinámicos**. A diferencia de otros sistemas, el diccionario NO reside en la memoria RAM constantemente; solo se carga cuando el usuario entra en el menú y se libera al salir. Esto garantiza que el motor de GIFs tenga toda la memoria disponible para las animaciones.

#### 📂 Ubicación y Nomenclatura
Los archivos deben estar en la carpeta `/idioma/` de la tarjeta SD. El nombre del archivo (sin la extensión) es el que aparecerá en el menú de selección.

- `/idioma/ES.json` -> Aparecerá como "ES"
- `/idioma/EN.json` -> Aparecerá como "EN"

#### 🛠️ Estructura del Archivo JSON
Si deseas crear una traducción nueva, puedes copiar el archivo `ES.json` y renombrarlo. Los campos están organizados por bloques:

1. **`MENU`**: Etiquetas del menú principal.
2. **`SUBMENU_XXX`**: Etiquetas específicas de cada sección.
3. **`ESTADOS`**: Palabras cortas de estado (ON, OFF, SI, NO, VOLVER).
4. **`CONFIG_INI`**: Comentarios que se escribirán en el archivo de configuración físico de la SD.

#### ⚠️ Reglas Críticas para la Edición
Para asegurar que el sistema no sufra bloqueos (*Kernel Panic*) o errores visuales, sigue estas reglas:

* **🚫 Sin Acentos ni Ñ:** La fuente actual del sistema no soporta caracteres Unicode extendidos. Usa `n` en lugar de `ñ` y evita tildes (ej: `Actualizacion` en lugar de `Actualización`).
* **📏 Límite de Caracteres:** Las etiquetas de los submenús no deben superar los **21 caracteres** para asegurar un centrado perfecto en el área de 128px sin salirse de los márgenes.
* **🔡 Formato de Etiquetas:** En las secciones de submenú, incluye los dos puntos y el espacio si deseas que aparezcan (ej: `"modo": "Modo: "`).
* **💾 Formato UTF-8:** Asegúrate de guardar el archivo en formato **UTF-8 (sin BOM)** para evitar caracteres extraños al inicio de la lectura.

#### 🔄 Flujo de Carga
Cuando cambias el idioma en el OSD:
1. El sistema actualiza el valor `LANGUAGE` en el `config.ini`.
2. Se reinicia el puntero del diccionario.
3. La siguiente vez que abras el menú, el sistema buscará el archivo correspondiente a la nueva configuración.

### 8. 📂 Explorador SD (FTP)
Esta función activa un servidor de archivos inalámbrico en tu Retro Pixel LED. Su objetivo principal es facilitar el mantenimiento del sistema sin necesidad de extraer la tarjeta MicroSD.

> [!IMPORTANT]
> **Uso recomendado:** Esta función ha sido diseñada específicamente para gestionar **archivos de configuración (`config.ini`)**, **archivos de idioma (`ES.json`)**,edición de **playlists (`.txt`)** y archivos de pequeño tamaño. Debido a las limitaciones de ancho de banda del hardware ESP32, **no se recomienda para la transferencia masiva de colecciones de GIFs**, ya que el proceso sería extremadamente lento comparado con un lector de tarjetas convencional.

#### 🚀 Cómo activar el servidor FTP
1. Navega en el menú OSD hasta **Explorador SD**.
2. Selecciona la opción **Iniciar FTP**.
3. El panel detendrá la reproducción de GIFs y mostrará:
   * **Dirección IP:** (ej. `192.168.1.109`)

#### 💻 Configuración de conexión
Se recomienda utilizar un cliente como **FileZilla** con los siguientes datos:

* **Protocolo:** FTP Protocolo de transferencia de Archivos.
* **Servidor/Host:** La IP que aparece en tu panel LED.
* **Cifrado:** usar solo FTP plano.
* **Modo de Acceso:** Normal
* **Usuario:** `admin`
* **Contraseña:** `admin`
* **Puerto:** `21`
* **Modo de transferencia:** Predeterminado
* **Limitar nº de conexiones simultaneas :** Activado
* **Número máximo de conexiones:** 1
  
<img width="545" height="227" alt="image" src="https://github.com/user-attachments/assets/1b537615-3e39-48ba-9eb0-48b03931c5f9" />

<img width="544" height="193" alt="image" src="https://github.com/user-attachments/assets/ba4c85bc-920a-48c9-83d8-99b96ecbc57f" />

**En Edición -> Opciones -> Transferencias**
* **Máximo numero de trasnferencis simultaneas:** 1
* **Activar límites de velocidad:** Activado
* **Límite de descarga:** 20 KiB/s
* **Límite de carga:** 20 KiB/s
<img width="841" height="522" alt="image" src="https://github.com/user-attachments/assets/e90d3e84-9c93-45c0-b942-8b601db40041" />



---
Si no deseas instalar software adicional como FileZilla, puedes integrar la tarjeta SD del panel directamente en tu ordenador como si fuera una carpeta más usando el **Explorador de Archivos**: 

`(Esta opción no se recomienda, durante las pruebas algunas veces los archivos no se han cargado completos, provocando fallos)`

1. **Abrir el Explorador:** Ve a **Este equipo** en tu PC.
2. **Agregar Ubicación:** Haz clic derecho en cualquier zona blanca de la ventana y selecciona **"Agregar una ubicación de red"**.
3. **Configurar Dirección:** Cuando el asistente te solicite la dirección de red, introduce la IP que muestra tu panel con el prefijo FTP. 
   * Ejemplo: `ftp://192.168.1.109`
4. **Credenciales:** Desmarca la casilla de "Iniciar sesión de forma anónima" e indica el usuario: `admin`.
5. **Finalizar:** Asigna un nombre descriptivo a la unidad (ej. `Retro Pixel LED`) para identificarla fácilmente en el futuro.

#### ⚠️ Notas de seguridad y uso
* **Bloqueo de pantalla:** Mientras el FTP esté activo, el panel no reproducirá GIFs para dedicar toda la CPU a la transferencia de datos.
* **Salida segura:** Para cerrar el servidor y volver al modo normal, presiona el botón físico o utiliza la tecla "Validar" de tu mando IR.
* **Cuidado con el apagado:** No desconectes la alimentación mientras estés editando un archivo vía FTP, ya que el archivo podría quedar corrupto.

### 9. 🕹️ Integración con Batocera (Arcade)
Si queremos activar que Rretro Pixel LED lite muestre las marquesinas del juego que estamos lanzando en Batocera debes de activar en el menú la opción **Arcade**.
```
🏠 MENÚ PRINCIPAL
├── 📂 Reproducción
│   └── 🖼️ Modo: [GIFs / Reloj]
│   └── 🔀 Aleatorio: [SI / NO]
│   └── 🕹️ Arcade: [SI / NO]   <-- AQUÍ
│   └── 🔙 Volver
```
> [!IMPORTANT]
> ### 🕹️ Configuración de Batocera
> Para aprender a sincronizar tus ROMs, usar el script de PC e instalar los scripts de comunicación, consulta nuestra guía detallada:
> **[👉 HAZ CLIC AQUÍ PARA VER LAS INSTRUCCIONES DE BATOCERA](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_BATOCERA.md)**
---

## 🧠 Características Core LITE

* **📡 Control IR & Mapeado Dinámico:** Soporte completo para mandos infrarrojos con mapeado de funciones desde el menú OSD (Brillo, Navegación, Power Toggle y Confirmación).
* **📂 Servidor FTP de Mantenimiento:** Permite la gestión inalámbrica del archivo `config.ini` y listas de reproducción. Ideal para ajustes rápidos sin necesidad de extraer la MicroSD.
* **Anti-Panic RAM Management:** Sistema de vigilancia del *heap*. Si el DMA no puede asignar memoria tras la actividad del WiFi, el sistema cambia a single Buffer para garantizar estabilidad total.
* **Motor de Búsqueda Binaria (Arcade):** Capacidad para localizar marquesinas entre miles de archivos en milisegundos. El sistema no "escanea" carpetas, sino que salta directamente a la posición del archivo en la SD gracias a índices ordenados alfabéticamente.
* **Memoria Adaptativa (Single/Double Buffer):** Gestión inteligente de la RAM. El sistema utiliza *Double Buffer* para una fluidez total en GIFs, pero conmuta automáticamente a *Single Buffer* en modo Arcade para garantizar estabilidad total al cargar bitmaps de alta definición.
* **API HTTP en Tiempo Real:** Receptor de comandos integrado que permite la sincronización con sistemas externos como Batocera o RetroPie para el cambio dinámico de marquesinas.
* **Smart Text Centering:** Motor dinámico que alinea automáticamente menús y estados en el centro de la matriz (`offset + 64px`) calculando el ancho de cada cadena de texto.
* **WiFi Stealth Mode:** El ESP32 solo activa el WiFi brevemente para sincronizar la hora y el clima. El resto del tiempo el sistema permanece **100% offline**, garantizando **0 lag** en la reproducción de los GIFs.
* **Barra de Notificaciones Dinámica:** Si activas el clima, el reloj baja automáticamente su posición (`startY=9`) para mostrar el mensaje personalizado (`WEATHER_MSG`), el icono del tiempo y la temperatura.
* **Iconos en Bitmap:** Incluye iconos optimizados de 8x8 píxeles dibujados a mano para representar: Sol, Nubes, Lluvia, Nieve, Tormenta y Niebla.
* **Iconografía Avanzada (Día/Noche):** Incluye iconos de 8x8 píxeles dibujados a mano que representan: Sol, Luna (Noche), Nubes, Lluvia, Nieve, Tormenta y Niebla, adaptándose dinámicamente según la fase horaria.
* **Sistema de Playlists Dinámicas:** Sustituye el antiguo motor de lista única. Ahora el sistema puede gestionar múltiples archivos `.txt` en la carpeta `/playlists/`, permitiendo saltar entre colecciones temáticas (Arcade, Consolas, Favoritos, etc.) desde el menú OSD.
* **Reloj Auto-Interrupción:** El panel interrumpe la galería cada "x" GIFs para mostrar la hora durante "x" segundos (ambos configurables desde el menú OSD y en config.ini), retomando la reproducción exactamente donde se quedó.
* **Resiliencia Offline:** Si no hay WiFi disponible, el sistema ignora la sincronización y comienza a reproducir GIFs inmediatamente usando el reloj interno del chip.
* **Motor de Renderizado Double Buffer:** Aprovecha el DMA del ESP32 para dibujar frames de forma invisible, logrando una fluidez absoluta y eliminando cualquier rastro de parpadeo en las animaciones.

## 🛒 Lista de Materiales

Para garantizar la compatibilidad, se recomienda el uso de los componentes probados durante el desarrollo:

* **Microcontrolador:** [ESP32 DevKit V1 (30 pines) - AliExpress](https://es.aliexpress.com/item/1005005704190069.html)
* **Panel LED Matrix (HUB75):** [P2.5 / P4 RGB Matrix Panel - AliExpress](https://es.aliexpress.com/item/1005008479388445.html)
* **Lector de Tarjetas:** [Módulo Adaptador Micro SD (SPI) - AliExpress](https://es.aliexpress.com/item/1005005591145849.html)
* **Placa conexión ESP32-Panel LED:** [DMDos Board V3 - Mortaca ](https://www.mortaca.com/) (Opcional, no hay que soldar y tiene lector SD incroporado)
* **Receptor de IR:** [Sensor de receptor de infrarrojos Universal - AliExpress](https://es.aliexpress.com/item/1005005343424296.html)
* **Pulsador:** [Interruptor momentáneo elegir DS-316 - AliExpress](https://es.aliexpress.com/item/4000888761296.html)
* **Alimentación:** Fuente de alimentación de 5V (Mínimo 2A recomendado para paneles de 64x32).

---
## ⚙️ Instalación

### 1. 🔌 Conexiones 
Si utilizas DMDos Board V3 esta parte ya la tienes, salta al siguiente punto.

#### 📂 Lector de Tarjeta Micro SD (Interfaz SPI)
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

#### 🕹️ Control de Usuario Menú OSD (Físico e Infrarrojo)

El sistema permite el control total mediante un pulsador físico (con lógica de pulsación larga) y un receptor IR para manejo a distancia.

| Componente | Pin ESP32 | Función |
| :--- | :--- | :--- |
| **Botón (PIN)** | GPIO 21 | **Multifunción:** Click (Navegar) / Long Press (Confirmar - Power Toggle). |
| **Botón (GND)** | GND | Referencia de tierra. |
| **Receptor IR (Data)** | GPIO 34 | Entrada de señal (Protocolo NEC/etc). |
| **Receptor IR (VCC)** | 3.3V | Alimentación del sensor. |
| **Receptor IR (GND)** | GND | Referencia de tierra. |

<img width="769" height="716" alt="image" src="https://github.com/user-attachments/assets/11fef006-59f3-405f-b00a-a32c9bba7bc5" />


---

## 🛠️ Hoja de Ruta (Roadmap LITE)

### ⚡ Optimización & Funcionalidad

### 🎨 Estética & Conectividad

---

## ⚖️ Licencia y Agradecimientos

Este proyecto se publica bajo la **Licencia MIT**.

Agradecimientos especiales a los desarrolladores de las librerías base:
* **Bitbank2** por la excelente librería `AnimatedGIF`.
* **Mrfaptastic** por el motor DMA de alto rendimiento para matrices.
* **Comunidad Telegram DMDos** al encontrarla y ver de lo que era capáz DMDos me animé a desarrollar **Retro Pixel LED**.
* **RpiTe@m** por compartir el pack de 600 GIFs **gratis** y su increíble recopilación de 11000 GIFs que puedes adquirirla [Aquí.](https://www.neo-arcadia.com/forum/viewtopic.php?t=67065)
* **shan-aya** por la traducción al Francés y su magnifico soft para crear [GIFs.](https://github.com/shan-aya/DMD_GIF_converter)
