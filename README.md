# ✨ Retro Pixel LED Lite v1.0.0

### **[✈️ Unirse al Grupo de Telegram: Retro Pixel LED](https://t.me/RetroPixelLed)**

## 💡 Descripción del Proyecto

**Retro Pixel LED Lite** es la versión de alto rendimiento diseñada para quienes buscan estabilidad absoluta, velocidad instantánea y un sistema libre de mantenimiento. A diferencia de la versión estándar, el firmware LITE elimina la carga del servidor web y la conectividad permanente para dedicar el 100% de la potencia del ESP32 al renderizado de GIFs.

Es la solución perfecta para marquesinas fijas, salones arcade o decoración retro donde solo quieres **encender y disfrutar**.

> [!TIP]
> **🚀 Filosofía Lite:** Menos es más. Al apagar el WiFi después de sincronizar la hora, el sistema elimina el lag, reduce el calor del chip y evita cuelgues por saturación de red, permitiendo reproducciones fluidas de colecciones masivas.

---

## 🚀 Diferencias Clave: Lite vs Estándar

| Característica | Versión Lite | Versión Estándar |
| :--- | :--- | :--- |
| **Arranque** | Instantáneo (Lectura de `lista.txt`) | Lento (Indexado de carpetas SD) |
| **Conectividad** | WiFi Sync & Sleep (Solo para hora) | Online Permanente (Web + MQTT) |
| **Configuración** | Archivo `config.ini` en la SD | Interfaz Web UI |
| **Límite de GIFs** | Ilimitado (+10.000 sin problemas) | Ilimitado (vía Caché SD) |
| **Estabilidad** | Máxima (Sistema aislado) | Alta (Depende del tráfico WiFi) |
| **Reloj** | Intermitente automático | Manual y Automático |

---

## 🛠️ Herramientas Exclusivas Lite

### 📜 Generador de Lista (Script Listar GIFs v1.0.0)
Para evitar que el ESP32 pierda tiempo escaneando la SD, utilizamos un indexador externo.
* **Ubicación en el repo:** `/Contenido SD/`
* **Destino:** El script debe copiarse y ejecutarse siempre desde la **raíz de la Micro SD**.
* **Función:** Escanea la carpeta `/gifs/` y genera el archivo `lista.txt` con las rutas exactas. Incluye un contador en tiempo real para confirmar el progreso en colecciones gigantes.

#### 🪟 Para Windows (`.bat`)
1. Copia `Listar GIFs v1.0.0.bat` a la raíz de tu SD.
2. Haz **doble clic** sobre el archivo.
3. Se abrirá una ventana de consola mostrando el progreso. Al terminar, pulsa cualquier tecla para cerrar.

#### 🍎 Para macOS / Linux (`.sh`)
1. Copia `Listar GIFs v1.0.0.sh` a la raíz de tu SD.
2. Abre la **Terminal** y accede a la SD (escribe `cd ` y arrastra la carpeta de la SD a la terminal).
3. Otorga permisos de ejecución (solo la primera vez):
   ```bash
   chmod +x "Listar GIFs v1.0.0.sh"
   ```
4. Ejecuta el script:
   ```bash
   ./"Listar GIFs v1.0.0.sh"
   ```
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
├── gifs/                     <-- Tus carpetas con GIFs (Arcade, Consolas, etc.)
├── config.ini                <-- Configuración de WiFi y Panel.
├── lista.txt                 <-- Generado automáticamente por el .bat
└── Listar GIFs v1.0.0.bat    <-- Ejecútalo siempre que añadas GIFs nuevos.
```
>[!IMPORTANT]
>El archivo lista.txt es el mapa que utiliza el ESP32 para saber qué reproducir. Si añades, borras o mueves GIFs dentro de la carpeta /gifs/, asegúrate de ejecutar el script **Listar GIFs v1.0.0** de nuevo para actualizar el índice.

### 3. 📝 Configuración via `config.ini`
Modifica el archivo de texto llamado `config.ini` en la raíz de la SD para dejar Retro Pixel LED Lite a tu gusto:

```ini
# ============================================================
# 🕹️ RETRO PIXEL LED LITE v1.0.0 - ARCHIVO DE CONFIGURACIÓN
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

# Refresco Mínimo (Hz): 30 a 120
REFRESH_MIN=60

# Anti-Ghosting (Latch Blanking): 1 a 4 (Sube si ves brillo fantasma)
LATCH_BLANK=1

[LOGIC]
# Activa o desactiva el reloj: 0=OFF (No usa WiFi), 1=ON
CLOCK_ENABLE=0

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

[END]
```
## 🛒 Lista de Materiales

Para garantizar la compatibilidad, se recomienda el uso de los componentes probados durante el desarrollo:

* **Microcontrolador:** [ESP32 DevKit V1 (38 pines) - AliExpress](https://es.aliexpress.com/item/1005005704190069.html)
* **Panel LED Matrix (HUB75):** [P2.5 / P3 / P4 RGB Matrix Panel - AliExpress](https://es.aliexpress.com/item/1005007439017560.html)
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
---

## 🧠 Características Core LITE

* **WiFi Stealth Mode:** Al arrancar, el ESP32 se conecta al WiFi durante 5 segundos para obtener la hora (NTP). Una vez sincronizado, **apaga el WiFi por completo** para eliminar interferencias y calor.
* **Motor de Lista Plana:** En lugar de navegar por directorios complejos, lee directamente desde `lista.txt`. Esto permite saltos de archivo en milisegundos sin latencia.
* **Reloj Auto-Interrupción:** El panel interrumpe la galería cada "X" GIFs para mostrar la hora durante 10 segundos, retomando la reproducción exactamente donde se quedó.
* **Resiliencia Offline:** Si no hay WiFi disponible, el sistema ignora la sincronización y comienza a reproducir GIFs inmediatamente usando el reloj interno del chip.

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
* **Comunidad Telegram DMDos** por la increíble recopilación de GIFs que dan vida a este proyecto.
