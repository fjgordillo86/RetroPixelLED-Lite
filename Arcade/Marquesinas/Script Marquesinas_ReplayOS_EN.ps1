<#
.SYNOPSIS
    RetroPixelLED - ReplayOS Toolkit v5.1
.DESCRIPTION
    Unified tool for preparing arcade marquees for RetroPixelLED-Lite
    when the frontend is ReplayOS. Replaces the previous script (a single
    linear workflow) with a menu containing three independent operations:

      1) Scrape system(s): downloads raw resources (PNG/JPG/etc) from ArcadeDB or
         TheGamesDB to a reusable local CACHE, organized by system and resource
         type. Does not generate BMP files yet.

      2) Generate images: converts the already cached resources to 128x32 BMPs
         100% offline (without accessing the network), using Floyd-Steinberg RGB565
         dithering if the destination is ReplayOS. You can repeat this option
         as many times as you like to test different settings without downloading
         anything again.

      3) Generate / audit lists: scans Arcade/<system>/ for .bmp and .gif files
         (grouping sequences such as mslug_01.gif, mslug_02.gif... as a single
         romset) and builds or checks the <system>.txt file that the ESP32 uses
         to determine which romsets have a marquee. If a .txt already exists,
         it first warns about orphaned entries (listed but without a file) and
         unindexed files (manually added, typically when adding individual GIFs
         without going through the scraper).

#>

$Host.UI.RawUI.WindowTitle = "RetroPixelLED - ReplayOS Toolkit v5.0"
$OutputEncoding = [System.Text.Encoding]::UTF8
$ErrorActionPreference = "Stop"

Add-Type -AssemblyName System.Drawing

# ============================================================
#   PERSISTENT CONFIGURATION (default paths and API key)
# ============================================================
$ConfigPath = Join-Path $PSScriptRoot "RetroPixelLED_Config.json"

function Load-Config {
    if (Test-Path $ConfigPath) {
        try {
            $json = [System.IO.File]::ReadAllText($ConfigPath)
            $obj = $json | ConvertFrom-Json
            if ($null -ne $obj) { return $obj }
        } catch {
            Write-Host "Warning: could not read $ConfigPath, using default values." -ForegroundColor DarkYellow
        }
    }
    return [PSCustomObject]@{}
}

function Save-Config($cfg) {
    $json = $cfg | ConvertTo-Json -Depth 5
    [System.IO.File]::WriteAllText($ConfigPath, $json, (New-Object System.Text.UTF8Encoding($false)))
}

$script:Config = Load-Config

