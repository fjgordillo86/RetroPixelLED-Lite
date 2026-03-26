# ✨ Retro Pixel LED Lite v2.0.0

### **[✈️ Unirse al Grupo de Telegram: Retro Pixel LED para estár al día de las actualizaciones](https://t.me/RetroPixelLed)**

## 💡 Descripción del Proyecto

**Retro Pixel LED Lite** es la versión de alto rendimiento diseñada para quienes buscan estabilidad absoluta, velocidad instantánea y un sistema libre de mantenimiento. A diferencia de la versión estándar, el firmware LITE elimina la carga del servidor web y la conectividad permanente para dedicar el 100% de la potencia del ESP32 al renderizado de GIFs. 

La versión **2.0.0** supone una revolución con la integración de un Menú OSD (On-Screen Display) nativo, que permite al usuario navegar por las listas de reproducción, ajustar el brillo, configurar el reloj y gestionar la conectividad WiFi.... directamente desde el panel LED, sin necesidad de dispositivos externos.

Es la solución perfecta para marquesinas fijas, salones arcade o decoración retro donde solo quieres **encender y disfrutar**.

> [!TIP]
> **🚀 Filosofía Lite:** Menos es más. Al apagar el WiFi después de sincronizar la hora y el tiempo, el sistema elimina el lag, reduce el calor del chip y evita cuelgues por saturación de red, permitiendo reproducciones fluidas de colecciones masivas.

