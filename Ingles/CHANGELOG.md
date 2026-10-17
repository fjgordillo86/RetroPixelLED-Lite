## 📝 Changelog

### [v3.1.5] - 2026-10-17
**Retro Pixel LED Lite: "Connections"**

#### ✨ Added
- **Built-in WiFi Portal:** Connect the panel to your network directly from your mobile phone, without editing `config.ini`. Opens from `OSD Menu → WiFi → Add network`, or automatically on first boot if no network is configured. The panel displays its network SSID and a random password; select your WiFi from a list, and it will verify the connection before saving. Texts available in Spanish, English, and French.
- **Access by Name (mDNS):** The panel responds at `retropixel.local`, without depending on an IP address. The PWA remembers the last known IP and prompts you to use it if the name cannot be resolved on your device.
- **`/diag` Diagnostics:** Quick view of the internal system state directly from your browser (`http://retropixel.local/diag`).

#### ⚙️ Improvements / Internal Changes
- **Update Notification in PWA:** An amber badge over the 🔄 icon notifies you when a new version is available, displaying its version number and changelog.
- **New "About" Page in PWA:** Shows PWA and firmware versions, creator credits, license, and **live diagnostics** of the panel (free memory, last reboot reason, WiFi signal, SD card status...), with a one-click button to copy the report.
- **`/diag` Diagnostics:** Quick status check of the internal system state directly from your browser (`http://retropixel.local/diag`).

#### 🛡️ Fixes
- **WiFi Passwords containing `#`:** Fixed a parsing issue in `config.ini` that truncated the SSID or password at a `#` character.

#### ⚠️ Update Notice
- **Upgrading from versions prior to v3.1.4:** Starting from v3.1.4, the *Minimal SPIFFS* partition scheme (1.9 MB application space with OTA support) is used, and the firmware no longer fits in the old partition layout. Installation via OTA is not possible: please reinstall using the Web Installer. Data stored in internal flash memory (active playlist, timer settings) will be reset; SD card configuration files and GIFs will remain intact. If you are already running v3.1.4, you can upgrade directly to v3.1.5 via OTA.

---
### [v3.1.4] - 2026-10-04
**Retro Pixel LED Lite: "Total Smoothness"**

#### ✨ Added
* **RetroBat Support:** automatically plays the GIF or retro marquee of the game you run in RetroBat.
* **Configurable SD Read Speed:** new `SD_SPEED` parameter (4, 10, 20, or 25 MHz) editable from the PWA (Hardware group), the OSD menu (Advanced Settings), and `config.ini`. Defaults to 10 MHz.
#### ⚙️ Improvements / Internal Changes
* **Two-Stage SD Boot:** the card first initializes at 4 MHz to read `config.ini` and then restarts at the configured speed. If the card fails to respond, it automatically falls back to a lower speed (25 → 20 → 10 → 4 MHz).
* **More Precise Inter-Frame Delay:** decoding and drawing time for each frame is now included within the GIF delay instead of being added to it, resulting in smoother playback.
* **New Partition Scheme (Minimal SPIFFS):** 1.9 MB for the app with OTA, compared to the previous 1.25 MB, giving more headroom for firmware growth.
* **New Language Key `sdspeed`:** added to the `SUBMENU_AVANZADO` section of language `.json` files for the new menu option.
#### 🛡️ Fixes
* **GIF Playback Speed:** fixed double frame delay (the library waited for the delay and the firmware waited for it again), which caused GIFs to play slower than their real-time rate.
#### ⚠️ Update Note
* Due to the partition scheme change, this version **cannot be updated via OTA** from previous versions: reinstall it using the web installer. Saved data in internal memory (active playlist and timer settings) will be wiped; settings and GIFs stored on the SD card are preserved. Also remember to update the `.json` language files.
---

### [v3.1.3] - 2026-09-19
**Retro Pixel LED Lite: "Arcade ReplayOS"**

#### ✨ Added
* **GIF Playback with ReplayOS:** plays the GIF of the game you run in ReplayOS.
* **New Clock Styles:** added new styles for the clock.
* **IR Configuration via PWA:** added the ability to configure IR remote control buttons to the PWA.
---

