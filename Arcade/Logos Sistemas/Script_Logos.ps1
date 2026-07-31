# ==========================================================
#   CONVERTIDOR UNIVERSAL DE MARQUESINAS - 24-BIT RGB (128x32)
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

# --- CONFIGURACION DE DIMENSIONES ---
$ancho = 128
$alto = 32

Write-Host "==========================================" -ForegroundColor Magenta
Write-Host "    CONVERTIDOR DE IMÁGENES EN LOTE" -ForegroundColor White
Write-Host "==========================================" -ForegroundColor Magenta

# 1. Solicitar Carpeta de Origen
$inputOrigen = Read-Host "Introduce la ruta de la carpeta con las imágenes"
$rutaOrigen = $inputOrigen.Trim('"')

if (!(Test-Path $rutaOrigen)) {
    Write-Host "`n[ERROR] No se encuentra la carpeta indicada: $rutaOrigen" -ForegroundColor Red
    Read-Host "Presiona Enter para salir"
    exit
}

# 2. Solicitar Carpeta de Destino (Opcional)
$inputDestino = Read-Host "Ruta de destino [Enter para guardar en la misma carpeta de origen]"
$rutaDestino = if ([string]::IsNullOrWhiteSpace($inputDestino)) { $rutaOrigen } else { $inputDestino.Trim('"') }

if (!(Test-Path $rutaDestino)) {
    New-Item -ItemType Directory -Force -Path $rutaDestino | Out-Null
}

# 3. Buscar todas las imágenes válidas (Soporta recursividad)
Write-Host "`nBuscando imágenes válidas..." -ForegroundColor Cyan
$imagenes = Get-ChildItem -Path $rutaOrigen -File -Recurse | Where-Object {
    $_.Extension -match '^\.(png|jpg|jpeg|bmp|gif)$'
}

if ($imagenes.Count -eq 0) {
    Write-Host "[AVISO] No se encontraron imágenes (.png, .jpg, .jpeg, .bmp, .gif) en la carpeta." -ForegroundColor Yellow
    Read-Host "Presiona Enter para salir"
    exit
}

Write-Host "Se encontraron $($imagenes.Count) imágenes listas para procesar.`n" -ForegroundColor Green

# 4. Procesamiento en lote
$contadorExito = 0
foreach ($img in $imagenes) {
    # El nombre se limpia a minúsculas para mantener consistencia con los scripts de Recalbox
    $nombreSinExt = [System.IO.Path]::GetFileNameWithoutExtension($img.Name).Trim().ToLower()
    $finalPath = Join-Path $rutaDestino "$nombreSinExt.bmp"
    
    Write-Host "Procesando: $($img.Name) -> $nombreSinExt.bmp ... " -NoNewline
    
    # Inicializamos variables de control para asegurar su liberación en caso de fallo
    $bmp = $null
    $g = $null
    $oldImg = $null
    
    try {
        # Fuerza el lienzo exacto a 128x32 con formato nativo de 24 bits (3 bytes por píxel)
        $bmp = New-Object System.Drawing.Bitmap($ancho, $alto, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
        $g = [System.Drawing.Graphics]::FromImage($bmp)
        
        # Interpolación Bicúbica de alta calidad para evitar dientes de sierra en 128x32
        $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
        $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
        $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
        $g.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
        
        # Cargamos y dibujamos la imagen original escalada
        $oldImg = [System.Drawing.Image]::FromFile($img.FullName)
        $g.DrawImage($oldImg, 0, 0, $ancho, $alto)
        
        # Cerramos el archivo original inmediatamente para evitar bloqueos en disco
        $oldImg.Dispose()
        $g.Dispose()
        
        # Pre-cuantizamos con dithering a RGB565 para evitar bandas de color en el panel
        Convert-ToRGB565Dithered $bmp
        
        # Guardamos como mapa de bits estándar
        $bmp.Save($finalPath, [System.Drawing.Imaging.ImageFormat]::Bmp)
        $bmp.Dispose()
        
        Write-Host "[OK]" -ForegroundColor Green
        $contadorExito++
    } catch {
        Write-Host "[ERROR]" -ForegroundColor Red
        Write-Host "    Detalle: $_" -ForegroundColor Yellow
        
        # Limpieza de emergencia si el archivo falla a mitad de proceso
        if ($null -ne $oldImg) { $oldImg.Dispose() }
        if ($null -ne $g) { $g.Dispose() }
        if ($null -ne $bmp) { $bmp.Dispose() }
    }
}

Write-Host "`n==========================================" -ForegroundColor Magenta
Write-Host "PROCESO COMPLETADO: $contadorExito de $($imagenes.Count) imágenes procesadas con éxito." -ForegroundColor Green
Write-Host "Las marquesinas están listas en: $rutaDestino" -ForegroundColor White
Write-Host "==========================================" -ForegroundColor Magenta
Write-Host "Presiona una tecla para salir"
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")