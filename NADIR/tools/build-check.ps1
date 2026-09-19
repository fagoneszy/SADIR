$ErrorActionPreference = "Stop"
$VcvarsPath = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
if (-not (Test-Path $VcvarsPath)) { $VcvarsPath = "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" }
Write-Host "Configurando MSVC..."
& cmd /c "`"$VcvarsPath`" x64 && set" | ForEach-Object {
  if ($_ -match '^([^=]+)=(.*)$') {
    $n=$matches[1]; $v=$matches[2]
    if ($n -in @('PATH','INCLUDE','LIB','LIBPATH','WindowsSdkDir','UniversalCRTSdkDir','VCToolsInstallDir','VCToolsVersion')) {
      [Environment]::SetEnvironmentVariable($n,$v,'Process')
    }
  }
}
$env:PATH += ";C:\Program Files\CMake\bin"
Set-Location "C:\Users\faguinho\Documents\SADIR\NADIR"
Write-Host "=== CMake configure ==="
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE -ne 0) { Write-Host "CONFIGURE FAILED" -ForegroundColor Red; exit 1 }
Write-Host "=== CMake build ==="
cmake --build build
if ($LASTEXITCODE -ne 0) { Write-Host "BUILD FAILED" -ForegroundColor Red; exit 1 }
Write-Host "=== CTest (core/render only, exclui ciencia) ==="
ctest --test-dir build --label-exclude science --output-on-failure
if ($LASTEXITCODE -ne 0) { Write-Host "CORE/RENDER TESTS FAILED" -ForegroundColor Red; exit 1 }
Write-Host "CORE/RENDER PASS" -ForegroundColor Green
Write-Host "=== CTest science (esperado falhar ate Vallado completo) ==="
ctest --test-dir build -R nadir_sgp4_verification --output-on-failure; $scienceCode=$LASTEXITCODE
if ($scienceCode -eq 0) { Write-Host "SGP4 VERIFICATION PASS" -ForegroundColor Green } else { Write-Host "SGP4 VERIFICATION EXPECTED FAIL (placeholder) code=$scienceCode" -ForegroundColor Yellow }
Write-Host "BUILD+TEST DONE" -ForegroundColor Green
