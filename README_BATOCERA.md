# 🕹️ Integración con Batocera

El **Modo Arcade** en la versión Lite permite que tu matriz LED funcione como una marquesina dinámica. El panel detectará el sistema y juego por el que estás navegando y te lo mostrará automáticamente — y si el juego tiene una marquesina animada preparada, la reproducirá en bucle mientras juegas.

#### Aprovechamiento de Recursos (Scraping)
La principal ventaja de este sistema es que **utiliza las imágenes que ya has scrapeado en Batocera** (marquesinas/wheel art, y también los vídeos de preview si los tienes). El script de PowerShell se encarga de buscarlas, redimensionarlas y convertirlas automáticamente.

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

> [!NOTE]
> Desde la versión que añade **marquesinas animadas**, el panel actualiza tu firmware detecta automáticamente la reconexión de WiFi si se pierde la conexión durante el uso — pero la IP fija sigue siendo necesaria igualmente, ya que los scripts de Batocera no saben "buscar" el panel, solo saben a qué IP concreta tienen que hablarle.

## 2. Instalación Automática en Batocera

A partir de la versión **v3.0.0**, ya no es necesario editar líneas de código a mano, preocuparse por los formatos de archivo de Windows o utilizar consolas SSH avanzadas (como PuTTY) para configurar los permisos de ejecución. 

He desarrollado un **Script Instalador Inteligente en PowerShell** que realiza todo el despliegue de forma automática desde tu PC.

---

### 📦 ¿Qué hace este instalador por ti?

* **Configuración de IP:** Inyecta automáticamente la dirección IP de tu panel LED en todos los scripts de comunicación.
* **Corrección de Formato:** Fuerza el formato de fin de línea **Unix (LF)**. Esto evita que los scripts fallen si fueron abiertos por error con el Bloc de Notas de Windows.
* **Organización de Archivos:** Crea la estructura de directorios necesaria en Batocera y copia los archivos en su lugar correspondiente.
* **Auto-Permisos (Sin PuTTY):** Genera un script del sistema (`custom.sh`) que hace que Batocera se otorgue a sí mismo permisos de ejecución (`chmod +x`) sobre las carpetas en cada arranque.
* **Marquesinas Animadas:** Instala también el motor de reproducción de GIF (`pixel_stream.py`) y el evento `game-start`, encargados de detectar y reproducir la marquesina animada del juego que acabas de lanzar.

---

### 🛠️ Requisitos Previos

1. Tener tu **PC** y tu **Batocera** conectados a la misma red local (o conectar el almacenamiento físico de Batocera directamente al PC).
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
   * **Paso 2:** Introduce la ruta de tu Batocera. Puede ser una ruta de red (ej: `\\192.168.1.120` o `\\BATOCERA`) o la letra de una unidad física si conectaste el disco/SD al PC (ej: `E:`).

4. El script nos solicitará:
    * El sistema que utilizamos, seleccionaremos 1 Batocera.
    * Que modo de funcionamiento deseas activar?
       * **Opción 1:** Menús y Juegos (Muestra sistemas al navegar + juego lanzado)
       * **Opción 2:** Solo Juegos (Marquesina fija/reloj en menús, cambia solo al jugar)

> [!NOTE]
> Las marquesinas **animadas** funcionan igual en los dos modos — la diferencia entre Opción 1 y Opción 2 es únicamente si el panel reacciona también al navegar por los sistemas, no afecta a si hay GIF o no al lanzar un juego.

5. El script procesará los archivos en un segundo. Al finalizar, verás el mensaje `INSTALACIÓN COMPLETADA!`. Pulsa cualquier tecla para salir.

<img width="1103" height="686" alt="image" src="https://github.com/user-attachments/assets/d94c2a67-c40a-451e-9c61-981a188a294d" />

6. **Reinicia tu sistema Batocera por completo.**
> [!CAUTION]
> El reinicio completo del sistema es **obligatorio**. Durante este arranque, el script `custom.sh` configurará los permisos internos. A partir de ese momento, cada vez que navegues por el menú, lances o cierres un juego, el panel reaccionará automáticamente.

