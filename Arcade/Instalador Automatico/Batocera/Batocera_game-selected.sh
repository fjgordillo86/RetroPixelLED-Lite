#!/bin/bash
# Colocar en: /userdata/system/configs/emulationstation/scripts/game-selected/
# Argumentos que envía EmulationStation: $1=sistema  $2=ruta_rom  $3=nombre_rom

IP_ESP32="192.168.1.117"
PORT_ESP32=8888
LOGOS_DIR="/userdata/roms/marquesinas/Logos"
BASE_DIR="/userdata/roms/marquesinas/Arcade"
SENDER="/userdata/userscripts/Batocera_marquesina.py"

SISTEMA="$1"
GAME_PATH="$2"

JUEGO_LIMPIO=$(basename "${GAME_PATH%.*}")
BMP_FILE="$BASE_DIR/$SISTEMA/$JUEGO_LIMPIO.bmp"
[ ! -f "$BMP_FILE" ] && BMP_FILE="$LOGOS_DIR/$SISTEMA.bmp"

python3 "$SENDER" "$IP_ESP32" "$PORT_ESP32" "$BMP_FILE" &
