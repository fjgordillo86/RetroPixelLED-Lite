<#
.SYNOPSIS
    RetroPixelLED - ReplayOS Toolkit v5.1
.DESCRIPTION
    Herramienta unificada para preparar marquesinas de arcade para RetroPixelLED-Lite
    cuando el frontend es ReplayOS. Sustituye al script anterior (un solo flujo lineal)
    por un menu con tres operaciones independientes:

      1) Scrapear sistema(s): descarga recursos crudos (PNG/JPG/etc) desde ArcadeDB o
         TheGamesDB a una CACHE local reutilizable, organizados por sistema y tipo de
         recurso. No genera BMP todavia.

      2) Generar imagenes: convierte, 100% offline (sin tocar la red), los recursos ya
         cacheados a BMP 128x32 (con dithering RGB565 Floyd-Steinberg si el destino es
         ReplayOS) listos para copiar a Arcade/<sistema>/. Puedes repetir esta opcion
         cuantas veces quieras para probar ajustes distintos sin volver a descargar nada.

      3) Generar / auditar listados: escanea Arcade/<sistema>/ en busca de .bmp y .gif
         (agrupando secuencias tipo mslug_01.gif, mslug_02.gif... como un unico romset)
         y construye o revisa el <sistema>.txt que el ESP32 usa para saber que romsets
         tienen marquesina. Si ya existe un .txt, primero te avisa de huerfanos (en el
         listado pero sin archivo) y de archivos sin indexar (colocados a mano, tipico
         al anadir GIFs sueltos sin pasar por el scraper).

#>

$Host.UI.RawUI.WindowTitle = "RetroPixelLED - ReplayOS Toolkit v5.1"
$OutputEncoding = [System.Text.Encoding]::UTF8
$ErrorActionPreference = "Stop"

Add-Type -AssemblyName System.Drawing

# ============================================================
#   CONFIGURACION PERSISTENTE (rutas y API key por defecto)
# ============================================================
$ConfigPath = Join-Path $PSScriptRoot "RetroPixelLED_Config.json"

function Load-Config {
    if (Test-Path $ConfigPath) {
        try {
            $json = [System.IO.File]::ReadAllText($ConfigPath)
            $obj = $json | ConvertFrom-Json
            if ($null -ne $obj) { return $obj }
        } catch {
            Write-Host "Aviso: no se pudo leer $ConfigPath, se usaran valores por defecto." -ForegroundColor DarkYellow
        }
    }
    return [PSCustomObject]@{}
}

function Save-Config($cfg) {
    $json = $cfg | ConvertTo-Json -Depth 5
    [System.IO.File]::WriteAllText($ConfigPath, $json, (New-Object System.Text.UTF8Encoding($false)))
}

$script:Config = Load-Config

# Pide un valor con un default sugerido (de la config o, si no hay, uno fijo), y lo
# guarda en la config para la proxima ejecucion.
function Get-ConfigValue([string]$key, [string]$defaultValue, [string]$prompt) {
    $cfg = $script:Config
    $current = $null
    if ($cfg.PSObject.Properties.Name -contains $key) { $current = $cfg.$key }
    if ([string]::IsNullOrWhiteSpace($current)) { $current = $defaultValue }

    $entrada = Read-Host "$prompt (Enter para usar: $current)"
    if ([string]::IsNullOrWhiteSpace($entrada)) { $entrada = $current }
    $entrada = $entrada.Trim()

    if ($cfg.PSObject.Properties.Name -contains $key) {
        $cfg.$key = $entrada
    } else {
        $cfg | Add-Member -MemberType NoteProperty -Name $key -Value $entrada
    }
    Save-Config $cfg
    return $entrada
}

# ============================================================
#   HELPERS DE MENU
# ============================================================

# Seleccion multiple por comas (ej: 1,3,5) o TODOS. $labelProp indica que propiedad de
# cada elemento de $items se muestra como texto (los sistemas usan "Nombre", los tipos
# de recurso usan "Name").
function Select-Multiple([array]$items, [string]$labelProp, [string]$promptTitle) {
    Write-Host ""
    Write-Host $promptTitle -ForegroundColor Cyan
    for ($i = 0; $i -lt $items.Count; $i++) {
        Write-Host ("  {0,2}) {1}" -f ($i + 1), $items[$i].$labelProp) -ForegroundColor White
    }
    Write-Host ""
    Write-Host "Escribe los numeros separados por comas (ej: 1,3,5) o TODOS" -ForegroundColor DarkGray
    $entrada = Read-Host "Seleccion"

    if ($entrada.Trim().ToUpper() -eq "TODOS") {
        return $items
    }

    $nums = $entrada -split "," | ForEach-Object { $_.Trim() } | Where-Object { $_ -match '^\d+$' }
    $seleccion = @()
    foreach ($n in $nums) {
        $idx = [int]$n - 1
        if ($idx -ge 0 -and $idx -lt $items.Count) { $seleccion += $items[$idx] }
    }
    return $seleccion
}

