@echo off
title Retro Pixel LED - Generateur de Playlists (STANDARD)
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

:: Validation des dossiers requis
if not exist "%TARGET_DIR%" (
    color 0C
echo [ERREUR] Le dossier '%TARGET_DIR%' est introuvable.
echo Veuillez creer le dossier '%TARGET_DIR%' dans le repertoire du script.
pause
    exit /b 1
)

if not exist "%PLAYLIST_DIR%" (
    mkdir "%PLAYLIST_DIR%"
    if errorlevel 1 (
        color 0C
echo [ERREUR] Impossible de creer le dossier '%PLAYLIST_DIR%'.
pause
        exit /b 1
    )
)

echo [1] Analyse des dossiers...
echo.

:: Comptage et stockage des dossiers
set /a folderCount=0
for /f "tokens=*" %%D in ('dir /b /ad "%TARGET_DIR%"') do (
    set /a folderCount+=1
    set "folder[!folderCount!]=%%D"
echo  [!folderCount!] %%D
)

if %folderCount% equ 0 (
    color 0C
echo [ERREUR] Aucun dossier trouve dans '%TARGET_DIR%'.
pause
    exit /b 1
)

echo.
echo [2] Selection des Dossiers
echo --------------------------------------------------------
echo Saisissez les numeros separes par des virgules (exemple: 1,3,5)
echo Ou ecrivez "TOUT" pour inclure tous les dossiers.
echo --------------------------------------------------------
set /p selection="Selection : "

:: Validation de la selection
if not defined selection (
    color 0C
echo [ERREUR] Veuillez entrer une selection valide.
pause
    exit /b 1
)

echo.
set /p playlistName="[3] Nom de la liste de lecture : "

:: Validation du nom de playlist
if not defined playlistName (
    color 0C
echo [ERREUR] Le nom de la playlist ne peut pas etre vide.
pause
    exit /b 1
)

set "OUTPUT_FILE=%ROOT_DIR%%PLAYLIST_DIR%\%playlistName%.txt"

:: Verification si le fichier existe deja
if exist "%OUTPUT_FILE%" (
    color 0E
echo [ATTENTION] Le fichier '%playlistName%.txt' existe deja.
set /p overwrite="Voulez-vous le remplacer ? (O/N) : "
    if /i not "!overwrite!"=="O" (
        echo Operation annulee.
pause
        exit /b 0
    )
    del "%OUTPUT_FILE%"
)

set /a totalGifs=0

:: Expansion de "TOUT" pour inclure tous les dossiers
if /i "%selection%"=="TOUT" (
    set "selection="
    for /L %%i in (1,1,%folderCount%) do (
        if %%i equ 1 (
            set "selection=%%i"
        ) else (
            set "selection=!selection!,%%i"
        )
    )
)

:: Boucle de traitement des dossiers selectionnes
echo.
echo [4] Indexation des GIFs...
echo --------------------------------------------------------

for /f "tokens=*" %%s in ("!selection!") do (
    if not defined folder[%%s] (
        color 0C
echo [ERREUR] Numero de dossier invalide: %%s
        pause
        exit /b 1
    )
    
    set "currentFolder=!folder[%%s]!"
echo  - Indexation: !currentFolder!
    
    :: Changement du repertoire de travail
    pushd "%ROOT_DIR%%TARGET_DIR%\!currentFolder!"
    
    if errorlevel 1 (
        color 0C
echo [ERREUR] Impossible d'acceder au dossier: !currentFolder!
popd
        pause
        exit /b 1
    )
    
    :: Recherche des fichiers .gif recursifs
    for /r %%F in (*.gif) do (
        set "FILE_ABS=%%F"
        :: Extraction du chemin relatif au dossier 'gifs'
        set "FILE_REL=!FILE_ABS:%ROOT_DIR%=!"
        :: Conversion des backslashes en forward slashes
        set "FILE_LINE=/!FILE_REL:\=/!"
        
echo !FILE_LINE!>>"%OUTPUT_FILE%"
        set /a totalGifs+=1
    )
    
    popd
)

echo.
color 0A
echo ========================================================
echo [SUCCES] Playlist '%playlistName%.txt' creee avec succes.
echo !totalGifs! GIFs ont ete indexes correctement.
echo Emplacement: %OUTPUT_FILE%
echo ========================================================
echo.
pause
exit /b 0