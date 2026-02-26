#!/bin/bash

# ========================================================
#   RETRO PIXEL LED LITE - GENERADOR DE LISTA DE GIFs
# ========================================================

OUTPUT_FILE="lista.txt"
TARGET_DIR="gifs"

# Colores para la terminal
CYAN='\033[0;36m'
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${CYAN}========================================================${NC}"
echo -e "${CYAN}   RETRO PIXEL LED LITE - GENERADOR PARA MAC/LINUX      ${NC}"
echo -e "${CYAN}========================================================${NC}"
echo ""

# Verificar si existe la carpeta de gifs
if [ ! -d "$TARGET_DIR" ]; then
    echo -e "${RED}[ERROR] No se ha encontrado la carpeta '$TARGET_DIR'.${NC}"
    echo "Asegúrate de que este archivo .sh está en la raíz de la SD."
    echo ""
    read -n 1 -s -p "Pulsa cualquier tecla para salir..."
    exit 1
fi

echo -e "[INFO] Indexando archivos .gif..."
echo ""

# Eliminar archivo anterior si existe
[ -f "$OUTPUT_FILE" ] && rm "$OUTPUT_FILE"

count=0

# Bucle de escaneo
# Buscamos archivos .gif (sin importar mayúsculas/minúsculas)
find "$TARGET_DIR" -type f -iname "*.gif" | while read -r FULL_PATH; do
    # En Unix, 'find' ya nos da la ruta relativa si empezamos desde 'gifs'
    # Solo nos aseguramos de que empiece por "/" para que el ESP32 la entienda
    REL_PATH="/$FULL_PATH"
    
    # Escribir en el archivo
    echo "$REL_PATH" >> "$OUTPUT_FILE"
    
    # Actualizar contador
    ((count++))
    
    # Efecto de contador limpio cada 5 archivos
    if (( count % 5 == 0 )); then
        printf "\r... Procesando: %d GIFs cargados " "$count"
    fi
done

# Obtener el total real después del bucle
total_count=$(wc -l < "$OUTPUT_FILE" | xargs)

echo ""
echo ""
echo -e "${GREEN}========================================================${NC}"
echo -e "${GREEN}[ÉXITO] Proceso completado.${NC}"
echo -e "Se han indexado ${total_count} GIFs correctamente."
echo -e "${GREEN}========================================================${NC}"
echo ""
read -n 1 -s -p "Pulsa cualquier tecla para terminar..."