# Lista las subcarpetas directas de una ruta como elementos seleccionables (Nombre/Ruta).
function Get-SystemFolders([string]$root) {
    if (-not (Test-Path $root)) { return @() }
    return Get-ChildItem -Path $root -Directory | Sort-Object Name | ForEach-Object {
        [PSCustomObject]@{ Nombre = $_.Name; Ruta = $_.FullName }
    }
}

# ============================================================
#   TABLA DE RECURSOS (ArcadeDB query_mame_media / TheGamesDB)
# ============================================================
$script:AllMediaTypes = @(
    @{ Name = "MARQUEE";           FolderSuffix = "Marquees";  ArcadeKey = "url_image_marquee";        TgdbKeys = @("banner");      DefExt = "png" }
    @{ Name = "LOGO (CLEARLOGO)";  FolderSuffix = "Logos";     ArcadeKey = "url_image_logo";           TgdbKeys = @("clearlogo");   DefExt = "png" }
    @{ Name = "TITLE";             FolderSuffix = "Titles";    ArcadeKey = "url_image_title";          TgdbKeys = @("titlescreen"); DefExt = "png" }
    @{ Name = "INGAME SNAPSHOT";   FolderSuffix = "Snaps";     ArcadeKey = "url_image_ingame";         TgdbKeys = @("screenshot");  DefExt = "png" }
    @{ Name = "FANART";            FolderSuffix = "Fanarts";   ArcadeKey = $null;                      TgdbKeys = @("fanart");      DefExt = "jpg" }
    @{ Name = "FLYER";             FolderSuffix = "Flyers";    ArcadeKey = "url_image_flyer";          TgdbKeys = @("boxart");      DefExt = "png" }
    @{ Name = "ICON";              FolderSuffix = "Icons";     ArcadeKey = "url_icon";                 TgdbKeys = @();              DefExt = "png" }
    @{ Name = "CABINET";           FolderSuffix = "Cabinets";  ArcadeKey = "url_image_cabinet";        TgdbKeys = @();              DefExt = "png" }
    @{ Name = "CONTROL PANEL";     FolderSuffix = "CPanels";   ArcadeKey = "url_image_cpanel";         TgdbKeys = @();              DefExt = "png" }
    @{ Name = "BEZEL";             FolderSuffix = "Bezels";    ArcadeKey = "url_image_bezel";          TgdbKeys = @();              DefExt = "png" }
    @{ Name = "PCB";               FolderSuffix = "PCBs";      ArcadeKey = "url_image_pcb";            TgdbKeys = @();              DefExt = "png" }
    @{ Name = "BOX";               FolderSuffix = "Boxes";     ArcadeKey = "url_image_box";            TgdbKeys = @();              DefExt = "png" }
    @{ Name = "ARTWORK PREVIEW";   FolderSuffix = "Artworks";  ArcadeKey = "url_image_artwork_preview";TgdbKeys = @();              DefExt = "png" }
    @{ Name = "BOSS";              FolderSuffix = "Bosses";    ArcadeKey = "url_image_boss";           TgdbKeys = @();              DefExt = "png" }
    @{ Name = "HOW-TO";            FolderSuffix = "HowTo";     ArcadeKey = "url_image_howto";          TgdbKeys = @();              DefExt = "png" }
    @{ Name = "SCORE";             FolderSuffix = "Scores";    ArcadeKey = "url_image_score";          TgdbKeys = @();              DefExt = "png" }
    @{ Name = "SELECT";            FolderSuffix = "Selects";   ArcadeKey = "url_image_select";         TgdbKeys = @();              DefExt = "png" }
    @{ Name = "DECAL";             FolderSuffix = "Decals";    ArcadeKey = "url_image_decal";          TgdbKeys = @();              DefExt = "png" }
    @{ Name = "VERSUS";            FolderSuffix = "Versus";    ArcadeKey = "url_image_versus";         TgdbKeys = @();              DefExt = "png" }
    @{ Name = "GAME OVER";         FolderSuffix = "GameOver";  ArcadeKey = "url_image_gameover";       TgdbKeys = @();              DefExt = "png" }
    @{ Name = "END";               FolderSuffix = "End";       ArcadeKey = "url_image_end";            TgdbKeys = @();              DefExt = "png" }
    @{ Name = "WARNING";           FolderSuffix = "Warning";   ArcadeKey = "url_image_warning";        TgdbKeys = @();              DefExt = "png" }
    @{ Name = "MANUAL (PDF)";      FolderSuffix = "Manuals";   ArcadeKey = "url_manual";               TgdbKeys = @();              DefExt = "pdf" }
    @{ Name = "VIDEO SHORT";       FolderSuffix = "Videos";    ArcadeKey = "url_video_shortplay";      TgdbKeys = @();              DefExt = "mp4" }
    @{ Name = "VIDEO SHORT HD";    FolderSuffix = "VideosHD";  ArcadeKey = "url_video_shortplay_hd";   TgdbKeys = @();              DefExt = "mp4" }
)

