@echo off
title Retro Pixel LED - Generador de lista.txt (Modo LITE)
color 0B
setlocal enabledelayedexpansion

set OUTPUT_FILE=lista.txt
set TARGET_DIR=gifs

echo ========================================================
echo   RETRO PIXEL LED LITE - GENERADOR DE LISTA DE GIFs
echo ========================================================
echo.

if not exist "%TARGET_DIR%" (
    color 0C
    echo [ERROR] No se ha encontrado la carpeta '%TARGET_DIR%'.
    echo Asegurate de que este archivo .bat esta en la raiz de la SD.
    echo.
    pause
    exit /b
)

echo [INFO] Indexando archivos .gif...
echo.

if exist "%OUTPUT_FILE%" del "%OUTPUT_FILE%"

set /a count=0

:: Bucle de escaneo
for /R "%TARGET_DIR%" %%F in (*.gif) do (
    set "FULL_PATH=%%F"
    set "REL_PATH=!FULL_PATH:%CD%=!"
    set "REL_PATH=!REL_PATH:\=/!"
    
    set "FIRST_CHAR=!REL_PATH:~0,1!"
    if NOT "!FIRST_CHAR!"=="/" (
        set "REL_PATH=/!REL_PATH!"
    )
    
    echo !REL_PATH!>>"%OUTPUT_FILE%"
    set /a count+=1
    
    :: EFECTO DE CONTADOR LIMPIO
    set /a modulo=!count! %% 5
    if !modulo! == 0 (
        <nul set /p "=... Procesando: !count! GIFs cargados "
        :: El truco: enviamos un retroceso de carro usando un comando de sistema
        for /f %%a in ('copy /Z "%~dpf0" nul') do <nul set /p "=%%a"
    )
)

echo.
echo.
color 0A
echo ========================================================
echo [EXITO] Proceso completado.
echo Se han indexado !count! GIFs correctamente.
echo ========================================================
echo.
pause