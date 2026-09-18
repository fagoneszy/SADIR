param(
    [string]$Root = "C:\Users\faguinho\Documents\SADIR\NADIR"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Stop-Gate {
    param([string]$Message)
    Write-Host ""
    Write-Host "GATE FAILED: $Message"
    exit 1
}

function Require-Command {
    param([string]$Name)
    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        Stop-Gate "Required command not found: $Name"
    }
}

function Invoke-Step {
    param(
        [string]$Name,
        [scriptblock]$Command
    )
    Write-Host ""
    Write-Host "=== $Name ==="
    & $Command
    if ($LASTEXITCODE -ne 0) {
        Stop-Gate "$Name returned exit code $LASTEXITCODE"
    }
}

function Resolve-NadirExecutable {
    param([string]$BuildDir)
    $Candidates = @(
        (Join-Path $BuildDir "nadir.exe"),
        (Join-Path $BuildDir "Release\nadir.exe")
    )
    foreach ($Candidate in $Candidates) {
        if (Test-Path -LiteralPath $Candidate) {
            return (Resolve-Path -LiteralPath $Candidate).Path
        }
    }
    $Found = Get-ChildItem -Path $BuildDir -Filter "nadir.exe" -File -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($null -eq $Found) {
        Stop-Gate "nadir.exe was not found after the build"
    }
    return $Found.FullName
}

function Invoke-Smoke {
    param(
        [string]$Exe,
        [string]$Name,
        [string[]]$Arguments,
        [bool]$Core,
        [bool]$Remote
    )

    Write-Host ""
    Write-Host "--- SMOKE: $Name ---"

    $Output = @()
    $ExitCode = 0

    try {
        $Output = & $Exe @Arguments 2>&1
        $ExitCode = $LASTEXITCODE
    }
    catch {
        $Output = @($_.Exception.Message)
        $ExitCode = 1
    }

    $Text = ($Output | Out-String).Trim()
    if ($Text.Length -gt 0) {
        Write-Host $Text
    }

    $FailurePattern = '(?im)(unknown command|invalid command|parse error|parser error|fatal error|unhandled exception|access violation|segmentation fault|assertion failed)'
    $OfflinePattern = '(?im)(could not resolve|couldn''t resolve|name resolution|dns|connection refused|could not connect|couldn''t connect|network.*unreachable|timed out|timeout|offline|http\s*(408|429|500|502|503|504)|curl.*\([56728]\))'

    $Status = "PASS"

    if ($ExitCode -ne 0 -or $Text -match $FailurePattern) {
        if ($Remote -and $Text -match $OfflinePattern -and -not $Core) {
            $Status = "OFFLINE"
        }
        else {
            $Status = "FAIL"
        }
    }

    Write-Host "STATUS: $Status"

    [pscustomobject]@{
        Name = $Name
        Status = $Status
        Core = $Core
        Remote = $Remote
        ExitCode = $ExitCode
    }
}

function Write-Validation {
    param(
        [string]$Path,
        [string]$Status,
        [string]$ManifestStatus,
        [string]$SmokeNetwork
    )

    $Content = @"
# NADIR Validation

Baseline: 0.3.1
Status: $Status

Configuration:
Release

Generator:
Ninja

Compiler:
MSVC

Configure:
PASS

Build:
PASS

CTest:
PASS

Manifest:
$ManifestStatus

Smoke Core:
PASS

Smoke Network:
$SmokeNetwork

Migration:
COMPLETE
"@

    $Utf8 = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText($Path, $Content, $Utf8)
}

if (-not (Test-Path -LiteralPath $Root)) {
    Stop-Gate "Root directory does not exist: $Root"
}

$Root = (Resolve-Path -LiteralPath $Root).Path
Set-Location $Root

Require-Command "cmake"
Require-Command "ninja"
Require-Command "cl.exe"
Require-Command "git.exe"

$VersionPath = Join-Path $Root "VERSION"
if (-not (Test-Path -LiteralPath $VersionPath)) {
    Stop-Gate "VERSION file not found"
}

$Version = (Get-Content -LiteralPath $VersionPath -Raw).Trim()
if ($Version -ne "0.3.1") {
    Stop-Gate "Expected VERSION 0.3.1 but found $Version"
}

$BuildDir = Join-Path $Root "build"

if (Test-Path -LiteralPath $BuildDir) {
    Remove-Item -LiteralPath $BuildDir -Recurse -Force
}

Invoke-Step "CMake configure" {
    & cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
}

Invoke-Step "CMake build" {
    & cmake --build build
}

Write-Host ""
Write-Host "=== CTest ==="
$CTestOutput = & ctest --test-dir build --output-on-failure 2>&1
$CTestExit = $LASTEXITCODE
$CTestText = ($CTestOutput | Out-String).Trim()
Write-Host $CTestText

if ($CTestExit -ne 0) {
    Stop-Gate "CTest failed"
}

if ($CTestText -notmatch '(?im)100%\s+tests\s+passed') {
    Stop-Gate "CTest did not report 100% tests passed"
}

$Nadir = Resolve-NadirExecutable $BuildDir

