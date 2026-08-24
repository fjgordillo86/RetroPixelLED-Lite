#!/bin/bash
# Colocar en: /userdata/system/configs/emulationstation/scripts/game-end/
# (Opcional) Envía STOP al ESP32 cuando sales de un juego y vuelves al menú.
# Ahora además para el streamer de gifs (Batocera_game-start.sh) si estaba activo.

IP_ESP32="192.168.31.209"
PORT_ESP32=8888
SENDER="/userdata/userscripts/Batocera_marquesina.py"
STOP_FLAG="/tmp/retropixel_marquee.stop"

# 1. Avisamos al streamer (si hay uno activo) para que se pare solo
touch "$STOP_FLAG"

# 2. Le damos un margen corto para que se entere y cierre limpio
sleep 1

# 3. Red de seguridad: si sigue vivo pasado ese margen, lo matamos por patrón
if pgrep -f "pixel_stream.py" > /dev/null; then
    pkill -f "pixel_stream.py"
fi

# 4. Limpieza por si el streamer no llegó a borrar su propia bandera
rm -f "$STOP_FLAG"

# 5. STOP al ESP32, igual que hasta ahora (cubre tanto el caso estático como el animado)
python3 "$SENDER" "$IP_ESP32" "$PORT_ESP32" "STOP" &
