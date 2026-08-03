# 🕹️ Integración con Recalbox

El **Modo Arcade** en la versión Lite permite que tu matriz LED funcione como una marquesina dinámica. El panel detectará el sistema y juego por el que estás navegando y te lo mostrará automáticamente.

#### Aprovechamiento de Recursos (Scraping)
La principal ventaja de este sistema es que **utiliza las imágenes que ya has scrapeado en Recalbox** (marquesinas/wheel art). El script de PowerShell se encarga de buscarlas, redimensionarlas y convertirlas automáticamente.

## 1. Configuración Crítica: IP Fija para el ESP32

Para que el modo **🕹️ Arcade** de Batocera funcione siempre correctamente, es fundamental que el ESP32 mantenga siempre la misma dirección IP.

> [!TIP]
> **Asignar IP fija al ESP32:** > Los scripts de Batocera envían las órdenes (como cambiar el GIF al lanzar un juego) a una dirección IP específica que tú configuras manualmente. Si el router reinicia y le asigna una IP distinta al ESP32, la comunicación se cortará y el panel dejará de actualizarse.
>
> **¿Cómo hacerlo?**
> 1. Accede a la configuración de tu router.
> 2. Busca la sección de **DHCP Estático** o **Asignación de IP por MAC**.
> 3. Vincula la dirección MAC de tu ESP32 con la IP que hayas escrito en tus scripts (ej: `192.168.1.117`).
> 4. Dado que cada router es diferente, si tienes dudas busca en Google: *"Cómo asignar IP fija [modelo de tu router]"*.

## 2. Instalación Automática en Batocera

A partir de la versión **v3.0.0**, ya no es necesario editar líneas de código a mano, preocuparse por los formatos de archivo de Windows o utilizar consolas SSH avanzadas (como PuTTY) para configurar los permisos de ejecución. 

He desarrollado un **Script Instalador Inteligente en PowerShell** que realiza todo el despliegue de forma automática desde tu PC.

---

### 📦 ¿Qué hace este instalador por ti?

* **Configuración de IP:** Inyecta automáticamente la dirección IP de tu panel LED en todos los scripts de comunicación.
* **Corrección de Formato:** Fuerza el formato de fin de línea **Unix (LF)**. Esto evita que los scripts fallen si fueron abiertos por error con el Bloc de Notas de Windows.
* **Organización de Archivos:** Crea la estructura de directorios necesaria en Batocera y copia los archivos en su lugar correspondiente.
* **Auto-Permisos (Sin PuTTY):** Genera un script del sistema (`custom.sh`) que hace que Batocera se otorgue a sí mismo permisos de ejecución (`chmod +x`) sobre las carpetas en cada arranque.

---

### 🛠️ Requisitos Previos

1. Tener tu **PC** y tu **Recalbox** conectados a la misma red local (o conectar el almacenamiento físico de Batocera directamente al PC).
2. Conocer la **IP local de tu panel LED** Retro Pixel LED (ej. `192.168.1.117`).
3. Descargar la carpeta completa `Instalador Automático` desde este repositorio, la puedes encontrar [aquí](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Instalador%20Automatico).

> [!IMPORTANT]
> Si descargaste el repositorio en un archivo `.zip`, asegúrate de **descomprimirlo por completo** antes de ejecutar el instalador.

---

### 💻 Paso a Paso

1. Abre la carpeta `Instalador Automatico` en tu PC. Dentro encontrarás dos archivos y dos carpetas:
   * `Ejecutar Script Instalador Arcade.bat`
   * `Script_Instalador_Arcade.ps1`
   * `Batocera`
   * `Recalbox`

2. Haz **clic** sobre `Ejecutar Script Instalador Arcade.bat`.

3. Sigue las instrucciones en la ventana de la consola:
   * **Paso 1:** Introduce la IP de tu panel LED y pulsa `Enter`.
   * **Paso 2:** Introduce la ruta de tu Recalbox. Puede ser una ruta de red (ej: `\\192.168.1.118`) o la letra de una unidad física si conectaste el disco/SD al PC (ej: `E:`).

4. El script nos solicitará:
   * El sistema que utilizamos, seleccionaremos 2 Recalbox.
   * Que modo de funcionamiento deseas activar?
     * Opción 1: Menús y Juegos (Muestra sistemas al navegar + juego lanzado)
     * Opción 2: Solo Juegos (Marquesina fija/reloj en menús, cambia solo al jugar)
5. El script procesará los archivos en un segundo. Al finalizar, verás el mensaje `INSTALACIÓN COMPLETADA!`. Pulsa cualquier tecla para salir.

<img width="1102" height="532" alt="image" src="https://github.com/user-attachments/assets/3e367c0a-d305-475e-95c9-ed9d3ae352e9" />


6. **Reinicia tu sistema Recalbox por completo.**
> [!CAUTION]
> El reinicio completo del sistema es **obligatorio**. A partir de ese momento, cada vez que navegues por el menú, lances o cierres un juego, el panel reaccionará automáticamente.

### 3. 🛠️ Marquesinas.
Usaremos el script se encuentra en la carpeta `Arcade/Marquesinas/` del proyecto [aquí](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Marquesinas). Consta de dos archivos `Ejecutar Script Marquesinas Recalbox.bat` y `Script Marquesinas Recalbox.ps1`.

