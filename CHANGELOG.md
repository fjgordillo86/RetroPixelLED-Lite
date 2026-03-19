## 📝 Changelog (Registro de Cambios)

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
