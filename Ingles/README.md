# ✨ Retro Pixel LED Lite

<p align="center">
  <img alt="Version" src="https://img.shields.io/badge/version-3.1.5-blue">
  <img alt="Platform" src="https://img.shields.io/badge/platform-ESP32-informational">
  <img alt="License" src="https://img.shields.io/badge/license-MIT-green">
  <img alt="Status" src="https://img.shields.io/badge/status-active-success">
</p>

<p align="center">
  <a href="https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README.md">🇪🇸 Español</a> ·
  <a href="https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/Frances/README.md">🇫🇷 Français</a> ·
  <a href="https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/Ingles/README.md">🇬🇧 English</a> ·
  <a href="https://t.me/RetroPixelLed">✈️ Telegram Group</a>
</p>

<p align="center">
  <a href="https://paypal.me/fjgordillo"><img alt="Donate with PayPal" src="https://img.shields.io/badge/☕_Buy_me_a_coffee-PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white"></a>
</p>

> 💛 If Retro Pixel LED is bringing joy to your retro corner at home, you can buy me a coffee using the button above. Every bit helps to keep bringing new features!

---

## 💡 Project Description

**Retro Pixel LED Lite** is the high-performance version of Retro Pixel LED, designed for those seeking absolute stability, instant speed, and a maintenance-free system. Unlike the standard version, the **LITE** firmware eliminates the web server and permanent connectivity to dedicate 100% of the ESP32's power to doing one thing exceptionally well: rendering GIFs.

While the 2.x.x branch brought the OSD Menu, **v3.0.0** was the ultimate leap toward hardware independence: the panel became an autonomous smart device, requiring no PC connection for maintenance or configuration. `config.ini` and playlists are edited directly from Windows Explorer or an FTP client, turning the SD card into a wireless network drive. It also features native remote control support: navigate the OSD Menu, adjust brightness, and turn power on/off right from your couch.

**Since v3.1.0, you can control the panel from an app (PWA)**, and since **v3.1.2**, also from **Home Assistant**. 🏠

**Since v3.1.5, you no longer need to edit any files to connect it to your WiFi:** the panel opens its own configuration portal from your phone, and you can always find it afterwards at `retropixel.local`. 📶

---

## 📑 Table of Contents