# Prompts for a value with a suggested default (from the config or, if none exists,
# a fixed value), and saves it to the config for the next run.
function Get-ConfigValue([string]$key, [string]$defaultValue, [string]$prompt) {
    $cfg = $script:Config
    $current = $null
    if ($cfg.PSObject.Properties.Name -contains $key) { $current = $cfg.$key }
    if ([string]::IsNullOrWhiteSpace($current)) { $current = $defaultValue }

    $entrada = Read-Host "$prompt (Press Enter to use: $current)"
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
#   MENU HELPERS
# ============================================================

# Multiple selection separated by commas (e.g. 1,3,5) or ALL. $labelProp specifies
# which property of each $items element is displayed as text (systems use "Nombre",
# resource types use "Name").
function Select-Multiple([array]$items, [string]$labelProp, [string]$promptTitle) {
    Write-Host ""
    Write-Host $promptTitle -ForegroundColor Cyan
    for ($i = 0; $i -lt $items.Count; $i++) {
        Write-Host ("  {0,2}) {1}" -f ($i + 1), $items[$i].$labelProp) -ForegroundColor White
    }
    Write-Host ""
    Write-Host "Enter numbers separated by commas (e.g. 1,3,5) or ALL" -ForegroundColor DarkGray
    $entrada = Read-Host "Selection"

    if ($entrada.Trim().ToUpper() -eq "ALL") {
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

# Lists the direct subfolders of a path as selectable items (Nombre/Ruta).
function Get-SystemFolders([string]$root) {
    if (-not (Test-Path $root)) { return @() }
    return Get-ChildItem -Path $root -Directory | Sort-Object Name | ForEach-Object {
        [PSCustomObject]@{ Nombre = $_.Name; Ruta = $_.FullName }
    }
}

# ============================================================
#   RESOURCE TABLE (ArcadeDB query_mame_media / TheGamesDB)
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
#   RGB565 DITHERING (Floyd-Steinberg) - identical to the previous script
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
#   TheGamesDB: normalizes the response to the same format used by ArcadeDB
#   ($item.NombreDelRecurso -> object with Url and Ext), so the main download
#   loop can handle both sources identically.
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
#   OPTION 1: SCRAPE SYSTEM(S)
# ============================================================
function Invoke-ScrapeSistemas {
    Write-Host ""
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host "  OPTION 1: SCRAPE SYSTEM(S)" -ForegroundColor Cyan
    Write-Host "===================================================" -ForegroundColor Magenta

    $romsRoot = Get-ConfigValue "RomsRoot" "D:\ROMS" "Path to the ReplayOS ROM folder"
    $cacheRoot = Get-ConfigValue "CacheRoot" (Join-Path $PSScriptRoot "Cache") "Path to the resource cache folder"

    $sistemas = Get-SystemFolders $romsRoot
    if ($sistemas.Count -eq 0) {
        Write-Host "No system subfolders were found in $romsRoot" -ForegroundColor Red
        return
    }

    $elegidos = Select-Multiple $sistemas "Nombre" "Systems found in ROMS (the exact folder name is the system code used by ReplayOS):"
    if ($elegidos.Count -eq 0) {
        Write-Host "No system was selected." -ForegroundColor Yellow
        return
    }

    Write-Host ""
    Write-Host "Select the download source:" -ForegroundColor Cyan
    Write-Host "  1) ArcadeDB / ArcadeItalia (Extended catalog - No API Key)" -ForegroundColor White
    Write-Host "  2) TheGamesDB (Requires API Key)" -ForegroundColor White
    $sourceOpt = Read-Host "Option (1 or 2)"

    $TheGamesDbApiKey = ""
    if ($sourceOpt -eq "2") {
        $Source = "TGDB"
        $TheGamesDbApiKey = Get-ConfigValue "TgdbApiKey" "" "TheGamesDB API Key"
        if ([string]::IsNullOrWhiteSpace($TheGamesDbApiKey)) {
            Write-Host "An API Key is required to use TheGamesDB." -ForegroundColor Red
            return
        }
    } else {
        $Source = "ArcadeDB"
    }

    $mediaSeleccionados = Select-Multiple $script:AllMediaTypes "Name" "Select the resource(s) to download (source: $Source):"
    if ($mediaSeleccionados.Count -eq 0) { $mediaSeleccionados = @($script:AllMediaTypes[0]) }

    $forzarOpt = Read-Host "Retry romsets previously marked as having no resources? (Y/N)"
    $forzarReintento = ($forzarOpt.Trim().ToUpper() -eq "Y")

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
    Write-Host "System: $SystemCode" -ForegroundColor Cyan
    Write-Host "---------------------------------------------------" -ForegroundColor DarkCyan

    $sistemaCacheDir = Join-Path $CacheRoot $SystemCode
    if (-not (Test-Path $sistemaCacheDir)) { New-Item -ItemType Directory -Path $sistemaCacheDir -Force | Out-Null }

    foreach ($m in $MediaList) {
        if ($Source -eq "ArcadeDB" -and -not $m.ArcadeKey) { continue }
        if ($Source -eq "TGDB" -and $m.TgdbKeys.Count -eq 0) { continue }
        $dirPath = Join-Path $sistemaCacheDir $m.FolderSuffix
        if (-not (Test-Path $dirPath)) { New-Item -ItemType Directory -Path $dirPath -Force | Out-Null }
    }

    # Romsets = filenames (without extension) of the .zip or .7z files in this system's ROM folder.
    # Common shared BIOS sets are excluded.
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

    Write-Host "Romsets found: $($games.Count)" -ForegroundColor Green
    if ($games.Count -eq 0) { return }

    # "No resources" cache: romsets that we already know return NOTHING from this
    # source, so we don't query them again on every run.
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
            Write-Host "[$count/$($games.Count)] [SKIPPED - previously no resources] $($game.romset)" -ForegroundColor DarkGray
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
                Write-Host "[$count/$($games.Count)] [EXISTS IN CACHE] [$($media.Name)] $($game.romset)" -ForegroundColor Gray
            } else {
                $neededMedia += $media
            }
        }
        if ($neededMedia.Count -eq 0) { continue }

        Write-Host "[$count/$($games.Count)] Querying: $($game.romset)" -ForegroundColor Cyan

        # Backoff: up to 3 attempts for ERRORS (network, timeout, etc.). A "truly" empty result
        # (without an exception) is NOT retried and is treated directly as NOT FOUND.
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
                    Write-Host "   [RETRY $intentos/$maxIntentos] $($game.romset): $_ (waiting ${espera}ms)" -ForegroundColor DarkYellow
                    Start-Sleep -Milliseconds $espera
                } else {
                    Write-Host "   [QUERY ERROR] $($game.romset): $_" -ForegroundColor Red
                    $errors.Add($game.romset)
                    $huboErrorFinal = $true
                }
            }
            if (-not $huboError) { break }
        }

        if ($null -eq $item) {
            if (-not $huboErrorFinal) {
                Write-Host "   [NOT FOUND] $($game.romset)" -ForegroundColor DarkYellow
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
                Write-Host "   [NO $label] $($game.romset)" -ForegroundColor DarkYellow
                $missing.Add("$($game.romset) [$label]")
                continue
            }

            $dest = Join-Path $dirPath "$($game.romset).$ext"
            try {
                Invoke-WebRequest -Uri $url.Trim() -OutFile $dest -TimeoutSec 90
                Write-Host "   [OK] [$label] $($game.romset).$ext" -ForegroundColor Green
            } catch {
                Write-Host "   [DOWNLOAD ERROR] [$label] $($game.romset): $_" -ForegroundColor Red
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
    Write-Host "Summary [$SystemCode]: $conRecursos of $($games.Count) romsets with data in $Source." -ForegroundColor Green
    Write-Host "Cache updated at: $sistemaCacheDir" -ForegroundColor White
}

# ============================================================
#   OPTION 2: GENERATE IMAGES (offline, from cache)
# ============================================================
function Invoke-GenerarImagenes {
    Write-Host ""
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host "  OPTION 2: GENERATE IMAGES (offline, from cache)" -ForegroundColor Cyan
    Write-Host "===================================================" -ForegroundColor Magenta

    $cacheRoot = Get-ConfigValue "CacheRoot" (Join-Path $PSScriptRoot "Cache") "Path to the resource cache folder"
    $arcadeRoot = Get-ConfigValue "ArcadeRoot" "D:\Arcade" "Path to the output Arcade folder (root, for copying to the SD card)"

    $sistemas = Get-SystemFolders $cacheRoot
    if ($sistemas.Count -eq 0) {
        Write-Host "No systems found in the cache ($cacheRoot). Run Option 1 first." -ForegroundColor Red
        return
    }

    $elegidos = Select-Multiple $sistemas "Nombre" "Systems available in cache (select one or more):"
    if ($elegidos.Count -eq 0) { return }

    $replayOpt = Read-Host "Does your Arcade use ReplayOS? (Y/N)"
    $esReplayOS = ($replayOpt.Trim().ToUpper() -eq "Y")
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
    Write-Host "System: $SystemCode" -ForegroundColor Cyan
    Write-Host "---------------------------------------------------" -ForegroundColor DarkCyan

    $imageExtRegex = '^\.(png|jpg|jpeg|bmp|gif)$'
    $carpetasRecurso = @(Get-ChildItem -Path $CacheDir -Directory -ErrorAction SilentlyContinue | Where-Object {
        (Get-ChildItem -Path $_.FullName -File -ErrorAction SilentlyContinue | Where-Object { $_.Extension -match $imageExtRegex }).Count -gt 0
    })

    if ($carpetasRecurso.Count -eq 0) {
        Write-Host "No cached images were found for this system." -ForegroundColor Yellow
        return
    }

    Write-Host ""
    Write-Host "Which resource do you want to use as the source for the marquees of [$SystemCode]?" -ForegroundColor Yellow
    for ($i = 0; $i -lt $carpetasRecurso.Count; $i++) {
        $n = (Get-ChildItem -Path $carpetasRecurso[$i].FullName -File | Where-Object { $_.Extension -match $imageExtRegex }).Count
        Write-Host ("  {0}) {1}  ({2} images)" -f ($i + 1), $carpetasRecurso[$i].Name, $n) -ForegroundColor White
    }
    $srcOpt = Read-Host "Select an option (1-$($carpetasRecurso.Count))"
    $srcIdx = 0
    if ($srcOpt -match '^\d+$' -and [int]$srcOpt -ge 1 -and [int]$srcOpt -le $carpetasRecurso.Count) {
        $srcIdx = [int]$srcOpt - 1
    }
    $marqueeDir = $carpetasRecurso[$srcIdx].FullName

    $bmpOutDir = Join-Path $ArcadeRoot $SystemCode
    if (-not (Test-Path $bmpOutDir)) { New-Item -ItemType Directory -Path $bmpOutDir -Force | Out-Null }

    $imagenes = Get-ChildItem -Path $marqueeDir -File | Where-Object { $_.Extension -match $imageExtRegex }
    Write-Host ""
    Write-Host "Converting $($imagenes.Count) images to ${Ancho}x${Alto} BMP..." -ForegroundColor Yellow

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
            Write-Host "    Details: $_" -ForegroundColor Yellow
            if ($null -ne $oldImg) { $oldImg.Dispose() }
            if ($null -ne $g) { $g.Dispose() }
            if ($null -ne $bmp) { $bmp.Dispose() }
        }
    }

    Write-Host ""
    Write-Host "Images converted: $contadorExito of $($imagenes.Count)" -ForegroundColor Green
    Write-Host "Output folder ready at: $((Resolve-Path $bmpOutDir).Path)" -ForegroundColor White
    Write-Host "Remember: run Option 3 to generate/update the $SystemCode.txt list" -ForegroundColor Yellow
}

