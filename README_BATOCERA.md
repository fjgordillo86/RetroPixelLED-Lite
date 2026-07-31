# 🕹️ Integración con Batocera (Modo Arcade Lite)

**¡¡¡¡ EN CONSTRUCCIÓN PARA LA NUEVA VERSIÓN 3.1.0 !!!!**

El **Modo Arcade** en la versión Lite permite que tu matriz LED funcione como una marquesina dinámica. El panel detectará el sistema y juego por el que estás navegando y te lo mostrará automáticamente.

#### Aprovechamiento de Recursos (Scraping)
La principal ventaja de este sistema es que **utiliza las imágenes que ya has scrapeado en Batocera** (marquesinas/wheel art). El script de PowerShell se encarga de buscarlas, redimensionarlas y convertirlas automáticamente.

## 1. Preparación de Assets (Script de PowerShell)
Para que el ESP32 encuentre los archivos de forma instantánea entre miles de juegos. Para ello, los archivos deben estar indexados y procesados correctamente.

### 1.1 🛠️ Cómo usar el Script (Ejecutar Script Marquesinas)
El script se encuentra en la carpeta `Arcade/Marquesinas/` del proyecto [aquí](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Marquesinas). Consta de dos archivos `Ejecutar Script Marquesinas Batocera.bat` y `Script Marquesinas Batocera.ps1`.