### [v3.1.2] - 2026-09-01
**Retro Pixel LED Lite: "Total Control"**

#### ✨ Added
* **Font Style Selection in PWA:** choose between Bold, SemiBold, Regular, and Light styles.
* **Home Assistant Integration:** integrated with Home Assistant via REST API.
---

### [v3.1.0] - 2026-08-02
**Retro Pixel LED Lite: "PWA Total Control"**

#### ✨ Added
* **Remote Control Progressive Web App (PWA):** installable web interface providing complete control of the panel from any device on the local network:
  - **Main Page:** real-time brightness and mode selector for GIF / Clock / Text, each displaying only relevant controls (playlist + random in GIF mode; style + color in Clock mode; live matrix preview + color + speed in Text mode).
  - **Timer:** power on/off schedule with immediate manual override from the app itself.
  - **Updater:** firmware OTA and language `.json` files downloader directly from GitHub into the SD card's `/idiomas` directory, without needing to remove the card.
  - **Settings:** remote editing of `config.ini` (WiFi, Hardware, Arcade, Scrolling Text, Clock, Weather, Language), with automatic reboot when required by changes.
* **Scrolling Text Mode with Custom Font Support:** scrolling text engine featuring a custom `GFXfont` (includes Polish characters) and a real-time UTF-8 decoder, togglable from the PWA with configurable color and speed.
* **Second Installation Mode for Batocera:** similar to Recalbox, you can now choose between "Menus and Games" (also reacts when browsing systems) or "Games Only" (fixed marquee/clock in menus, changes only upon launching a game).
* **Shutdown/Reboot Hooks for Batocera:** new scripts for EmulationStation's `quit`, `shutdown`, and `reboot` events, notifying the panel to exit the marquee upon exiting the system.
#### ⚙️ Improvements / Internal Changes
* **Web Routes Refactoring:** over 20 HTTP server endpoints extracted from `setup()` into a dedicated `WebRoutes.ino` file, organized by function (status, live control, timer, config, playlists, OTA, languages, arcade, text).
* **Rebootless Live Control:** brightness, playback mode, random toggle, clock style, and color are now instantly applied from the PWA and saved to `config.ini` without requiring a panel reboot.
* **Revamped Power-Off Animation:** replaced the sleeping face with a CRT screen power-off style animation, while halving the transition's blocking time.
* **Image Quality in Marquee Tools:** Floyd-Steinberg dithering to RGB565 and high-quality rendering ported to Logos and Batocera converters to eliminate color banding in gradients when displayed on the panel.
* **Reorganized Arcade Scripts Installer:** Batocera and Recalbox scripts are now distributed in separate `Batocera/` and `Recalbox/` subfolders instead of being mixed in a single folder.
#### 🛡️ Fixes
* **Brightness Adjustment Cycle:** fixed logic bug in the brightness loop that prevented resetting to 5% after reaching 100%.
* **Timer and Web Server:** fixed an issue where the panel stopped responding to HTTP requests while sleeping, preventing remote power-on from the PWA.
---

### [v3.0.5] - 2026-06-18

#### 🛡️ Fixes
* **Rainbow Effect Stability:** fixed rendering of the dynamic *Rainbow* effect, which was negatively affected by clock anti-flicker optimization when *Double Buffer* was disabled.
* **OSD Brightness Refresh:** fixed visual glitch in the OSD menu; brightness percentage now updates dynamically on-screen in real time while adjusting via remote or button.

---  

### [v3.0.4] - 2026-05-31
**Retro Pixel LED Lite: "Particles & System Stability"**

#### ✨ Added
- **"Particle Explosion" Transition:** new dynamic effect for the clock appearing and disappearing, improving visual smoothness.
- **Color Selection via OSD:** new menu option to change clock color in real time without editing `.ini` files.
- **Built-in FTP Server:** wireless file transfer protocol to manage playlists and configurations directly on the SD card.
- **IR Remote Control:** dynamic function mapping to navigate menus, adjust brightness, and control the panel remotely.

