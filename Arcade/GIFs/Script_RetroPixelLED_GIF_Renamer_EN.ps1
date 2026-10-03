<#
.SYNOPSIS
    RetroPixelLED - Arcade GIF Renamer v2.0
.DESCRIPTION
    Replaces the "blind" renaming of the previous script (which assumed that cleaning the
    human-readable name of the file already gave the MAME romset, which is almost never true)
    with resolution against an authoritative source:

      1) Rename GIFs: for each .gif, attempts to resolve its real MAME romset in
         this order of priority:
           a) Manual dictionary ($mapeoEspecial) - for already known/forced cases.
           b) MAME.dat (libretro-database) - EXACT search against the real title,
              trying several forms of the name (with spaces, without spaces, with
              sequence/decoration suffixes removed, with numbers as digits or
              as words).
           c) If approximate mode is enabled: best match by word overlap
              against the MAME.dat titles, with a minimum threshold.
         If it is not resolved by any method, the GIF is moved (without touching the name) to a
         SinResolver\ folder so that you can review it and rename it manually.

      2) Copy GIFs to system folders: given the ROMS folder and the Arcade folder,
         you choose a system (or several) and the script copies to Arcade/<sistema>/ the
         already renamed GIFs whose name matches a romset present in that system.

    DESIGN NOTES (consistent with the rest of the toolkit):
    - Without accents or special characters in any console text.
    - Uses the same RetroPixelLED_Config.json (ROMS/Arcade/cache paths) as the
      RetroPixelLED_ReplayOS_Toolkit.ps1, so you do not have to repeat paths.
    - MAME.dat is downloaded once to the cache and reused (an update can be forced). It is a DAT
      in ClrMamePro format maintained by the libretro/RetroArch team, not an official MAME source,
      but it is the most complete and accessible public source of "real title -> romset name"
      that exists.
#>

$Host.UI.RawUI.WindowTitle = "RetroPixelLED - GIF Renamer v2.0"
$OutputEncoding = [System.Text.Encoding]::UTF8
$ErrorActionPreference = "Stop"

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

function Select-Multiple([array]$items, [string]$labelProp, [string]$promptTitle) {
    Write-Host ""
    Write-Host $promptTitle -ForegroundColor Cyan
    for ($i = 0; $i -lt $items.Count; $i++) {
        Write-Host ("  {0,2}) {1}" -f ($i + 1), $items[$i].$labelProp) -ForegroundColor White
    }
    Write-Host ""
    Write-Host "Enter numbers separated by commas (e.g. 1,3,5) or ALL" -ForegroundColor DarkGray
    $entrada = Read-Host "Selection"

    if ($entrada.Trim().ToUpper() -eq "ALL") { return $items }

    $nums = $entrada -split "," | ForEach-Object { $_.Trim() } | Where-Object { $_ -match '^\d+$' }
    $seleccion = @()
    foreach ($n in $nums) {
        $idx = [int]$n - 1
        if ($idx -ge 0 -and $idx -lt $items.Count) { $seleccion += $items[$idx] }
    }
    return $seleccion
}

function Get-SystemFolders([string]$root) {
    if (-not (Test-Path $root)) { return @() }
    return Get-ChildItem -Path $root -Directory | Sort-Object Name | ForEach-Object {
        [PSCustomObject]@{ Nombre = $_.Name; Ruta = $_.FullName }
    }
}

# Returns an available "<baseName>.gif" filename in $carpeta, trying
# _01, _02... if needed. Checks the DISK directly (not a counter in
# memory), so it is safe to rerun the script on a folder that already contains
# GIFs from a previous run without conflicting with them. $rutaActual (optional)
# is the path of the file currently being renamed: if the available name
# found matches its own current path, it is considered available (it already
# has that name, so it is not a conflict with ANOTHER file).
function Get-NombreDisponible([string]$carpeta, [string]$baseName, [string]$rutaActual = "") {
    $candidato = "$baseName.gif"
    $candidatoPath = Join-Path $carpeta $candidato
    $sufijo = 0
    while ((Test-Path $candidatoPath) -and ($candidatoPath -ne $rutaActual)) {
        $sufijo++
        $candidato = "${baseName}_$($sufijo.ToString('D2')).gif"
        $candidatoPath = Join-Path $carpeta $candidato
    }
    return $candidato
}

