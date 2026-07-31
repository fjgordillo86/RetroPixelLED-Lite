# ==========================================================
#   GESTOR DE MARQUESINAS RETRO PIXEL - RECALBOX (COMPLETO)
# ==========================================================
Add-Type -AssemblyName System.Drawing

# --- DITHERING RGB565 (Floyd-Steinberg) ---
# El panel LED solo soporta 5 bits R, 6 bits G, 5 bits B (RGB565).
# Esta funcion "pre-cuantiza" el BMP con difusion de error para que,
# al truncar en el ESP32, no se vean bandas de color en degradados.
function Get-QuantizedValue([double]$valor, [int]$bits) {
    $niveles = [math]::Pow(2, $bits) - 1
    $paso = 255.0 / $niveles
    $q = [math]::Round($valor / $paso) * $paso
    if ($q -lt 0) { $q = 0 }
    if ($q -gt 255) { $q = 255 }
    return $q
}

function Convert-ToRGB565Dithered([System.Drawing.Bitmap]$bmp) {
    $w = $bmp.Width
    $h = $bmp.Height
    $rect = New-Object System.Drawing.Rectangle(0, 0, $w, $h)
    $bmpData = $bmp.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::ReadWrite, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
    $stride = $bmpData.Stride
    $bytes = $stride * $h
    $buffer = New-Object byte[] $bytes
    [System.Runtime.InteropServices.Marshal]::Copy($bmpData.Scan0, $buffer, 0, $bytes)

    # Copiamos a matrices de doble precision para poder acumular error negativo/positivo
    $r = New-Object double[] ($w * $h)
    $g = New-Object double[] ($w * $h)
    $b = New-Object double[] ($w * $h)
    for ($y = 0; $y -lt $h; $y++) {
        for ($x = 0; $x -lt $w; $x++) {
            $idx = $y * $stride + $x * 3
            $i = $y * $w + $x
            $b[$i] = $buffer[$idx]
            $g[$i] = $buffer[$idx + 1]
            $r[$i] = $buffer[$idx + 2]
        }
    }

    for ($y = 0; $y -lt $h; $y++) {
        for ($x = 0; $x -lt $w; $x++) {
            $i = $y * $w + $x
            $oldR = $r[$i]; $oldG = $g[$i]; $oldB = $b[$i]

            $newR = Get-QuantizedValue $oldR 5
            $newG = Get-QuantizedValue $oldG 6
            $newB = Get-QuantizedValue $oldB 5

            $r[$i] = $newR; $g[$i] = $newG; $b[$i] = $newB

            $errR = $oldR - $newR
            $errG = $oldG - $newG
            $errB = $oldB - $newB

            # Difusion de error Floyd-Steinberg: derecha 7/16, abajo-izq 3/16, abajo 5/16, abajo-der 1/16
            if ($x + 1 -lt $w) {
                $j = $i + 1
                $r[$j] += $errR * 7 / 16; $g[$j] += $errG * 7 / 16; $b[$j] += $errB * 7 / 16
            }
            if ($y + 1 -lt $h) {
                if ($x - 1 -ge 0) {
                    $j = $i + $w - 1
                    $r[$j] += $errR * 3 / 16; $g[$j] += $errG * 3 / 16; $b[$j] += $errB * 3 / 16
                }
                $j = $i + $w
                $r[$j] += $errR * 5 / 16; $g[$j] += $errG * 5 / 16; $b[$j] += $errB * 5 / 16
                if ($x + 1 -lt $w) {
                    $j = $i + $w + 1
                    $r[$j] += $errR * 1 / 16; $g[$j] += $errG * 1 / 16; $b[$j] += $errB * 1 / 16
                }
            }
        }
    }

    for ($y = 0; $y -lt $h; $y++) {
        for ($x = 0; $x -lt $w; $x++) {
            $idx = $y * $stride + $x * 3
            $i = $y * $w + $x
            $buffer[$idx]     = [byte]([math]::Round($b[$i]))
            $buffer[$idx + 1] = [byte]([math]::Round($g[$i]))
            $buffer[$idx + 2] = [byte]([math]::Round($r[$i]))
        }
    }

    [System.Runtime.InteropServices.Marshal]::Copy($buffer, 0, $bmpData.Scan0, $bytes)
    $bmp.UnlockBits($bmpData)
}