### 3. 🛠️ Marquesinas.
Usaremos el script se encuentra en la carpeta `Arcade/Marquesinas/` del proyecto [aquí](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Marquesinas). Consta de dos archivos `Ejecutar Script Marquesinas Batocera.bat` y `Script Marquesinas Batocera.ps1`.

1.  **Ejecuta el archivo** ``Ejecutar Script Marquesinas Batocera.bat` (Lanzador para evitar bloqueos de Windows).
2.  **Configuración de rutas:**
    * **Origen:** Introduce la ruta de tus ROMs de Batocera (ej: `\\192.168.1.119\share\roms`).
    * **Destino:** Introduce la ruta `C:\marquesinas`.
3.  **Selección de Sistema:** El script detectará automáticamente qué sistemas tienen un archivo `gamelist.xml`. Puedes elegir procesar uno solo por su número, varios o **Todos (0)**.
4.  **Copiar:** Si seleccionaste la ruta `C:\marquesinas` copia la carpeta  `marquesinas` y todo su contenido en la SD o SSD donde está instalado Batocera `roms/`, como se indica en el punto `6. Estructura de archivos en la SD o SSD de Batocera`.

<img width="1096" height="572" alt="image" src="https://github.com/user-attachments/assets/388368a7-a57b-4611-89fc-4bfc184c1fa7" />

### ¿Qué hace el script automáticamente?
* **Redimensionado:** Convierte tus marquesinas originales a **128x32 píxeles**.
* **Formato:** Fuerza el color a **BMP de 24 bits** (formato compatible con el driver DMA del ESP32).

> [!CAUTION]
> **Acceso por Red (Samba):**
> Si al ejecutar el script no tiene acceso a la ruta indicada tendrás que acceder mediante el explorador de archivos y logearte con los credenciales de Batocera para que el script tenga acceso a la carpeta.
> Acceder a la ruta `ej-> \\192.168.1.120\share\roms` Windows te solicita credenciales, utiliza las que trae Batocera por defecto:
> * **Usuario:** `root`
> * **Contraseña:** `linux`

> [!CAUTION]
> Cada vez que añadas nuevos juegos o hagas un "Scrape" en Batocera, **debes volver a ejecutar el script de PowerShell** en tu PC para actualizar los índices y las imágenes. Sin este paso, el ESP32 no sabrá que los nuevos archivos existen.

### 4. 🎬 Marquesinas Animadas (GIF)

Además de la imagen estática, el panel puede reproducir un **GIF animado** al lanzar un juego — la marquesina se mueve mientras estás jugando, en vez de quedarse fija.

#### ¿Cómo funciona?

- Mientras **navegas** por sistemas y juegos, el panel se comporta exactamente igual que con las marquesinas estáticas: no hay ninguna diferencia ahí, el GIF no entra en juego todavía.
- En el momento en que **lanzas** un juego de verdad, el panel busca si existe un `.gif` con el mismo nombre que la marquesina estática de ese juego, en la misma carpeta.
- **Si lo encuentra, lo reproduce en bucle** durante toda la partida, y vuelve a la reproducción normal de GIFs/reloj en cuanto sales.
- **Si no lo encuentra, no pasa nada** — la marquesina estática que ya se estaba mostrando se queda tal cual, como si el modo GIF no existiera para ese juego. No hace falta preparar un GIF para cada juego; puedes ir añadiéndolos poco a poco.

#### Nombrado de archivos

El GIF tiene que llamarse **igual que la marquesina `.bmp`** del mismo juego, en la misma carpeta:

```
roms/marquesinas/Arcade/neogeo/mslug.bmp   <- ya la tenías
roms/marquesinas/Arcade/neogeo/mslug.gif   <- la añades tú, mismo nombre
```

También puedes preparar una **secuencia de varios GIFs** para un mismo juego, añadiendo el sufijo `_01`, `_02`, `_03`... El panel los reproduce todos en orden, uno detrás de otro, y al llegar al último vuelve a empezar por el primero, en bucle continuo:

```
roms/marquesinas/Arcade/neogeo/mslug.gif
roms/marquesinas/Arcade/neogeo/mslug_01.gif
roms/marquesinas/Arcade/neogeo/mslug_02.gif
```

> [!TIP]
> No hace falta que existan los tres — con solo `mslug.gif` ya funciona perfectamente en bucle. Los sufijos `_01`, `_02`... son opcionales, para cuando quieras alternar entre varios clips distintos para el mismo juego.

#### ¿De dónde saco los GIFs?

Tú decides cómo generarlos — el panel solo necesita que el archivo final esté a **128×32 píxeles**. Como referencia, si tu colección de Batocera ya tiene vídeos de preview scrapeados (`<video>` en el `gamelist.xml`), puedes convertirlos a GIF con una herramienta como [dmd_gif_converter](https://github.com/red77290/dmd_gif_converter), que además de redimensionar incluye un modo de encuadre automático pensado para no perder la acción al reducir un vídeo grande a un tamaño tan pequeño. Es un proyecto de terceros, independiente de este repositorio — cualquier otro método que te deje un `.gif` de 128×32 servirá igual de bien.

 ### 5. 🛠️ Logos de Sistemas.
 Podemos usar los logos ya redimensionados que se encuentran en la carpeta `Arcade/Logos Sistemas/` del proyecto [aquí](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas).
 1.  **Copiar:** Copia la carpeta  `Logos` y todo su contenido en la SD o SSD donde está instalado Batocera `roms/marquesinas/`, como se indica en el punto `6. Estructura de archivos en la SD o SSD de Batocera`.
    
 Si prefieres usar otros logos como por ejemplo los del tema que tienes instalado. Usaremos el script se encuentra en la carpeta `Arcade/Logos Sistemas/` del proyecto [aquí](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas). Consta de dos archivos `Ejecutar Script Logos.bat` y `Script Logos.ps1`.

1.  **Ejecuta el archivo** ``Ejecutar Script Logos.bat` (Lanzador para evitar bloqueos de Windows).
2.  **Configuración de rutas:**
    * **Origen:** Introduce la ruta donde tienes los logos (ej: `\\192.168.1.119\userdata\themes\Animatics-DX-master\art\logos`).
    * **Destino:** Introduce la ruta `C:\Logos`.
