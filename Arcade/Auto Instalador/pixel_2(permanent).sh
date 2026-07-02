#!/bin/bash
IP_ESP32="192.168.1.117"
STATE_FILE="/tmp/es_state.inf"

# Escuchamos de forma continua el canal oficial MQTT interno de Recalbox
mosquitto_sub -h 127.0.0.1 -p 1883 -q 0 -t "Recalbox/EmulationStation/Event" | while read -r EVENTO; do
    
    # Forzamos minúsculas por seguridad
    EVENTO=$(echo "$EVENTO" | tr '[:upper:]' '[:lower:]')

    case "$EVENTO" in
        rungame)
            # Recalbox ya ha escrito los datos en es_state.inf antes de mandar el evento por MQTT
            if [ -f "$STATE_FILE" ]; then
                # Extraemos el sistema (short name) limpiando retornos de carro
                SISTEMA=$(grep -i "^SystemId=" "$STATE_FILE" | cut -d'=' -f2 | tr -d '\r')
                
                # Como solo entramos aquí al lanzar un juego, leemos la ruta de la ROM directamente
                GAME_PATH=$(grep -i "^GamePath=" "$STATE_FILE" | cut -d'=' -f2 | tr -d '\r')

                if [ -z "$GAME_PATH" ]; then
                    JUEGO_LIMPIO="_default"
                else
                    # Limpiamos el nombre del juego (quitamos ruta, extensión y barras)
                    JUEGO_SUCIO=$(basename -- "$GAME_PATH")
                    JUEGO_SIN_EXT="${JUEGO_SUCIO%.*}"
                    JUEGO_LIMPIO=$(echo "$JUEGO_SIN_EXT" | sed 's/\\//g')
                fi

                # Enviamos la petición HTTP al ESP32 únicamente si el sistema es válido
                if [ -n "$SISTEMA" ]; then
                    curl -s -G \
                        --data-urlencode "s=$SISTEMA" \
                        --data-urlencode "g=$JUEGO_LIMPIO" \
                        "http://$IP_ESP32/batocera" > /dev/null &
                fi
            fi
            ;;

        endgame|stop|shutdown|reboot)
            # Volvemos al estado por defecto (reloj o GIF base) al salir del juego o apagar
            curl -s -G \
                --data-urlencode "s=STOP" \
                --data-urlencode "g=STOP" \
                "http://$IP_ESP32/batocera" > /dev/null &
            ;;
    esac
done