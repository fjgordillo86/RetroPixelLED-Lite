#!/bin/bash
# Colocar en: /userdata/system/configs/emulationstation/scripts/quit/
# Se dispara al reiniciar Batocera de forma normal. Avisa al panel para que deje la marquesina.

IP_ESP32="192.168.31.209"
PORT_ESP32=8888
SENDER="/userdata/userscripts/Batocera_marquesina.py"

python3 "$SENDER" "$IP_ESP32" "$PORT_ESP32" "STOP"