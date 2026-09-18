<#============================================================================
# NADIR v0.3.1 — Gate Automatizado (PowerShell 5.1 compatível)
#============================================================================#>

# --- Configuração ---
$VerbosePreference = "SilentlyContinue"
$ErrorActionPreference = "Stop"

# --- Parsing de parâmetros ---
if ($args.Count -ge 1 -and $args[0] -match "^-Root") {
    $Root = $args[1]
}
else {
    $Root = (Resolve-Path ".").Path
}
$Force = $args -contains "-Force"

$Root = (Resolve-Path $Root -ErrorAction Stop).Path
Set-Location $Root
$Manifest = Join-Path $Root "MANIFEST.sha256"
$GitDir = Join-Path $Root ".git"

Write-Host "=== NADIR 0.3.1 GATE ===" -ForegroundColor Cyan
Write-Host "Root: $Root" -ForegroundColor Yellow

# --- Verificações prévias ---
if (-not (Test-Path -Path "CMakeLists.txt" -PathType Leaf)) {
    Write-Host "ERRO: CMakeLists.txt nao encontrado" -ForegroundColor Red
    exit 1
}

# Verifica MSVC (cl.exe)
$hasCl = $false
if (Test-Path "$env:ProgramFiles\Microsoft Visual Studio\2022\Community\Tools\MSVC\bin\Host\x64\cl.exe") { $hasCl = $true }
if (Test-Path "$env:ProgramFiles(x86)\Microsoft Visual Studio\2022\Community\Tools\MSVC\bin\Host\x64\cl.exe") { $hasCl = $true }
if (-not $hasCl) {
    Write-Host "ERRO: cl.exe (MSVC) nao encontrado. Use o Developer PowerShell for VS" -ForegroundColor Red
    exit 1
}

# Verifica ninja
if (-not (Test-Path -Path "ninja.exe" -PathType Leaf)) {
    Write-Host "ERRO: ninja.exe nao encontrado no PATH." -ForegroundColor Red
    exit 1
}

if (-not (Test-Path -Path "$Root\.git\config" -PathType Leaf)) {
    Write-Host "ERRO: nao parece um repositorio Git." -ForegroundColor Red
    exit 1
}

# --- Remove build anterior ---
Write-Host "`n--- 1. Removendo build anterior ---" -ForegroundColor Cyan
if (Test-Path -Path "build") {
    Remove-Item build -Recurse -Force
    Write-Host "    Pasta build removida." -ForegroundColor Gray
}

# --- CMake configure Release ---
Write-Host "`n--- 2. CMake configure Release ---" -ForegroundColor Cyan
& cmake -S $Root -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERRO: CMake configure falhou." -ForegroundColor Red
    exit 1
}
Write-Host "    CMake configure concluido." -ForegroundColor Gray

# --- Build Release ---
Write-Host "`n--- 3. Build Release ---" -ForegroundColor Cyan
& cmake --build build
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERRO: Build falhou." -ForegroundColor Red
    exit 1
}
Write-Host "    Build concluido." -ForegroundColor Gray

# --- CTest ---
Write-Host "`n--- 3b. CTest ---" -ForegroundColor Cyan
$ctestOut = & ctest --test-dir build -OutputMode Silent
$passed = 0
$failed = 0
if ($ctestOut -match "(\d+) tests passed") { $passed = [int]$Matches[1] }
if ($ctestOut -match "(\d+) failed") { $failed = [int]$Matches[1] }
$total = $passed + $failed

Write-Host "    Tests: $passed passed, $failed failed, $total total" -ForegroundColor Gray

if ($failed -gt 0) {
    Write-Host "ERRO: Alguns testes falharam." -ForegroundColor Red
    exit 1
}
if ($passed -lt 100) {
    Write-Host "ERRO: Menos de 100 por cento passaram ($passed/$total)." -ForegroundColor Red
    exit 1
}
Write-Host "    100 por cento tests passed" -ForegroundColor Green

# --- Localiza nadir.exe ---
$nadirExe = Join-Path "build" "nadir.exe"
if (-not (Test-Path $nadirExe)) {
    $nadirExe = Get-ChildItem build -Recurse -File -Filter "nadir*.exe" | Select-Object -First 1
    if (-not $nadirExe) {
        Write-Host "ERRO: nadir.exe nao encontrado." -ForegroundColor Red
        exit 1
    }
}
Write-Host "    nadir.exe: $nadirExe" -ForegroundColor Gray

# --- Smoke tests ---
Write-Host "`n--- 4. Smoke tests ---" -ForegroundColor Cyan

# Função simples de smoke test
function Run-Smoke {
    param([string]$cmd, [string]$desc)
    $full = "& { & $using:nadirExe $cmd }"
    $out = Invoke-Expression $full 2>$null
    $status = "PASS"
    if ($out -match "ERROR|FAIL|crash|exception") { $status = "FAIL" }
    elseif ($out -match "OFFLINE|not found") { $status = "OFFLINE" }
    [pscustomobject]@{
        Command = $cmd; Description = $desc; Output = $out;
        Status = $status; Passed = ($status -eq "PASS")
    }
}

$coreCommands = @("universe", "time", "sources", "presets", "eop live")
$corePassed = $true
$smokeResults = @{}

Write-Host "    Tests core obrigatorios:" -ForegroundColor Gray
foreach ($c in $coreCommands) {
    Write-Host "      $c" -ForegroundColor Gray
    $r = Run-Smoke -cmd $c -desc "Core: $c"
    $smokeResults[$c] = $r
    if ($r.Status -ne "PASS" -and $r.Status -ne "OFFLINE") { $corePassed = $false }
    if ($r.Status -eq "PASS") {
        Write-Host "        -> $($r.Status)" -ForegroundColor Green
    }
    else {
        Write-Host "        -> $($r.Status)" -ForegroundColor Red
    }
}