# ============================================================
#   DITHERING RGB565 (Floyd-Steinberg) - identico al script anterior
# ============================================================
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

# ============================================================
#   TheGamesDB: normaliza la respuesta a la misma forma que ArcadeDB
#   ($item.NombreDelRecurso -> objeto con Url y Ext), para que el bucle
#   principal de descarga trate ambas fuentes de forma identica.
# ============================================================
function Get-TgdbItem($game, $apiKey) {
    $cleanTitle = $game.title -replace '\.zip$', '' -replace ' - .*$', '' -replace '\(.*?\)', '' -replace '\[.*?\]', ''
    $cleanTitle = $cleanTitle.Trim()

    $searchUrl = "https://api.thegamesdb.net/v1/Games/ByGameName?apikey=$apiKey&name=$([Uri]::EscapeDataString($cleanTitle))&fields=id,game_title"
    $searchRes = Invoke-RestMethod -Uri $searchUrl -Method Get -TimeoutSec 30
    if (-not $searchRes.data.games -or $searchRes.data.games.Count -eq 0) { return $null }

    $gameId = [string]$searchRes.data.games[0].id
    $imagesUrl = "https://api.thegamesdb.net/v1/Games/Images?apikey=$apiKey&games_id=$gameId"
    $imagesRes = Invoke-RestMethod -Uri $imagesUrl -Method Get -TimeoutSec 30
    $baseUrl = $imagesRes.data.base_url.original
    $gameImages = $imagesRes.data.images."$gameId"
    if (-not $gameImages) { return $null }

    $resultado = [PSCustomObject]@{}
    foreach ($media in $script:AllMediaTypes) {
        if ($media.TgdbKeys.Count -eq 0) { continue }
        $imgData = $null
        foreach ($key in $media.TgdbKeys) {
            $imgData = $gameImages | Where-Object { $_.type -eq $key } | Select-Object -First 1
            if ($imgData) { break }
        }
        $entrada = [PSCustomObject]@{ Url = $null; Ext = $media.DefExt }
        if ($imgData) {
            $entrada.Url = "$baseUrl$($imgData.filename)"
            $remoteExt = ([System.IO.Path]::GetExtension($imgData.filename)).TrimStart('.')
            if (-not [string]::IsNullOrWhiteSpace($remoteExt)) { $entrada.Ext = $remoteExt }
        }
        $resultado | Add-Member -MemberType NoteProperty -Name $media.Name -Value $entrada
    }
    return $resultado
}