# ============================================================
#   MANUAL DICTIONARY (priority 1, above MAME.dat)
# ============================================================
$mapeoEspecial = [ordered]@{
    "fatalfuryspecial|fatal_fury_special"       = "fatfursp"
    "fatalfury2|fatal_fury_2"                   = "fatfury2"
    "fatalfury3|fatal_fury_3"                   = "fatfury3"
    "fatalfury|fatal_fury"                      = "fatfury1"
    "real_bout_fatal_fury_special"              = "rbffspec"
    "real_bout_fatal_fury_2"                    = "rbff2"
    "real_bout_fatal_fury"                      = "rbff1"
    "garou"                                     = "garou"
    "dragon_ball_z_2|dragonballz2|dbz2"         = "dbz2"
    "dragon_ball_z_vr"                          = "dbzvr"
    "dragon_ball_z|dragonballz|dragon_ball|dbz"   = "dbz"
    "metal_slug_x|metalslugx"                   = "mslugx"
    "metal_slug_5|metalslug5"                   = "mslug5"
    "metal_slug_4|metalslug4"                   = "mslug4"
    "metal_slug_3|metalslug3"                   = "mslug3"
    "metal_slug_2|metalslug2"                   = "mslug2"
    "metal_slug|metalslug"                      = "mslug"
    "kof_2003|kof2003"                          = "kof2003"
    "kof_2002|kof2002"                          = "kof2002"
    "kof_2001|kof2001"                          = "kof2001"
    "kof_2000|kof2000"                          = "kof2000"
    "kof_99|kof99"                              = "kof99"
    "kof_98|kof98"                              = "kof98"
    "kof_97|kof97"                              = "kof97"
    "kof_96|kof96"                              = "kof96"
    "kof_95|kof95"                              = "kof95"
    "kof_94|kof94"                              = "kof94"
    "street_fighter_alpha_3|sfa3"               = "sfa3"
    "street_fighter_alpha_2|sfa2"               = "sfa2"
    "street_fighter_alpha|sfa"                  = "sfa"
    "street_fighter_ii|streetfighter2|sf2"      = "sf2"
    "street_fighter|sf1"                        = "sf1"
    "cadillacs_and_dinosaurs|canddino"          = "dino"
    "captain_commando|captaincommando"          = "captcomm"
    "final_fight|finalfight"                    = "ffight"
    "ghosts_n_goblins|ghostsngoblins|ghosts.?n.?goblins" = "gng"
    "ghouls_n_ghosts|ghoulsnghosts|ghouls.?n.?ghosts"    = "ghouls"
    "golden_axe|goldenaxe"                      = "goldnaxe"
    "alien_vs_predator|alienvspredator"         = "avsp"
    "wonderboy_in_monsterland"                  = "wbml"
    "wonderboy"                                 = "wboy"
    "wonder_momo"                               = "wondrmom"
    "vulcan_venture"                            = "vulcvent"
    "willow"                                    = "willow"
    "xaind_sleena|xaindsleena"                  = "xsleena"
    "xevious"                                   = "xevious"
    "xexex"                                     = "xexex"

    # --- Added from the review of sin_resolver.txt (verified against MAME.dat) ---
    "streetfighteralpha2"                       = "sfa2"
    "streetfighterii|streetfighter_ii"          = "sf2"
    "aero_fighters_2|aerofighters2"             = "sonicwi2"
    "aggressors_of_dark_kombat|aggressorsofdarkkombat" = "aodk"
    "aof_2"                                     = "aof2"
    "aof_3"                                     = "aof3"
    "blazstar"                                  = "blazstar"
    "karnovs_revenge|karnovsrevenge"            = "karnovr"
    "lastblade2|last_blade_2"                   = "lastbld2"
    "magical_drop_3|magicaldrop3"               = "magdrop3"
    "moneypuzzleexchanger|money_puzzle_exchanger" = "miexchng"
    "neodriftout|neo_drift_out"                 = "neodrift"
    "neoturfmasters|neo_turf_masters"           = "turfmast"
    "ninja_masters|ninjamasters"                = "ninjamas"
    "pleasuregoal|pleasure_goal"                = "pgoal"
    "progear"                                   = "progear"
    "samurai_shodown_4|samuraishodown4"         = "samsho4"
    "savage_reign|savagereign"                  = "savagere"
    "kizuna_encounter|kizunaencounter"          = "kizuna"
    "prehistoric_isle_ii|prehistoricisle2|prehistoric_isle_2" = "preisle2"
    "rage of the dragons|rage_of_the_dragons|rageofthedragons" = "rotd"
    "shock_troopers|shocktroopers"              = "shocktro"
    "super dodge ball|super_dodge_ball|superdodgeball" = "sdodgeb"
    "sengoku3|sengoku_3"                        = "sengoku3"
    "streethoop|street_hoop"                    = "strhoop"
    "superspy"                                  = "superspy"
    "voltage_fighter_gowcaizer|gowcaizer"       = "gowcaizr"
    "windjammers"                               = "wjammers"
    "night_warriors|nightwarriors"              = "dstlk"
    "onna_sansirou|onnasansirou"                = "onna34ro"
    "pacmancatch|pacmanwaiting"                 = "pacman"
    "pac.?man.?25th.?anniversary"               = "25pacman"
    "ms_pac|mspacman"                           = "mspacman"
    "parodius"                                  = "parodius"
    "pengoback|pengo_back"                      = "pengo"
    "pocketfighters|pocket_fighter"             = "pfghtj"
    "^arcade_pong_"                             = "pong"
    "^arcade_pow\.gif$"                         = "pow"
    "punisher"                                  = "punisher"
    "qbertingame|qbertleft|qbertright"          = "qbert"
    "rainbow_islands|rainbowislands"            = "rainbow"
    "robotronwilliams"                          = "robotron"
    "rockman2|rockman_2"                        = "megaman2"
    "rygarloading|rygar_03_loading"             = "rygar"
    "sailormoon|sailor_moon|pretty_solider_sailor_moon" = "sailormn"
    "^arcade_sci\.gif$"                         = "sci"
    "shadow_dancer|shadowdancer"                = "shdancer"
    "shinobi"                                   = "shinobi"
    "tetris_atari|tetrisatari"                  = "atetris"
    "^arcade_tophunter\.gif$"                   = "tophuntr"
    "track_and_field|trackandfield"             = "trackfld"
    "vampiresavior2|vampire_savior_2"           = "vsav2"
    "xmenvsstreet|xmen_vs_street"               = "xmvsf"
    "xmencota|xmen_cota"                        = "xmcota"
    "froggercar"                                = "frogger"
    "mrdocastle|mr_do_castle"                   = "docastle"
    "machbreakers|mach_breakers"                = "machbrkr"
    "mappy_goro"                                = "mappy"
    "capcomsportsclub"                          = "csclub"
    "capcom_vs_snk|capcomvssnk"                 = "capsnk"
    "captain_america|captainamerica"            = "captaven"
    "combattribes|combatribes"                  = "ctribe"
    "donpachipowerup|donpachiship"              = "donpachi"
    "espradeplayers|espradepowerup"             = "esprade"
    "gradius_ii|gradiusii"                      = "gradius2"
    "guiltygearxx|guilty_gear_xx"               = "ggxx"
    "junofirstscore|juno_first"                 = "junofrst"
    "matrimelee"                                = "matrim"
    "mazinger"                                  = "mazinger"
    "terminator2|terminator_2"                  = "term2"
    "timepilot|time_pilot"                      = "timeplt"
    "vigilante"                                 = "vigilant"
}

