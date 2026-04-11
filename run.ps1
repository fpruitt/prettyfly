param(
    [switch]$Release
)

$Config = if ($Release) { "Release" } else { "Debug" }
$ExePath = "$PSScriptRoot\engine\build\$Config\PrettyFly.exe"

if (-not (Test-Path $ExePath)) {
    Write-Host "[run] Binary not found, building first..." -ForegroundColor Cyan
    & "$PSScriptRoot\build.ps1" $(if ($Release) { "-Release" })
}

& $ExePath