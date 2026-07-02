#!/bin/bash
IP_ESP32="192.168.1.108"

# Limpiamos el sistema: de /userdata/roms/snes/... sacamos solo "snes"
SISTEMA_SUCIO="$1"
SISTEMA=$(echo "$SISTEMA_SUCIO" | awk -F'/' '{print $(NF-1)}')

# Limpiamos el juego: quitamos ruta, extensión y barras invertidas
JUEGO_SUCIO=$(basename -- "$2")
JUEGO_SIN_EXT="${JUEGO_SUCIO%.*}"
JUEGO_LIMPIO=$(echo "$JUEGO_SIN_EXT" | sed 's/\\//g')

# Enviamos los datos ya limpios
curl -s -G \
    --data-urlencode "s=$SISTEMA" \
    --data-urlencode "g=$JUEGO_LIMPIO" \
    "http://$IP_ESP32/batocera" > /dev/null &