# ==========================================================
#   GESTOR DE MARQUESINAS RETRO PIXEL - UNIVERSAL (ESTABLE)
# ==========================================================
Add-Type -AssemblyName System.Drawing

# --- CONFIGURACION INICIAL ---
$defRutaRoms = "\\192.168.1.119\share\roms"
$defRutaSD = "C:\Export_Arcade"
$ancho = 128
$alto = 32

Write-Host "==========================================" -ForegroundColor Magenta
Write-Host "     CONFIGURACION DE RUTAS" -ForegroundColor White
Write-Host "==========================================" -ForegroundColor Magenta

# 1. Solicitar Rutas
$inputRoms = Read-Host "Ruta ROMs Batocera [Enter para: $defRutaRoms]"
$rutaRomsBatocera = if ([string]::IsNullOrWhiteSpace($inputRoms)) { $defRutaRoms } else { $inputRoms.Trim('"') }

$inputSD = Read-Host "Ruta Destino SD [Enter para: $defRutaSD]"
$rutaSD = if ([string]::IsNullOrWhiteSpace($inputSD)) { $defRutaSD } else { $inputSD.Trim('"') }

if (!(Test-Path $rutaRomsBatocera)) {
    Write-Host "`n[ERROR] No se encuentra: $rutaRomsBatocera" -ForegroundColor Red
    Read-Host "Presiona Enter para salir"
    exit
}

# 2. Obtener sistemas
$todosLosSistemas = Get-ChildItem -Path $rutaRomsBatocera -Directory | Where-Object { 
    Test-Path (Join-Path $_.FullName "gamelist.xml") 
}

# 3. Menú de seleccion
Write-Host "`n==========================================" -ForegroundColor Magenta
Write-Host "   SELECCION DE SISTEMA" -ForegroundColor White
Write-Host "==========================================" -ForegroundColor Magenta
Write-Host "0. [TODOS LOS SISTEMAS]" -ForegroundColor Yellow
$i = 1
foreach ($sys in $todosLosSistemas) {
    Write-Host "$i. $($sys.Name)"
    $i++
}
Write-Host "=========================================="

$seleccion = Read-Host "Selecciona el numero de sistema a indexar"

$sistemasAProcesar = @()
if ($seleccion -eq "0") {
    $sistemasAProcesar = $todosLosSistemas
} elseif ($seleccion -gt 0 -and $seleccion -le $todosLosSistemas.Count) {
    $sistemasAProcesar = $todosLosSistemas[$seleccion - 1]
} else {
    Write-Host "Seleccion no valida." -ForegroundColor Red
    exit
}

# 4. Procesamiento
foreach ($sysFolder in $sistemasAProcesar) {
    $sistemaNombre = $sysFolder.Name
    $xmlPath = Join-Path $sysFolder.FullName "gamelist.xml"
    Write-Host "`n>>> TRABAJANDO EN: $($sistemaNombre.ToUpper())" -ForegroundColor Cyan
    
    $destDirImagenes = Join-Path $rutaSD "Arcade\$sistemaNombre"
    if (!(Test-Path $destDirImagenes)) { New-Item -ItemType Directory -Force -Path $destDirImagenes | Out-Null }

    [xml]$xml = Get-Content $xmlPath -Raw -Encoding UTF8
    $listaJuegos = New-Object System.Collections.Generic.List[string]

    foreach ($game in $xml.gameList.game) {
        if ($game.path -and $game.marquee) {
            $marqueeValue = $game.marquee
            if ($marqueeValue -is [System.Array]) { $marqueeValue = $marqueeValue[0] }
            
            $romName = [System.IO.Path]::GetFileNameWithoutExtension($game.path).Trim().ToLower()
            $relImgPath = $marqueeValue.TrimStart('.').TrimStart('/').TrimStart('\')
            $imgPath = Join-Path $sysFolder.FullName $relImgPath
            
            if (Test-Path $imgPath) {
                try {
                    $bmp = New-Object System.Drawing.Bitmap($ancho, $alto, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
                    $g = [System.Drawing.Graphics]::FromImage($bmp)
                    $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
                    
                    $oldImg = [System.Drawing.Image]::FromFile($imgPath)
                    $g.DrawImage($oldImg, 0, 0, $ancho, $alto)
                    $oldImg.Dispose()
                    
                    $finalPath = Join-Path $destDirImagenes "$romName.bmp"
                    $bmp.Save($finalPath, [System.Drawing.Imaging.ImageFormat]::Bmp)
                    $bmp.Dispose()
                    $g.Dispose()
                    
                    if (!$listaJuegos.Contains($romName)) { $listaJuegos.Add($romName) }
                    Write-Host " OK: $romName"
                } catch {
                    Write-Host " Error: $romName" -ForegroundColor Yellow
                }
            }
        }
    }

    if ($listaJuegos.Count -gt 0) {
        $listaOrdenada = $listaJuegos | Sort-Object -Unique
        $destDirArcadeRaiz = Join-Path $rutaSD "Arcade"
        if (!(Test-Path $destDirArcadeRaiz)) { New-Item -ItemType Directory -Force -Path $destDirArcadeRaiz | Out-Null }
        $txtPath = Join-Path $destDirArcadeRaiz "$sistemaNombre.txt"
        [System.IO.File]::WriteAllLines($txtPath, $listaOrdenada)
        Write-Host "Indice $sistemaNombre.txt generado." -ForegroundColor Green
    }
}

Write-Host "`nPROCESO FINALIZADO" -ForegroundColor White
Write-Host "Presiona una tecla para salir"
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")