# ============================================================
#   MAME.dat: download/cache + parsing into a title->romset dictionary
# ============================================================
$MameDatUrl = "https://raw.githubusercontent.com/libretro/libretro-database/master/metadat/mame/MAME.dat"

# Words that penalize an entry as the "main" candidate for a title
# (conversions to other hardware, unofficial sets...), and words that
# favor it (standard regions). Used only to decide which romset to associate
# when several MAME.dat entries share the same cleaned title.
$PalabrasMalas = @("mega-tech", "mega-play", "playchoice", "nintendo vs", "vs. system", "bootleg", "hack", "prototype")
$PalabrasBuenas = @("world", " usa", " us)", " u)", "europe", "euro")

$NumerosPalabra = @{
    "1" = "one"; "2" = "two"; "3" = "three"; "4" = "four"; "5" = "five"
    "6" = "six"; "7" = "seven"; "8" = "eight"; "9" = "nine"
}

function Get-NormalizedSpaced([string]$t) {
    $t2 = $t -replace '\s*\([^)]*\)\s*', ' '
    $t2 = $t2.ToLower() -replace '[^a-z0-9 ]', ''
    $t2 = $t2 -replace '\s+', ' '
    return $t2.Trim()
}

function Get-NormalizedConcat([string]$t) {
    return (Get-NormalizedSpaced $t) -replace ' ', ''
}

