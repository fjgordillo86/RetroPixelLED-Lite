@echo off
title Retro Pixel LED - Générateur de Playlists (STANDARD)
color 0B
setlocal enabledelayedexpansion

:: Configuration
set "TARGET_DIR=gifs"
set "PLAYLIST_DIR=playlists"
set "ROOT_DIR=%~dp0"

echo ========================================================
echo   RETRO PIXEL LED - GENERATEUR DE PLAYLISTS INTERACTIF
echo ========================================================
echo.

if not exist "%TARGET_DIR%" (
    color 0C
    echo [ERREUR] Le dossier '%TARGET_DIR%' est introuvable.
    pause
    exit /b
)

if not exist "%PLAYLIST_DIR%" mkdir "%PLAYLIST_DIR%"

echo [1] Analyse des dossiers...
echo.

set /a folderCount=0
for /f "tokens=*" %%D in ('dir /b /ad "%TARGET_DIR%"') do (
    set /a folderCount+=1
    set "folder[!folderCount!]=%%D"
    echo  [!folderCount!] %%D
)

echo.
echo [2] Sélection des Dossiers
echo --------------------------------------------------------
echo Saisis les numéros séparés par des virgules (exemple : 1,3,5)
echo Ou écris "TOUT" pour inclure tous les dossiers.
echo --------------------------------------------------------
set /p selection="Sélection : "

echo.
set /p playlistName="[3] Nom de la liste : "
set "OUTPUT_FILE=%ROOT_DIR%%PLAYLIST_DIR%\%playlistName%.txt"

if exist "%OUTPUT_FILE%" del "%OUTPUT_FILE%"

set /a totalGifs=0

if /i "%selection%"=="TOUT" (
    set "selection="
    for /L %%i in (1,1,%folderCount%) do (
        if %%i equ 1 (set "selection=%%i") else (set "selection=!selection!,%%i")
    )
)

:: Boucle d’indexation
for %%s in (%selection%) do (
    set "currentFolder=!folder[%%s]!"
    echo  - Indexation : !currentFolder!
    
    :: Entrons dans le dossier spécifique à l’intérieur de 'gifs'
    pushd "%ROOT_DIR%%TARGET_DIR%\!currentFolder!"
    
    :: Recherche des fichiers .gif dans ce dossier et ses sous-dossiers
    for /r %%F in (*.gif) do (
        set "FILE_ABS=%%F"
        :: Obtenir le chemin relatif au dossier 'gifs'
        set "FILE_REL=!FILE_ABS:%ROOT_DIR%=!"
        :: Remplacer les \ par /
        set "FILE_LINE=/!FILE_REL:\=/!"
        
        echo !FILE_LINE!>>"%OUTPUT_FILE%"
        set /a totalGifs+=1
    )
    popd
)

echo.
color 0A
echo ========================================================
echo [SUCCÈS] Playlist '%playlistName%.txt' créée.
echo !totalGifs! GIFs ont été indexés correctement.
echo ========================================================
echo.
pause
