# ==================================================================
#   INSTALADOR AUTOMATICO ARCADE - RETRO PIXEL LED (v2.2)
# ==================================================================
$Host.UI.RawUI.WindowTitle = "Instalador Retro Pixel Universal v2.2"
$OutputEncoding = [System.Text.Encoding]::UTF8

Write-Host "===================================================" -ForegroundColor Magenta
Write-Host "      INSTALADOR ARCADE RETRO PIXEL LED Lite" -ForegroundColor White
Write-Host "===================================================" -ForegroundColor Magenta

# 1. SOLICITAR DATOS
$IP_PANEL = Read-Host "1. Introduce la IP de tu PANEL LED (ej. 192.168.1.117)"
$IP_PANEL = $IP_PANEL.Trim()

$INPUT_RUTA = Read-Host "2. Introduce la ruta o IP de la consola (ej. \\192.168.1.119 o D:)"
$INPUT_RUTA = $INPUT_RUTA.Trim([char]34).TrimEnd([char]92)

# --- DETECCION DE RUTAS ---
$RUTA_SYSTEM = ""
$RUTA_USERSCRIPTS = ""
$RUTA_BATOCERA_SCRIPTS = ""

if (Test-Path -LiteralPath (Join-Path $INPUT_RUTA "share\system")) {
    $RUTA_SYSTEM = Join-Path $INPUT_RUTA "share\system"
    $RUTA_USERSCRIPTS = Join-Path $INPUT_RUTA "share\userscripts"
    $RUTA_BATOCERA_SCRIPTS = Join-Path $INPUT_RUTA "share\system\configs\emulationstation\scripts"
} elseif (Test-Path -LiteralPath (Join-Path $INPUT_RUTA "recalbox\system")) {
    $RUTA_SYSTEM = Join-Path $INPUT_RUTA "recalbox\system"
    $RUTA_USERSCRIPTS = Join-Path $INPUT_RUTA "recalbox\userscripts"
} elseif (Test-Path -LiteralPath (Join-Path $INPUT_RUTA "system")) {
    $RUTA_SYSTEM = Join-Path $INPUT_RUTA "system"
    $RUTA_BATOCERA_SCRIPTS = Join-Path $INPUT_RUTA "system\configs\emulationstation\scripts"
    if (Test-Path -LiteralPath (Join-Path $INPUT_RUTA "userscripts")) {
        $RUTA_USERSCRIPTS = Join-Path $INPUT_RUTA "userscripts"
    } else {
        $RUTA_USERSCRIPTS = Join-Path (Split-Path $INPUT_RUTA -Parent) "userscripts"
    }
} else {
    Write-Host ""
    Write-Host "[ERROR] No se pudo encontrar la estructura de carpetas de la consola." -ForegroundColor Red
    Read-Host "Presiona Enter para salir"
    exit
}

# 2. SELECCION DE SISTEMA OPERATIVO
Write-Host ""
Write-Host "Que sistema operativo tiene esa ruta?" -ForegroundColor Cyan
Write-Host "  1) Batocera" -ForegroundColor White
Write-Host "  2) Recalbox" -ForegroundColor White
$opcion = Read-Host "Selecciona una opcion (1 o 2)"

# --- FUNCION INTERNA DE PROCESAMIENTO UNIX ---
# $SubCarpeta: subcarpeta LOCAL (junto al .ps1) donde vive el script de origen,
# p.ej. "Batocera" o "Recalbox". No afecta al nombre con el que se instala en la consola.
function Instalar-Script-Unix($FileName, $DestinoPath, $SubCarpeta = "") {
    $CurrentDir = $PSScriptRoot
    if ([string]::IsNullOrEmpty($CurrentDir)) { $CurrentDir = Get-Location }

    if ([string]::IsNullOrEmpty($SubCarpeta)) {
        $PathOrigen = Join-Path $CurrentDir $FileName
    } else {
        $PathOrigen = Join-Path (Join-Path $CurrentDir $SubCarpeta) $FileName
    }

    # SOLUCION: Usamos -LiteralPath para evitar que los corchetes [ ] rompan la busqueda
    if (Test-Path -LiteralPath $PathOrigen) {
        if (!(Test-Path -LiteralPath $DestinoPath)) {
            $null = New-Item -ItemType Directory -Force -Path $DestinoPath
        }
        
        # SOLUCION: Get-Content tambien requiere -LiteralPath para archivos con corchetes
        $Contenido = Get-Content -LiteralPath $PathOrigen -Raw
        
        # Inyeccion segura de IP usando caracteres ASCII directos
        $Quote = [char]34
        $StringReemplazo = "IP_ESP32=" + $Quote + $IP_PANEL + $Quote
        $Contenido = $Contenido -replace ("IP_ESP32=" + $Quote + ".*" + $Quote), $StringReemplazo
        
        # Forzamos formato Linux (LF)
        $ContenidoFinal = $Contenido -replace "`r`n", "`n"
        $PathFinal = Join-Path $DestinoPath $FileName
        
        $Utf8NoBom = New-Object System.Text.UTF8Encoding($false)
        [System.IO.File]::WriteAllText($PathFinal, $ContenidoFinal, $Utf8NoBom)
        
        Write-Host "   [OK] Configurado e Instalado: $FileName" -ForegroundColor Green
        return $true
    } else {
        Write-Host ""
        Write-Host "   [ERROR] No se encontro el archivo $FileName en la carpeta '$SubCarpeta' junto al instalador." -ForegroundColor Red
        Write-Host "   Asegurate de que los .sh/.py esten dentro de las subcarpetas Batocera\ o Recalbox\." -ForegroundColor Yellow
        return $false
    }
}

