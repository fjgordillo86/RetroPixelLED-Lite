@echo off
title Retro Pixel LED - Generador de Playlists (ESTANDAR)
color 0B
setlocal enabledelayedexpansion

:: Configuración
set "TARGET_DIR=gifs"
set "PLAYLIST_DIR=playlists"
set "ROOT_DIR=%~dp0"

echo ========================================================
echo   RETRO PIXEL LED - GENERADOR DE PLAYLISTS INTERACTIVO
echo ========================================================
echo.

if not exist "%TARGET_DIR%" (
    color 0C
    echo [ERROR] No se encuentra la carpeta '%TARGET_DIR%'.
    pause
    exit /b
)

if not exist "%PLAYLIST_DIR%" mkdir "%PLAYLIST_DIR%"

echo [1] Escaneando carpetas...
echo.

set /a folderCount=0
for /f "tokens=*" %%D in ('dir /b /ad "%TARGET_DIR%"') do (
    set /a folderCount+=1
    set "folder[!folderCount!]=%%D"
    echo  [!folderCount!] %%D
)

echo.
echo [2] Seleccion de Carpetas
echo --------------------------------------------------------
echo Escribe los numeros separados por comas (ejemplo: 1,3,5)
echo O escribe "TODO" para incluir todas las carpetas.
echo --------------------------------------------------------
set /p selection="Seleccion: "

echo.
set /p playlistName="[3] Nombre de la lista: "
set "OUTPUT_FILE=%ROOT_DIR%%PLAYLIST_DIR%\%playlistName%.txt"

if exist "%OUTPUT_FILE%" del "%OUTPUT_FILE%"

set /a totalGifs=0

if /i "%selection%"=="TODO" (
    set "selection="
    for /L %%i in (1,1,%folderCount%) do (
        if %%i equ 1 (set "selection=%%i") else (set "selection=!selection!,%%i")
    )
)

:: Bucle de indexación
for %%s in (%selection%) do (
    set "currentFolder=!folder[%%s]!"
    echo  - Indexando: !currentFolder!
    
    :: Entramos en la carpeta específica de la carpeta gifs
    pushd "%ROOT_DIR%%TARGET_DIR%\!currentFolder!"
    
    :: Buscamos archivos .gif en esa carpeta y subcarpetas
    for /r %%F in (*.gif) do (
        set "FILE_ABS=%%F"
        :: Obtenemos la ruta relativa a la carpeta 'gifs'
        set "FILE_REL=!FILE_ABS:%ROOT_DIR%=!"
        :: Cambiamos barras \ por /
        set "FILE_LINE=/!FILE_REL:\=/!"
        
        echo !FILE_LINE!>>"%OUTPUT_FILE%"
        set /a totalGifs+=1
    )
    popd
)

echo.
color 0A
echo ========================================================
echo [EXITO] Playlist '%playlistName%.txt' creada.
echo Se han indexado !totalGifs! GIFs correctamente.
echo ========================================================
echo.
pause