1. [🆕 What's new in the current version](#-whats-new-in-version-v315-lite)
2. [🚀 Quick Guide](#-quick-guide-getting-started)
3. [🎛️ Main Features](#️-main-features)
   - [🖥️ OSD Menu](#️-osd-menu-smart-navigation)
   - [📱 PWA — Remote Control App](#-pwa--remote-control-app)
   - [🏠 Home Assistant](#-home-assistant)
   - [🕹️ Arcade Mode](#️-arcade-mode-batocera-recalbox-retrobat--replayos)
   - [🕒 Clock & Weather](#-clock--weather)
   - [⏰ Timer](#-timer)
   - [🌐 Multi-language](#-multi-language)
   - [📂 FTP Server](#-ftp-server)
   - [🔄 OTA Updates](#-ota-updates)
   - [📶 WiFi Portal & Name Access](#-wifi-portal--access-by-name-mdns)
4. [⚙️ Installation and Setup](#️-installation-and-setup)
   - [1. Program the ESP32](#1--program-the-esp32-web-installer)
   - [2. Prepare the SD Card](#2--sd-card-preparation)
   - [3. The `config.ini` File](#3--configuration-via-configini)
   - [4. Time Zone (TZ)](#4--time-zone-tz-configuration)
   - [5. Weather API Key](#5-%EF%B8%8F-how-to-get-your-weather-api-key)
5. [📖 Playlist Generator (Windows)](#-playlist-generator-windows)
6. [🕹️ Arcade Integration](#️-arcade-integration-batocera-recalbox-retrobat-or-replayos)
7. [🏠 Home Assistant Integration](#-home-assistant-integration-complete-guide)
8. [🧠 Internal Architecture / Core Lite](#-internal-architecture--core-lite)
9. [📜 Detailed Changelog](#-detailed-changelog-v300--v315)
10. [🛒 Bill of Materials](#-bill-of-materials)
11. [🔌 Connections (Pinout)](#-connections)
12. [🛠️ Roadmap](#️-roadmap)
13. [⚖️ License & Acknowledgments](#️-license-and-acknowledgments)

---

## 🆕 What's new in version v3.1.5 Lite

- **📶 Built-in WiFi Portal:** Connect the panel to your network from your mobile phone, without editing `config.ini`. It can be opened from `OSD Menu → WiFi → Add network`, or automatically on the first boot if no network is configured. The panel displays its network and a random password; select your WiFi from a list and it will check the connection before saving it. Texts in Spanish, English, and French. [See how it works](#-wifi-portal--access-by-name-mdns).
- **🏷️ Access by Name (mDNS):** The panel responds at `retropixel.local`, without relying on an IP address. The PWA remembers the last known IP and offers to fall back to it if the name does not resolve on your device.
- **🔔 Update Notice in PWA:** An amber dot over the 🔄 icon alerts you when a new version is available, displaying its version number and changelog.
- **ℹ️ New "About" Page in PWA:** PWA and firmware versions, creator, license, and **live diagnostics** of the panel (memory, last reboot reason, WiFi, SD...), including a button to copy the report.
- **🩺 Diagnostics `/diag`:** Quick internal status check from any web browser (`http://retropixel.local/diag`).
- **🔐 WiFi Passwords with `#`:** Fixed a parsing issue in `config.ini` where the SSID or password was truncated when containing a `#`.
- **🛡️ More Reliable PWA Updates:** The service worker now uses a unique version, installs even if a single file fails, and stops intercepting panel requests.

> ⚠️ **When updating from versions prior to v3.1.4:** Starting from that version, the *Minimal SPIFFS* partition scheme is used (1.9 MB for the app with OTA) and the firmware no longer fits in the old partition space. It cannot be updated via OTA: reinstall it using the web installer. Saved internal memory data will be erased (active playlist, timer settings); SD configuration and GIFs will be preserved. If you already have v3.1.4, you can update to v3.1.5 via OTA.

For details on previous versions (Arcade GIF marquees, WiFi reconnection, IP in OSD menu...), see the [Detailed Changelog](#-detailed-changelog-v300--v315).

---

## 🚀 Quick Guide (Getting Started)

If this is your first time installing Retro Pixel LED Lite, this is the shortest path:

1. **Flash the firmware** using the [Web Installer](#1--program-the-esp32-web-installer) — all you need is Chrome or Edge, nothing to install on your PC.
2. **Prepare the MicroSD card** in FAT32 format with the [contents of the `Contenido SD` folder](#2--sd-card-preparation).
3. **Review `config.ini`** with your preferences (time zone, weather city...) — [full reference](#3--configuration-via-configini). You can leave the WiFi settings empty: the panel will ask for them via your phone on the first boot.
4. **Power on the panel.** If no network is configured, open the [WiFi Portal](#-wifi-portal--access-by-name-mdns): connect your phone to the network shown on the screen and select your WiFi. Afterward, it will sync the time, load your GIFs, and be ready. ✨
5. *(Optional)* Install the [PWA](#-pwa--remote-control-app) to control it from your phone, or [integrate it with Home Assistant](#-home-assistant-integration-complete-guide) if you use home automation.
6. *(Optional)* If you have Batocera, Recalbox, RetroBat, or ReplayOS, follow the [Arcade Integration Guide](#️-arcade-integration-batocera-recalbox-retrobat-or-replayos) for dynamic marquees.

The remainder of this document serves as a detailed reference for every feature — you don't need to read it all to get started. 🙂

---

## 🎛️ Main Features

This section summarizes **what** each part of the system does. For step-by-step installation instructions, go to [⚙️ Installation and Setup](#️-installation-and-setup).

### 🖥️ OSD Menu (Smart Navigation)

The system is controlled via a **single physical button** (or the IR remote control), utilizing press logic adapted to the current menu context:

- **Short Press:**
  - **In menus:** Move cursor / navigate down.
  - **In sleep mode:** Instantly wake up the panel.
- **Long Press:**
  - **General action:** Enter submenus or confirm selection.
  - **In time configuration (timer):** Subtracts **-5 minutes** from the current value.
- **Extra Long Press (> 4 sec):**
  - **Manual override:** Forces shutdown (sleep mode), locking the timer until the next scheduled cycle.
- **Continuous Hold:**
  - **In time configuration (timer):** Automatically increments **+5 minutes** continuously in a loop while held down.

```text
🏠 MAIN MENU
├── 📂 Playlists
│   ├── 📄 Favorites
│   ├── 📄 Arcade
│   ├── 📄 ...
│   └── 🔙 Back
├── 📂 Playback
│   ├── 🖼️ Mode: [GIFs / Clock]
│   ├── 🔀 Random: [YES / NO]
│   ├── 🕹️ Arcade: [OFF / Batocera / Recalbox / ReplayOS / RetroBat]
│   ├── 💬 Text: [YES / NO]
│   └── 🔙 Back
├── ☀️ Brightness
│   └── Brightness: [5% - 100%]
├── 📶 WiFi: [ON / OFF]
│   ├── 🔄 Activate: [YES / NO]
│   ├── ➕ Add WiFi network
│   ├── 📱 APP Control: [YES / NO]
│   ├── 🏷️ IP: [192.168.1.117]
│   ├── 🔎 Show IP: [YES / NO]
│   └── 🔙 Back
├── 🕒 Clock: [ON / OFF]
│   ├── 🔄 Activate: [YES / NO]
│   ├── 🖼️ Every: [1...20] GIFs
│   ├── ⏳ Duration: [5...30] sec
│   ├── 🎨 Clock Style: [Matrix, Solid, Rainbow, Pulse, Gradient]
│   ├── 🎨 Color: [White, Red, Green, Blue, Yellow, Cyan, Magenta, Orange, Pink]
│   ├── 🔄 Transition: [YES / NO]
│   └── 🔙 Back
├── 🌡️ Weather: [ON / OFF]
│   ├── 🔄 Activate: [YES / NO]
│   └── 🔙 Back
├── 🕒 Timer: [ON / OFF]
│   ├── 🔄 Activate: [YES / NO]
│   ├── ⏳ ON: [00:00 to 24:00]
│   ├── ⏳ OFF: [00:00 to 24:00]
│   └── 🔙 Back
├── ⚙️ Advanced Settings
│   ├── ⚡ I2S Speed: [8, 10, 16, 20MHz]
│   ├── 🔄 Refresh Rate: [30, 60, 90, 120Hz]
│   ├── 🖼️ Buffer: [YES / NO]
│   ├── 👻 AntiGhost: [1, 2, 3, 4]
│   ├── 🚀 SD Read Speed: [4, 10, 20, 25MHz]
│   ├── 🎮 IR Remote Mapping: [On, Off, Menu, Validate, Up, Down, Brightness+, Brightness-]
│   ├── ⚠️ Reset
│   └── 🔙 Back
├── 🚀 Update
│   ├── 🔄 Check OTA
│   ├── 🔤 Download Languages
│   └── 🔙 Back
├── 📂 SD Explorer
│   ├── 🔄 Start FTP
│   └── 🔙 Back
├── 🌐 Language
│   ├── [ES] Español
│   ├── [EN] English
│   ├── [FR] Français
│   ├── ...
│   └── 🔙 Back
├── 💾 Save
└── 🔙 Exit
```

---

### 📱 PWA — Remote Control App

**[👉 Install or Try Retro Pixel LED Control](https://fjgordillo86.github.io/RetroPixelLED-Lite/control/)**

A modern web application installable on any device (phone, tablet, computer) connected to the same network as the panel. Requires no external server, works on the local network, and functions offline once installed. 📴

https://github.com/user-attachments/assets/f5231448-7862-4476-901e-ac25ac7f4248

The interface is divided into **6 sections**:

**1️⃣ Main Page (Home)**
- **☀️ Brightness Control:** 0-100% slider with instantaneous application, no reboot required.
- **🎛️ Mode Selector:** GIF, Clock, or Text in real time.
  - **GIF Mode:** Active playlist + random playback toggle.
  - **Clock Mode:** 5 styles (Matrix, Solid, Rainbow, Pulse, Gradient) and 9 colors.
  - **Text Mode:** Live 26×7 matrix preview as you type, with color, font, and scroll speed selection.
- **🔌 Connection Status:** Visual indicator (green/red) of the WiFi status.

**2️⃣ Timer ⏰**
- Toggle enable/disable.
- Power ON and Power OFF time (24h format).
- Immediate manual ON/OFF override button.
- Current status: Active (✓ green) or Sleeping (● gray).

**3️⃣ Text Mode**
- Real-time 26×7 matrix preview while typing.
- Color selector: 9-color palette + custom hex picker.
- **Font Selector:** `Bold`, `SemiBold`, `Regular`, `Light` — *(new in v3.1.2)*.
- Speed control: 5-200ms/step slider with live preview.
- "▶ Send" and "■ Stop" buttons.

**4️⃣ Update 🔄**
- **Firmware OTA:** Checks GitHub and automatically updates if a newer version is found.
- **New Version Notice:** An amber dot over the 🔄 icon alerts you when a update is available; opening the modal reveals the version number and changelog. *(new in v3.1.5)*
- **Download Languages:** Fetches `.json` files directly from GitHub into the SD card's `/idiomas` folder.

**5️⃣ Settings 🛠**

Remote editing of `config.ini`, organized across 7 sections: WiFi (SSID, Password, Show IP on boot, Timezone), Hardware (number of panels, RGB/RBG/GBR color order, brightness, I2S speed, refresh rate, buffering, anti-ghosting), Arcade (Batocera, Recalbox, ReplayOS, or None), Scrolling Text, Clock (active, particle transition, interval, duration, style, color), Weather (enable, city, API key, interval, custom text over clock), and Language (ES, EN, FR...).

Automatic reboot after saving, only when necessary for changes to take effect.

**6️⃣ About ℹ️** *(new in v3.1.5)*

Accessible from the "ℹ About" link at the footer of the main screen.
- **Versions:** PWA and panel firmware versions, with alerts if an update is available.
- **Credits:** Creator (`fjgordillo86`), MIT license, and source code link.
- **Live Diagnostics:** Free memory, last reboot reason, WiFi signal, SD card info, CPU, and firmware partition usage, with a **copy report** button to attach when reporting an issue.

#### ⚙️ PWA Installation and Setup

1. Open <https://fjgordillo86.github.io/RetroPixelLED-Lite/control/> from your device.
2. Tap the connection icon (⚙) and enter the panel's hostname (`retropixel.local`) or local IP address (e.g., `192.168.1.117`). If the hostname is not resolved on your device, the PWA will offer to use the last known IP.
3. *(Optional)* Install it as an app: Chrome/Edge usually prompts automatically; otherwise, go to Menu (⋮) → "Install application".
4. Done — it now appears as a native app, without typing URLs each time. 🎉

#### 📝 Requirements

- The panel must be on the same WiFi network as your device.
- `CONFI_APP_ENABLE=1` on the panel to enable control via the app.
- `TEXT_ENABLE=1` for sending custom text messages.
- Internet connection on the panel for OTA and language downloads (outbound access to GitHub).
- Languages are downloaded once and then work fully offline.

---

### 🏠 Home Assistant

Complete control from your Home Assistant dashboard via **local REST API** — no cloud, no internet dependency. Allows you to:
- 🟢 **Power ON / OFF** the panel using a `switch`.
- 📊 **Query current status** (active mode, playing playlist, etc.).
- 🔄 **Switch modes** (Clock / GIF) and **playlists** instantly.
- 💬 **Send scrolling text messages** choosing color, speed, and **font** directly from the dashboard.

See the [Complete Home Assistant Integration Guide](#-home-assistant-integration-complete-guide) below, containing a ready-to-copy `.yaml` configuration file.

---

### 🕹️ Arcade Mode (Batocera, Recalbox, RetroBat & ReplayOS)

Turns the panel into a dynamic marquee that reacts to your active gameplay, featuring two synchronization methods: local scripts (**Batocera / Recalbox / RetroBat**) or native network monitoring (**ReplayOS**).

Automatically displays:
1. **Game Marquee:** 24-bit `.bmp` image, or an **animated GIF** if one exists for that game — including multi-GIF sequences played one after another in a loop.
2. **System Logo:** `.bmp` image displayed while browsing systems.

Activated via `Menu → Playback → Arcade`. Complete installation instructions are in the [Arcade Integration Section](#️-arcade-integration-batocera-recalbox-retrobat-or-replayos).

---

### 🕒 Clock & Weather

- **Clock:** 5 styles (Matrix, Solid, Rainbow, Pulse, Gradient) and 9 colors, featuring a particle transition effect on appearance/disappearance. Interrupts the GIF gallery every *x* GIFs to show the clock for *x* seconds (configurable), resuming playback exactly where it left off.
- **Weather:** With a free OpenWeatherMap API key, displays temperature and weather icons above the clock alongside a customizable message (`WEATHER_MSG`). See [how to get your API key](#5--how-to-get-your-weather-api-key).

---

### ⏰ Timer

Scheduled power ON/OFF by time, featuring manual override via the physical button or the PWA. Pressing the button overrides the automated schedule until the next scheduled cycle.

---

### 🌐 Multi-language

**Dynamic dictionary system**: Language files are NOT kept permanently in RAM; they load only upon entering the menu and are released upon exiting — giving the GIF engine access to all available memory.

- **Location:** `/idioma/` on the SD card. The file name (without extension) corresponds to the menu option: `/idioma/EN.json` → "EN".
- **JSON Structure:** Blocks named `MENU`, `SUBMENU_XXX`, `ESTADOS`, `CONFIG_INI`.
- **Critical Rules:**
  - 🚫 No special non-ASCII characters or accents.
  - 📏 Maximum of 21 characters for submenu labels to ensure 128px centering.
  - 🔡 Include colons and spaces if you want them displayed (e.g., `"modo": "Mode: "`).
  - 💾 Save in UTF-8 format without BOM.
- **Remote Download:** From the PWA or OSD menu (`Update → Download Languages`), directly from GitHub, without removing the SD card.
- **WiFi Portal:** Built-in portal texts (panel and web) are compiled inside the firmware in ES, EN, and FR based on `LANGUAGE`; any other language defaults to English. They do not rely on SD language files.
- **New keys in v3.1.5:** The `SUBMENU_WIFI` block includes `anadir_red`, `portal_abriendo`, and `portal_reinicio`. If maintaining a custom language file, add these keys; if missing, the menu defaults to Spanish text for those specific options.

---

### 📂 FTP Server

Wireless file server for maintenance without removing the MicroSD card.

> [!IMPORTANT]
> Designed for **`config.ini`**, language files (`.json`), and **playlists** (`.txt`) — not intended for transferring entire GIF collections, as speed is limited compared to a hardware card reader.

**Activation:** `OSD Menu → SD Explorer → Start FTP`. The panel stops GIF playback and displays its IP address.

**Recommended Client — FileZilla:**

| Parameter | Value |
| :--- | :--- |
| Protocol | Plain FTP (insecure) |
| Server/Host | IP displayed on the panel |
| User / Password | `admin` / `admin` |
| Port | `21` |
| Simultaneous connections | 1 (limit enabled) |
| Download/Upload speed limit | 20 KiB/s |

You can also mount it as a network location in Windows Explorer (`ftp://<IP>`, user `admin`) — though FileZilla is recommended due to potential incomplete transfers in Windows Explorer tests.

**Safety Notes:** The panel suspends GIF playback while FTP is active (dedicating all CPU resources to file transfer). To exit, press the physical button or the "Validate" key on the IR remote. Do not disconnect power while editing files over FTP.

See the [Complete Step-by-Step FTP Guide](#-sd-explorer-ftp--details) below.

---

### 🔄 OTA Updates

Update without connecting the panel to a PC:
1. WiFi configured and active (in `config.ini` or via the [WiFi Portal](#-wifi-portal--access-by-name-mdns)).
2. The PWA alerts you when a new version is available: amber dot over 🔄, showing available version and release notes.
3. `OSD Menu → Update → Check OTA` (or via the PWA button).
4. The system downloads the firmware from GitHub and automatically reboots. 🔃

> [!WARNING]
> Do not disconnect power during the update process.

---

### 📶 WiFi Portal & Access by Name (mDNS)

Connect the panel to your network **without editing `config.ini`** or removing the MicroSD card, and access it anytime at `retropixel.local`.

**When the portal opens:**
- `OSD Menu → WiFi → Add network` — to change router or password at any time.
- **Automatically** on boot if `WIFI_ENABLE=1` and `SSID` is empty in `config.ini` (first installation).

**Step-by-Step:**
1. The panel displays its own AP network: `WiFi: RetroPixel-LED`, `Pass:` (8 random digits, generated fresh each time), and `IP: 192.168.4.1`.
2. Connect your phone to that network. The configuration page will open automatically; if not, navigate to `192.168.4.1`.
3. Tap your network in the list (or manually enter the name if hidden), enter the password, and press **Connect**.
4. The panel tests the connection (up to 15s). If successful, it saves settings to `config.ini`, displays its assigned IP and `retropixel.local`, and reboots. If it fails, it displays `NO CONECTA` and lets you try again.

**To exit without saving:** Tap **Cancel** on the webpage, press **OK** on the IR remote, or click the physical button. If inactive, the portal automatically closes after 5 minutes.

> [!NOTE]
> - Requires an SD card present, as network credentials are saved to `config.ini`.
> - The ESP32 supports **2.4 GHz** networks only.
> - Texts render in Spanish or French if `LANGUAGE` is `ES` or `FR`; any other language defaults to English.
> - To conserve memory, the panel temporarily disables *Double Buffer* while the portal is active. Your original buffering setting is restored upon saving.

**Access by Name (mDNS):** When WiFi is active, the panel resolves at **`http://retropixel.local`**. In the PWA, enter `retropixel.local` instead of an IP address; if your device fails to resolve mDNS, the PWA offers to fall back to the last known IP.

> [!TIP]
> `.local` works on Windows 10/11, macOS, iOS, and Linux. Some Android devices and specific network configurations (guest networks, VLANs, routers blocking multicast) may not resolve mDNS: in those cases, use the static local IP.

**Diagnostics:** Navigating to `http://retropixel.local/diag` returns a read-only JSON object detailing free and minimum heap, last reboot reason, WiFi signal strength, SD status, and firmware partition usage. This matches the data shown in the **About** page of the PWA.

---

## ⚙️ Installation and Setup

### 1. 🚀 Program the ESP32 (Web Installer)

Install this version without installing software on your PC:

**[👉 Open the Retro Pixel LED Lite Web Installer](https://fjgordillo86.github.io/RetroPixelLED-Lite/)**

1. Use a supported browser (**Google Chrome** or **Microsoft Edge**).
2. Connect your ESP32 via USB.
3. Click **"Install"** and select the appropriate COM port.
4. **Important:** Check **"Erase device"** for a complete memory wipe to prevent fragmentation errors.

> 💡 **ESP32 not recognized?** If no COM port appears, install the USB driver for your board's interface chip:
> - **CP2102 Chip:** [Silicon Labs Drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
> - **CH340/CH341 Chip:** [SparkFun Drivers](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all)

### 2. 📂 SD Card Preparation

Format your MicroSD card in **FAT32** format and copy the entire contents of the [Contenido SD](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD) folder to the root directory:

```text
/ (SD Root)
├── gifs/                        <-- Your GIF folders (Arcade, Consoles, etc.)
├── idioma/                      <-- .json files containing translated texts.
│   ├── ES.json
│   ├── EN.json
│   └── FR.json
├── playlists/                   <-- Lists generated by the "Playlist Generator" script.
│   ├── Arcade.txt
│   ├── Computers.txt
│   ├── Consolas.txt
│   └── Todos.txt
├── config.ini                   <-- WiFi and panel configuration file.
└── Generador de Playlists.bat   <-- Windows script to generate playlists.
```

> [!IMPORTANT]
> Whenever you add, delete, or move GIFs inside `/gifs/`, re-run **Generador de Playlists.bat** to update the index.

### 3. 📝 Configuration via `config.ini`

The `config.ini` file is located in the [Contenido SD](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD) folder — copy it to the root of your SD card and configure it:

```ini
# ============================================================
# 🕹️ RETRO PIXEL LED LITE v3.1.5 - CONFIGURATION FILE
# ============================================================
# Note: Do not leave spaces around the '=' symbol.
# Correct example: BRIGHTNESS=40

[WIFI_NTP]
# Configure your WiFi network. Leave empty (SSID= and PASS=) to configure via phone using the panel's WiFi portal.
# If your password contains '#', write it as is (a '#' is only treated as a comment when preceded by a space).
WIFI_ENABLE=1
SSID=
PASS=
# Show IP address on boot
MOSTRAR_IP=0
# Configure your time zone
TZ=CET-1CEST,M3.5.0,M10.5.0/3

[HARDWARE]
# Number of chained panels
PANEL_CHAIN=2
# Panel color order: RGB, RBG, or GBR
COLOR_ORDER=RGB
# Brightness (0 to 255)
BRIGHTNESS=38
# I2S Speed: 0=8MHz, 1=10MHz, 2=16MHz, 3=20MHz (Turbo)
I2S_SPEED=2
# Minimum Refresh Rate (Hz): 30 to 120
REFRESH_MIN=120
# Double Buffer: 0=OFF, 1=ON (Eliminates flickering)
DOUBLE_BUFF=0
# Anti-Ghosting: 1 to 4 (Increase if ghosting artifacts appear)
LATCH_BLANK=1
# SD Read Speed (MHz): 4, 10, 20, or 25. Lower if reading errors occur
SD_SPEED=10

[LOGIC]
# Display Mode: 0=GIFs, 1=Clock Only
PLAY_MODE=0
# Enable or disable configuration via APP: 0=OFF, 1=ON (Requires WiFi)
CONFI_APP_ENABLE=1
# Select Arcade System: 0=OFF, 1=Batocera, 2=Recalbox, 3=ReplayOS, 4=RetroBat
ARCADE_ENABLE=0
# Enable or disable scrolling text: 0=OFF, 1=ON (Requires WiFi)
TEXT_ENABLE=1
# Enable or disable clock: 0=OFF, 1=ON (Requires WiFi)
CLOCK_ENABLE=1
# Playback Mode: 0=Sequential, 1=Random
RANDOM_MODE=1
# Interval: How many GIFs play between clock displays
AUTO_CLOCK_INT=6
# Duration: How many seconds the clock is displayed
CLOCK_DURATION=10
# Clock Styles: 0=Solid, 1=Aurora, 2=Rainbow, 3=Gradient, 4=Fire, 5=Glitch
CLOCK_STYLE=2
# Enable particle burst transition from clock to GIFs: 0=OFF, 1=ON
TRANSITION_ENABLE=1
# Clock Color (0=White, 1=Red, 2=Green, 3=Blue, 4=Yellow, 5=Cyan, 6=Magenta, 7=Orange, 8=Pink)
CLOCK_COLOR=4

[WEATHER]
# Enable weather display: 0=OFF, 1=ON (Requires WiFi)
WEATHER_ENABLE=1
# Your city (No spaces, use '+' if needed: Madrid,ES or Buenos+Aires,AR)
CITY=Navalmoral+de+la+Mata,ES
# Your free OpenWeatherMap API Key
API_KEY=xxxxxxxxxxxxxxxxxxxxxxx
# Weather update interval in MINUTES
WEATHER_INT=60
# Text displayed above the clock
WEATHER_MSG=Game Room

[LANGUAGE]
# Set Language (File name without .json extension: ES, EN, FR...)
LANGUAGE=ES

[IR_REMOTE]
# IR Remote HEX codes (Retro Pixel LED will save these automatically via OSD mapping)
BTN_ON=F20DFF00
BTN_OFF=E01FFF00
BTN_BRILLO_UP=F609FF00
BTN_BRILLO_DOWN=E21DFF00
BTN_MENU=EA15FF00
BTN_OK=ED12FF00
BTN_SUBIR=E41BFF00
BTN_BAJAR=B34CFF00

[REPLAY_OS]
# IP address assigned to ReplayOS
IP=192.168.1.101
# ReplayOS Token: SYSTEM > INFORMATION > NET CONTROL CODE
TOKEN=xxxxxx

[END]
```

### 4. 🌍 Time Zone (TZ) Configuration

For the **Clock** and **Timer** to function correctly, `TZ` must strictly follow POSIX format syntax.

- **Spain (Mainland & Balearic Islands) / France / Italy:** `TZ=CET-1CEST,M3.5.0,M10.5.0/3`
- **Canary Islands / Portugal / UK:** `TZ=WET0WEST,M3.5.0/1,M10.5.0`

👉 Refer to the **[ESP32 TZ Tool / Database](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)** to retrieve the exact string for your region.

**Format Breakdown:** `CET-1CEST` (Standard name, UTC offset UTC+1, Daylight name) · `M3.5.0` (Daylight transition: March, last week, Sunday) · `M10.5.0/3` (Standard transition: October, last week, Sunday at 03:00).

### 5. ☁️ How to Get Your Weather API Key

1. Go to [OpenWeatherMap.org](https://openweathermap.org/) and register a free account.
2. Under your profile, click **"My API Keys"**.
3. Generate a new key (e.g., "RetroPixel").
4. **Important:** New keys can take between 30 minutes and 2 hours to activate. If the panel reads "0.0C", wait for key activation. ⏳
5. Copy the generated key into the `API_KEY=` field in your `config.ini`.

**🔍 Verify your city setting:** Paste `http://api.openweathermap.org/data/2.5/weather?q=YOUR_CITY&appid=YOUR_API_KEY` into your browser (replacing placeholder values). If a JSON payload returns, setup is correct; HTTP 401/404 errors indicate an inactive API key or incorrect city name formatting.

---

## 📖 Playlist Generator (Windows)

The `Generador de Playlist v1.0.1.bat` script builds customized playlists without editing code. It is located in the [Contenido SD](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Contenido%20SD) folder.

1. **Setup:** Place the `.bat` file in the root directory of your SD card, alongside the `gifs` folder.
2. **Execution:** Double-click the file — a command prompt window will launch.
3. **Selection:** The script scans and lists subfolders inside `/gifs`. Enter subfolder numbers separated by commas (e.g., `3,4,10`) or type `TODO` for all.
4. **Naming:** Enter your desired playlist name (e.g., `MyFavorites`).
5. **Output:** Generates `playlists/MyFavorites.txt` containing formatted paths for the ESP32.
6. **Usage:** Insert the SD card into the panel; it will automatically play the first playlist found. To change playlists, go to `OSD Menu → Playlists`. 🎞️

<img width="514" height="565" alt="Playlist Script" src="https://github.com/user-attachments/assets/3c600615-5539-4430-af7b-26cd219fc7fe" />

---

## 🕹️ Arcade Integration (Batocera, Recalbox, RetroBat or ReplayOS)

Enable `Menu → Playback → Arcade` to instruct the panel to display active game or system marquees:

```text
🏠 MAIN MENU
├── 📂 Playback
│   ├── 🖼️ Mode: [GIFs / Clock]
│   ├── 🔀 Random: [YES / NO]
│   ├── 🕹️ Arcade: [OFF / Batocera / Recalbox / ReplayOS / RetroBat]   <-- HERE
│   └── 🔙 Back
```

> [!IMPORTANT]
> To synchronize ROMs, run the PC script, and install communication scripts, consult the system-specific integration guides:
>
> **[👉 Batocera Instructions](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_BATOCERA.md)**
>
> **[👉 Recalbox Instructions](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_RECALBOX.md)**
>
> **[👉 RetroBat Instructions](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_RETROBAT.md)**
>
> **[👉 ReplayOS Instructions](https://github.com/fjgordillo86/RetroPixelLED-Lite/blob/main/README_REPLAYOS.md)**

---

## 🏠 Home Assistant Integration (Complete Guide)

Integrate and fully control **Retro Pixel LED Lite** from **Home Assistant** using the local REST API, without cloud dependencies.

This integration allows you to:
- 🟢 **Turn ON / OFF** the panel using a `switch`.
- 📊 **Monitor current status** (active mode, currently playing playlist, etc.).
- 🔄 **Switch modes** (Clock / GIF) and **playlists** instantly.
- 💬 **Send scrolling text messages**, setting custom color, speed, and font options directly from the dashboard.

### 📦 Adding the Configuration to Home Assistant

If using package-based configuration structuring (`packages`), place the `retropixel.yaml` file located **[here](https://github.com/fjgordillo86/RetroPixelLED-Lite/tree/main/Home%20Asisstant)** inside your `/config/packages/` directory. If not using `packages`, paste its contents into your primary `configuration.yaml` file.

> ⚠️ **Important:** Replace IP address `192.168.31.210` with your ESP32's assigned local IP address, and update playlist names to match your SD card structure.

An `entities.yaml` card configuration is also included, ready to add to your Home Assistant dashboard.

<img width="822" height="1005" alt="HA Screenshot" src="https://github.com/user-attachments/assets/9294b479-f428-4c68-9fcc-c871ad2e88e4" />

---

## 🧠 Internal Architecture / Core Lite

Technical details describing internal system operations:

- **📡 Dynamic IR Control & Mapping:** Infraded remotes with runtime function mapping configurable via OSD (brightness, menu navigation, power toggle, selection confirm).
- **📂 Maintenance FTP Server:** Wireless file system management for `config.ini` and playlists without removing the MicroSD card.
- **Anti-Panic RAM Management:** Heap allocation monitoring — if DMA memory allocation fails after WiFi initialization, the system gracefully switches to Single Buffer mode to guarantee stability.
- **Binary Search Engine (Arcade):** Locates game marquees across thousands of files within milliseconds, jumping directly to exact byte offsets on the SD card using alphabetically sorted indices — avoids scanning directory listings.
- **Adaptive Memory Management (Single/Double Buffer):** Double Buffer for smooth GIF frame transitions; dynamically switches to Single Buffer in Arcade mode to process high-resolution bitmap assets.
- **Real-Time HTTP API:** Command endpoint handler facilitating real-time synchronization with Batocera, Recalbox, ReplayOS, and Home Assistant.
- **📶 Built-in WiFi Portal:** Temporary access point featuring captive portal DNS and a dedicated lightweight web server that destroys itself upon completion, leaving no exposed open routes on the network. Scans WiFi networks *before* spinning up the AP, generates a random password displayed strictly on the panel matrix, and runs in Single Buffer mode to minimize RAM overhead (restoring original buffer mode upon saving).
- **🏷️ mDNS:** Advertises host presence as `retropixel.local` while connected to WiFi; PWA preserves last known IP as fallback target.
- **🩺 Diagnostics `/diag`:** Read-only JSON endpoint returning system uptime, free/minimum heap, maximum allocatable block, last reboot reason, RSSI, SD capacity/speed, CPU clock, and OTA partition metrics.
- **🧩 Modular Codebase Architecture:** HTTP API routing resides in `WebRoutes.ino`, while portal operations are contained in `PortalWiFi.ino` (leveraging `PortalPaginas.h` and `PortalTextos.h`).
- **Smart Text Centering:** Automatically centers menu entries and status strings across the display matrix (`offset + 64px`) by calculating character width pixel lengths dynamically.
- **WiFi Stealth Mode:** WiFi initializes briefly to synchronize NTP time and fetch weather data; the system then operates **100% offline**, maintaining **0 lag** during media playback.
- **Dynamic Notification Bar:** When weather is active, the clock shifts lower (`startY=9`) to render `WEATHER_MSG`, animated condition icons, and current temperature readings.
- **Advanced Day/Night Iconography:** Custom 8×8 px weather icons (Sun, Moon, Clouds, Rain, Snow, Thunderstorm, Fog), rendered according to local time.
- **Dynamic Playlist System:** Parses `.txt` files located in `/playlists/`, enabling instant switching between thematic media sets via the OSD menu.
- **Clock Auto-Interruption:** Automatically interrupts GIF loops every *x* GIFs to display time for *x* seconds, resuming media rendering without losing frame positions.
- **Offline Resilience:** If WiFi is unreachable, NTP sync loops bypass gracefully, rendering media immediately using the ESP32's internal RTC.
- **Double Buffer Rendering Engine:** Leverages ESP32 DMA hardware to render display frames off-screen without display flicker.

---

## 📜 Detailed Changelog (v3.0.0 → v3.1.5)

| Feature | Technical Detail | Benefit |
| :--- | :--- | :--- |
| **📶 Built-in WiFi Portal** | Temporary Access Point (`RetroPixel-LED`) showing a random key on screen, captive portal DNS, and an embedded web page displaying available networks sorted by signal strength. Validates connections prior to writing `config.ini`. Triggered from `OSD Menu → WiFi → Add network` or automatically when `WIFI_ENABLE=1` and `SSID` is empty. Texts in ES/EN/FR embedded in firmware. | **Connect the panel without manually editing files.** Network changes no longer require removing the SD card. |
| **🏷️ Access by Name (mDNS)** | Panel advertises itself as `retropixel.local`. PWA supports hostname or IP configuration (`GET /status` now includes `ip`), preserves last known IP, and offers fallback if mDNS fails to resolve. | **No longer tied to dynamic IP addresses** assigned by routers. |
| **🔔 Update Notification** | PWA compares firmware versions (`fw` and `fwNum` in `GET /status`) against `version.json` hosted on GitHub Pages. Displays an amber indicator over 🔄 alongside release version and changelog details (`notes`). | **Stay informed about new releases** without manual checks. |
| **ℹ️ "About" Page (PWA)** | Reports PWA version (parsed from service worker cache) and firmware version, creator details, MIT license, and live diagnostic stats with copy-to-clipboard functionality. | **Easier issue reporting** and live visibility into panel health. |
| **🩺 Diagnostic Endpoint `/diag`** | `GET /diag`: Returns uptime, free and minimum heap, largest free block, last reset reason, RSSI, SD size/speed, CPU frequency, and OTA partition utilization stats. | **Memory and stability diagnostics** without needing a USB debugging connection. |
| **🔐 WiFi Passwords with `#`** | Updated `config.ini` parser so `#` symbols within `SSID` and `PASS` values are preserved unless preceded by whitespace. | **Supports all WiFi passwords**, including those containing `#`. |
| **🛡️ PWA Service Worker** | Added versioning to `sw.js` (cache naming), fault-tolerant installation handling, strict path routing (bypassing panel API calls), and isolated cache purging routines. | **More reliable PWA updates.** |
| **🕹️ RetroBat Support** | Integration with RetroBat frontend for automatic detection and rendering of game-associated marquees and GIFs. | **Dynamic arcade experience.** Displays active game marquees upon switching titles in RetroBat. |
| **💾 Configurable SD Read Speed** | Introduced `SD_SPEED` parameter (4, 10, 20, or 25 MHz; default 10) editable via PWA, OSD (Advanced Settings), and `config.ini`. Implements two-phase boot with automatic speed fallback if SD fails to respond. | **Faster GIF loading** customized to individual SD card performance specs without recompilation. |
| **🎞️ GIF Playback Timing Fix** | Eliminated double-delay calls between frames (synchronized `playFrame` + artificial delay). Frame decoding time is now subtracted from wait intervals. | **Smoother playback** matching true frame rates encoded in GIFs. |
| **🧩 Minimal SPIFFS Partitioning** | Migrated to *Minimal SPIFFS* partition scheme (1.9 MB application space with OTA capacity) upgrading from default 1.25 MB footprint. | **Expanded firmware space** for upcoming features. Requires web-flashing when updating from older versions. |
| **🕹️ RePlayOS Support** | Integration with RePlayOS frontend for automatic detection and rendering of game marquees and GIFs. | **Dynamic arcade experience.** Displays active game marquees upon switching titles in RePlayOS. |
| **⏰ New Clock Rendering Styles** | Added customizable visual clock layouts and rendering routines. | **Greater aesthetic customization** tailored to different environments. |
| **📡 IR Mapping via PWA** | PWA interface to capture, assign, and commit IR remote button codes directly to `config.ini`. | **Intuitive IR remote setup** available directly from the PWA. |
| **🏠 Home Assistant Integration** | Exposed REST API endpoints (`GET /status`, `POST /control`, `/playlist`, `/texto`, `/timer/toggle`) alongside a complete YAML configuration package. | **Home automation and control.** Turn on/off, change modes, switch playlists, and push text messages from HA. |
| **🔤 Font Selection for Text** | 4 selectable typography styles (`Bold`, `SemiBold`, `Regular`, `Light`) configurable via HTTP parameters, PWA, and REST integrations. | **Visual customization** for scrolling text displays. |
| **🎛️ PWA Control Panel** | Progressive Web App interface featuring 5 control modules and full remote editing for `config.ini`. | **Complete device management** from any local device without external servers. |
| **☀️ Brightness Control** | Real-time slider (0-100%) with immediate display updates. | **Smooth adjustments** matching ambient light conditions. |
| **🔤 Scrolling Text UTF-8 Support** | On-the-fly UTF-8 to Latin-1 decoder adding support for accented characters and extended alphabets. | Displays accented characters (ñ, á, ł, ą) without corruption. |
| **🎨 GIF / Clock / Text Modes** | Mode switcher on PWA homepage applying changes without requiring reboot. | Instant visual updates on display matrix. |
| **🎞️ Dynamic Playlists** | Real-time playlist switching via PWA interface. | Change media categories without interrupting engine execution. |
| **⏰ Smart Timer** | Automated power schedules with manual button and PWA overrides. | Full power management automation. |
| **🔄 Remote Updates (OTA + Languages)** | Downloads firmware binaries and language packs directly from GitHub without removing SD cards. | Fully wireless maintenance workflows. |
| **⚙️ Full Remote Configuration** | Remotely edit `config.ini` settings: WiFi, hardware parameters, playback options, weather settings, and languages. | Complete wireless configuration control. |
| **💬 Scrolling Text Management** | HTTP POST endpoints + PWA modules for text messages, color parameters, and scroll speed settings. | Push messages on the fly without reprogramming firmware. |
| **💥 Particle Transitions** | Custom particle engine animating clock entry and exit transitions. | Visually fluid transitions without static screen cuts. |
| **🎨 OSD Color Selection** | Interactive OSD menu mapped to IR remotes and saved to EEPROM/SD. | Change clock colors without editing `config.ini`. |
| **⚡ Flicker-Free Clock** | Optimized *Single Buffer* rendering execution. | Zero flickering during fast screen value redraws. |
| **🧠 RAM Optimization** | Replaced `String` references with `char[]`, widespread deployment of `PSTR()`/`F()` macros. | Zero heap fragmentation; maximizes allocatable RAM for Double Buffering. |
| **🛡️ Anti-Panic System** | Validates `display->begin()` calls with automatic fallback to Single Buffer mode on memory allocation errors. | Prevents system crashes (`StoreProhibited`) caused by memory fragmentation after WiFi calls. |
| **🖱️ Confirmed Input Actions** | Enforces long-press holding times for structural configuration steps. | Prevents accidental navigation into submenus. |
| **📂 Integrated FTP Server** | Direct wireless file management access to MicroSD storage. | Manage playlists, `.ini`, and `.json` files without removing storage cards. |
| **📡 IR Remote Control** | Dynamic function mapping routines supporting flexible infrared remotes. | Control brightness, power states, and OSD menus via remote. |
| **🎨 Custom Color Ordering** | Configurable `colorOrder` parameter (RGB/RBG/GBR) specified dynamically in `config.ini`. | Compatibility with all HUB75 matrix hardware panels without recompiling. |

---

## 🛒 Bill of Materials

Tested components used during hardware development:

- **Microcontroller:** [ESP32 DevKit V1 (30 pins) - AliExpress](https://es.aliexpress.com/item/1005005704190069.html)
- **LED Matrix Panel (HUB75):** [P2.5 / P4 RGB Matrix Panel - AliExpress](https://es.aliexpress.com/item/1005008479388445.html)
- **Card Reader:** [Micro SD Adapter Module (SPI Interface) - AliExpress](https://es.aliexpress.com/item/1005005591145849.html)
- **ESP32-to-Panel Interconnect PCB:** [DMDos Board V3 - Mortaca](https://www.mortaca.com/) *(optional solderless solution featuring integrated SD slot)*
- **IR Receiver:** [Universal Infrared Receiver Sensor - AliExpress](https://es.aliexpress.com/item/1005005343424296.html)
- **Push Button:** [DS-316 Momentary Push Button Switch - AliExpress](https://es.aliexpress.com/item/4000888761296.html)
- **Power Supply:** 5V Power Supply (minimum 2A recommended for 64×32 panels).

---

## 🔌 Connections

If using the **DMDos Board V3**, wiring is pre-routed — skip to the next section.

#### 📂 Micro SD Card Reader (SPI Interface)
| SD Pin | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **CS** | GPIO 5 | Chip Select |
| **CLK** | GPIO 18 | Clock |
| **MOSI** | GPIO 23 | Master Out Slave In |
| **MISO** | GPIO 19 | Master In Slave Out |
| **VCC** | 3.3V | Power Supply |
| **GND** | GND | Ground Reference |

#### 🖼️ RGB LED Matrix Panel (HUB75 Interface)
| Panel Pin | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **R1** | GPIO 25 | Red Data (Upper Half) |
| **G1** | GPIO 26 | Green Data (Upper Half) |
| **B1** | GPIO 27 | Blue Data (Upper Half) |
| **R2** | GPIO 14 | Red Data (Lower Half) |
| **G2** | GPIO 12 | Green Data (Lower Half) |
| **B2** | GPIO 13 | Blue Data (Lower Half) |
| **A** | GPIO 33 | Row Select A |
| **B** | GPIO 32 | Row Select B |
| **C** | GPIO 22 | Row Select C |
| **D** | GPIO 17 | Row Select D |
| **E** | GND | Ground Reference |
| **CLK** | GPIO 16 | Clock |
| **LAT** | GPIO 4 | Latch |
| **OE** | GPIO 15 | Output Enable (Brightness Control) |

#### 🕹️ User Controls (Physical & Infrared)
| Component | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **Button (PIN)** | GPIO 21 | **Multifunction:** Click (Navigate) / Long Press (Confirm - Power Toggle). |
| **Button (GND)** | GND | Ground Reference. |
| **IR Receiver (Data)** | GPIO 34 | Signal Input (NEC Protocol/etc). |
| **IR Receiver (VCC)** | 3.3V | Sensor Power Supply. |
| **IR Receiver (GND)** | GND | Ground Reference. |

<img width="769" height="716" alt="Wiring diagram" src="https://github.com/user-attachments/assets/11fef006-59f3-405f-b00a-a32c9bba7bc5" />

---

### 📂 SD Explorer (FTP) — Details

This mode activates an embedded wireless FTP server to simplify maintenance without removing the MicroSD card.

> [!IMPORTANT]
> **Recommended Usage:** Managing `config.ini`, language files (`.json`), playlists (`.txt`), and lightweight files. Due to ESP32 bandwidth limits, **transferring full GIF media collections over FTP is not recommended** — use a hardware card reader for large files.

**Starting the FTP Server:**
1. Navigate to `OSD Menu → SD Explorer`.
2. Select **Start FTP**.
3. The panel pauses GIF rendering and displays its **IP address** (e.g., `192.168.1.109`).

**Connection Parameters (FileZilla):**
- **Protocol:** Plain FTP (insecure).
- **Host/Server:** IP address displayed on the panel matrix.
- **Logon Type:** Normal.
- **User / Password:** `admin` / `admin`.
- **Port:** `21`.
- **Simultaneous connections:** Limit strictly to 1.

<img width="545" height="227" alt="FileZilla setup 1" src="https://github.com/user-attachments/assets/1b537615-3e39-48ba-9eb0-48b03931c5f9" />
<img width="544" height="193" alt="FileZilla setup 2" src="https://github.com/user-attachments/assets/ba4c85bc-920a-48c9-83d8-99b96ecbc57f" />

**Under Edit → Settings → Transfers:**
- Maximum simultaneous transfers: Set to 1.
- Speed Limits: Enable limits and set both Download/Upload limits to 20 KiB/s.

<img width="841" height="522" alt="FileZilla limits" src="https://github.com/user-attachments/assets/e90d3e84-9c93-45c0-b942-8b601db40041" />

**Alternative Method (Windows Explorer)** *(not recommended — testing revealed occasional incomplete file uploads)*:
1. Open **This PC** → Right-click → **"Add a network location"**.
2. Specify location address: `ftp://<PANEL_IP>` (e.g., `ftp://192.168.1.109`).
3. Uncheck "Log on anonymously", set user to `admin`.
4. Assign a descriptive name to the network shortcut.

**Safety Notes:**
- The panel does not play GIFs while FTP is active.
- To exit FTP mode: Press the physical button or the "Validate" key on your IR remote.
- Do not disconnect power during active file operations to prevent SD corruption.

---

## 🛠️ Roadmap

### ⚡ Optimization and Functionality

- 📂 PWA File Manager: Upload GIFs and create custom playlists directly over WiFi (pending performance and stability testing).

### 🎨 Aesthetics and Connectivity

- 🎮 Built-in Mini-Games (Snake, Pong) controlled using the IR remote control.

---

## ⚖️ License and Acknowledgments

This project is licensed under the **MIT License**.

Special thanks to the developers of the underlying libraries:
- **Bitbank2** for the excellent `AnimatedGIF` rendering library.
- **Mrfaptastic** for the high-performance ESP32 DMA LED matrix display driver engine.
- **DMDos Telegram Community**, where witnessing the capabilities of DMDos inspired the development of **Retro Pixel LED**.
- **RpiTe@m** for sharing the free 600 GIF pack and their collection of 11,000 GIFs, available [here](https://www.neo-arcadia.com/forum/viewtopic.php?t=67065).
- **shan-aya** for providing French translations and authoring the [DMD GIF converter](https://github.com/shan-aya/DMD_GIF_converter) utility.
- **joseAveleira** for designing the particle transition effects used in the clock engine. [GitHub Repository](https://github.com/joseAveleira/RelojPixel/tree/main)

---

<p align="center">Made with ❤️ and lots of retro GIFs. If you find this project useful, leaving a ⭐ on the repo helps out a lot!</p>