# ============================================================
#   OPCION 1: SCRAPEAR SISTEMA(S)
# ============================================================
function Invoke-ScrapeSistemas {
    Write-Host ""
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host "  OPCION 1: SCRAPEAR SISTEMA(S)" -ForegroundColor Cyan
    Write-Host "===================================================" -ForegroundColor Magenta

    $romsRoot = Get-ConfigValue "RomsRoot" "D:\ROMS" "Ruta de la carpeta ROMS de ReplayOS"
    $cacheRoot = Get-ConfigValue "CacheRoot" (Join-Path $PSScriptRoot "Cache") "Ruta de la carpeta de cache de recursos"

    $sistemas = Get-SystemFolders $romsRoot
    if ($sistemas.Count -eq 0) {
        Write-Host "No se encontraron subcarpetas de sistemas en $romsRoot" -ForegroundColor Red
        return
    }

    $elegidos = Select-Multiple $sistemas "Nombre" "Sistemas encontrados en ROMS (el nombre exacto de la carpeta es el codigo de sistema que usara ReplayOS):"
    if ($elegidos.Count -eq 0) {
        Write-Host "No se selecciono ningun sistema." -ForegroundColor Yellow
        return
    }

    Write-Host ""
    Write-Host "Selecciona la fuente de descarga:" -ForegroundColor Cyan
    Write-Host "  1) ArcadeDB / ArcadeItalia (Catalogo ampliado - Sin API Key)" -ForegroundColor White
    Write-Host "  2) TheGamesDB (Requiere API Key)" -ForegroundColor White
    $sourceOpt = Read-Host "Opcion (1 o 2)"

    $TheGamesDbApiKey = ""
    if ($sourceOpt -eq "2") {
        $Source = "TGDB"
        $TheGamesDbApiKey = Get-ConfigValue "TgdbApiKey" "" "API Key de TheGamesDB"
        if ([string]::IsNullOrWhiteSpace($TheGamesDbApiKey)) {
            Write-Host "Se requiere una API Key para usar TheGamesDB." -ForegroundColor Red
            return
        }
    } else {
        $Source = "ArcadeDB"
    }

    $mediaSeleccionados = Select-Multiple $script:AllMediaTypes "Name" "Selecciona el/los recursos a descargar (fuente: $Source):"
    if ($mediaSeleccionados.Count -eq 0) { $mediaSeleccionados = @($script:AllMediaTypes[0]) }

    $forzarOpt = Read-Host "Reintentar tambien romsets marcados anteriormente como sin recursos? (S/N)"
    $forzarReintento = ($forzarOpt.Trim().ToUpper() -eq "S")

    foreach ($sistema in $elegidos) {
        Invoke-ScrapeUnSistema -SystemCode $sistema.Nombre -RomFolder $sistema.Ruta -CacheRoot $cacheRoot -Source $Source -ApiKey $TheGamesDbApiKey -MediaList $mediaSeleccionados -Forzar $forzarReintento
    }
}

