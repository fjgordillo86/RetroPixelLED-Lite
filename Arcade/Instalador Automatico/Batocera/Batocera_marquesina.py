#!/usr/bin/env python3
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
        f.seek(54)  # Saltamos la cabecera del BMP (54 bytes)
        data = f.read()

        # Un BMP se guarda al revés y en BGR, le damos la vuelta y lo pasamos a RGB
        bytes_por_fila = 128 * 3  # 384 bytes
        filas = [data[i:i + bytes_por_fila] for i in range(0, len(data), bytes_por_fila)]
        filas.reverse()

        buffer_rgb = bytearray()
        for fila in filas:
            for j in range(0, bytes_por_fila, 3):
                b = fila[j]
                g = fila[j + 1]
                r = fila[j + 2]
                buffer_rgb.extend([r, g, b])

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(3.0)
    s.connect((ESP32_IP, ESP32_PORT))
    s.sendall(buffer_rgb)
    s.close()
except Exception:
    pass
