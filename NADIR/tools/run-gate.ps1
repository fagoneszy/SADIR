# run-gate.ps1 - Wrapper que configura o ambiente MSVC e roda o gate

$ErrorActionPreference = "Stop"

# Caminhos das ferramentas
$CmakePath = "C:\Program Files\CMake\bin"
$VcvarsPath = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"

if (-not (Test-Path $VcvarsPath)) {
    # Tenta BuildTools
    $VcvarsPath = "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
}
if (-not (Test-Path $VcvarsPath)) {
    Write-Host "vcvarsall.bat não encontrado!" -ForegroundColor Red
    exit 1
}

# Configura o ambiente MSVC x64
Write-Host "Configurando ambiente MSVC x64..." -ForegroundColor Cyan
& cmd /c "`"$VcvarsPath`" x64 && set" | ForEach-Object {
    if ($_ -match '^([^=]+)=(.*)$') {
        $name = $matches[1]
        $value = $matches[2]
        if ($name -in @('PATH','INCLUDE','LIB','LIBPATH','WindowsSdkDir','UniversalCRTSdkDir','VCToolsInstallDir','VCToolsVersion')) {
            [Environment]::SetEnvironmentVariable($name, $value, 'Process')
        }
    }
}

# Adiciona cmake ao PATH
$env:PATH += ";$CmakePath"

# Verifica
Write-Host "Verificando ferramentas..."
cmake --version | Select-Object -First 1
ninja --version
# cl.exe check (suppress output)
$null = cmd /c "cl.exe 2>nul"
Write-Host "cl.exe OK"

# Executa o gate
Write-Host "Executando gate-0.3.1.ps1..." -ForegroundColor Green
cd C:\Users\faguinho\Documents\SADIR
& .\gate-0.3.1.ps1