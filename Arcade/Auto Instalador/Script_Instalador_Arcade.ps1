# ==================================================================
#   INSTALADOR AUTOMATICO Y UNIVERSAL - RETRO PIXEL LED (v2.9)
# ==================================================================
$Host.UI.RawUI.WindowTitle = "Instalador Retro Pixel Universal v2.9"
$OutputEncoding = [System.Text.Encoding]::UTF8

Write-Host "===================================================" -ForegroundColor Magenta
Write-Host "     INSTALADOR UNIVERSAL RETRO PIXEL LED Lite" -ForegroundColor White
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
function Instalar-Script-Unix($FileName, $DestinoPath) {
    $CurrentDir = $PSScriptRoot
    if ([string]::IsNullOrEmpty($CurrentDir)) { $CurrentDir = Get-Location }
    
    $PathOrigen = Join-Path $CurrentDir $FileName
    
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
        Write-Host "   [ERROR] No se encontro el archivo $FileName en la carpeta de tu PC." -ForegroundColor Red
        Write-Host "   Asegurate de dejar los archivos .sh junto a este instalador." -ForegroundColor Yellow
        return $false
    }
}

# 3. EJECUCION FILTRADA SEGUN EL SISTEMA
switch ($opcion) {
    "1" {
        Write-Host ""
        Write-Host "--> Instalando en BATOCERA..." -ForegroundColor Cyan
        
        $RUTA_START = Join-Path $RUTA_BATOCERA_SCRIPTS "game-start"
        $RUTA_END = Join-Path $RUTA_BATOCERA_SCRIPTS "game-end"

        $inst_start = Instalar-Script-Unix "pixel_start.sh" $RUTA_START
        $inst_stop = Instalar-Script-Unix "pixel_stop.sh" $RUTA_END

        if ($inst_start -or $inst_stop) {
            Write-Host " Creando custom.sh para auto-permisos en Batocera..." -ForegroundColor Yellow
            $PathCustom = Join-Path $RUTA_SYSTEM "custom.sh"
            
            $LineasCustom = @(
                "#!/bin/bash",
                "# Otorga permisos a los scripts de marquesinas RetroPixel",
                "chmod -R +x /userdata/system/configs/emulationstation/scripts/game-start/",
                "chmod -R +x /userdata/system/configs/emulationstation/scripts/game-end/"
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
        Write-Host "  1) Opcion 1: Menus y Juegos (Muestra sistemas al navegar + juego lanzado)" -ForegroundColor White
        Write-Host "  2) Opcion 2: Solo Juegos (Marquesina fija/reloj en menus, cambia solo al jugar)" -ForegroundColor White
        $modoRecalbox = Read-Host "Selecciona una opcion (1 o 2)"

        if ($modoRecalbox -eq "1") {
            Write-Host ""
            Write-Host " Ejecutando instalacion de Opcion 1 (Permanente)..." -ForegroundColor Yellow
            $null = Instalar-Script-Unix "pixel_1(permanent).sh" $RUTA_USERSCRIPTS
            
            $conflictivo = Join-Path $RUTA_USERSCRIPTS "pixel_2(permanent).sh"
            if (Test-Path -LiteralPath $conflictivo) { Remove-Item -LiteralPath $conflictivo -Force }
        } 
        elseif ($modoRecalbox -eq "2") {
            Write-Host ""
            Write-Host " Ejecutando instalacion de Opcion 2 (Por Eventos)..." -ForegroundColor Yellow
            $null = Instalar-Script-Unix "pixel_2(permanent).sh" $RUTA_USERSCRIPTS
            
            $conflictivo = Join-Path $RUTA_USERSCRIPTS "pixel_1(permanent).sh"
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