$SmokeSpecs = @(
    @{ Name = "universe"; Args = @("universe"); Core = $true; Remote = $false },
    @{ Name = "time"; Args = @("time"); Core = $true; Remote = $false },
    @{ Name = "sources"; Args = @("sources"); Core = $true; Remote = $false },
    @{ Name = "presets"; Args = @("presets"); Core = $true; Remote = $false },
    @{ Name = "eop live"; Args = @("eop", "live"); Core = $true; Remote = $true },
    @{ Name = "orbit live starlink"; Args = @("orbit", "live", "starlink"); Core = $false; Remote = $true },
    @{ Name = "orbit live gps"; Args = @("orbit", "live", "gps"); Core = $false; Remote = $true },
    @{ Name = "orbit live stations"; Args = @("orbit", "live", "stations"); Core = $false; Remote = $true },
    @{ Name = "target live Moon"; Args = @("target", "live", "Moon"); Core = $false; Remote = $true },
    @{ Name = "target live Europa"; Args = @("target", "live", "Europa"); Core = $false; Remote = $true },
    @{ Name = "target live Mars"; Args = @("target", "live", "Mars"); Core = $false; Remote = $true },
    @{ Name = "seismic hour 20"; Args = @("seismic", "hour", "20"); Core = $false; Remote = $true },
    @{ Name = "fireballs 10"; Args = @("fireballs", "10"); Core = $false; Remote = $true },
    @{ Name = "craft Starlink V3"; Args = @("craft", "Starlink", "V3"); Core = $false; Remote = $false },
    @{ Name = "craft Webb JWST"; Args = @("craft", "Webb", "JWST"); Core = $false; Remote = $false },
    @{ Name = "craft Voyager V1V2"; Args = @("craft", "Voyager", "V1V2"); Core = $false; Remote = $false },
    @{ Name = "station"; Args = @("station"); Core = $false; Remote = $false }
)

$SmokeResults = @()

foreach ($Spec in $SmokeSpecs) {
    $Result = Invoke-Smoke `
        -Exe $Nadir `
        -Name $Spec.Name `
        -Arguments $Spec.Args `
        -Core $Spec.Core `
        -Remote $Spec.Remote

    $SmokeResults += $Result

    if ($Result.Core -and $Result.Status -ne "PASS") {
        Stop-Gate "Mandatory core smoke test failed: $($Result.Name)"
    }

    if ($Result.Status -eq "FAIL") {
        Stop-Gate "Smoke test failed: $($Result.Name)"
    }
}

$NetworkStatus = if (@($SmokeResults | Where-Object { $_.Remote -and $_.Status -eq "OFFLINE" }).Count -gt 0) {
    "REMOTE_DEPENDENCY"
}
else {
    "PASS"
}

$ValidationPath = Join-Path $Root "docs\VALIDATION.md"
Write-Validation `
    -Path $ValidationPath `
    -Status "CANDIDATE" `
    -ManifestStatus "PENDING" `
    -SmokeNetwork $NetworkStatus

$Regenerate = Join-Path $Root "tools\regenerate-manifest.ps1"
$Verify = Join-Path $Root "tools\verify-manifest.ps1"

if (-not (Test-Path -LiteralPath $Regenerate)) {
    Stop-Gate "Missing tools/regenerate-manifest.ps1"
}

if (-not (Test-Path -LiteralPath $Verify)) {
    Stop-Gate "Missing tools/verify-manifest.ps1"
}

Invoke-Step "Manifest regeneration candidate" {
    & powershell -ExecutionPolicy Bypass -File $Regenerate
}

Invoke-Step "Manifest verification candidate" {
    & powershell -ExecutionPolicy Bypass -File $Verify
}

Write-Validation `
    -Path $ValidationPath `
    -Status "ACCEPTED" `
    -ManifestStatus "PASS" `
    -SmokeNetwork $NetworkStatus

Invoke-Step "Manifest regeneration final" {
    & powershell -ExecutionPolicy Bypass -File $Regenerate
}

Invoke-Step "Manifest verification final" {
    & powershell -ExecutionPolicy Bypass -File $Verify
}

if (-not (Test-Path -LiteralPath ".git")) {
    Stop-Gate "The NADIR root is not a Git repository"
}

$ExistingTag = (& git tag -l "v0.3.1" | Out-String).Trim()
if ($ExistingTag -eq "v0.3.1") {
    Stop-Gate "Git tag v0.3.1 already exists"
}

Invoke-Step "Git stage" {
    & git add -A -- . ":(exclude)build/**" ":(exclude)data/cache/**"
}

$DiffCached = (& git diff --cached --name-only | Out-String).Trim()
if ([string]::IsNullOrWhiteSpace($DiffCached)) {
    Stop-Gate "No staged changes available for the baseline commit"
}

Invoke-Step "Git commit" {
    & git commit -m "chore: establish NADIR 0.3.1 baseline"
}

Invoke-Step "Git tag" {
    & git tag -a v0.3.1 -m "NADIR 0.3.1 baseline"
}

Write-Host ""
Write-Host "=============================================="
Write-Host "NADIR 0.3.1 BASELINE ACCEPTED"
Write-Host "Build: PASS"
Write-Host "CTest: PASS"
Write-Host "Smoke Core: PASS"
Write-Host "Smoke Network: $NetworkStatus"
Write-Host "Manifest: PASS"
Write-Host "Git tag: v0.3.1"
Write-Host "=============================================="
