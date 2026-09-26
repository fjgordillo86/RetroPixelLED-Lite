# 🕹️ Integration with Batocera

The **Arcade Mode** in the Lite version allows your LED matrix to function as a dynamic marquee. The panel will automatically detect the system and game you are browsing and display it — and if the game has an animated marquee ready, it will play it in a loop while you play.

#### Resource Optimization (Scraping)
The main advantage of this system is that it **uses the images you have already scraped in Batocera** (marquees/wheel art, and preview videos if available). The PowerShell script takes care of finding, resizing, and converting them automatically.

## 1. Critical Configuration: Static IP for the ESP32

For the **🕹️ Arcade** mode in Batocera to always work properly, it is essential that the ESP32 maintains the exact same IP address.

> [!TIP]
> **Assigning a static IP to the ESP32:**
> Batocera scripts send commands (such as changing the GIF when launching a game) to a specific IP address that you configure manually. If the router reboots and assigns a different IP to the ESP32, communication will break and the panel will stop updating.
>
> **How to do it?**
> 1. Access your router configuration page.
> 2. Look for the **Static DHCP** or **IP Assignment by MAC** section.
> 3. Bind the MAC address of your ESP32 to the IP address you set in your scripts (e.g., `192.168.1.117`).
> 4. Since every router is different, if you have doubts, search Google: *"How to assign static IP [your router model]"*.

> [!NOTE]
> Since the update that added **animated marquees**, the panel firmware automatically detects Wi-Fi reconnection if the connection is lost during use — but a static IP is still required, as Batocera scripts cannot "search" for the panel; they only know how to communicate with a specific IP.

## 2. Automatic Installation on Batocera

As of version **v3.0.0**, you no longer need to edit code lines manually, worry about Windows line-ending formats, or use advanced SSH consoles (such as PuTTY) to configure execution permissions.

I have developed a **Smart PowerShell Installer Script** that performs the entire deployment automatically from your PC.

---

### 📦 What does this installer do for you?

* **IP Configuration:** Automatically injects the IP address of your LED panel into all communication scripts.
* **Format Correction:** Forces the **Unix (LF)** line ending format. This prevents scripts from failing if they were accidentally edited with Windows Notepad.
* **File Organization:** Creates the required directory structure in Batocera and copies the files to their proper destination.
* **Auto-Permissions (No PuTTY needed):** Generates a system script (`custom.sh`) that makes Batocera grant execution permissions (`chmod +x`) to the folders on every startup.
* **Animated Marquees:** Installs the GIF playback engine (`pixel_stream.py`) and the `game-start` event handler, responsible for detecting and playing the animated marquee of the game you just launched.

---

### 🛠️ Prerequisites