# 3. EJECUCION FILTRADA SEGUN EL SISTEMA
switch ($opcion) {
    "1" {
        Write-Host ""
        Write-Host "--> Instalando en BATOCERA..." -ForegroundColor Cyan

        Write-Host ""
        Write-Host "Que modo de funcionamiento deseas activar?" -ForegroundColor Cyan
        Write-Host "  Opcion 1: Menus y Juegos (Muestra sistemas al navegar + juego lanzado)" -ForegroundColor White
        Write-Host "  Opcion 2: Solo Juegos (Marquesina fija/reloj en menus, cambia solo al jugar)" -ForegroundColor White
        $modoBatocera = Read-Host "Selecciona una opcion (1 o 2)"

        if ($modoBatocera -ne "1" -and $modoBatocera -ne "2") {
            Write-Host ""
            Write-Host "[ERROR] Opcion de modo invalida." -ForegroundColor Red
            Read-Host "Presiona Enter para salir"
            exit
        }
        
        $RUTA_GAME_SELECTED = Join-Path $RUTA_BATOCERA_SCRIPTS "game-selected"
        $RUTA_SYSTEM_SELECTED = Join-Path $RUTA_BATOCERA_SCRIPTS "system-selected"
        $RUTA_GAME_END = Join-Path $RUTA_BATOCERA_SCRIPTS "game-end"
        $RUTA_QUIT = Join-Path $RUTA_BATOCERA_SCRIPTS "quit"
        $RUTA_SHUTDOWN = Join-Path $RUTA_BATOCERA_SCRIPTS "shutdown"
        $RUTA_REBOOT = Join-Path $RUTA_BATOCERA_SCRIPTS "reboot"

        # El decodificador Python es el motor comun; se instala una sola vez en userscripts
        # (que en Batocera equivale a /userdata/userscripts) y los hooks lo invocan.
        $inst_sender = Instalar-Script-Unix "Batocera_marquesina.py" $RUTA_USERSCRIPTS "Batocera"

        # Hooks comunes a los dos modos: juego lanzado, fin de juego, y apagado/reinicio del sistema.
        $inst_gameselected = Instalar-Script-Unix "Batocera_game-selected.sh" $RUTA_GAME_SELECTED "Batocera"
        $inst_gameend = Instalar-Script-Unix "Batocera_game-end.sh" $RUTA_GAME_END "Batocera"
        $inst_quit = Instalar-Script-Unix "Batocera_quit.sh" $RUTA_QUIT "Batocera"
        $inst_shutdown = Instalar-Script-Unix "Batocera_shutdown.sh" $RUTA_SHUTDOWN "Batocera"
        $inst_reboot = Instalar-Script-Unix "Batocera_reboot.sh" $RUTA_REBOOT "Batocera"

        # system-selected solo se instala en el modo "Menus y Juegos".
        $inst_systemselected = $false
        if ($modoBatocera -eq "1") {
            Write-Host ""
            Write-Host " Modo 'Menus y Juegos': instalando system-selected..." -ForegroundColor Yellow
            $inst_systemselected = Instalar-Script-Unix "Batocera_system-selected.sh" $RUTA_SYSTEM_SELECTED "Batocera"
        } else {
            Write-Host ""
            Write-Host " Modo 'Solo Juegos': no se instala system-selected." -ForegroundColor Yellow
            $conflictivo = Join-Path $RUTA_SYSTEM_SELECTED "Batocera_system-selected.sh"
            if (Test-Path -LiteralPath $conflictivo) {
                Remove-Item -LiteralPath $conflictivo -Force
                Write-Host "   [OK] Eliminado system-selected de una instalacion anterior en modo 'Menus y Juegos'." -ForegroundColor Green
            }
        }

        if ($inst_gameselected -or $inst_systemselected -or $inst_gameend -or $inst_quit -or $inst_shutdown -or $inst_reboot) {
            Write-Host " Creando custom.sh para auto-permisos en Batocera..." -ForegroundColor Yellow
            $PathCustom = Join-Path $RUTA_SYSTEM "custom.sh"
            
            $LineasCustom = @(
                "#!/bin/bash",
                "# Otorga permisos a los scripts de marquesinas RetroPixel",
                "chmod -R +x /userdata/system/configs/emulationstation/scripts/game-selected/",
                "chmod -R +x /userdata/system/configs/emulationstation/scripts/system-selected/",
                "chmod -R +x /userdata/system/configs/emulationstation/scripts/game-end/",
                "chmod -R +x /userdata/system/configs/emulationstation/scripts/quit/",
                "chmod -R +x /userdata/system/configs/emulationstation/scripts/shutdown/",
                "chmod -R +x /userdata/system/configs/emulationstation/scripts/reboot/",
                "chmod -R +x /userdata/userscripts"
            )
            $CustomSH = $LineasCustom -join "`n"
            
            $Utf8NoBom = New-Object System.Text.UTF8Encoding($false)
            [System.IO.File]::WriteAllText($PathCustom, $CustomSH, $Utf8NoBom)
            Write-Host "   [OK] custom.sh de Batocera generado con exito." -ForegroundColor Green
        }
    }
    
    "2" {
        Write-Host ""
        Write-Host "--> Configuracion para RECALBOX..." -ForegroundColor Cyan

        Write-Host ""
        Write-Host "Que modo de funcionamiento deseas activar?" -ForegroundColor Cyan
        Write-Host "  Opcion 1: Menus y Juegos (Muestra sistemas al navegar + juego lanzado)" -ForegroundColor White
        Write-Host "  Opcion 2: Solo Juegos (Marquesina fija/reloj en menus, cambia solo al jugar)" -ForegroundColor White
        $modoRecalbox = Read-Host "Selecciona una opcion (1 o 2)"

        if ($modoRecalbox -eq "1") {
            Write-Host ""
            Write-Host " Ejecutando instalacion de Opcion 1 (Permanente)..." -ForegroundColor Yellow
            $null = Instalar-Script-Unix "Recalbox_1(permanent).sh" $RUTA_USERSCRIPTS "Recalbox"
            
            $conflictivo = Join-Path $RUTA_USERSCRIPTS "Recalbox_2(permanent).sh"
            if (Test-Path -LiteralPath $conflictivo) { Remove-Item -LiteralPath $conflictivo -Force }
        } 
        elseif ($modoRecalbox -eq "2") {
            Write-Host ""
            Write-Host " Ejecutando instalacion de Opcion 2 (Por Eventos)..." -ForegroundColor Yellow
            $null = Instalar-Script-Unix "Recalbox_2(permanent).sh" $RUTA_USERSCRIPTS "Recalbox"
            
            $conflictivo = Join-Path $RUTA_USERSCRIPTS "Recalbox_1(permanent).sh"
            if (Test-Path -LiteralPath $conflictivo) { Remove-Item -LiteralPath $conflictivo -Force }
        } 
        else {
            Write-Host ""
            Write-Host "[ERROR] Opcion de modo invalida." -ForegroundColor Red
            Read-Host "Presiona Enter para salir"
            exit
        }
    }
    
    default {
        Write-Host ""
        Write-Host "[ERROR] Opcion de sistema invalida." -ForegroundColor Red
        Read-Host "Presiona Enter para salir"
        exit
    }
}

Write-Host ""
Write-Host "=================================================" -ForegroundColor Magenta
Write-Host "             INSTALACION COMPLETADA!" -ForegroundColor Green
Write-Host "=================================================" -ForegroundColor Magenta
if ($opcion -eq "1") {
    Write-Host "IMPORTANTE: Reinicia Batocera."
} else {
    Write-Host "IMPORTANTE: Reinicia Recalbox."
}
Write-Host "--------------------------------------------------"
Read-Host "Presiona Enter para finalizar"