function Get-CandidateScore([string]$originalTitle, [string]$romset) {
    $low = $originalTitle.ToLower()
    $tieneParentesis = $originalTitle.Contains("(")
    $esMalo = $false
    foreach ($p in $PalabrasMalas) { if ($low.Contains($p)) { $esMalo = $true; break } }
    $esBueno = $false
    foreach ($p in $PalabrasBuenas) { if ($low.Contains($p)) { $esBueno = $true; break } }

    if ($esMalo) { $tier = 3 }
    elseif (-not $tieneParentesis) { $tier = 0 }
    elseif ($esBueno) { $tier = 1 }
    else { $tier = 2 }

    # Combined score: tier first, romset length as the tiebreaker.
    return ($tier * 1000) + $romset.Length
}

function Get-MameDat([string]$cacheRoot) {
    $mameDir = Join-Path $cacheRoot "_mame"
    if (-not (Test-Path $mameDir)) { New-Item -ItemType Directory -Path $mameDir -Force | Out-Null }
    $datPath = Join-Path $mameDir "MAME.dat"

    $descargar = $true
    if (Test-Path $datPath) {
        $actualizar = Read-Host "MAME.dat already exists in the cache. Download an updated version? (Y/N)"
        $descargar = ($actualizar.Trim().ToUpper() -eq "Y")
    }

    if ($descargar) {
        Write-Host "Downloading MAME.dat (libretro-database, may take a few seconds)..." -ForegroundColor Cyan
        try {
            Invoke-WebRequest -Uri $MameDatUrl -OutFile $datPath -TimeoutSec 120
            Write-Host "Download completed." -ForegroundColor Green
        } catch {
            if (Test-Path $datPath) {
                Write-Host "Could not download; using the existing cached copy." -ForegroundColor DarkYellow
            } else {
                throw "Could not download MAME.dat and no cached copy exists: $_"
            }
        }
    }

    return $datPath
}

# Parses MAME.dat and returns two hashtables: one for titles "with spaces" and
# one for titles "without spaces" (concatenated), each mapping normalized title -> romset.
function Build-MameIndex([string]$datPath) {
    Write-Host "Parsing MAME.dat (this may take a moment)..." -ForegroundColor Cyan
    $texto = [System.IO.File]::ReadAllText($datPath)

    $patron = '(?s)game\s*\(\s*name\s+"([^"]+)".*?rom\s*\(\s*name\s+(\S+?)\.zip'
    $coincidencias = [regex]::Matches($texto, $patron)

    $porEspacio = @{}
    $puntuacionEspacio = @{}
    $porConcat = @{}
    $puntuacionConcat = @{}

    foreach ($m in $coincidencias) {
        $titulo = $m.Groups[1].Value
        $romset = $m.Groups[2].Value
        $puntos = Get-CandidateScore $titulo $romset

        $ks = Get-NormalizedSpaced $titulo
        if ($ks) {
            if ((-not $porEspacio.ContainsKey($ks)) -or $puntos -lt $puntuacionEspacio[$ks]) {
                $porEspacio[$ks] = $romset
                $puntuacionEspacio[$ks] = $puntos
            }
        }

        $kc = Get-NormalizedConcat $titulo
        if ($kc) {
            if ((-not $porConcat.ContainsKey($kc)) -or $puntos -lt $puntuacionConcat[$kc]) {
                $porConcat[$kc] = $romset
                $puntuacionConcat[$kc] = $puntos
            }
        }
    }

    Write-Host "MAME.dat: $($coincidencias.Count) romsets, $($porEspacio.Count) unique titles." -ForegroundColor Green
    return [PSCustomObject]@{ PorEspacio = $porEspacio; PorConcat = $porConcat }
}

