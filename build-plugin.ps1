$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

if (-not (Test-Path $vswhere)) {
    throw "vswhere.exe not found. Install Visual Studio 2022 with Desktop development with C++."
}

$msbuild = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe | Select-Object -First 1

if (-not $msbuild) {
    throw "MSBuild not found. Install the Visual Studio 2022 C++ workload."
}

Write-Host "MSBuild: $msbuild" -ForegroundColor Cyan
Write-Host "Building x64 Release..." -ForegroundColor Cyan

& $msbuild ".\SemanticProgrammingLanguage.sln" `
    /m `
    /t:Build `
    /p:Configuration=Release `
    /p:Platform=x64

if ($LASTEXITCODE -ne 0) {
    throw "x64 build failed."
}

$dll = ".\build\x64\Release\SemanticProgrammingLanguage.dll"

if (-not (Test-Path $dll)) {
    throw "Build succeeded but the DLL was not found: $dll"
}

$out = ".\dist"
$pluginFolder = Join-Path $out "SemanticProgrammingLanguage"

if (Test-Path $out) {
    Remove-Item $out -Recurse -Force
}

New-Item -ItemType Directory -Force -Path $pluginFolder | Out-Null
Copy-Item $dll (Join-Path $pluginFolder "SemanticProgrammingLanguage.dll")

$zip = Join-Path $out "SemanticProgrammingLanguage-1.0.0-x64.zip"
Compress-Archive -Path $pluginFolder -DestinationPath $zip -Force

$hash = (Get-FileHash $zip -Algorithm SHA256).Hash.ToLowerInvariant()

Write-Host ""
Write-Host "Build completed." -ForegroundColor Green
Write-Host "Plugin DLL:" -ForegroundColor Green
Write-Host (Resolve-Path $dll)
Write-Host ""
Write-Host "Plugins Admin release ZIP:" -ForegroundColor Green
Write-Host (Resolve-Path $zip)
Write-Host ""
Write-Host "SHA-256:" -ForegroundColor Green
Write-Host $hash
Write-Host ""
Write-Host "Press ENTER to close..." -ForegroundColor Cyan
[void](Read-Host)