#### ⚙️ Improvements
- **Single Buffer Implementation:** refactored drawing logic to eliminate flickering in the clock and menus.
- **RAM Optimization:** full migration from `String` objects to `char[]` and extensive use of `PSTR()` / `F()` to free Heap memory and prevent fragmentation.
- **Anti-Panic System:** safety check in `display->begin()` with automatic fallback to Single Buffer in case of memory fragmentation after using WiFi.
- **Safe Confirmation (Long Press):** implemented long press detection on the physical button to prevent accidental menu entries.
- **Universal Color Configuration:** dynamic processing of the `colorOrder` parameter (RGB/RBG/GBR) from `config.ini` for compatibility with any HUB75 panel.

#### 🛡️ Fixes
- **Rendering Stability:** eliminated memory allocation errors (*StoreProhibited*) under high network load conditions.
- **Log Cleanup:** improved system initialization diagnostics to detect buffer allocation failures early.
  
---  

### [v2.1.4] - 2026-04-24
**Retro Pixel LED Lite: "Arcade Mastery & Binary Speed"**

#### ✨ Added
- **Arcade Mode (Batocera Integration):** implemented HTTP event receiver for automatic marquee synchronization with external systems.
- **Binary Search Engine:** high-performance algorithm to locate games on the SD card in milliseconds, eliminating lag in massive collections.
- **Fallback Cascading Logic:** smart display priority: Game > System Logo > Default Image.
- **Indexing Tool (PowerShell):** interactive PC script that automates image processing (24-bit BMP) and `.txt` index creation.

#### ⚙️ Improvements
- **Dynamic Buffer Management:** system automatically switches to **Single Buffer** when Arcade mode is active to maximize available RAM.
- **SD Memory Optimization:** enforced file closure after reading indexes to prevent running out of File Handles.
- **Console Feedback:** detailed Serial Log to diagnose IP command reception and file existence on the SD card.

#### 🛡️ Fixes
- **Script Terminator Error:** fixed syntax error in the `.ps1` launcher that prevented execution on Windows systems with restrictive execution policies.
- **Name Trimming:** game and system names now ignore accidental trailing whitespace sent by Batocera, preventing "File Not Found" errors.
- **State Switch Stability:** fixed bug preventing proper return to the GIF gallery after receiving a stop command (STOP/OFF).

---

### [v2.1.0] - 2026-04-18
**Retro Pixel LED Lite: "Global Voice & Wireless Evolution"**

#### ✨ Added
- **Dynamic Multi-Language System:** support for external dictionaries in `.json` format (ES, EN, FR...). Smart loading from SD to save RAM.
- **Wireless Update (OTA):** firmware download and installation engine directly from the OSD menu via GitHub.
- **Smart Menu Centering:** automatic text centering algorithm that adjusts menus based on character width per language.
- **"Sleep" Visual Feedback:** custom Moon icon and pixel-designed 😴 Emoji for power-saving mode.

#### ⚙️ Improvements
- **RAM Management (Anti-Panic):** enforced memory release after closing the OSD menu to prevent accidental reboots.
- **Config.ini Generation:** system now auto-generates configuration file comments in the user's selected language.
- **Language UX:** real-time language selector that applies changes without manually rebooting the panel.

#### 🛡️ Fixes
- **JSON Parser Stability:** fixed critical bug causing a *Kernel Panic* when trying to read language files with overly long tags.
- **OTA Secure Client:** adjusted certificate handling to guarantee secure connections with update servers.
- **OSD Text:** eliminated duplicate ":" symbols in menu text strings to improve visual cleanliness.

---

### [v2.0.5] - 2026-04-11
**Retro Pixel LED Lite: "Smart Energy, Dual Vision & Safety Core"**

#### ✨ Added
- **Dual Display Mode:** menu selector to toggle between "Clock Only" (minimalist) or "GIF Playlist" (animated).
- **Smart Timer:** automatic power on/off scheduling with support for midnight crossover (Over-midnight).
- **Manual Override (User Priority):** extra long press function (4s) to force power state, locking out the timer until the next cycle.
- **I2S Safety Shield:** protection system that dynamically caps frequency at 16MHz when Double Buffer is enabled to ensure total stability.

