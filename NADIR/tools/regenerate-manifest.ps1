<#============================================================================
# NADIR v0.3.1 — Regenerador de MANIFEST.sha256
#
# Este script gera o MANIFEST.sha256 oficial a partir da árvore de origens.
# Ele:
#   - Percorre a árvore de forma recursiva determinística (ordenação por caminho)
#   - Exclui: build/, data/cache/, .git/, .vs/, .idea/, o próprio manifest
#   - Normaliza paths para separador '/' (Windows-compatível)
#   - Ordena arquivos por caminho lowercase para repetibilidade
#   - Usa Get-FileHash (SHA-256) em cada arquivo
#   - Escreve em UTF-8 sem BOM
#   - Exclui: MANIFEST.sha256 mesmo, data/cache/**, e pastas de build/IDE
#
# Usage:
#   .\regenerate-manifest.ps1
#
# After running, verify with: .\verify-manifest.ps1
#============================================================================#>

# --- Configurações ---
$Root = (Resolve-Path "$PSScriptRoot\..").Path
$Manifest = Join-Path $Root "MANIFEST.sha256"

# Pastas e arquivos a EXCLUIR da geração
$ExcludedDirectories = @(
    ".git",
    ".vs",
    ".idea",
    "build",
    "out"
)

$ExcludedFiles = @(
    "MANIFEST.sha256"
)

# --- Coleta de arquivos ---
$Files = Get-ChildItem `
    -Path $Root `
    -Recurse `
    -File |
    Where-Object {
        # Converte path para relativo a $Root
        $Relative = $_.FullName.Substring($Root.Length).TrimStart('\', '/')

        # Verifica se algum componente do path está na lista de exclusão
        $HasExcludedDirectory = $false
        $Parts = $Relative -split '[\\/]'
        foreach ($Part in $Parts) {
            if ($ExcludedDirectories -contains $Part.Trim()) {
                $HasExcludedDirectory = $true
                break
            }
        }

        # Filtros finais:
        # - Não tem pasta excluída no caminho
        # - Não é o próprio manifest
        # - Não está em data/cache/ (dados de aquisição, não source release)
        # - Nome de arquivo não está na lista exclusa
        $isExcludedByName = $ExcludedFiles -contains $_.Name

        $isInCache = $Relative -like "data\cache\*" -or $Relative -like "data\cache\*"

        -not $HasExcludedDirectory -and
        -not $isInCache -and
        -not $isExcludedByName
    }

# Ordenação determinística por caminho lowercase
$Files = $Files | Sort-Object {
    $_.FullName.Substring($Root.Length).ToLowerInvariant()
}

# --- Geração de linhas do manifest ---
$Lines = foreach ($File in $Files) {
    $Hash = (Get-FileHash -LiteralPath $File.FullName -Algorithm SHA256).Hash.ToLowerInvariant()

    $Relative = $File.FullName.Substring($Root.Length).TrimStart('\', '/').Replace('\', '/')

    "$Hash  $Relative"
}

# --- Escrita do manifest ---
# Usa UTF-8 sem BOM
$Utf8 = New-Object System.Text.UTF8Encoding($false)

[System.IO.File]::WriteAllLines(
    $Manifest,
    $Lines,
    $Utf8
)

Write-Host ""
Write-Host "Manifest regenerated."
Write-Host "Root: $Root"
Write-Host "Files: $($Lines.Count)"
Write-Host "Path: $Manifest"
Write-Host ""

# --- Pós-escrita: verificação rápida ---
Write-Host "Validating..."

$Failed = 0
$Checked = 0

# Lê o manifest gerado e verifica cada entrada
foreach ($Line in $Lines) {
    if ([string]::IsNullOrWhiteSpace($Line)) { continue }

    # Split by exactly two spaces (hash<space><space>path)
    $Parts = $Line -split '  ', 2
    if ($Parts.Count -ne 2) {
        Write-Host "INVALID FORMAT: $Line"
        $Failed++
        continue
    }

    $Path = Join-Path $Root ($RelativePath.Replace('/', '\'))

    if (-not (Test-Path -LiteralPath $Path)) {
        Write-Host "MISSING  $RelativePath"
        $Failed++
        continue
    }

    $Actual = (
        Get-FileHash `
            -LiteralPath $Path `
            -Algorithm SHA256
    ).Hash.ToLowerInvariant()

    $Checked++

    if ($Actual -ne $Expected) {
        Write-Host "FAILED   $RelativePath"
        Write-Host " expected: $Expected"
        Write-Host " actual:   $Actual"
        $Failed++
    }
    else {
        Write-Host "OK       $RelativePath"
    }
}

Write-Host ""
Write-Host "Checked: $Checked"
Write-Host "Failed:  $Failed"

if ($Failed -ne 0) {
    Write-Host "Manifest verification FAILED."
    exit 1
}
else {
    Write-Host "Manifest verification PASSED."
    exit 0
}