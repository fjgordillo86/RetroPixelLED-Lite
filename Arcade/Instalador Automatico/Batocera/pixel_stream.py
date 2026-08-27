#!/usr/bin/env python3
"""
pixel_stream.py - Reproducción optimizada de marquesinas animadas por TCP (Versión FFmpeg).

Mejoras respecto a la versión de Pillow:
- Cero dependencias: Utiliza el FFmpeg nativo de Recalbox (no requiere pip ni Pillow).
- Rendimiento extremo: FFmpeg realiza el escalado NEAREST y la conversión de color en C.
- Framerate unificado: FFmpeg estabiliza los GIFs a un máximo de 12 FPS automáticamente.
- RGB565 nativo: Si se activa, FFmpeg hace la conversión bit a bit directamente.
"""
import sys
import os
import socket
import glob
import time
import subprocess

ESP32_IP = sys.argv[1]
ESP32_PORT = int(sys.argv[2])
CARPETA_SISTEMA = sys.argv[3]
ROM_NAME = sys.argv[4]
STOP_FLAG = sys.argv[5]

ANCHO, ALTO = 128, 32
FPS_DESTINO = 12
INTERVALO_ENVIO = 1.0 / FPS_DESTINO

# False = RGB888 (24 bits - 12.288 bytes/frame)
# True  = RGB565 (16 bits -  8.192 bytes/frame)
USA_RGB565 = False

if USA_RGB565:
    BYTES_POR_FRAME = ANCHO * ALTO * 2
    PIX_FMT = "rgb565le"  # Little-Endian RGB565 directo desde FFmpeg
else:
    BYTES_POR_FRAME = ANCHO * ALTO * 3
    PIX_FMT = "rgb24"     # RGB888 directo desde FFmpeg


def cargar_gif_ffmpeg(ruta_gif):
    """
    Utiliza FFmpeg para extraer todos los fotogramas del GIF de una pasada a la memoria RAM.
    Aplica el escalado NEAREST y ajusta los FPS al instante.
    """
    cmd = [
        'ffmpeg',
        '-loglevel', 'error',
        '-i', ruta_gif,
        '-vf', f'fps={FPS_DESTINO},scale={ANCHO}:{ALTO}:flags=neighbor',
        '-pix_fmt', PIX_FMT,
        '-f', 'rawvideo',
        'pipe:1'
    ]
    
    try:
        # Popen ejecuta FFmpeg; capturamos todo el flujo binario (stdout) en memoria.
        proceso = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        raw_video, _ = proceso.communicate()
        
        # Troceamos el binario gigante en fotogramas individuales
        frames = []
        for i in range(0, len(raw_video), BYTES_POR_FRAME):
            frame = raw_video[i:i + BYTES_POR_FRAME]
            if len(frame) == BYTES_POR_FRAME:
                frames.append(frame)
        return frames
    except Exception:
        return []


def construir_secuencia():
    base_gif = os.path.join(CARPETA_SISTEMA, f"{ROM_NAME}.gif")
    patron_secuencia = os.path.join(CARPETA_SISTEMA, f"{ROM_NAME}_*.gif")

    secuencia = []
    if os.path.exists(base_gif):
        secuencia.append(base_gif)
    secuencia.extend(sorted(glob.glob(patron_secuencia)))
    return secuencia


def debe_pararse():
    return os.path.exists(STOP_FLAG)


def limpiar_stop_flag():
    if os.path.exists(STOP_FLAG):
        try:
            os.remove(STOP_FLAG)
        except OSError:
            pass


class ConexionMarquesina:
    """Encapsula el socket TCP con reconexión automática y backoff progresivo."""
    PASO_ESPERA = 0.15

    def __init__(self, ip, puerto):
        self.ip = ip
        self.puerto = puerto
        self.sock = None
        self.fallos_consecutivos = 0
        self._conectar_con_backoff()

    def _crear_socket(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0)
        s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        return s

    def _esperar_troceado(self, segundos):
        transcurrido = 0.0
        while transcurrido < segundos and not debe_pararse():
            time.sleep(self.PASO_ESPERA)
            transcurrido += self.PASO_ESPERA

    def _conectar_con_backoff(self):
        while not debe_pararse():
            try:
                nuevo_sock = self._crear_socket()
                nuevo_sock.connect((self.ip, self.puerto))
                self.sock = nuevo_sock
                self.fallos_consecutivos = 0
                return True
            except Exception:
                self.fallos_consecutivos += 1
                espera = min(3 * (2 ** min(self.fallos_consecutivos - 1, 3)), 30)
                self._esperar_troceado(espera)
        return False

    def enviar(self, frame_bytes):
        while not debe_pararse():
            try:
                self.sock.sendall(frame_bytes)
                return True
            except Exception:
                try:
                    self.sock.close()
                except Exception:
                    pass
                if not self._conectar_con_backoff():
                    return False
        return False

    def cerrar(self):
        if self.sock:
            try:
                self.sock.close()
            except Exception:
                pass


def main():
    secuencia = construir_secuencia()
    if not secuencia:
        sys.exit(0)

    conexion = ConexionMarquesina(ESP32_IP, ESP32_PORT)
    if conexion.sock is None:
        limpiar_stop_flag()
        sys.exit(1)

    cache_frames = {}

    try:
        while not debe_pararse():
            for ruta_gif in secuencia:
                if debe_pararse():
                    break

                # Fase 1: Cargar a memoria usando FFmpeg si no está cacheado
                if ruta_gif not in cache_frames:
                    frames_extraidos = cargar_gif_ffmpeg(ruta_gif)
                    cache_frames[ruta_gif] = frames_extraidos
                
                frames = cache_frames[ruta_gif]
                if not frames:
                    continue  # Si hubo error leyendo el GIF, pasamos al siguiente

                # Fase 2: Bucle de envío desde la memoria RAM (Timing preciso)
                for frame_bytes in frames:
                    if debe_pararse():
                        break

                    inicio = time.monotonic()
                    
                    if not conexion.enviar(frame_bytes):
                        return

                    # Controlamos el ritmo exacto según los FPS que le pedimos a FFmpeg
                    transcurrido = time.monotonic() - inicio
                    restante = INTERVALO_ENVIO - transcurrido
                    if restante > 0:
                        time.sleep(restante)

    finally:
        conexion.cerrar()
        limpiar_stop_flag()


if __name__ == "__main__":
    main()