#### ⚙️ Improvements
- **Smart UI Navigation:**
    - Short press: Wake panel / Navigate.
    - Long press: Fast decrement (-5 min in Timer).
    - Continuous press: Fast increment (+5 min in Timer).
- **Ultra-Responsive Loop:** removed blocking code; button input now instantly interrupts any animation or network process.
- **Optimized Clock Cycle:** clock appearance range adjusted to [2...10] GIFs in steps of +2 for faster, more logical configuration.
- **Weather API Sanitization:** improved URL handling for cities with spaces or hyphens, preventing weather data fetch failures.
- **Paged Menus:** restructured OSD into multiple pages to improve visibility of new advanced settings.
  
---
### [v2.0.0] - 2026-03-26
**Retro Pixel LED Lite: "OSD Menu, Night Mode & Smart RAM"**

#### ✨ Added
- **OSD (On-Screen Display) Menu:** native interface on the LED panel to configure Playlists, Brightness, WiFi, and Clock using a single physical button.
- **Dynamic Night Mode:** integration with Moon icons and automatic cool color palettes based on local time and OpenWeatherMap data.
- **Automatic Plug & Play:** system scans and plays the first playlist found in `/playlists` if none is selected.
- **SD Persistence:** automatic saving of all settings made from the OSD menu directly to `config.ini`.

#### ⚙️ Improvements
- **Smart RAM Refresh:** intelligent reboot logic when refreshing weather/time to prevent memory fragmentation caused by Double Buffering.
- **Stealth WiFi Management:** complete shutdown of the network stack after synchronization to eliminate lag and reduce ESP32 temperature.
- **Silent NTP Sync:** internal clock adjustment during each weather update window to prevent time drift.
- **Playlist Optimization:** instant transitions between thematic lists from the menu without rebooting the device.

---
### [v1.1.2] - 2026-03-19
**Retro Pixel LED Lite: "Double Buffering, Splash Screen & Branding"**

#### ✨ Added
- **Rendering Engine:** implemented Double Buffering technique for ultra-smooth content playback.
- **Dynamic RGB Logo:** startup sequence displaying the "RETRO PIXEL LED lite" logo with independent colors for the LED acronym and stylized outline borders.
- **Firmware Identification:** direct display of the system version (`v1.1.2`) on the loading screen for easier version tracking and support.

#### ⚙️ Improvements
* **Critical Sequencing:** system now manages WiFi connection, NTP synchronization, and weather download *before* initializing the LED panel. 
* **Resource Release:** once data is fetched, the WiFi driver is completely shut down to yield all RAM memory to the graphics engine, avoiding initialization error `0x3001`.

---
### [v1.1.0] - 2026-03-03
**Retro Pixel LED Lite: "The Weather & Notification Update"**

#### ✨ Added
- **Notification Bar:** implemented a top bar (Y=0 to Y=8) for system information.
- **Custom Message:** new `WEATHER_MSG` key in `config.ini` to display fixed text (e.g., "Game Room") on the marquee.
- **OpenWeatherMap Support:** integration with the official API to download real-time weather data.
- **Bitmap Iconography:** added 6 custom 8x8 pixel icons (Sun, Clouds, Rain, Snow, Storm, Fog) optimized for LED panels.
- **Dynamic Positioning:** automatic Clock adjustment logic (`startY=9`) when weather is active to prevent visual overlap.

#### ⚙️ Improvements
- **WiFi Management:** "Stealth Mode" optimization. WiFi now periodically wakes up according to `WEATHER_INT` to refresh data and turns back off.
- **INI Reading:** added parsing logic for `CITY`, `API_KEY`, and `WEATHER_MSG`.
- **Clock Aesthetics:** degree symbol (°C) now uses a crisp 2x2 pixel vector drawing.

#### 🛡️ Fixes
- Fixed top bar flickering by integrating rendering into the clock's DMA buffer.
- Adjusted temperature conversion to display integer values only, preventing text overflow.