# ============================================================
#   OPTION 3: GENERATE / AUDIT LISTS
# ============================================================
function Invoke-GenerarListados {
    Write-Host ""
    Write-Host "===================================================" -ForegroundColor Magenta
   Write-Host "  OPTION 3: GENERATE / AUDIT LISTS" -ForegroundColor Cyan
    Write-Host "===================================================" -ForegroundColor Magenta

    $arcadeRoot = Get-ConfigValue "ArcadeRoot" "D:\Arcade" "Path to the Arcade folder (root)"

    $sistemas = Get-SystemFolders $arcadeRoot
    if ($sistemas.Count -eq 0) {
        Write-Host "No system subfolders were found in $arcadeRoot" -ForegroundColor Red
        return
    }

    $elegidos = Select-Multiple $sistemas "Nombre" "Systems found in Arcade (select one or more):"
    if ($elegidos.Count -eq 0) { return }

    foreach ($sistema in $elegidos) {
        Invoke-AuditarListadoUnSistema -SystemCode $sistema.Nombre -SistemaDir $sistema.Ruta -ArcadeRoot $arcadeRoot
    }
}

# Detects romsets from .bmp and .gif files in a folder, grouping sequences
# such as mslug_01.gif / mslug_02.gif as a single romset "mslug". Ignores the
# "_default" wildcard (not indexed: the ESP32 checks it directly without going
# through the list).
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
    Write-Host "System: $SystemCode" -ForegroundColor Cyan
    Write-Host "---------------------------------------------------" -ForegroundColor DarkCyan

    $romsetsEnDisco = Get-RomsetsDesdeCarpeta $SistemaDir
    Write-Host "Romsets with a marquee on disk (bmp or gif): $($romsetsEnDisco.Count)" -ForegroundColor Green

    $txtPath = Join-Path $ArcadeRoot "$SystemCode.txt"
    if (Test-Path $txtPath) {
        $lineasActuales = @(Get-Content -Path $txtPath | Where-Object { $_.Trim() -ne "" })
        $enIndice = [System.Collections.Generic.HashSet[string]]::new([string[]]$lineasActuales, [StringComparer]::OrdinalIgnoreCase)

        $huerfanos = @($enIndice | Where-Object { -not $romsetsEnDisco.Contains($_) })
        $sinIndexar = @($romsetsEnDisco | Where-Object { -not $enIndice.Contains($_) })

        if ($huerfanos.Count -gt 0) {
            Write-Host "In the list but WITHOUT a file (orphans): $($huerfanos.Count)" -ForegroundColor DarkYellow
            $huerfanos | ForEach-Object { Write-Host "    - $_" -ForegroundColor DarkYellow }
        }
        if ($sinIndexar.Count -gt 0) {
            Write-Host "With a file but NOT indexed (manually added): $($sinIndexar.Count)" -ForegroundColor DarkYellow
            $sinIndexar | ForEach-Object { Write-Host "    - $_" -ForegroundColor DarkYellow }
        }
        if ($huerfanos.Count -eq 0 -and $sinIndexar.Count -eq 0) {
            Write-Host "The list is already up to date; it does not need to be regenerated." -ForegroundColor Green
            return
        }
    } else {
        Write-Host "$SystemCode.txt does not exist yet (it will be generated from scratch)." -ForegroundColor Yellow
    }

    $confirmar = Read-Host "Generate/update $SystemCode.txt with the $($romsetsEnDisco.Count) detected romsets? (Y/N)"
    if ($confirmar.Trim().ToUpper() -ne "Y") {
        Write-Host "Cancelled; the list was not modified." -ForegroundColor Yellow
        return
    }

    $listadoFinal = [string[]]$romsetsEnDisco
    [array]::Sort($listadoFinal, [System.StringComparer]::OrdinalIgnoreCase)
    [System.IO.File]::WriteAllLines($txtPath, $listadoFinal, (New-Object System.Text.UTF8Encoding($false)))

    Write-Host "List updated: $txtPath" -ForegroundColor Green
}

# ============================================================
#   MAIN MENU
# ============================================================
do {
    Clear-Host
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host "     RETROPIXELLED - REPLAYOS TOOLKIT v5.1" -ForegroundColor White
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host ""
    Write-Host "  1) Scrape system(s) from ROMS" -ForegroundColor White
    Write-Host "  2) Generate BMP images (offline, from cache)" -ForegroundColor White
    Write-Host "  3) Generate / audit .txt lists" -ForegroundColor White
    Write-Host "  4) Exit" -ForegroundColor White
    Write-Host ""
    $opcion = Read-Host "Choose an option (1-4)"

    switch ($opcion) {
        "1" { Invoke-ScrapeSistemas }
        "2" { Invoke-GenerarImagenes }
        "3" { Invoke-GenerarListados }
        "4" { }
        Default { Write-Host "Invalid option." -ForegroundColor Red }
    }

    if ($opcion -ne "4") {
        Write-Host ""
        Read-Host "Press Enter to return to the main menu"
    }
} while ($opcion -ne "4")

Write-Host ""
Write-Host "===================================================" -ForegroundColor Magenta
Write-Host "             PROCESS COMPLETED" -ForegroundColor Green
Write-Host "===================================================" -ForegroundColor Magenta
