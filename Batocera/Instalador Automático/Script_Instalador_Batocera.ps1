# --- CONFIGURACIÓN ---
$Host.UI.RawUI.WindowTitle = "Instalador Retro Pixel v1.0"
$OutputEncoding = [System.Text.Encoding]::UTF8

Write-Host "--------------------------------------------------" -ForegroundColor Cyan
Write-Host " INSTALADOR AUTOMATICO RETRO PIXEL" -ForegroundColor Cyan
Write-Host "--------------------------------------------------" -ForegroundColor Cyan

# 1. DATOS
$IP_PANEL = Read-Host "1. Introduce la IP de tu PANEL LED (ej. 192.168.1.109)"
$RUTA_BATOCERA = Read-Host "2. Introduce la ruta de Batocera (ej. \\192.168.1.119 o D:)"

# Rutas Destino
$RUTA_SCRIPTS = Join-Path $RUTA_BATOCERA "share\system\configs\emulationstation\scripts"
$RUTA_SYSTEM = Join-Path $RUTA_BATOCERA "share\system"

if (!(Test-Path $RUTA_SYSTEM)) {
    Write-Host " Error: No se puede acceder a la carpeta de Batocera." -ForegroundColor Red
    pause; exit
}

# 2. CREAR CARPETAS
$null = New-Item -ItemType Directory -Force -Path (Join-Path $RUTA_SCRIPTS "game-start")
$null = New-Item -ItemType Directory -Force -Path (Join-Path $RUTA_SCRIPTS "game-end")

# 3. FUNCIÓN DE COPIADO
function Instalar-Script($FileName, $Destino) {
    $CurrentDir = Get-Location
    $PathOrigen = Join-Path $CurrentDir $FileName
    
    if (Test-Path $PathOrigen) {
        $Contenido = Get-Content $PathOrigen -Raw
        $Contenido = $Contenido -replace 'IP_ESP32=".*"', "IP_ESP32=`"$IP_PANEL`""
        
        $PathFinal = Join-Path $Destino $FileName
        # Forzamos formato UNIX (LF) y UTF8 sin BOM
        $ContenidoFinal = $Contenido -replace "`r`n", "`n"
        [System.IO.File]::WriteAllText($PathFinal, $ContenidoFinal, (New-Object System.Text.UTF8Encoding($false)))
        
        Write-Host " OK: Instalado $FileName" -ForegroundColor Green
    } else {
        Write-Host " Error: No se encuentra $FileName en la carpeta actual." -ForegroundColor Red
    }
}

# Ejecutar copiado de scripts
Instalar-Script "pixel_start.sh" (Join-Path $RUTA_SCRIPTS "game-start")
Instalar-Script "pixel_stop.sh" (Join-Path $RUTA_SCRIPTS "game-end")

# 4. CREAR CUSTOM.SH (AUTO-PERMISOS)
Write-Host " Creando configurador de permisos automaticos..." -ForegroundColor Yellow

$CustomSH = "#!/bin/bash`n"
$CustomSH += "# Otorga permisos a los scripts de marquesinas`n"
$CustomSH += "chmod -R +x /userdata/system/configs/emulationstation/scripts/game-start/`n"
$CustomSH += "chmod -R +x /userdata/system/configs/emulationstation/scripts/game-end/`n"

$PathCustom = Join-Path $RUTA_SYSTEM "custom.sh"
[System.IO.File]::WriteAllText($PathCustom, $CustomSH, (New-Object System.Text.UTF8Encoding($false)))

Write-Host "--------------------------------------------------" -ForegroundColor Cyan
Write-Host " INSTALACION COMPLETADA!" -ForegroundColor Green
Write-Host "--------------------------------------------------" -ForegroundColor Cyan
Write-Host "IMPORTANTE: Reinicia tu Batocera ahora."
Write-Host "El sistema aplicara los permisos automaticamente."
Write-Host "--------------------------------------------------"
pause