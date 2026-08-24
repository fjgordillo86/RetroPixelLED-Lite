#!/bin/bash
# Colocar en: /userdata/system/configs/emulationstation/scripts/game-start/
# Sustituye a pixel_start.sh (el endpoint /batocera al que apuntaba ya no existe).
#
# Si hay un gif (o secuencia de gifs) para este juego, lanza el streamer en
# segundo plano. Si no hay ninguno, no hace nada: la imagen estática que ya
# pintó game-selected se queda tal cual en el panel.

IP_ESP32="192.168.31.209"
PORT_ESP32=8888
BASE_DIR="/userdata/roms/marquesinas/Arcade"
STREAMER="/userdata/userscripts/pixel_stream.py"
STOP_FLAG="/tmp/retropixel_marquee.stop"

# Limpiamos el sistema: de /userdata/roms/snes/... sacamos solo "snes"
SISTEMA_SUCIO="$1"
SISTEMA=$(echo "$SISTEMA_SUCIO" | awk -F'/' '{print $(NF-1)}')

# Limpiamos el juego: quitamos ruta, extensión y barras invertidas
JUEGO_SUCIO=$(basename -- "$2")
JUEGO_SIN_EXT="${JUEGO_SUCIO%.*}"
JUEGO_LIMPIO=$(echo "$JUEGO_SIN_EXT" | sed 's/\\//g')

# Por si quedó una bandera de una partida anterior que no se limpió bien
rm -f "$STOP_FLAG"

CARPETA_SISTEMA="$BASE_DIR/$SISTEMA"

python3 "$STREAMER" "$IP_ESP32" "$PORT_ESP32" "$CARPETA_SISTEMA" "$JUEGO_LIMPIO" "$STOP_FLAG" &