# ============================================================
#   GENERATING CANDIDATES FROM THE FILENAME
# ============================================================
$regexFirmas = "(_RattenJager|_RattenJagger|_RatteJager|_clivefrog|_wolfsoft|_Wolfsoft|_shabazz|_Shabazz|_2vinci|_hellbent|_raph|_vybyvy|_nicko51|_zzoomm|_sephirot68|_Qris|_Venoim|_Mitchbucannon|_INVERT|_SCROLL|_Div\d+|_DIV\d+)+$"
$palabrasDecorativas = "Title|Go|Scroll|Ending|Boss\d*|Intro|Demo|Attract|Continue|GameOver|Start|Select|Stage"

function ConvertTo-CamelSpaced([string]$s) {
    return [regex]::Replace($s, '(?<=[a-z0-9])(?=[A-Z])', ' ')
}

function Get-CandidatosTitulo([string]$baseName) {
    $n = $baseName -replace '^ARCADE_NEOGEO_', '' -replace '^ARCADE_', ''
    $n = $n -replace $regexFirmas, ''

    $candidatosCrudos = [System.Collections.Generic.List[string]]::new()
    $candidatosCrudos.Add($n)                                                          # as-is
    $candidatosCrudos.Add(($n -replace '(_?\d{1,2})$', ''))                             # without final numeric suffix
    $candidatosCrudos.Add(($n -replace "(_?($palabrasDecorativas)\d*)$", ''))  # without decorative word + digits (case-insensitive by default)
    $candidatosCrudos.Add(($n -replace '(_?\d{1,2}[A-Za-z]+)$', ''))                     # without trailing digits+letters (e.g. 04Go)

    $resultado = [System.Collections.Generic.List[string]]::new()
    foreach ($c in $candidatosCrudos) {
        if ([string]::IsNullOrWhiteSpace($c)) { continue }
        $espaciado = ConvertTo-CamelSpaced ($c -replace '_', ' ')
        if (-not $resultado.Contains($espaciado)) { [void]$resultado.Add($espaciado) }

        # Variant with the first number (1-9) replaced by its English word,
        # for titles such as "3 Wonders" -> "Three Wonders"
        if ($espaciado -match '^(\d)(\s|$)') {
            $digito = $matches[1]
            if ($NumerosPalabra.ContainsKey($digito)) {
                $conPalabra = $espaciado -replace "^$digito", $NumerosPalabra[$digito]
                if (-not $resultado.Contains($conPalabra)) { [void]$resultado.Add($conPalabra) }
            }
        }
    }
    return $resultado
}

# Searches the MAME.dat index (spaced and concatenated) for each candidate,
# in order. Returns the romset or $null.
function Resolve-ExactoMame($candidatos, $indiceMame) {
    foreach ($c in $candidatos) {
        $ks = Get-NormalizedSpaced $c
        if ($ks -and $indiceMame.PorEspacio.ContainsKey($ks)) { return $indiceMame.PorEspacio[$ks] }
        $kc = Get-NormalizedConcat $c
        if ($kc -and $indiceMame.PorConcat.ContainsKey($kc)) { return $indiceMame.PorConcat[$kc] }
    }
    return $null
}

# Approximate matching: word overlap against the "spaced" titles in MAME.dat.
# Threshold: at least 70% of words in common (relative to the longer of the two),
# and at least 2 matching words unless the candidate contains only one word.
function Resolve-AproximadoMame([string]$candidatoPrincipal, $indiceMame) {
    $ks = Get-NormalizedSpaced $candidatoPrincipal
    if (-not $ks) { return $null }
    $palabrasCand = $ks -split ' ' | Where-Object { $_ -ne '' }
    if ($palabrasCand.Count -eq 0) { return $null }

    $mejorScore = 0.0
    $mejorRomset = $null

    foreach ($clave in $indiceMame.PorEspacio.Keys) {
        $palabrasClave = $clave -split ' ' | Where-Object { $_ -ne '' }
        if ($palabrasClave.Count -eq 0) { continue }

        $comunes = 0
        foreach ($pw in $palabrasCand) { if ($palabrasClave -contains $pw) { $comunes++ } }
        if ($comunes -eq 0) { continue }
        if ($palabrasCand.Count -gt 1 -and $comunes -lt 2) { continue }

        $maxLen = [Math]::Max($palabrasCand.Count, $palabrasClave.Count)
        $sc = $comunes / $maxLen
        if ($sc -gt $mejorScore) {
            $mejorScore = $sc
            $mejorRomset = $indiceMame.PorEspacio[$clave]
        }
    }

    if ($mejorScore -ge 0.7) { return $mejorRomset }
    return $null
}

