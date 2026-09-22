param(
    [string]$BuildDir = (Join-Path $PSScriptRoot "..\build"),
    [string[]]$Objects = @("1", "100", "1000", "10000"),
    [string]$OutputPath = (Join-Path $PSScriptRoot "..\artifacts\benchmark-orbit.json"),
    [switch]$Force
)

$ErrorActionPreference = "Stop"
$BuildDir = (Resolve-Path -LiteralPath $BuildDir).Path
$Executable = Join-Path $BuildDir "nadir_orbit_benchmark.exe"
if (-not (Test-Path -LiteralPath $Executable)) { throw "Benchmark executable not found: $Executable" }
if ((Test-Path -LiteralPath $OutputPath) -and -not $Force) {
    throw "Output already exists: $OutputPath (use -Force to replace it)"
}

$objectCounts = @()
foreach ($entry in $Objects) {
    foreach ($token in $entry.Split(',')) {
        try { $count = [int]::Parse($token.Trim(), [Globalization.CultureInfo]::InvariantCulture) } catch { throw "Invalid object count: $token" }
        if ($count -lt 1 -or $count -gt 100000) { throw "Object count must be between 1 and 100000: $count" }
        $objectCounts += $count
    }
}
$workloads = @()
foreach ($objectCount in $objectCounts | Select-Object -Unique) {
    $raw = & $Executable $objectCount
    if ($LASTEXITCODE -ne 0) { throw "Benchmark failed with exit code $LASTEXITCODE for $objectCount objects" }
    $fields = @{}
    foreach ($line in $raw) {
        if ($line -match '^([A-Z_]+)\s+(.+)$') { $fields[$Matches[1]] = $Matches[2] }
    }
    foreach ($required in @("OBJECTS", "SECONDS", "PROPAGATIONS_PER_SECOND")) {
        if (-not $fields.ContainsKey($required)) { throw "Benchmark output missing $required for $objectCount objects" }
    }
    $workloads += [ordered]@{
        objects = [int]$fields.OBJECTS
        propagator = "SGP4"
        minutes_since_epoch = 30.0
        seconds = [double]::Parse($fields.SECONDS, [Globalization.CultureInfo]::InvariantCulture)
        propagations_per_second = [double]::Parse($fields.PROPAGATIONS_PER_SECOND, [Globalization.CultureInfo]::InvariantCulture)
    }
}

$compiler_command = Get-Command cl.exe -ErrorAction SilentlyContinue
$compiler = if ($compiler_command) { (& $compiler_command.Source 2>&1 | Select-Object -First 1) -join "" } else { "unavailable" }
$cpu = Get-CimInstance Win32_Processor | Select-Object -First 1
$architecture = if ([Environment]::Is64BitOperatingSystem) { "x64" } else { "x86" }
$result = [ordered]@{
    schema = "nadir.orbit-benchmark.v2"
    utc = (Get-Date).ToUniversalTime().ToString("o")
    workloads = $workloads
    environment = [ordered]@{ os = [Environment]::OSVersion.VersionString; architecture = $architecture; cpu = $cpu.Name.Trim(); logical_processors = [Environment]::ProcessorCount; compiler = $compiler; executable = $Executable }
}

$parent = Split-Path -Parent $OutputPath
if (-not (Test-Path -LiteralPath $parent)) { New-Item -ItemType Directory -Path $parent | Out-Null }
$result | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $OutputPath -Encoding utf8
Write-Host "Benchmark result: $OutputPath"