function Invoke-ScrapeUnSistema {
    param(
        [string]$SystemCode,
        [string]$RomFolder,
        [string]$CacheRoot,
        [string]$Source,
        [string]$ApiKey,
        [array]$MediaList,
        [bool]$Forzar
    )

    Write-Host ""
    Write-Host "---------------------------------------------------" -ForegroundColor DarkCyan
    Write-Host "Sistema: $SystemCode" -ForegroundColor Cyan
    Write-Host "---------------------------------------------------" -ForegroundColor DarkCyan

    $sistemaCacheDir = Join-Path $CacheRoot $SystemCode
    if (-not (Test-Path $sistemaCacheDir)) { New-Item -ItemType Directory -Path $sistemaCacheDir -Force | Out-Null }

    foreach ($m in $MediaList) {
        if ($Source -eq "ArcadeDB" -and -not $m.ArcadeKey) { continue }
        if ($Source -eq "TGDB" -and $m.TgdbKeys.Count -eq 0) { continue }
        $dirPath = Join-Path $sistemaCacheDir $m.FolderSuffix
        if (-not (Test-Path $dirPath)) { New-Item -ItemType Directory -Path $dirPath -Force | Out-Null }
    }

    # Romsets = nombres de fichero (sin extension) de los .zip o .7z en la carpeta de ROMS
    # de este sistema. Se excluyen los sets de BIOS compartidos habituales.
    $files = Get-ChildItem -Path $RomFolder -File -ErrorAction SilentlyContinue | Where-Object { $_.Extension -in ".zip", ".7z" }
    $seenRomsets = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    $bioNames = @("neogeo", "cps1", "cps2", "cps3", "pgm", "naomi", "naomibios")
    $games = @()
    foreach ($f in $files) {
        $romset = [System.IO.Path]::GetFileNameWithoutExtension($f.Name)
        if (-not $romset -or $seenRomsets.Contains($romset)) { continue }
        if ($romset -in $bioNames) { continue }
        [void]$seenRomsets.Add($romset)
        $games += [PSCustomObject]@{ romset = $romset; title = $romset }
    }
    $games = $games | Sort-Object romset

    Write-Host "Romsets encontrados: $($games.Count)" -ForegroundColor Green
    if ($games.Count -eq 0) { return }

    # Cache de "sin recursos": romsets que ya sabemos que no devuelven NADA en esta
    # fuente, para no volver a preguntar por ellos en cada ejecucion.
    $failCachePath = Join-Path $sistemaCacheDir "_sin_recursos_$($Source.ToLower()).txt"
    $sinRecursosPrevios = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    if ((-not $Forzar) -and (Test-Path $failCachePath)) {
        Get-Content -Path $failCachePath | ForEach-Object { if ($_.Trim()) { [void]$sinRecursosPrevios.Add($_.Trim()) } }
    }

    $missing = [System.Collections.Generic.List[string]]::new()
    $errors  = [System.Collections.Generic.List[string]]::new()
    $sinRecursosNuevo = [System.Collections.Generic.List[string]]::new()
    $DelayMs = 1200

    $count = 0
    $conRecursos = 0
    foreach ($game in $games) {
        $count++

        if ($sinRecursosPrevios.Contains($game.romset)) {
            Write-Host "[$count/$($games.Count)] [OMITIDO - sin recursos previamente] $($game.romset)" -ForegroundColor DarkGray
            [void]$sinRecursosNuevo.Add($game.romset)
            continue
        }

        $neededMedia = @()
        foreach ($media in $MediaList) {
            if ($Source -eq "ArcadeDB" -and -not $media.ArcadeKey) { continue }
            if ($Source -eq "TGDB" -and $media.TgdbKeys.Count -eq 0) { continue }
            $dirPath = Join-Path $sistemaCacheDir $media.FolderSuffix
            $existente = Get-ChildItem -Path $dirPath -Filter "$($game.romset).*" -ErrorAction SilentlyContinue
            if ($existente) {
                Write-Host "[$count/$($games.Count)] [EXISTE EN CACHE] [$($media.Name)] $($game.romset)" -ForegroundColor Gray
            } else {
                $neededMedia += $media
            }
        }
        if ($neededMedia.Count -eq 0) { continue }

        Write-Host "[$count/$($games.Count)] Consultando: $($game.romset)" -ForegroundColor Cyan

        # Backoff: hasta 3 intentos ante ERRORES (red, timeout, etc). Un resultado vacio
        # "de verdad" (sin excepcion) NO se reintenta, se trata como NO ENCONTRADO directo.
        $item = $null
        $intentos = 0
        $maxIntentos = 3
        $huboErrorFinal = $false
        while ($intentos -lt $maxIntentos) {
            $intentos++
            $huboError = $false
            try {
                if ($Source -eq "ArcadeDB") {
                    $queryUrl = "https://adb.arcadeitalia.net/service_scraper.php?ajax=query_mame_media&game_name=$([Uri]::EscapeDataString($game.romset))"
                    $response = Invoke-RestMethod -Uri $queryUrl -TimeoutSec 60
                    if ($response.result -and $response.result.Count -gt 0) { $item = $response.result[0] }
                } else {
                    $item = Get-TgdbItem -Game $game -ApiKey $ApiKey
                }
            } catch {
                $huboError = $true
                if ($intentos -lt $maxIntentos) {
                    $espera = 2000 * $intentos
                    Write-Host "   [REINTENTO $intentos/$maxIntentos] $($game.romset): $_ (esperando ${espera}ms)" -ForegroundColor DarkYellow
                    Start-Sleep -Milliseconds $espera
                } else {
                    Write-Host "   [ERROR CONSULTA] $($game.romset): $_" -ForegroundColor Red
                    $errors.Add($game.romset)
                    $huboErrorFinal = $true
                }
            }
            if (-not $huboError) { break }
        }

        if ($null -eq $item) {
            if (-not $huboErrorFinal) {
                Write-Host "   [NO ENCONTRADO] $($game.romset)" -ForegroundColor DarkYellow
                foreach ($m in $neededMedia) { $missing.Add("$($game.romset) [$($m.Name)]") }
                [void]$sinRecursosNuevo.Add($game.romset)
            }
            Start-Sleep -Milliseconds $DelayMs
            continue
        }

        $conRecursos++
        foreach ($media in $neededMedia) {
            $dirPath = Join-Path $sistemaCacheDir $media.FolderSuffix
            $label = $media.Name

            $url = $null
            $ext = $media.DefExt
            if ($Source -eq "ArcadeDB") {
                $url = $item.$($media.ArcadeKey)
            } else {
                $tgdbEntry = $item.($media.Name)
                if ($tgdbEntry) { $url = $tgdbEntry.Url; $ext = $tgdbEntry.Ext }
            }

            if ([string]::IsNullOrWhiteSpace($url)) {
                Write-Host "   [SIN $label] $($game.romset)" -ForegroundColor DarkYellow
                $missing.Add("$($game.romset) [$label]")
                continue
            }

            $dest = Join-Path $dirPath "$($game.romset).$ext"
            try {
                Invoke-WebRequest -Uri $url.Trim() -OutFile $dest -TimeoutSec 90
                Write-Host "   [OK] [$label] $($game.romset).$ext" -ForegroundColor Green
            } catch {
                Write-Host "   [ERROR DESCARGA] [$label] $($game.romset): $_" -ForegroundColor Red
                $errors.Add("$($game.romset) [$label]")
            }
        }

        Start-Sleep -Milliseconds $DelayMs
    }

    if ($missing.Count -gt 0) {
        $missing | Sort-Object -Unique | Out-File -FilePath (Join-Path $sistemaCacheDir "_faltantes.txt") -Encoding utf8
    }
    if ($errors.Count -gt 0) {
        $errors | Sort-Object -Unique | Out-File -FilePath (Join-Path $sistemaCacheDir "_errores.txt") -Encoding utf8
    }
    $sinRecursosNuevo | Sort-Object -Unique | Out-File -FilePath $failCachePath -Encoding utf8

    Write-Host ""
    Write-Host "Resumen [$SystemCode]: $conRecursos de $($games.Count) romsets con datos en $Source." -ForegroundColor Green
    Write-Host "Cache actualizada en: $sistemaCacheDir" -ForegroundColor White
}

