# install-deps.ps1 - Instala dependências para NADIR (cmake, ninja, Visual Studio Build Tools)

$ErrorActionPreference = "Stop"

function Check-Admin {
    $identity = [System.Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object System.Security.Principal.WindowsPrincipal($identity)
    return $principal.IsInRole([System.Security.Principal.WindowsBuiltInRole]::Administrator)
}

if (-not (Check-Admin)) {
    Write-Host "Este script precisa ser executado como Administrador." -ForegroundColor Red
    Write-Host "Clique com botão direito no PowerShell -> 'Executar como administrador'"
    exit 1
}

# Verifica se Chocolatey está instalado
if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
    Write-Host "Instalando Chocolatey..." -ForegroundColor Cyan
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    refreshenv
}

# Atualiza Chocolatey
Write-Host "Atualizando Chocolatey..." -ForegroundColor Cyan
choco upgrade chocolatey -y

# Instala cmake
Write-Host "Instalando cmake..." -ForegroundColor Cyan
choco install cmake -y --params 'ADD_CMAKE_TO_PATH=System'

# Instala ninja
Write-Host "Instalando ninja..." -ForegroundColor Cyan
choco install ninja -y

# Instala Visual Studio Build Tools 2022 (inclui cl.exe, MSVC, Windows SDK)
Write-Host "Instalando Visual Studio Build Tools 2022..." -ForegroundColor Cyan
choco install visualstudio2022buildtools -y `
    --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools --includeRecommended --quiet --wait"

# Instala Windows SDK se necessário
Write-Host "Instalando Windows 10/11 SDK..." -ForegroundColor Cyan
choco install windows-sdk-10.1 -y

Write-Host ""
Write-Host "==========================================" -ForegroundColor Green
Write-Host "Dependências instaladas com sucesso!" -ForegroundColor Green
Write-Host "==========================================" -ForegroundColor Green
Write-Host ""
Write-Host "IMPORTANT: Reinicie o terminal/PowerShell para que as variáveis de ambiente sejam atualizadas." -ForegroundColor Yellow
Write-Host "Depois execute: .\gate-0.3.1.ps1" -ForegroundColor Cyan