<#
Creates a local, reproducible release directory from an already configured
build. It never overwrites an existing artifact directory.

Usage (from NADIR): .\tools\package-release.ps1
#>
[CmdletBinding()]
param(
    [string]$BuildDirectory = "build",
    [string]$OutputDirectory = "dist"
)

$ErrorActionPreference = "Stop"
$Root = (Resolve-Path "$PSScriptRoot\..").Path
$Build = Join-Path $Root $BuildDirectory
$Output = Join-Path $Root $OutputDirectory
$Binary = Join-Path $Build "nadir.exe"

if (-not (Test-Path -LiteralPath $Build)) { throw "Build directory not found: $Build" }
if (-not (Test-Path -LiteralPath $Binary)) { throw "Executable not found: $Binary. Build NADIR first." }

& powershell -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot "verify-manifest.ps1")
if ($LASTEXITCODE -ne 0) { throw "Source manifest verification failed." }

$Version = (Select-String -LiteralPath (Join-Path $Root "CMakeLists.txt") -Pattern '^project\(nadir VERSION ([0-9.]+)' | Select-Object -First 1).Matches.Groups[1].Value
if ([string]::IsNullOrWhiteSpace($Version)) { throw "Could not determine project version." }
$Artifact = Join-Path $Output "nadir-$Version-win64"
if (Test-Path -LiteralPath $Artifact) { throw "Refusing to overwrite existing artifact: $Artifact" }

New-Item -ItemType Directory -Path $Artifact | Out-Null
Copy-Item -LiteralPath $Binary -Destination (Join-Path $Artifact "nadir.exe")
Copy-Item -LiteralPath (Join-Path $Root "LICENSE") -Destination (Join-Path $Artifact "LICENSE")
Copy-Item -LiteralPath (Join-Path $Root "LICENSES-DATA.md") -Destination (Join-Path $Artifact "LICENSES-DATA.md")
Copy-Item -LiteralPath (Join-Path $Root "MANIFEST.sha256") -Destination (Join-Path $Artifact "SOURCE-MANIFEST.sha256")
Copy-Item -LiteralPath (Join-Path $Root "docs\SBOM.md") -Destination (Join-Path $Artifact "SBOM.md")
Copy-Item -LiteralPath (Join-Path $Root "third_party\vallado\NOTICE.md") -Destination (Join-Path $Artifact "VALLADO-NOTICE.md")

$Lines = Get-ChildItem -LiteralPath $Artifact -File | Sort-Object Name | ForEach-Object {
    "{0}  {1}" -f (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant(), $_.Name
}
[System.IO.File]::WriteAllLines((Join-Path $Artifact "SHA256SUMS"), $Lines, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Release artifact created: $Artifact"