1.  **Ejecuta el archivo** ``Ejecutar Script Marquesinas Recalbox.bat` (Lanzador para evitar bloqueos de Windows).
2.  **Configuración de rutas:**
    * **Origen:** Introduce la ruta de tus ROMs de Recalbox (ej: `\\192.168.1.118\share\roms`).
    * **Destino:** Introduce la ruta `C:\marquesinas`.
3.  **Selección de Imagen:** Selecciona el tipo de imagen a usar para las marquesinas.
4.  **Selección de Sistema:** El script detectará automáticamente qué sistemas tienen un archivo `gamelist.xml`. Puedes elegir procesar uno solo por su número, varios o **Todos (0)**.
5.  **Copiar:** Si seleccionaste la ruta `C:\marquesinas` copia la carpeta  `marquesinas` y todo su contenido en la SD o SSD donde está instalado Recalbox `share\`, como se indica en el punto `5. Estructura de archivos en la SD o SSD de Recalbox`.

<img width="1098" height="630" alt="image" src="https://github.com/user-attachments/assets/f2a99ce2-0b83-40cc-84bc-d24962f2c83e" />

### ¿Qué hace el script automáticamente?
* **Redimensionado:** Convierte tus marquesinas originales a **128x32 píxeles**.
* **Formato:** Fuerza el color a **BMP de 24 bits** (formato compatible con el driver DMA del ESP32).

> [!CAUTION]
> **Acceso por Red (Samba):**
> Si al ejecutar el script no tiene acceso a la ruta indicada tendrás que acceder mediante el explorador de archivos y logearte con los credenciales de Recalbox para que el script tenga acceso a la carpeta.
> Acceder a la ruta `ej-> \\192.168.1.120\share\roms` Windows te solicita credenciales, utiliza las que trae Batocera por defecto:
> * **Usuario:** `root`
> * **Contraseña:** `recalboxroot`

> [!CAUTION]
> Cada vez que añadas nuevos juegos o hagas un "Scrape" en Recalbox, **debes volver a ejecutar el script de PowerShell** en tu PC para actualizar los índices y las imágenes. Sin este paso, el ESP32 no sabrá que los nuevos archivos existen.

 ### 4. 🛠️ Logos de Sistemas.
 Podemos usar los logos ya redimensionados que se encuentran en la carpeta `Arcade/Logos Sistemas/` del proyecto [aquí](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas).
 1.  **Copiar:** Copia la carpeta  `Logos` y todo su contenido en la SD o SSD donde está instalado Recalbox `share\marquesinas`, como se indica en el punto `5. Estructura de archivos en la SD o SSD de Recalbox`
    
 Si prefieres usar otros logos como por ejemplo los del tema que tienes instalado. Usaremos el script se encuentra en la carpeta `Arcade/Logos Sistemas/` del proyecto [aquí](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas). Consta de dos archivos `Ejecutar Script Logos.bat` y `Script Logos.ps1`.

1.  **Ejecuta el archivo** ``Ejecutar Script Logos.bat` (Lanzador para evitar bloqueos de Windows).
2.  **Configuración de rutas:**
    * **Origen:** Introduce la ruta donde tienes los logos (ej: `\\192.168.1.119\share\themes\Animatics-DX-master\art\logos`).
    * **Destino:** Introduce la ruta `C:\Logos`.
4.  **Copiar:** Si seleccionaste la ruta `C:\Logos` copia la carpeta  `Logos` y todo su contenido en la SD o SSD donde está instalado Recalbox `share/marquesinas/`, como se indica en el punto `5. Estructura de archivos en la SD o SSD de Recalbox`.

<img width="1102" height="573" alt="image" src="https://github.com/user-attachments/assets/7d90cc90-3cad-4991-8498-591081ab2004" />


### ¿Qué hace el script automáticamente?
* **Redimensionado:** Convierte tus marquesinas originales a **128x32 píxeles**.
* **Formato:** Fuerza el color a **BMP de 24 bits** (formato compatible con el driver DMA del ESP32).

> [!CAUTION]
> **Acceso por Red (Samba):**
> Si al ejecutar el script no tiene acceso a la ruta indicada tendrás que acceder mediante el explorador de archivos y logearte con los credenciales de Recalbox para que el script tenga acceso a la carpeta.
> Acceder a la ruta `ej-> \\192.168.1.120\share\themes\Animatics-DX-master\art\logos` Windows te solicita credenciales, utiliza las que trae Batocera por defecto:
> * **Usuario:** `root`
> * **Contraseña:** `recalboxroot`

## 5. Estructura de archivos en la SD o SSD de Recalbox

Para que la integración funcione correctamente, debemos de pegar la carpeta marquesinas en la carpeta `share/`
* **`share/marquesinas/Arcade/sistema/rom_name.bmp`** (Marquesina del juego procesada, ej: `mslug.bmp`)
* **`share/marquesinas/Logos/sistema_name.bmp`** (Marquesina del sistema procesada, ej: `mame.bmp`)

#### Ejemplo visual de carpetas:
```
📂 share/
├── 📂 marquesinas/
│   └── 📂 Arcade/
│   │   └── 📂 neogeo/
│   │   │   ├── 📄 mslug.bmp
│   │   │   ├── 📄 kof98.bmp
│   │   │   └── ...
│   │   └── 📂 mame/
│   │       ├── 📄 pacman.bmp
│   │       ├── 📄 tetris.bmp
│   │       └── ...
│   └── 📂 Logos/
│       ├── 📄 atari2600.bmp
│       ├── 📄 mame.bmp
│       └── ...
```

## 6. ¡Disfruta de las marquesinas mientras juegas en tu Arcade!
