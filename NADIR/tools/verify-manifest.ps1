<#============================================================================
# NADIR v0.3.1 — Verificador de MANIFEST.sha256
#
# Este script verifica se o MANIFEST.sha256 corresponde à árvore de origens.
# Ele:
#   - Lê cada linha do manifest
#   - Extrai o hash esperado e o caminho relativo
#   - Computa o hash SHA-256 real do arquivo
#   - Compara expected vs actual
#   - Reporta OK/FAILED/MISSING para cada entrada
#   - No final, reporta total de checked e failed
#
# Usage:
#   .\verify-manifest.ps1
#
# Saída esperada:
#   OK       filename
#   FAILED   filename (hash mismatch)
#   MISSING  filename (arquivo não encontrado)
#   Checked: N
#   Failed:  M
#
# Se Failed for 0, a baseline está aceita.
#============================================================================#>

# --- Configurações ---
$Root = (Resolve-Path "$PSScriptRoot\..").Path
$Manifest = Join-Path $Root "MANIFEST.sha256"

if (-not (Test-Path -LiteralPath $Manifest)) {
    throw "MANIFEST.sha256 not found at: $Manifest"
}

# --- Processamento ---
$Failed = 0
$Checked = 0

foreach ($Line in Get-Content -LiteralPath $Manifest) {
    if ([string]::IsNullOrWhiteSpace($Line)) {
        continue
    }

    # Parse da linha: "hash  relative/path"
    if ($Line -notmatch '^([0-9a-fA-F]{64})\s{2}(.+)$') {
        Write-Host "INVALID ENTRY: $Line"
        $Failed++
        continue
    }

    $Expected = $Matches[1].ToLowerInvariant()
    $Relative = $Matches[2]

    $Checked++

    # Constrói o path completo
    $Path = Join-Path $Root (
        $Relative.Replace('/', '\')
    )

    if (-not (Test-Path -LiteralPath $Path)) {
        Write-Host "MISSING  $Relative"
        $Failed++
        continue
    }

    $Actual = (
        Get-FileHash `
            -LiteralPath $Path `
            -Algorithm SHA256
    ).Hash.ToLowerInvariant()

    if ($Actual -ne $Expected) {
        Write-Host "FAILED   $Relative"
        Write-Host " expected: $Expected"
        Write-Host " actual:   $Actual"
        $Failed++
    }
    else {
        Write-Host "OK       $Relative"
    }
}

Write-Host ""
Write-Host "Checked: $Checked"
Write-Host "Failed:  $Failed"

if ($Failed -ne 0) {
    Write-Host ""
    Write-Host "Manifest verification FAILED. Base 0.3.1 não aceita."
    exit 1
}
else {
    Write-Host ""
    Write-Host "Manifest verification PASSED. Base 0.3.1 ACEITA."
    exit 0
}