1.  **Conecta la SD** de tu Retro Pixel LED al PC.
2.  **Ejecuta el archivo** ``Ejecutar Script Marquesinas Batocera.bat` (Lanzador para evitar bloqueos de Windows).
3.  **Configuración de rutas:**
    * **Origen:** Introduce la ruta de tus ROMs de Batocera (ej: `\\192.168.1.119\share\roms`).
    * **Destino:** Introduce la ruta `C:\marquesinas`.
4.  **Selección de Sistema:** El script detectará automáticamente qué sistemas tienen un archivo `gamelist.xml`. Puedes elegir procesar uno solo por su número, varios o **Todos (0)**.
5.  **Copiar:** Si seleccionaste la ruta `C:\marquesinas` copia la carpeta  `marquesinas` y todo su contenido en la SD o SSD donde está instalado Batocera `roms/`, como se indica en siguiente punto.

<img width="1111" height="619" alt="image" src="https://github.com/user-attachments/assets/cac94465-3382-477c-98e8-b73e5de22939" />

### ¿Qué hace el script automáticamente?
* **Redimensionado:** Convierte tus marquesinas originales a **128x32 píxeles**.
* **Formato:** Fuerza el color a **BMP de 24 bits** (formato compatible con el driver DMA del ESP32).
* **Índices .txt:** Genera archivos de texto (ej: `neogeo.txt`) ordenados alfabéticamente. Estos archivos son los que lee el ESP32 para saber qué archivos existen sin explorar toda la SD.

> [!CAUTION]
> **Acceso por Red (Samba):**
> Si al intentar acceder a la ruta `ej-> \\192.168.1.119\share\roms` Windows te solicita credenciales, utiliza las que trae Batocera por defecto:
> * **Usuario:** `root`
> * **Contraseña:** `linux`

## 3. Estructura de archivos en la SD o SSD de Batocera


Para que la integración funcione correctamente, debemos de pegar la carpeta marquesinas en la carpeta roms/ (es donde tenemos todos los juegos)
* **`roms/marquesinas/Arcade/sistema/rom_name.bmp`** (Marquesina del juego procesada, ej: `mslug.bmp`)



#### Ejemplo visual de carpetas:
```
📂 roms/
├── 📂 marquesinas/
│   └── 📂 Arcade/
│       └── 📂 neogeo/
│       │   ├── 📄 mslug.bmp
│       │   └── 📄 kof98.bmp
│       └── 📂 mame/
│       │   ├── 📄 pacman.bmp
│       │   └── 📄 tetris.bmp
```
## 4. Instalación Automática en Batocera

A partir de la versión **v3.0.0**, ya no es necesario editar líneas de código a mano, preocuparse por los formatos de archivo de Windows o utilizar consolas SSH avanzadas (como PuTTY) para configurar los permisos de ejecución. 

Hemos desarrollado un **Script Instalador Inteligente en PowerShell** que realiza todo el despliegue de forma automática desde tu PC.

---

### 📦 ¿Qué hace este instalador por ti?

* **Configuración de IP:** Inyecta automáticamente la dirección IP de tu panel LED en todos los scripts de comunicación.
* **Corrección de Formato:** Fuerza el formato de fin de línea **Unix (LF)**. Esto evita que los scripts fallen si fueron abiertos por error con el Bloc de Notas de Windows.
* **Organización de Archivos:** Crea la estructura de directorios necesaria en Batocera (`game-start` y `game-end`) y copia los archivos en su lugar correspondiente.
* **Auto-Permisos (Sin PuTTY):** Genera un script del sistema (`custom.sh`) que hace que Batocera se otorgue a sí mismo permisos de ejecución (`chmod +x`) sobre las carpetas del panel en cada arranque.

---

### 🛠️ Requisitos Previos

1. Tener tu **PC** y tu **Batocera** conectados a la misma red local (o conectar el almacenamiento físico de Batocera directamente al PC).
2. Conocer la **IP local de tu panel LED** Retro Pixel LED (ej. `192.168.1.109`).
3. Descargar la carpeta completa `Auto Instalador Batocera` desde este repositorio, lo puedes encontar [aquí](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Batocera/Instalador%20Autom%C3%A1tico).

> [!IMPORTANT]
> Si descargaste el repositorio en un archivo `.zip`, asegúrate de **descomprimirlo por completo** antes de ejecutar el instalador.

---

### 💻 Paso a Paso

1. Abre la carpeta `Instalador Automático` en tu PC. Dentro encontrarás cuatro archivos:
   * `Ejecutar Script Instalador Batocera.bat`
   * `Script_Instalador_Batocera.ps1`
   * `pixel_start.sh`
   * `pixel_stop.sh`

3. Haz **clic** sobre `Ejecutar Script Instalador Batocera.bat`.

4. Sigue las instrucciones en la ventana de la consola:
   * **Paso 1:** Introduce la IP de tu panel LED y pulsa `Enter`.
   * **Paso 2:** Introduce la ruta de tu Batocera. Puede ser una ruta de red (ej: `\\192.168.1.119` o `\\BATOCERA`) o la letra de una unidad física si conectaste el disco/SD al PC (ej: `E:`).

5. El script procesará los archivos en un segundo. Al finalizar, verás el mensaje `🎉 ¡INSTALACIÓN COMPLETADA!`. Pulsa cualquier tecla para salir.

<img width="1110" height="373" alt="image" src="https://github.com/user-attachments/assets/cf5f3ac3-5906-4071-ac0a-45c11341c896" />

7. **Reinicia tu sistema Batocera por completo.**
> [!CAUTION]
> El reinicio completo del sistema es **obligatorio**. Durante este arranque, el script `custom.sh` configurará los permisos internos. A partir de ese momento, cada vez que lances o cierres un juego, el panel reaccionará automáticamente.

## 5. Funcionamiento en Tiempo Real

* **Al lanzar un juego:** Batocera envía el sistema y el nombre de la ROM. El panel busca en el índice y muestra la marquesina correspondiente.
* **Al salir del juego:** Batocera envía el comando `STOP`. El panel interrumpe el modo Arcade y vuelve automáticamente a la reproducción de **GIFs** o al **Reloj**.
* **Gestión de Errores:** Gracias a la lógica de cascada, si el juego es nuevo y aún no has procesado su BMP, el panel mostrará el logo del sistema, manteniendo siempre una estética profesional en tu recreativa.

> [!CAUTION]
> Cada vez que añadas nuevos juegos o hagas un "Scrape" en Batocera, **debes volver a ejecutar el script de PowerShell** en tu PC para actualizar los índices y las imágenes en la SD. Sin este paso, el ESP32 no sabrá que los nuevos archivos existen.

## 6. Configuración Crítica: IP Fija para el ESP32

Para que el modo **🕹️ Arcade** de Batocera funcione siempre correctamente, es fundamental que el ESP32 mantenga siempre la misma dirección IP.

> [!TIP]
> **Asignar IP fija al ESP32:** > Los scripts de Batocera envían las órdenes (como cambiar el GIF al lanzar un juego) a una dirección IP específica que tú configuras manualmente. Si el router reinicia y le asigna una IP distinta al ESP32, la comunicación se cortará y el panel dejará de actualizarse.
>
> **¿Cómo hacerlo?**
> 1. Accede a la configuración de tu router.
> 2. Busca la sección de **DHCP Estático** o **Asignación de IP por MAC**.
> 3. Vincula la dirección MAC de tu ESP32 con la IP que hayas escrito en tus scripts (ej: `192.168.1.109`).
> 4. Dado que cada router es diferente, si tienes dudas busca en Google: *"Cómo asignar IP fija [modelo de tu router]"*.

## 7. ¡Disfruta de las marquesinas mientras juegas en tu Arcade!