# --- CONFIGURACION INICIAL ---
$defRutaRoms = "\\192.168.1.123\share\roms"
$defRutaSD = "C:\Export_Arcade"
$ancho = 128
$alto = 32

Write-Host "==========================================" -ForegroundColor Magenta
Write-Host "     CONFIGURACION DE RUTAS RECALBOX" -ForegroundColor White
Write-Host "==========================================" -ForegroundColor Magenta

# 1. Solicitar Rutas
$inputRoms = Read-Host "Ruta ROMs Recalbox [Enter para: $defRutaRoms]"
$rutaRomsRecalbox = if ([string]::IsNullOrWhiteSpace($inputRoms)) { $defRutaRoms } else { $inputRoms.Trim('"') }

$inputSD = Read-Host "Ruta Destino SD [Enter para: $defRutaSD]"
$rutaSD = if ([string]::IsNullOrWhiteSpace($inputSD)) { $defRutaSD } else { $inputSD.Trim('"') }

if (!(Test-Path $rutaRomsRecalbox)) {
    Write-Host "`n[ERROR] No se encuentra: $rutaRomsRecalbox" -ForegroundColor Red
    Read-Host "Presiona Enter para salir"
    exit
}

# 1.5. Selección del tipo de imagen de origen
Write-Host "`n==========================================" -ForegroundColor Magenta
Write-Host "   ORIGEN DE LAS IMAGENES (SCRAPER)" -ForegroundColor White
Write-Host "==========================================" -ForegroundColor Magenta
Write-Host "1. Imagen estandar (<image>)"
Write-Host "2. Miniatura (<thumbnail>)"
Write-Host "3. Logo / Wheel Art (<logo>)"
Write-Host "=========================================="

$opcionImagen = Read-Host "Selecciona el tipo de imagen a usar [Enter para opcion 1]"
if ([string]::IsNullOrWhiteSpace($opcionImagen)) { $opcionImagen = "1" }

# 2. Obtener sistemas
$todosLosSistemas = Get-ChildItem -Path $rutaRomsRecalbox -Directory | Where-Object { 
    Test-Path (Join-Path $_.FullName "gamelist.xml") 
}

# 3. Menú de seleccion de sistema
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
        # Selección del nodo XML según la opción elegida por el usuario
        $nodoImagen = if ($opcionImagen -eq "2") { 
            $game.thumbnail 
        } elseif ($opcionImagen -eq "3") { 
            $game.logo 
        } else { 
            $game.image 
        }
        
        # Si existe el juego y tiene la etiqueta seleccionada, se procesa
        if ($game.path -and $nodoImagen) {
            $imageValue = $nodoImagen
            if ($imageValue -is [System.Array]) { $imageValue = $imageValue[0] }
            
            $romName = [System.IO.Path]::GetFileNameWithoutExtension($game.path).Trim().ToLower()
            $relImgPath = $imageValue.TrimStart('.').TrimStart('/').TrimStart('\')
            $imgPath = Join-Path $sysFolder.FullName $relImgPath
            
            if (Test-Path $imgPath) {
                try {
                    $bmp = New-Object System.Drawing.Bitmap($ancho, $alto, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
                    $g = [System.Drawing.Graphics]::FromImage($bmp)
                    $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
                    
                    $oldImg = [System.Drawing.Image]::FromFile($imgPath)
                    $g.DrawImage($oldImg, 0, 0, $ancho, $alto)
                    $oldImg.Dispose()

                    # Pre-cuantizamos con dithering a RGB565 para evitar bandas de color en el panel
                    Convert-ToRGB565Dithered $bmp

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