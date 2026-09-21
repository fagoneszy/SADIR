param(
    [string]$BuildDir = (Join-Path $PSScriptRoot "..\build"),
    [ValidateRange(1, 100000)] [int]$Objects = 10000,
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

$raw = & $Executable $Objects
if ($LASTEXITCODE -ne 0) { throw "Benchmark failed with exit code $LASTEXITCODE" }
$fields = @{}
foreach ($line in $raw) {
    if ($line -match '^([A-Z_]+)\s+(.+)$') { $fields[$Matches[1]] = $Matches[2] }
}
foreach ($required in @("OBJECTS", "SECONDS", "PROPAGATIONS_PER_SECOND")) {
    if (-not $fields.ContainsKey($required)) { throw "Benchmark output missing $required" }
}

$compiler_command = Get-Command cl.exe -ErrorAction SilentlyContinue
$compiler = if ($compiler_command) { (& $compiler_command.Source 2>&1 | Select-Object -First 1) -join "" } else { "unavailable" }
$cpu = Get-CimInstance Win32_Processor | Select-Object -First 1
$architecture = if ([Environment]::Is64BitOperatingSystem) { "x64" } else { "x86" }
$result = [ordered]@{
    schema = "nadir.orbit-benchmark.v1"
    utc = (Get-Date).ToUniversalTime().ToString("o")
    workload = [ordered]@{ objects = [int]$fields.OBJECTS; propagator = "SGP4"; minutes_since_epoch = 30.0 }
    result = [ordered]@{ seconds = [double]::Parse($fields.SECONDS, [Globalization.CultureInfo]::InvariantCulture); propagations_per_second = [double]::Parse($fields.PROPAGATIONS_PER_SECOND, [Globalization.CultureInfo]::InvariantCulture) }
    environment = [ordered]@{ os = [Environment]::OSVersion.VersionString; architecture = $architecture; cpu = $cpu.Name.Trim(); logical_processors = [Environment]::ProcessorCount; compiler = $compiler; executable = $Executable }
}

$parent = Split-Path -Parent $OutputPath
if (-not (Test-Path -LiteralPath $parent)) { New-Item -ItemType Directory -Path $parent | Out-Null }
$result | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $OutputPath -Encoding utf8
Write-Host "Benchmark result: $OutputPath"