1. Have your **PC** and **Batocera** connected to the same local network (or connect Batocera's physical storage directly to the PC).
2. Know the **local IP address of your Retro Pixel LED panel** (e.g., `192.168.1.117`).
3. Download the full `Instalador Automático` folder from this repository, located [here](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Instalador%20Automatico).

> [!IMPORTANT]
> If you downloaded the repository as a `.zip` file, make sure to **extract it completely** before running the installer.

---

### 💻 Step-by-Step

1. Open the `Instalador Automatico` folder on your PC. Inside you will find two files and two folders:
   * `Ejecutar Script Instalador Arcade.bat`
   * `Script_Instalador_Arcade.ps1`
   * `Batocera`
   * `Recalbox`

2. **Click** on `Ejecutar Script Instalador Arcade.bat`.

3. Follow the instructions in the console window:
   * **Step 1:** Enter the IP address of your LED panel and press `Enter`.
   * **Step 2:** Enter the path to your Batocera. This can be a network path (e.g., `\\192.168.1.120` or `\\BATOCERA`) or a drive letter if you connected the drive/SD card directly to your PC (e.g., `E:`).

4. The script will ask:
    * Which system you are using: select **1 Batocera**.
    * Which operating mode you want to activate?
       * **Option 1:** Menus and Games (Displays systems while browsing + launched game)
       * **Option 2:** Games Only (Static marquee/clock in menus, changes only when playing)

> [!NOTE]
> **Animated** marquees work the same way in both modes — the difference between Option 1 and Option 2 is only whether the panel reacts when browsing systems; it does not affect whether GIFs play when launching a game.

5. The script will process the files in seconds. Once finished, you will see the message `INSTALACIÓN COMPLETADA!`. Press any key to exit.

<img width="941" height="834" alt="image" src="https://github.com/user-attachments/assets/cec91be3-82b4-44e4-8873-6ce67383e2e1" />

6. **Fully reboot your Batocera system.**
> [!CAUTION]
> A full system reboot is **required** for the permissions service to become available in the menu.

7. **Enable it (first time only):** go to `Main Menu > System Settings > Services` and enable **`retropixelperms`**.
> [!NOTE]
> This service replaces the old `custom.sh` and is what grants execute permissions to the marquee scripts on every boot. Once you enable it here, it stays enabled forever — even if you run the installer again later (reinstalling only regenerates the service file, it doesn't disable it). From then on, every time you browse the menu, launch, or exit a game, the panel will react automatically.

### 3. 🛠️ Marquees
We will use the script located in the `Arcade/Marquesinas/` folder of the project [here](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Marquesinas). It consists of two files: `Ejecutar Script Marquesinas Batocera.bat` and `Script Marquesinas Batocera.ps1`.

1. **Run the file** `Ejecutar Script Marquesinas Batocera.bat` (Launcher to prevent Windows execution blocks).
2. **Path configuration:**
    * **Source:** Enter the path to your Batocera ROMs (e.g., `\\192.168.1.119\share\roms`).
    * **Destination:** Enter the path `C:\marquesinas`.
3. **System Selection:** The script will automatically detect which systems contain a `gamelist.xml` file. You can choose to process a single system by its number, multiple systems, or **All (0)**.
4. **Copying files:** If you selected the path `C:\marquesinas`, copy the `marquesinas` folder and all its contents to the SD or SSD where Batocera is installed inside `roms/`, as indicated in step `6. File structure on Batocera SD or SSD`.

<img width="1096" height="572" alt="image" src="https://github.com/user-attachments/assets/388368a7-a57b-4611-89fc-4bfc184c1fa7" />

### What does the script do automatically?
* **Resizing:** Converts your original marquees to **128x32 pixels**.
* **Formatting:** Forces color to **24-bit BMP** (format compatible with the ESP32 DMA driver).

> [!CAUTION]
> **Network Access (Samba):**
> If the script does not have access to the specified path when executed, you will need to open File Explorer and log in with Batocera credentials so the script gains access to the folder.
> Accessing the path (e.g., `\\192.168.1.120\share\roms`), Windows will ask for credentials; use Batocera's defaults:
> * **Username:** `root`
> * **Password:** `linux`

> [!CAUTION]
> Every time you add new games or run a "Scrape" in Batocera, **you must run the PowerShell script on your PC again** to update indexes and images. Without this step, the ESP32 won't know the new files exist.

### 4. 🎬 Animated Marquees (GIF)

In addition to static images, the panel can play an **animated GIF** when launching a game — the marquee moves while you play instead of remaining static.

#### How does it work?

- While **browsing** systems and games, the panel behaves exactly the same as with static marquees: there is no difference, and GIFs are not used yet.
- The moment you **launch** a game, the panel checks if a `.gif` file with the exact same name as the static marquee exists in the same folder.
- **If found, it plays it in a loop** throughout your gameplay session and returns to normal GIF/clock playback as soon as you exit.
- **If not found, nothing breaks** — the static marquee already displayed remains as is, as if GIF mode did not exist for that game. You don't need to prepare a GIF for every single game; you can add them over time.

#### File Naming

The GIF must be named **identically to the static `.bmp` marquee** of the game, in the same folder:

```
roms/marquesinas/Arcade/neogeo/mslug.bmp   <- already existing
roms/marquesinas/Arcade/neogeo/mslug.gif   <- added by you, same name
```

You can also prepare a **sequence of multiple GIFs** for the same game by adding suffixes `_01`, `_02`, `_03`... The panel will play them in order, one after another, and restart from the first one in a continuous loop:

```
roms/marquesinas/Arcade/neogeo/mslug.gif
roms/marquesinas/Arcade/neogeo/mslug_01.gif
roms/marquesinas/Arcade/neogeo/mslug_02.gif
```

> [!TIP]
> You don't need to have all three — having just `mslug.gif` works perfectly in a loop. Suffixes like `_01`, `_02`... are optional for when you want to alternate between different clips for the same game.

#### Where do I get GIFs from?

If you already have (or downloaded) an arcade GIF collection with human-readable names instead of romset names (e.g., `ARCADE_NEOGEO_MetalSlugStory.gif` instead of `mslug.gif`), use the following script to rename them. Download it from [here](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/GIFs) and follow these steps:

1. Run `Ejecutar Script_RetroPixelLED_GIF_Renamer.bat`.

2. **Option 1 — Rename GIFs:** Specify the folder where your GIFs are located. The script queries a public MAME catalog ([`MAME.dat`](https://github.com/libretro/libretro-database)) to identify which romset corresponds to each title, along with an internal dictionary for common cases. You can choose between exact matching only, or exact + approximate matching (resolves more cases with slightly higher risk). Unidentified items are moved to a `SinResolver\` folder for manual review — it never renames blindly.
   <img width="1090" height="830" alt="image" src="https://github.com/user-attachments/assets/58ba389f-367c-4114-b6e1-533018f50e77" />

3. **Option 2 — Copy GIFs to system folders:** Once renamed, this option compares GIFs against actual romsets in your `ROMS/` folder and automatically copies them to `Arcade/<system>/`, alongside existing `.bmp` files.
   <img width="1106" height="1204" alt="image" src="https://github.com/user-attachments/assets/ee3e51a6-2dd7-4297-8a2a-a4486171f60d" />

You can also create them yourself — the panel only requires the final file resolution to be **128×32 pixels**. As a reference, if your Batocera collection already has scraped preview videos (`<video>` in `gamelist.xml`), you can convert them to GIF using a tool like [dmd_gif_converter](https://github.com/red77290/dmd_gif_converter), which includes automatic framing to keep action visible when downscaling large videos. It is a third-party project independent of this repository — any method producing a 128×32 `.gif` file will work equally well.

 ### 5. 🛠️ System Logos
 You can use pre-resized system logos found in the `Arcade/Logos Sistemas/` folder [here](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas).
 1. **Copy:** Copy the `Logos` folder and all its contents to the SD or SSD where Batocera is installed under `roms/marquesinas/`, as shown in step `6. File structure on Batocera SD or SSD`.
    
 If you prefer using custom logos (such as those from your installed theme), use the script located in `Arcade/Logos Sistemas/` [here](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Arcade/Logos%20Sistemas). It consists of `Ejecutar Script Logos.bat` and `Script Logos.ps1`.

1. **Run the file** `Ejecutar Script Logos.bat` (Launcher to bypass Windows execution policies).
2. **Path configuration:**
    * **Source:** Enter the path where your logos are stored (e.g., `\\192.168.1.119\userdata\themes\Animatics-DX-master\art\logos`).
    * **Destination:** Enter the path `C:\Logos`.
4. **Copy:** If you selected `C:\Logos`, copy the `Logos` folder and its contents to `roms/marquesinas/` on your Batocera SD or SSD.

<img width="1102" height="573" alt="image" src="https://github.com/user-attachments/assets/7d90cc90-3cad-4991-8498-591081ab2004" />


### What does the script do automatically?
* **Resizing:** Converts your original marquees/logos to **128x32 pixels**.
* **Formatting:** Forces color output to **24-bit BMP** (format compatible with ESP32 DMA driver).

> [!CAUTION]
> **Network Access (Samba):**
> If the script fails to access the given network path, open Windows File Explorer and log in using Batocera's credentials:
> * **Username:** `root`
> * **Password:** `linux`

## 6. File structure on Batocera SD or SSD

For the integration to function properly, paste the `marquesinas` folder inside `roms/`:
* **`roms/marquesinas/Arcade/system/rom_name.bmp`** (Static game marquee, e.g., `mslug.bmp`)
* **`roms/marquesinas/Arcade/system/rom_name.gif`** (Optional: animated game marquee, e.g., `mslug.gif`)
* **`roms/marquesinas/Logos/system_name.bmp`** (Processed system logo marquee, e.g., `mame.bmp`)

#### Visual folder example:
```
📂 roms/
├── 📂 marquesinas/
│   └── 📂 Arcade/
│   │   └── 📂 neogeo/
│   │   │   ├── 📄 mslug.bmp
│   │   │   ├── 📄 mslug.gif       <- optional animated marquee
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

## 7. Enjoy your marquees while gaming on your Arcade machine!