4.  **Copiar:** Si seleccionaste la ruta `C:\Logos` copia la carpeta  `Logos` y todo su contenido en la SD o SSD donde está instalado Batocera `roms/marquesinas/`, como se indica en el punto `6. Estructura de archivos en la SD o SSD de Batocera`.

<img width="1102" height="573" alt="image" src="https://github.com/user-attachments/assets/7d90cc90-3cad-4991-8498-591081ab2004" />


### ¿Qué hace el script automáticamente?
* **Redimensionado:** Convierte tus marquesinas originales a **128x32 píxeles**.
* **Formato:** Fuerza el color a **BMP de 24 bits** (formato compatible con el driver DMA del ESP32).

> [!CAUTION]
> **Acceso por Red (Samba):**
> Si al ejecutar el script no tiene acceso a la ruta indicada tendrás que acceder mediante el explorador de archivos y logearte con los credenciales de Batocera para que el script tenga acceso a la carpeta.
> Acceder a la ruta `ej-> \\192.168.1.120\userdata\themes\Animatics-DX-master\art\logos` Windows te solicita credenciales, utiliza las que trae Batocera por defecto:
> * **Usuario:** `root`
> * **Contraseña:** `linux`

## 6. Estructura de archivos en la SD o SSD de Batocera

Para que la integración funcione correctamente, debemos de pegar la carpeta marquesinas en la carpeta `roms/`
* **`roms/marquesinas/Arcade/sistema/rom_name.bmp`** (Marquesina estática del juego, ej: `mslug.bmp`)
* **`roms/marquesinas/Arcade/sistema/rom_name.gif`** (Opcional: marquesina animada del mismo juego, ej: `mslug.gif`)
* **`roms/marquesinas/Logos/sistema_name.bmp`** (Marquesina del sistema procesada, ej: `mame.bmp`)

#### Ejemplo visual de carpetas:
```
📂 roms/
├── 📂 marquesinas/
│   └── 📂 Arcade/
│   │   └── 📂 neogeo/
│   │   │   ├── 📄 mslug.bmp
│   │   │   ├── 📄 mslug.gif       <- opcional, marquesina animada
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

## 7. ¡Disfruta de las marquesinas mientras juegas en tu Arcade!