# ============================================================
#   OPCION 2: GENERAR IMAGENES (offline, desde cache)
# ============================================================
function Invoke-GenerarImagenes {
    Write-Host ""
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host "  OPCION 2: GENERAR IMAGENES (offline, desde cache)" -ForegroundColor Cyan
    Write-Host "===================================================" -ForegroundColor Magenta

    $cacheRoot = Get-ConfigValue "CacheRoot" (Join-Path $PSScriptRoot "Cache") "Ruta de la carpeta de cache de recursos"
    $arcadeRoot = Get-ConfigValue "ArcadeRoot" "D:\Arcade" "Ruta de la carpeta Arcade de salida (raiz, para copiar a la SD)"

    $sistemas = Get-SystemFolders $cacheRoot
    if ($sistemas.Count -eq 0) {
        Write-Host "No hay ningun sistema en la cache ($cacheRoot). Ejecuta antes la Opcion 1." -ForegroundColor Red
        return
    }

    $elegidos = Select-Multiple $sistemas "Nombre" "Sistemas disponibles en cache (elige uno o varios):"
    if ($elegidos.Count -eq 0) { return }

    $replayOpt = Read-Host "El sistema que usa tu Arcade es ReplayOS? (S/N)"
    $esReplayOS = ($replayOpt.Trim().ToUpper() -eq "S")
    $aplicarDithering = $esReplayOS

    $anchoBmp = 128
    $altoBmp = 32

    foreach ($sistema in $elegidos) {
        Invoke-GenerarImagenesUnSistema -SystemCode $sistema.Nombre -CacheDir $sistema.Ruta -ArcadeRoot $arcadeRoot -AplicarDithering $aplicarDithering -Ancho $anchoBmp -Alto $altoBmp
    }
}

