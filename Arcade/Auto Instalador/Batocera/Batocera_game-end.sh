#!/bin/bash
# Colocar en: /userdata/system/configs/emulationstation/scripts/game-end/
# (Opcional) Envía STOP al ESP32 cuando sales de un juego y vuelves al menú.

IP_ESP32="192.168.1.117"
PORT_ESP32=8888
SENDER="/userdata/userscripts/Batocera_marquesina.py"

python3 "$SENDER" "$IP_ESP32" "$PORT_ESP32" "STOP" &
