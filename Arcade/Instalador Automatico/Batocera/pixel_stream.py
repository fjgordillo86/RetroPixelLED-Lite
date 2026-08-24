#!/usr/bin/env python3
"""
pixel_stream.py - Reproducción optimizada de marquesinas animadas por TCP.

Mejoras de rendimiento:
- Desactivado el algoritmo de Nagle (TCP_NODELAY) para latencia ultrabaja.
- Frame Skipping adaptativo: salta fotogramas en GIFs de alta velocidad (>12 FPS)
  para evitar el efecto cámara lenta y mantener el tiempo real.
- Escalado por NEAREST: procesado de fotogramas hasta 10x más rápido en CPU.
- Carga progresiva en caché RAM.
- Backoff progresivo + reconexión (E): si se corta la conexión, se reintenta con
  espera creciente (3s->6s->12s->30s tope), interrumpible al instante por la
  bandera de stop, reenviando el MISMO fotograma en el que se cortó.
"""
import sys
import os
import socket
import glob
import time
from PIL import Image

ESP32_IP = sys.argv[1]
ESP32_PORT = int(sys.argv[2])
CARPETA_SISTEMA = sys.argv[3]
ROM_NAME = sys.argv[4]
STOP_FLAG = sys.argv[5]

ANCHO, ALTO = 128, 32
FPS_MAXIMOS = 12
RETRASO_MINIMO = 1.0 / FPS_MAXIMOS

# False = RGB888 (24 bits - 12.288 bytes/frame)
# True  = RGB565 (16 bits -  8.192 bytes/frame). NO ACTIVAR sin cambiar antes verificarMarquesinaTCP().
USA_RGB565 = False


def rgb888_a_rgb565(frame_pil):
    raw_bytes = frame_pil.tobytes()
    out = bytearray(ANCHO * ALTO * 2)
    idx_out = 0
    for i in range(0, len(raw_bytes), 3):
        r = raw_bytes[i]
        g = raw_bytes[i + 1]
        b = raw_bytes[i + 2]
        val = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
        out[idx_out] = (val >> 8) & 0xFF
        out[idx_out + 1] = val & 0xFF
        idx_out += 2
    return bytes(out)


def procesar_frame(frame_pil):
    # NEAREST es mucho más rápido que BILINEAR/LANCZOS y perfecto para Pixel Art
    if frame_pil.size != (ANCHO, ALTO):
        frame_pil = frame_pil.resize((ANCHO, ALTO), Image.Resampling.NEAREST)

    if USA_RGB565:
        return rgb888_a_rgb565(frame_pil)
    else:
        return frame_pil.tobytes()


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
    """Encapsula el socket TCP con reconexión automática y backoff progresivo
    (3s -> 6s -> 12s -> tope 30s) — mismo patrón ya validado en
    verificarReplayOSLite() del firmware, aplicado aquí al streamer."""

    PASO_ESPERA = 0.15  # segundos por tramo, para poder cortar la espera al instante

    def __init__(self, ip, puerto):
        self.ip = ip
        self.puerto = puerto
        self.sock = None
        self.fallos_consecutivos = 0
        self._conectar_con_backoff()

    def _crear_socket(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0)
        # Nagle desactivado en TODO socket nuevo, incluidas las reconexiones
        s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        return s

    def _esperar_troceado(self, segundos):
        """Espera 'segundos' en pasos pequeños, comprobando la bandera de stop
        en cada uno — para no tardar hasta 30s en reaccionar si sales del juego
        a media espera de backoff."""
        transcurrido = 0.0
        while transcurrido < segundos and not debe_pararse():
            time.sleep(self.PASO_ESPERA)
            transcurrido += self.PASO_ESPERA

    def _conectar_con_backoff(self):
        """Reintenta conectar indefinidamente (mientras no llegue la bandera de
        stop), con espera progresiva entre intentos. Sin límite de reintentos:
        la bandera de stop + el pkill de seguridad de pixel_stop.sh ya cortan
        esto en cuanto termine la partida."""
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
        """Envía un fotograma. Si falla, cierra el socket roto y reconecta con
        backoff antes de reintentar EL MISMO fotograma. Devuelve False solo si
        hubo que abandonar porque llegó la bandera de stop."""
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

                if ruta_gif not in cache_frames:
                    cache_frames[ruta_gif] = []
                    img = Image.open(ruta_gif)

                    # Analizar velocidad nativa del GIF para Frame Skipping
                    # duration viene en milisegundos por frame (ej. 40ms = 25 FPS)
                    duracion_frame_ms = img.info.get('duration', 80)
                    if duracion_frame_ms <= 0:
                        duracion_frame_ms = 80

                    fps_gif = 1000.0 / duracion_frame_ms

                    # Cuántos frames saltar si el GIF supera los FPS_MAXIMOS
                    paso_frame = max(1, int(round(fps_gif / FPS_MAXIMOS)))
                    intervalo_envio = max(RETRASO_MINIMO, duracion_frame_ms / 1000.0 * paso_frame)

                    frame_idx = 0
                    try:
                        while True:
                            if debe_pararse():
                                break

                            # Solo procesamos y enviamos 1 de cada 'paso_frame' fotogramas
                            if frame_idx % paso_frame == 0:
                                inicio = time.monotonic()

                                frame = img.convert("RGB")
                                frame_bytes = procesar_frame(frame)

                                cache_frames[ruta_gif].append((frame_bytes, intervalo_envio))

                                if not conexion.enviar(frame_bytes):
                                    return  # se pidió parar mientras se reconectaba

                                transcurrido = time.monotonic() - inicio
                                restante = intervalo_envio - transcurrido
                                if restante > 0:
                                    time.sleep(restante)

                            frame_idx += 1
                            img.seek(img.tell() + 1)
                    except EOFError:
                        pass
                    finally:
                        img.close()

                else:
                    # Pasadas siguientes desde RAM utilizando el timing ya calculado
                    for frame_bytes, intervalo_envio in cache_frames[ruta_gif]:
                        if debe_pararse():
                            break

                        inicio = time.monotonic()
                        if not conexion.enviar(frame_bytes):
                            return

                        transcurrido = time.monotonic() - inicio
                        restante = intervalo_envio - transcurrido
                        if restante > 0:
                            time.sleep(restante)

    finally:
        conexion.cerrar()
        limpiar_stop_flag()


if __name__ == "__main__":
    main()