Si quieres probar la versión estandar aquí tienes el enlace al **[GitHub.](https://github.com/fjgordillo86/RetroPixelLED)**

---
## 🆕 Novedades de la Versión v2.0.0 Lite

| Característica | Detalle Técnico | Beneficio |
| :--- | :--- | :--- |
| **🖥️ Native OSD Menu** | Interfaz visual renderizada directamente en el panel LED. | **Control Total.** Ajusta brillo, WiFi y Playlists con un solo botón sin usar el PC. |
| **🖼️ Double Buffering** | Implementación de doble buffer de memoria en el stack DMA. | **Cero Parpadeo.** Eliminación total del flickering y scrolls laterales ultra fluidos a 120Hz. |
| **🌙 Dynamic Night Mode** | Integración astronómica con OpenWeatherMap (campo `icon`). | **Estética Realista.** El panel muestra iconos de Luna y tonos fríos automáticamente al anochecer. |
| **🧠 Smart RAM Refresh** | Lógica de Soft-Reset tras actualizaciones de clima/hora. | **Estabilidad 24/7.** Evita la fragmentación de memoria al usar Double Buffer, garantizando 0 cuelgues. |
| **🔌 Auto-Playlist P&P** | Escaneo automático de la carpeta `/playlists` al arranque. | **Plug & Play.** El panel funciona de forma autónoma desde el primer segundo, incluso sin botones. |
| **📶 WiFi Stealth Mode** | Gestión radical del stack WiFi (Active/Sleep selectivo). | **Cero Lag.** El radio se apaga tras sincronizar, dedicando el 100% del CPU al renderizado de GIFs. |
| **💾 SD Persistence** | Guardado de ajustes en `config.ini` tras cada cambio en el OSD. | **Memoria Persistente.** El panel recuerda tu brillo, modo de red y playlist elegida tras apagarlo. |
---

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
Formatea tu MicroSD en **FAT32** añade los archivos Listar GIFs v1.0.0.bat y config.ini quedando organiza la  Micro SD de la siguiente manera:

```text
/ (Raíz de la SD)
├── gifs/                        <-- Tus carpetas con GIFs (Arcade, Consolas, etc.)
├── playlists/                   <-- Aquí estarán las listas generadas por el script "Generador de Playlists".
│   ├── Mis Favoritos.txt        <-- Lista .txt.
│   ├── Metal Slug.txt           <-- Lista .txt.
│   └── Todos.txt                <-- Lista .txt.
├── config.ini                   <-- Configuración de WiFi y Panel.
└── Generador de Playlists.bat   <-- Script para generar las Playlist.
```
>[!IMPORTANT]
>Si añades, borras o mueves GIFs dentro de la carpeta /gifs/, asegúrate de ejecutar el script **Generador de Playlists.bat** de nuevo para actualizar el índice.

### 3. 📝 Configuración via `config.ini`
Modifica el archivo de texto llamado `config.ini` en la raíz de la SD para dejar Retro Pixel LED Lite a tu gusto:

```ini
# ============================================================
# 🕹️ RETRO PIXEL LED LITE v2.0.0 - ARCHIVO DE CONFIGURACIÓN
# ============================================================
# Nota: No dejes espacios alrededor del símbolo '='.
# Ejemplo correcto: BRIGHTNESS=40

[WIFI_NTP]
# Configura tu red solo si vas a usar el reloj (CLOCK_ENABLE=1)
SSID=Nombre_De_Tu_Red
PASS=Password_De_Tu_Red
TZ=CET-1CEST,M3.5.0,M10.5.0/3

[HARDWARE]
PANEL_CHAIN=2     # Número de paneles en cascada
BRIGHTNESS=40    # Brillo general (0 a 255)

# Velocidad I2S: 0=8MHz, 1=10MHz, 2=16MHz, 3=20MHz (Turbo)
I2S_SPEED=2

# Refresco Mínimo (Hz): 30 a 1240
REFRESH_MIN=120

# Doble Buffer Activa o desactiva esta función: 0=OFF, 1=ON (Elimina parpadeos)
DOUBLE_BUFF=1

# Anti-Ghosting (Latch Blanking): 1 a 4 (Sube si ves brillo fantasma)
LATCH_BLANK=1

[LOGIC]
# Activa o desactiva el reloj: 0=OFF (No usa WiFi), 1=ON
CLOCK_ENABLE=1

# Modo de reproducción: 0=Secuencial (Sigue lista.txt), 1=Aleatorio
RANDOM_MODE=1

# Intervalo: Cada cuántos GIFs aparece el reloj (ej: 5)
AUTO_CLOCK_INT=5

# Duración: Cuántos segundos se muestra el reloj
CLOCK_DURATION=10

# Estilos de Reloj:
# 0: Matrix (Verde clásico)
# 1: Solid (Azul sólido)
# 2: Rainbow (Colores cambiantes)
# 3: Pulse (Efecto respiración)
# 4: Gradient (Degradado premium)
CLOCK_STYLE=2

# Color del Reloj (Formato HEX)
# Usado en estilos Solid, Pulse y Gradient.
CLOCK_COLOR=#FF0055

[WEATHER]
# Activa el clima: 0=OFF, 1=ON (Requiere CLOCK_ENABLE=1)
WEATHER_ENABLE=1

# Tu ciudad (Sin espacios, usa '+' si es necesario: Azuaga,ES o Navalmoral+de+la+Mata,ES)
CITY=Azuaga,ES

# Tu API Key gratuita de OpenWeatherMap
API_KEY=xxxxxxxxxxxxxxxxxxxxxxx

# Intervalo de actualización en MINUTOS (Recomendado: 60)
# El ESP32 encenderá el WiFi brevemente solo para esto.
WEATHER_INT=60

# Texto que se muestra encima del reloj a modo de notificación 
# |Game Room ☀️21ºC|
# |  14 : 20 : 56  |
WEATHER_MSG=Game Room

[END]
```
### 4. 🖥️ Estructura del Menú OSD (Navegación)

El sistema se controla mediante un **único botón**. La navegación es intuitiva:
* **Pulsación Corta (0.5 a 1 seg):** Acceder al menú OSD o moverse por las opciones (Bajar).
* **Pulsación Larga (1.5 a 3 seg):** Entrar en un submenú o confirmar una selección.
* **Pulsación super Larga (+10 seg):** Reset.

```text
🏠 MENÚ PRINCIPAL
├── 📂 Playlists
│   ├── 📄 Favoritos
│   ├── 📄 Arcade
│   ├── 📄 ...
│   └── 🔙 Volver
├── 📂 Reproducción
│   └── 🔀 Aleatorio: [SI / NO]
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
├── ⚙️ Ajustes Avanzados
│   ├── ⚡ I2sSeep: [8, 10, 16, 20MHz]
│   ├── 🔄 Refresco: [30, 60, 90, 120Hz]
│   ├── 🖼️ Buffer: [SI / NO]
│   ├── 👻 AntiGhot: [1, 2, 3, 4]
│   ├── ⚠️ Reset:
│   └── 🔙 Volver
├── 💾 Guardar
└── 🔙 Salir
```

## ☁️ Cómo obtener tu API KEY de Clima

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
---

## 🧠 Características Core LITE

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

* **Microcontrolador:** [ESP32 DevKit V1 (38 pines) - AliExpress](https://es.aliexpress.com/item/1005005704190069.html)
* **Panel LED Matrix (HUB75):** [P2.5 / P4 RGB Matrix Panel - AliExpress](https://es.aliexpress.com/item/1005007439017560.html)
* **Lector de Tarjetas:** [Módulo Adaptador Micro SD (SPI) - AliExpress](https://es.aliexpress.com/item/1005005591145849.html)
* **Placa conexión ESP32-Panel LED:** [DMDos Board V3 - Mortaca ](https://www.mortaca.com/) (Opcional, no hay que soldar y tiene lector SD incroporado)
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

#### 🕹️ Botón momentáneo (pulsador) de Control (Menú OSD)
| Componente | Pin ESP32 | Función |
| :--- | :--- | :--- |
| **Botón (PIN)** | GPIO 21 | Entrada de señal (Pull-Up interno) |
| **Botón (GND)** | GND | Referencia de tierra |

<img width="652" height="609" alt="Pulsador" src="https://github.com/user-attachments/assets/7b2ad821-e369-498a-a9cf-b1fac93472de" />

---

## 🛠️ Hoja de Ruta (Roadmap LITE)

### ⚡ Optimización

### 🎨 Estética

---

## ⚖️ Licencia y Agradecimientos

Este proyecto se publica bajo la **Licencia MIT**.

Agradecimientos especiales a los desarrolladores de las librerías base:
* **Bitbank2** por la excelente librería `AnimatedGIF`.
* **Mrfaptastic** por el motor DMA de alto rendimiento para matrices.
* **Comunidad Telegram DMDos** al encontrarla y ver de lo que era capáz DMDos me animé a desarrollar **Retro Pixel LED**.
* **RpiTe@m** por la increíble recopilación de [GIFs.](https://www.neo-arcadia.com/forum/viewtopic.php?t=67065)