function Invoke-GenerarImagenesUnSistema {
    param(
        [string]$SystemCode, [string]$CacheDir, [string]$ArcadeRoot,
        [bool]$AplicarDithering, [int]$Ancho, [int]$Alto
    )

    Write-Host ""
    Write-Host "---------------------------------------------------" -ForegroundColor DarkCyan
    Write-Host "Sistema: $SystemCode" -ForegroundColor Cyan
    Write-Host "---------------------------------------------------" -ForegroundColor DarkCyan

    $imageExtRegex = '^\.(png|jpg|jpeg|bmp|gif)$'
    $carpetasRecurso = @(Get-ChildItem -Path $CacheDir -Directory -ErrorAction SilentlyContinue | Where-Object {
        (Get-ChildItem -Path $_.FullName -File -ErrorAction SilentlyContinue | Where-Object { $_.Extension -match $imageExtRegex }).Count -gt 0
    })

    if ($carpetasRecurso.Count -eq 0) {
        Write-Host "No hay imagenes cacheadas para este sistema." -ForegroundColor Yellow
        return
    }

    Write-Host ""
    Write-Host "Que recurso quieres usar como fuente de las marquesinas de [$SystemCode]?" -ForegroundColor Yellow
    for ($i = 0; $i -lt $carpetasRecurso.Count; $i++) {
        $n = (Get-ChildItem -Path $carpetasRecurso[$i].FullName -File | Where-Object { $_.Extension -match $imageExtRegex }).Count
        Write-Host ("  {0}) {1}  ({2} imagenes)" -f ($i + 1), $carpetasRecurso[$i].Name, $n) -ForegroundColor White
    }
    $srcOpt = Read-Host "Selecciona una opcion (1-$($carpetasRecurso.Count))"
    $srcIdx = 0
    if ($srcOpt -match '^\d+$' -and [int]$srcOpt -ge 1 -and [int]$srcOpt -le $carpetasRecurso.Count) {
        $srcIdx = [int]$srcOpt - 1
    }
    $marqueeDir = $carpetasRecurso[$srcIdx].FullName

    $bmpOutDir = Join-Path $ArcadeRoot $SystemCode
    if (-not (Test-Path $bmpOutDir)) { New-Item -ItemType Directory -Path $bmpOutDir -Force | Out-Null }

    $imagenes = Get-ChildItem -Path $marqueeDir -File | Where-Object { $_.Extension -match $imageExtRegex }
    Write-Host ""
    Write-Host "Convirtiendo $($imagenes.Count) imagenes a ${Ancho}x${Alto} BMP..." -ForegroundColor Yellow

    $contadorExito = 0
    foreach ($img in $imagenes) {
        $nombreSinExt = [System.IO.Path]::GetFileNameWithoutExtension($img.Name).Trim().ToLower()
        $finalPath = Join-Path $bmpOutDir "$nombreSinExt.bmp"

        Write-Host "  $($img.Name) -> $nombreSinExt.bmp ... " -NoNewline

        $bmp = $null; $g = $null; $oldImg = $null
        try {
            $bmp = New-Object System.Drawing.Bitmap($Ancho, $Alto, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
            $g = [System.Drawing.Graphics]::FromImage($bmp)
            $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
            $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
            $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
            $g.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality

            $oldImg = [System.Drawing.Image]::FromFile($img.FullName)
            $g.DrawImage($oldImg, 0, 0, $Ancho, $Alto)
            $oldImg.Dispose()
            $g.Dispose()

            if ($AplicarDithering) { Convert-ToRGB565Dithered $bmp }

            $bmp.Save($finalPath, [System.Drawing.Imaging.ImageFormat]::Bmp)
            $bmp.Dispose()

            Write-Host "[OK]" -ForegroundColor Green
            $contadorExito++
        } catch {
            Write-Host "[ERROR]" -ForegroundColor Red
            Write-Host "    Detalle: $_" -ForegroundColor Yellow
            if ($null -ne $oldImg) { $oldImg.Dispose() }
            if ($null -ne $g) { $g.Dispose() }
            if ($null -ne $bmp) { $bmp.Dispose() }
        }
    }

    Write-Host ""
    Write-Host "Imagenes convertidas: $contadorExito de $($imagenes.Count)" -ForegroundColor Green
    Write-Host "Carpeta lista en: $((Resolve-Path $bmpOutDir).Path)" -ForegroundColor White
    Write-Host "Recuerda: ejecuta la Opcion 3 para generar/actualizar el listado $SystemCode.txt" -ForegroundColor Yellow
}

# ============================================================
#   OPCION 3: GENERAR / AUDITAR LISTADOS
# ============================================================
function Invoke-GenerarListados {
    Write-Host ""
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host "  OPCION 3: GENERAR / AUDITAR LISTADOS" -ForegroundColor Cyan
    Write-Host "===================================================" -ForegroundColor Magenta

    $arcadeRoot = Get-ConfigValue "ArcadeRoot" "D:\Arcade" "Ruta de la carpeta Arcade (raiz)"

    $sistemas = Get-SystemFolders $arcadeRoot
    if ($sistemas.Count -eq 0) {
        Write-Host "No se encontraron subcarpetas de sistemas en $arcadeRoot" -ForegroundColor Red
        return
    }

    $elegidos = Select-Multiple $sistemas "Nombre" "Sistemas encontrados en Arcade (elige uno o varios):"
    if ($elegidos.Count -eq 0) { return }

    foreach ($sistema in $elegidos) {
        Invoke-AuditarListadoUnSistema -SystemCode $sistema.Nombre -SistemaDir $sistema.Ruta -ArcadeRoot $arcadeRoot
    }
}

# Detecta romsets a partir de .bmp y .gif en una carpeta, agrupando secuencias
# tipo mslug_01.gif / mslug_02.gif como un unico romset "mslug". Ignora el
# comodin "_default" (no se indexa: el ESP32 lo comprueba directo, sin pasar
# por el listado).
function Get-RomsetsDesdeCarpeta([string]$dir) {
    $romsets = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)

    $bmps = Get-ChildItem -Path $dir -File -Filter "*.bmp" -ErrorAction SilentlyContinue
    foreach ($f in $bmps) {
        $nombre = [System.IO.Path]::GetFileNameWithoutExtension($f.Name)
        if ($nombre -eq "_default") { continue }
        [void]$romsets.Add($nombre)
    }

    $gifs = Get-ChildItem -Path $dir -File -Filter "*.gif" -ErrorAction SilentlyContinue
    foreach ($f in $gifs) {
        $nombre = [System.IO.Path]::GetFileNameWithoutExtension($f.Name)
        if ($nombre -eq "_default") { continue }
        $base = $nombre -replace '_\d{2}$', ''
        [void]$romsets.Add($base)
    }

    return $romsets
}

