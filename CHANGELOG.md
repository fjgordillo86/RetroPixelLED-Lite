## 📝 Changelog (Registro de Cambios)

### [v2.1.0] - 2026-04-18
**Retro Pixel LED Lite: "Global Voice & Wireless Evolution"**

#### ✨ Añadido
- **Sistema Multidioma Dinámico:** Soporte para diccionarios externos en formato `.json` (ES, EN, FR...). Carga inteligente desde la SD para ahorrar RAM.
- **Actualización Inalámbrica (OTA):** Motor de descarga e instalación de firmware directamente desde el menú OSD vía GitHub.
- **Smart Menu Centering:** Algoritmo de centrado automático de texto que ajusta los menús basándose en el ancho de los caracteres de cada idioma.
- **Feedback Visual "Sleep":** Iconografía personalizada de Luna y Emoji 😴 diseñada píxel a píxel para el modo de ahorro de energía.

#### ⚙️ Mejoras
- **Gestión de RAM (Anti-Panic):** Implementada liberación forzada de memoria tras cerrar el menú OSD para prevenir reinicios accidentales.
- **Generación de Config.ini:** El sistema ahora autogenera los comentarios del archivo de configuración en el idioma seleccionado por el usuario.
- **UX de Idiomas:** Selector de idioma en tiempo real que aplica los cambios sin necesidad de reiniciar el panel manualmente.

#### 🛡️ Fixes
- **JSON Parser Stability:** Corregido error crítico que provocaba un *Kernel Panic* al intentar leer archivos de idioma con etiquetas demasiado largas.
- **OTA Secure Client:** Ajuste en el manejo de certificados para garantizar la conexión segura con los servidores de actualización.
- **Texto OSD:** Eliminación de duplicidad de símbolos ":" en las cadenas de texto del menú para mejorar la limpieza visual.

---

### [v2.0.5] - 2026-04-11
**Retro Pixel LED Lite: "Smart Energy, Dual Vision & Safety Core"**

#### ✨ Añadido
- **Modo Visual Dual:** Selector en el menú para alternar entre "Solo Reloj" (minimalista) o "Playlist de GIFs" (animado).
- **Temporizador Inteligente (Smart Timer):** Programación de encendido/apagado automático con soporte para cruce de medianoche (Over-midnight).
- **Manual Override (Prioridad de Usuario):** Función de pulsación extra larga (4s) para forzar el estado de energía, bloqueando el temporizador hasta el siguiente ciclo.
- **I2S Safety Shield:** Sistema de protección que limita dinámicamente la frecuencia a 16MHz al activar el Double Buffer para garantizar estabilidad total.

#### ⚙️ Mejoras
- **Navegación UI Inteligente:**
    - Pulsación rápida: Despertar panel / Navegar.
    - Pulsación larga: Retroceso rápido de valores (-5 min en Timer).
    - Pulsación continua: Aceleración de valores (+5 min en Timer).
- **Ultra-Responsive Loop:** Eliminación de código bloqueante; el botón ahora interrumpe cualquier animación o proceso de red al instante.
- **Ciclo de Reloj Optimizado:** Rango de aparición del reloj ajustado de [2...10] GIFs con saltos de +2 para una configuración más lógica y rápida.
- **Sanitización de API Weather:** Mejora en el manejo de URLs para ciudades con espacios o guiones, evitando fallos en la obtención de datos meteorológicos.
- **Menús Paginados:** Reestructuración del OSD en varias páginas para mejorar la visibilidad de los nuevos ajustes avanzados.
  
---
### [v2.0.0] - 2026-03-26
**Retro Pixel LED Lite: "OSD Menu, Night Mode & Smart RAM"**

#### ✨ Añadido
- **Menú OSD (On-Screen Display):** Interfaz nativa en el panel LED para configurar Playlists, Brillo, WiFi y Reloj con un solo botón físico.
- **Modo Noche Dinámico:** Integración con iconos de Luna y paletas de colores fríos automáticas basadas en la hora local y datos de OpenWeatherMap.
- **Plug & Play Automático:** El sistema ahora escanea y reproduce la primera playlist encontrada en `/playlists` si no hay ninguna seleccionada.
- **Persistencia en SD:** Guardado automático de todos los ajustes realizados desde el menú OSD directamente en el archivo `config.ini`.

#### ⚙️ Mejoras
- **Smart RAM Refresh:** Lógica de reinicio inteligente al actualizar clima/hora para evitar la fragmentación de memoria causada por el Double Buffer.
- **Gestión WiFi Stealth:** Desactivación radical del stack de red tras la sincronización para eliminar el lag y reducir la temperatura del ESP32.
- **Sincronización NTP Silenciosa:** Ajuste del reloj interno en cada ventana de actualización de clima para evitar desviaciones horarias.
- **Optimización de Playlists:** Transiciones instantáneas entre listas temáticas desde el menú sin necesidad de reiniciar el dispositivo.

---
### [v1.1.2] - 2026-03-19
**Retro Pixel LED Lite: "Double Buffering & Splash Screen y Branding"**

#### ✨ Añadido
- **Motor de Renderizado:** Se ha implementado la técnica de Double Buffering (Doble Buffer de Memoria) para una reproducción de contenido ultra fluida.
- **Logo RGB Dinámico:** Inicio con el logotipo "RETRO PIXEL LED lite" utilizando colores independientes para las siglas LED y marcos de contorno estilizados.
- **Identificación de Firmware:** Visualización directa de la versión del sistema (`v1.1.2`) en la pantalla de carga, facilitando el control de versiones y soporte.

#### ⚙️ Mejoras
* **Secuencialidad Crítica:** El sistema ahora gestiona la conexión WiFi, sincronización NTP y descarga del clima *antes* de inicializar el panel LED. 
* **Liberación de Recursos:** Una vez obtenidos los datos, el driver de WiFi se apaga por completo para ceder toda la memoria RAM al motor gráfico, evitando el error de inicialización `0x3001`.

---
### [v1.1.0] - 2026-03-03
**Retro Pixel LED Lite: "The Weather & Notification Update"**

#### ✨ Añadido
- **Barra de Notificaciones:** Implementación de una franja superior (Y=0 a Y=8) para información del sistema.
- **Mensaje Personalizado:** Nueva etiqueta `WEATHER_MSG` en `config.ini` para mostrar un texto fijo (ej: "Game Room") en la marquesina.
- **Soporte de OpenWeatherMap:** Integración con la API oficial para descargar datos meteorológicos en tiempo real.
- **Iconografía Bitmap:** Añadidos 6 iconos exclusivos de 8x8 píxeles (Sol, Nubes, Lluvia, Nieve, Tormenta, Niebla) optimizados para paneles LED.
- **Posicionamiento Dinámico:** Lógica de ajuste automático del Reloj (`startY=9`) cuando el clima está activo para evitar colisiones visuales.

#### ⚙️ Mejoras
- **Gestión de WiFi:** Optimización del "Stealth Mode". Ahora el WiFi también se despierta cíclicamente según `WEATHER_INT` para refrescar los datos y vuelve a apagarse.
- **Lectura de INI:** Añadida lógica de parseo para `CITY`, `API_KEY` y `WEATHER_MSG`.
- **Estética del Reloj:** El símbolo de grado (°C) ahora utiliza un dibujo vectorial de 2x2 píxeles para mayor nitidez.

#### 🛡️ Correcciones
- Corregido el parpadeo de la barra superior al integrar el dibujo dentro del buffer DMA del reloj.
- Ajustada la conversión de temperaturas para mostrar solo valores enteros, evitando que el texto se salga del panel.