# ============================================================
#   OPTION 1: RENAME GIFS
# ============================================================
function Invoke-RenombrarGifs {
    Write-Host ""
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host "  OPTION 1: RENAME GIFS" -ForegroundColor Cyan
    Write-Host "===================================================" -ForegroundColor Magenta

    $cacheRoot = Get-ConfigValue "CacheRoot" (Join-Path $PSScriptRoot "Cache") "Cache folder path (for MAME.dat)"
    $origenPath = Get-ConfigValue "GifSourceRoot" "" "Path to the folder containing the GIFs to rename"

    if (-not (Test-Path -Path $origenPath -PathType Container)) {
        Write-Host "Error: the specified path does not exist or is not a valid folder." -ForegroundColor Red
        return
    }

    $modoOpt = Read-Host "Resolution mode: 1) EXACT MATCH only  2) Exact + APPROXIMATE (checks more cases, with some risk)"
    $usarAproximada = ($modoOpt.Trim() -eq "2")

    $datPath = Get-MameDat $cacheRoot
    $indiceMame = Build-MameIndex $datPath

    # --- System/company logo detection and classification ---
    $patronesLogos = @(
        "acclaim", "activision", "atari", "bust_a_move", "capcom", "defender", "irem",
        "namco", "neogeocd", "neogeo_logonb", "neogeo_logosnk", "neogeo_mvs-aes", "neogeo_snk",
        "pocketfighterscapcom", "raizing", "rene_pierre", "rygar", "segaastro", "spaceinvaders",
        "virtua_racing", "williamsdefender"
    )
    $carpetaLogos = Join-Path -Path $origenPath -ChildPath "logo"
    $carpetaSinResolver = Join-Path -Path $origenPath -ChildPath "SinResolver"

    $sinResolver = [System.Collections.Generic.List[string]]::new()
    $archivosGIF = Get-ChildItem -Path $origenPath -Filter "*.gif" | Sort-Object Name

    Write-Host ""
    Write-Host "Processing $($archivosGIF.Count) GIFs..." -ForegroundColor Cyan

    $numLogos = 0
    $numExacta = 0
    $numManual = 0
    $numAproximada = 0
    $numSinResolver = 0

    foreach ($archivo in $archivosGIF) {
        $nombreMin = $archivo.Name.ToLower()

        # STEP 1: company/system logos
        $esLogoSystem = $false
        $nombreEmpresa = $null
        if ($nombreMin -like "arcade_logo_*") {
            $esLogoSystem = $true
            $baseLogo = $archivo.BaseName -replace "^ARCADE_LOGO_", ""
            $baseLogoLimpia = $baseLogo -replace $regexFirmas, ""
            if ($baseLogoLimpia -like "*pacman*") {
                $esLogoSystem = $false
            } else {
                $nombreEmpresa = $baseLogoLimpia.ToLower()
            }
        }

        if ($esLogoSystem) {
            if (-not (Test-Path -Path $carpetaLogos)) { New-Item -ItemType Directory -Path $carpetaLogos | Out-Null }
            $nuevoNombreLogo = Get-NombreDisponible $carpetaLogos "logo_${nombreEmpresa}"
            $destinoRuta = Join-Path -Path $carpetaLogos -ChildPath $nuevoNombreLogo
            Move-Item -Path $archivo.FullName -Destination $destinoRuta -Force
            Write-Host "[LOGO] '$($archivo.Name)' -> 'logo\$nuevoNombreLogo'" -ForegroundColor Yellow
            $numLogos++
            continue
        }

        # STEP 2: resolve the actual romset
        $romNameTarget = $null
        $origenResolucion = $null

        foreach ($patron in $mapeoEspecial.Keys) {
            if ($nombreMin -match $patron) {
                $romNameTarget = $mapeoEspecial[$patron]
                $origenResolucion = "MANUAL"
                break
            }
        }

        if (-not $romNameTarget) {
            $candidatos = Get-CandidatosTitulo $archivo.BaseName
            $romNameTarget = Resolve-ExactoMame $candidatos $indiceMame
            if ($romNameTarget) { $origenResolucion = "EXACTA" }
        }

        if ((-not $romNameTarget) -and $usarAproximada) {
            $candidatos = Get-CandidatosTitulo $archivo.BaseName
            $romNameTarget = Resolve-AproximadoMame $candidatos[0] $indiceMame
            if ($romNameTarget) { $origenResolucion = "APROXIMADA" }
        }

        if (-not $romNameTarget) {
            if (-not (Test-Path -Path $carpetaSinResolver)) { New-Item -ItemType Directory -Path $carpetaSinResolver | Out-Null }
            $destinoRuta = Join-Path -Path $carpetaSinResolver -ChildPath $archivo.Name
            Move-Item -Path $archivo.FullName -Destination $destinoRuta -Force
            Write-Host "[UNRESOLVED] '$($archivo.Name)' -> moved to SinResolver\ for manual review" -ForegroundColor Red
            $sinResolver.Add($archivo.Name)
            $numSinResolver++
            continue
        }

        switch ($origenResolucion) {
            "MANUAL"     { $numManual++ }
            "EXACTA"     { $numExacta++ }
            "APROXIMADA" { $numAproximada++ }
        }

        $nuevoNombre = Get-NombreDisponible $origenPath $romNameTarget $archivo.FullName

        if ($archivo.Name -ne $nuevoNombre) {
            Rename-Item -Path $archivo.FullName -NewName $nuevoNombre -Force
            $etiqueta = "[$origenResolucion]"
            Write-Host "$etiqueta '$($archivo.Name)' -> '$nuevoNombre'" -ForegroundColor Green
        }
    }

    if ($sinResolver.Count -gt 0) {
        $sinResolver | Out-File -FilePath (Join-Path $origenPath "sin_resolver.txt") -Encoding utf8
    }

    Write-Host ""
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host "Summary:" -ForegroundColor White
    Write-Host "  Logos moved:               $numLogos" -ForegroundColor Yellow
    Write-Host "  Resolved (manual):         $numManual" -ForegroundColor Green
    Write-Host "  Resolved (exact):          $numExacta" -ForegroundColor Green
    Write-Host "  Resolved (approximate):    $numAproximada" -ForegroundColor Green
    Write-Host "  Unresolved (review):       $numSinResolver" -ForegroundColor Red
    Write-Host "===================================================" -ForegroundColor Magenta
    if ($numSinResolver -gt 0) {
        Write-Host "Check the SinResolver\ folder and sin_resolver.txt" -ForegroundColor Yellow
    }
}

