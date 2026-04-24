#!/bin/bash
# Dirección IP de tu ESP32
IP_ESP32="192.168.1.109"

# Enviamos la señal de STOP para que el ESP32 ponga el GIF por defecto
curl -G "http://$IP_ESP32/batocera" \
    --data-urlencode "s=STOP" \
    --data-urlencode "g=STOP" > /dev/null 2>&1 &