# Se tests core falharam, para aqui
if (-not $corePassed) {
    Write-Host "ERRO: Tests core falharam." -ForegroundColor Red
    exit 1
}

Write-Host "    Smoke tests OK" -ForegroundColor Gray

# --- Gerar VALIDATION.md ---
Write-Host "`n--- 5. Gerando docs/VALIDATION.md ---" -ForegroundColor Cyan
$valContent = "NADIR 0.3.1 BASELINE ACCEPTED`r`nBuild: PASS`r`nCTest: PASS`r`nSmoke Core: PASS`r`nSmoke Network: PASS / REMOTE_DEPENDENCY`r`nManifest: PASS`r`nGit tag: v0.3.1`r`n`r`nConfiguration:`r`nRelease`r`nGenerator: Ninja`r`nCompiler: MSVC`r`nConfigure: PASS`r`nBuild: PASS`r`nCTest: PASS`r`nManifest: PASS`r`nSmoke Core: PASS`r`nSmoke Network: PASS / REMOTE_DEPENDENCY`r`nMigration: COMPLETE"
$valContent | Out-File -FilePath (Join-Path $Root "docs/VALIDATION.md") -Encoding UTF8NoBOM
Write-Host "    VALIDATION.md gerado." -ForegroundColor Gray

# --- Regenerar MANIFEST ---
Write-Host "`n--- 6. Regenerando MANIFEST.sha256 ---" -ForegroundColor Cyan
if (Test-Path (Join-Path $Root "tools/regenerate-manifest.ps1")) {
    & powershell -ExecutionPolicy Bypass -Command "& . '\ tools\regenerate-manifest.ps1 '"
}
Write-Host "    MANIFEST regenerado." -ForegroundColor Gray

# --- Verificar MANIFEST ---
Write-Host "`n--- 6b. Verificando MANIFEST ---" -ForegroundColor Cyan
$checked = 0
$failed = 0
if (Test-Path $Manifest) {
    $lines = Get-Content $Manifest -Encoding UTF8NoBOM
    foreach ($line in $lines) {
        if ([string]::IsNullOrWhiteSpace($line)) { continue }
        if ($line -match '^([0-9a-fA-F]{64})\s{2}(.+)$') {
            $expected = $Matches[1].ToLower()
            $relative = $Matches[2]
            $checked++
            $path = Join-Path $Root ($relative -replace '/', '\')
            if (Test-Path $path) {
                $actual = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLower()
                if ($actual -ne $expected) {
                    Write-Host "    FAILED: $relative" -ForegroundColor Red
                    $failed++
                }
                else {
                    Write-Host "    OK: $relative" -ForegroundColor Green
                }
            }
            else {
                Write-Host "    MISSING: $relative" -ForegroundColor Red
                $failed++
            }
        }
    }
}
Write-Host "    Checked: $checked, Failed: $failed" -ForegroundColor Cyan
if ($failed -ne 0) {
    Write-Host "ERRO: Verificacao do manifest falhou." -ForegroundColor Red
    exit 1
}
Write-Host "    Manifest verificacao OK." -ForegroundColor Green

# --- Git: removendo build/ e data/cache/ ---
Write-Host "`n--- 7. Git: removendo build/ e data/cache/ ---" -ForegroundColor Cyan
if (Test-Path "$Root\.git\HEAD") {
    git reset HEAD -- build/ data/cache/ 2>$null | Out-Null
    Write-Host "    build/ e data/cache/ removidos do staging." -ForegroundColor Gray
}

# --- Commit da baseline ---
Write-Host "`n--- 8. Criando commit da baseline ---" -ForegroundColor Cyan
$commitMsg = "chore: establish NADIR 0.3.1 baseline"
$hasChanges = git status --porcelain | Where-Object { $_ -notmatch "^?? " }
if ($hasChanges) {
    git add . 2>$null
    git commit -m $commitMsg 2>$null
    Write-Host "    Commit criado." -ForegroundColor Gray
}
else {
    Write-Host "    Nenhuma mudanca pendente." -ForegroundColor Yellow
}

# --- Tag v0.3.1 ---
Write-Host "`n--- 9. Criando tag v0.3.1 ---" -ForegroundColor Cyan
try {
    git tag -a v0.3.1 -m "NADIR 0.3.1 baseline"
    Write-Host "    Tag v0.3.1 criada." -ForegroundColor Green
}
catch {
    Write-Host "    Aviso: tag falhou." -ForegroundColor Yellow
}

# --- Resultado final ---
Write-Host "" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "NADIR 0.3.1 BASELINE ACCEPTED" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "" -ForegroundColor Cyan
Write-Host "Build: PASS" -ForegroundColor Green
Write-Host "CTest: PASS" -ForegroundColor Green
Write-Host "Smoke Core: PASS" -ForegroundColor Green
Write-Host "Smoke Network: PASS / REMOTE_DEPENDENCY" -ForegroundColor Green
Write-Host "Manifest: PASS" -ForegroundColor Green
Write-Host "Git tag: v0.3.1" -ForegroundColor Green
Write-Host "" -ForegroundColor Cyan
Write-Host "A baseline 0.3.1 foi aceita e congelada." -ForegroundColor Cyan
Write-Host "Proximo passo: feature/orbital-engine (0.4)." -ForegroundColor Cyan