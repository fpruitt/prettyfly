param(
    [string]$VcpkgPath = "$PSScriptRoot\tools\vcpkg"
)

$ErrorActionPreference = "Stop"

function Log($msg) { Write-Host "[setup] $msg" -ForegroundColor Cyan }
function Success($msg) { Write-Host "[setup] $msg" -ForegroundColor Green }
function Fail($msg) { Write-Host "[setup] $msg" -ForegroundColor Red; exit 1 }

function Check-Command($cmd, $installHint) {
    if (-not (Get-Command $cmd -ErrorAction SilentlyContinue)) {
        Fail "$cmd not found. $installHint"
    }
}

Check-Command "git"   "Install from https://git-scm.com"
Check-Command "cmake" "Install from https://cmake.org or via winget: winget install Kitware.CMake"

# ── vcpkg ────────────────────────────────────────────────────────────────────

if (Test-Path "$VcpkgPath\vcpkg.exe") {
    Log "vcpkg already bootstrapped, skipping."
} else {
    if (-not (Test-Path $VcpkgPath)) {
        Log "Cloning vcpkg..."
        git clone https://github.com/microsoft/vcpkg.git $VcpkgPath
    }
    Log "Bootstrapping vcpkg..."
    & "$VcpkgPath\bootstrap-vcpkg.bat" -disableMetrics
}

Log "Integrating vcpkg with Visual Studio..."
& "$VcpkgPath\vcpkg.exe" integrate install

# ── CMake configure ───────────────────────────────────────────────────────────

$buildDir = "$PSScriptRoot\engine\build"
$toolchain = "$VcpkgPath\scripts\buildsystems\vcpkg.cmake"

if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

Log "Running CMake configure..."
cmake -S "$PSScriptRoot\engine" -B $buildDir -DCMAKE_TOOLCHAIN_FILE="$toolchain"

if ($LASTEXITCODE -ne 0) { Fail "CMake configure failed." }

Success "Setup complete! Open engine\build\PrettyFly.sln in Visual Studio."