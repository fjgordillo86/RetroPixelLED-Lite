#!/bin/bash
# Colocar en: /userdata/system/configs/emulationstation/scripts/system-selected/
# Argumento que envía EmulationStation: $1=sistema

IP_ESP32="192.168.1.117"
PORT_ESP32=8888
LOGOS_DIR="/userdata/roms/marquesinas/Logos"
SENDER="/userdata/userscripts/Batocera_marquesina.py"

SISTEMA="$1"
BMP_FILE="$LOGOS_DIR/$SISTEMA.bmp"

python3 "$SENDER" "$IP_ESP32" "$PORT_ESP32" "$BMP_FILE" &