# ============================================================
#   OPTION 2: COPY RENAMED GIFS TO SYSTEM FOLDERS
# ============================================================
function Invoke-CopiarGifsASistemas {
    Write-Host ""
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host "  OPTION 2: COPY GIFS TO SYSTEM FOLDERS" -ForegroundColor Cyan
    Write-Host "===================================================" -ForegroundColor Magenta

    $romsRoot = Get-ConfigValue "RomsRoot" "D:\ROMS" "Path to the ReplayOS ROMS folder"
    $gifSourceRoot = Get-ConfigValue "GifSourceRoot" "" "Path to the folder containing the renamed GIFs"

    Write-Host ""
    Write-Host "Which system are these GIFs for?" -ForegroundColor Cyan
    Write-Host "  1) ReplayOS (RetroPixelLED SD card)" -ForegroundColor White
    Write-Host "  2) Batocera (marquees/Arcade on the network share)" -ForegroundColor White
    Write-Host "  3) Recalbox (marquees/Arcade on the network share)" -ForegroundColor White
    $destinoOpt = Read-Host "Option (1-3)"

    switch ($destinoOpt) {
        "2" {
            $arcadeRoot = Get-ConfigValue "ArcadeRoot_Batocera" "\\BATOCERA\roms\marquesinas\Arcade" "Batocera marquee/Arcade path (network share)"
        }
        "3" {
            $arcadeRoot = Get-ConfigValue "ArcadeRoot_Recalbox" "\\RECALBOX\share\marquesinas\Arcade" "Recalbox marquee/Arcade path (network share)"
        }
        Default {
            $arcadeRoot = Get-ConfigValue "ArcadeRoot" "D:\Arcade" "Destination Arcade folder path (ReplayOS SD card)"
        }
    }

    if (-not (Test-Path -Path $gifSourceRoot -PathType Container)) {
        Write-Host "Error: the GIF folder does not exist." -ForegroundColor Red
        return
    }

    $sistemas = Get-SystemFolders $romsRoot
    if ($sistemas.Count -eq 0) {
        Write-Host "No system subfolders were found in $romsRoot" -ForegroundColor Red
        return
    }

    $elegidos = Select-Multiple $sistemas "Nombre" "Systems found in ROMS (choose which ones to copy GIFs to):"
    if ($elegidos.Count -eq 0) { return }

    $gifsDisponibles = Get-ChildItem -Path $gifSourceRoot -Filter "*.gif" -File

    foreach ($sistema in $elegidos) {
        Write-Host ""
        Write-Host "---------------------------------------------------" -ForegroundColor DarkCyan
        Write-Host "System: $($sistema.Nombre)" -ForegroundColor Cyan
        Write-Host "---------------------------------------------------" -ForegroundColor DarkCyan

        $romsets = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
        Get-ChildItem -Path $sistema.Ruta -File -Filter "*.zip" -ErrorAction SilentlyContinue | ForEach-Object {
            [void]$romsets.Add([System.IO.Path]::GetFileNameWithoutExtension($_.Name))
        }

        if ($romsets.Count -eq 0) {
            Write-Host "No .zip romsets were found in this system." -ForegroundColor Yellow
            continue
        }

        $destinoDir = Join-Path $arcadeRoot $sistema.Nombre
        if (-not (Test-Path $destinoDir)) { New-Item -ItemType Directory -Path $destinoDir -Force | Out-Null }

        $copiados = 0
        foreach ($gif in $gifsDisponibles) {
            $baseRomset = $gif.BaseName -replace '_\d{2}$', ''
            if ($romsets.Contains($baseRomset)) {
                Copy-Item -Path $gif.FullName -Destination (Join-Path $destinoDir $gif.Name) -Force
                Write-Host "  [COPIED] $($gif.Name) -> $($sistema.Nombre)\" -ForegroundColor Green
                $copiados++
            }
        }

        Write-Host "GIFs copied to [$($sistema.Nombre)]: $copiados" -ForegroundColor White
        if ($copiados -gt 0) {
            Write-Host "Remember: run Option 3 of the main toolkit to update $($sistema.Nombre).txt" -ForegroundColor Yellow
        }
    }
}

# ============================================================
#   MAIN MENU
# ============================================================
do {
    Clear-Host
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host "   RETROPIXELLED - GIF RENAMER v2.0" -ForegroundColor White
    Write-Host "===================================================" -ForegroundColor Magenta
    Write-Host ""
    Write-Host "  1) Rename GIFs (MAME.dat + manual dictionary)" -ForegroundColor White
    Write-Host "  2) Copy renamed GIFs to system folders" -ForegroundColor White
    Write-Host "  3) Exit" -ForegroundColor White
    Write-Host ""
    $opcion = Read-Host "Choose an option (1-3)"

    switch ($opcion) {
        "1" { Invoke-RenombrarGifs }
        "2" { Invoke-CopiarGifsASistemas }
        "3" { }
        Default { Write-Host "Invalid option." -ForegroundColor Red }
    }

    if ($opcion -ne "3") {
        Write-Host ""
        Read-Host "Press Enter to return to the main menu"
    }
} while ($opcion -ne "3")

Write-Host ""
Write-Host "===================================================" -ForegroundColor Magenta
Write-Host "             PROCESS COMPLETED" -ForegroundColor Green
Write-Host "===================================================" -ForegroundColor Magenta
