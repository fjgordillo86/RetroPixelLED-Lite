#!/bin/bash
IP_ESP32="192.168.1.117"
PORT_ESP32=8888
STATE_FILE="/tmp/es_state.inf"

# --- CONFIGURACIÓN DE RUTAS ---
# Carpeta donde el script de PC exportó las marquesinas de los juegos (separadas por sistema)
BASE_DIR="/recalbox/share/marquesinas/Arcade"

# Carpeta ÚNICA donde tienes metidos todos los logos de los sistemas (snes.bmp, mame.bmp...)
LOGOS_DIR="/recalbox/share/marquesinas/Logos"

# Logo por defecto a mostrar si falla todo lo demás
DEFAULT_BMP="$LOGOS_DIR/_default.bmp"


# 1. GENERAMOS EL DECODIFICADOR PYTHON ON-THE-FLY
cat << 'EOF' > /tmp/marquesina_sender_juego.py
import sys, os, socket

ESP32_IP = sys.argv[1]
ESP32_PORT = int(sys.argv[2])
BMP_PATH = sys.argv[3]

# Envío de comando STOP
if BMP_PATH == "STOP":
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        s.connect((ESP32_IP, ESP32_PORT))
        s.sendall(b"STOP")
        s.close()
    except:
        pass
    sys.exit(0)

# Verificamos si la imagen existe
if not os.path.exists(BMP_PATH):
    sys.exit(1)

# Procesamos la imagen a píxeles RGB y la enviamos
try:
    with open(BMP_PATH, 'rb') as f:
        f.seek(54) # Saltamos la cabecera del BMP (54 bytes)
        data = f.read()

        # Un BMP se guarda al revés y en BGR, le damos la vuelta y lo pasamos a RGB
        bytes_por_fila = 128 * 3 # 384 bytes
        filas = [data[i:i + bytes_por_fila] for i in range(0, len(data), bytes_por_fila)]
        filas.reverse()

        buffer_rgb = bytearray()
        for fila in filas:
            for j in range(0, bytes_por_fila, 3):
                b = fila[j]
                g = fila[j+1]
                r = fila[j+2]
                buffer_rgb.extend([r, g, b])

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(3.0)
    s.connect((ESP32_IP, ESP32_PORT))
    s.sendall(buffer_rgb)
    s.close()
except Exception as e:
    pass
EOF


# 2. ESCUCHAMOS LOS EVENTOS DE RECALBOX
mosquitto_sub -h 127.0.0.1 -p 1883 -q 0 -t "Recalbox/EmulationStation/Event" | while read -r EVENTO; do

    EVENTO=$(echo "$EVENTO" | tr '[:upper:]' '[:lower:]')

    case "$EVENTO" in
        rungame)
            if [ -f "$STATE_FILE" ]; then
                SISTEMA=$(grep -i "^SystemId=" "$STATE_FILE" | cut -d'=' -f2 | tr -d '\r')
                GAME_PATH=$(grep -i "^GamePath=" "$STATE_FILE" | cut -d'=' -f2 | tr -d '\r')
                IS_FOLDER=$(grep -i "^IsFolder=" "$STATE_FILE" | cut -d'=' -f2 | tr -d '\r')

                if [ "$IS_FOLDER" = "1" ] || [ -z "$GAME_PATH" ]; then
                    # No es un juego real (carpeta), mostramos el logo del sistema
                    BMP_FILE="$LOGOS_DIR/$SISTEMA.bmp"
                else
                    # Construimos la ruta del juego en su carpeta de sistema correspondiente
                    JUEGO_SUCIO=$(basename -- "$GAME_PATH")
                    JUEGO_SIN_EXT="${JUEGO_SUCIO%.*}"
                    JUEGO_LIMPIO=$(echo "$JUEGO_SIN_EXT" | sed 's/\\//g')

                    BMP_FILE="$BASE_DIR/$SISTEMA/$JUEGO_LIMPIO.bmp"
                fi

                # --- LÓGICA DE FALLBACK (Prioridades) ---
                # 1. Si no encontró la imagen del juego, cae al logo del sistema
                if [ ! -f "$BMP_FILE" ]; then
                    BMP_FILE="$LOGOS_DIR/$SISTEMA.bmp"
                fi
                # 2. Si tampoco encontró el logo del sistema, cae al default universal
                if [ ! -f "$BMP_FILE" ]; then
                    BMP_FILE="$DEFAULT_BMP"
                fi

                # Ejecutamos el envío de forma asíncrona (&)
                if [ -n "$SISTEMA" ]; then
                    python3 /tmp/marquesina_sender_juego.py "$IP_ESP32" "$PORT_ESP32" "$BMP_FILE" &
                fi
            fi
            ;;

        endgame|stop|shutdown|reboot)
            # Mandamos el comando STOP para regresar a los GIFs o Reloj
            python3 /tmp/marquesina_sender_juego.py "$IP_ESP32" "$PORT_ESP32" "STOP" &
            ;;
    esac
done