function Invoke-AuditarListadoUnSistema {
    param([string]$SystemCode, [string]$SistemaDir, [string]$ArcadeRoot)

    Write-Host ""
    Write-Host "---------------------------------------------------" -ForegroundColor DarkCyan
    Write-Host "Sistema: $SystemCode" -ForegroundColor Cyan
    Write-Host "---------------------------------------------------" -ForegroundColor DarkCyan

    $romsetsEnDisco = Get-RomsetsDesdeCarpeta $SistemaDir
    Write-Host "Romsets con marquesina en disco (bmp o gif): $($romsetsEnDisco.Count)" -ForegroundColor Green

    $txtPath = Join-Path $ArcadeRoot "$SystemCode.txt"
    if (Test-Path $txtPath) {
        $lineasActuales = @(Get-Content -Path $txtPath | Where-Object { $_.Trim() -ne "" })
        $enIndice = [System.Collections.Generic.HashSet[string]]::new([string[]]$lineasActuales, [StringComparer]::OrdinalIgnoreCase)

        $huerfanos = @($enIndice | Where-Object { -not $romsetsEnDisco.Contains($_) })
        $sinIndexar = @($romsetsEnDisco | Where-Object { -not $enIndice.Contains($_) })

        if ($huerfanos.Count -gt 0) {
            Write-Host "En el listado pero SIN archivo (huerfanos): $($huerfanos.Count)" -ForegroundColor DarkYellow
            $huerfanos | ForEach-Object { Write-Host "    - $_" -ForegroundColor DarkYellow }
        }
        if ($sinIndexar.Count -gt 0) {
            Write-Host "Con archivo pero SIN indexar (colocados a mano): $($sinIndexar.Count)" -ForegroundColor DarkYellow
            $sinIndexar | ForEach-Object { Write-Host "    - $_" -ForegroundColor DarkYellow }
        }
        if ($huerfanos.Count -eq 0 -and $sinIndexar.Count -eq 0) {
            Write-Host "El listado ya esta al dia, no hace falta regenerarlo." -ForegroundColor Green
            return
        }
    } else {
        Write-Host "No existe todavia $SystemCode.txt (se generara desde cero)." -ForegroundColor Yellow
    }

    $confirmar = Read-Host "Generar/actualizar $SystemCode.txt con los $($romsetsEnDisco.Count) romsets detectados? (S/N)"
    if ($confirmar.Trim().ToUpper() -ne "S") {
        Write-Host "Cancelado, no se ha tocado el listado." -ForegroundColor Yellow
        return
    }

    $listadoFinal = [string[]]$romsetsEnDisco
    [array]::Sort($listadoFinal, [System.StringComparer]::OrdinalIgnoreCase)
    [System.IO.File]::WriteAllLines($txtPath, $listadoFinal, (New-Object System.Text.UTF8Encoding($false)))

    Write-Host "Listado actualizado: $txtPath" -ForegroundColor Green
}

# ============================================================
#   MENU PRINCIPAL
# ============================================================
do {
    Clear-Host
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host "     Retro Pixel LED - ReplayOS TOOLKIT v5.1" -ForegroundColor White
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host ""
    Write-Host "  1) Scrapear sistema(s) desde ROMS" -ForegroundColor White
    Write-Host "  2) Generar imagenes BMP (offline, desde cache)" -ForegroundColor White
    Write-Host "  3) Generar / auditar listados .txt" -ForegroundColor White
    Write-Host "  4) Salir" -ForegroundColor White
    Write-Host ""
    $opcion = Read-Host "Elige una opcion (1-4)"

    switch ($opcion) {
        "1" { Invoke-ScrapeSistemas }
        "2" { Invoke-GenerarImagenes }
        "3" { Invoke-GenerarListados }
        "4" { }
        Default { Write-Host "Opcion no valida." -ForegroundColor Red }
    }

    if ($opcion -ne "4") {
        Write-Host ""
        Read-Host "Presiona Enter para volver al menu principal"
    }
} while ($opcion -ne "4")

Write-Host ""
Write-Host "===================================================" -ForegroundColor Magenta
Write-Host "             PROCESO FINALIZADO" -ForegroundColor Green
Write-Host "===================================================" -ForegroundColor Magenta
