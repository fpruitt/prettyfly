param(
    [switch]$Release
)

$VcpkgPath = "$PSScriptRoot\tools\vcpkg"
$BuildDir = "$PSScriptRoot\engine\build"
$Config = if ($Release) { "Release" } else { "Debug" }

if (Test-Path $BuildDir) {
    Remove-Item -Recurse -Force $BuildDir
}
New-Item -ItemType Directory $BuildDir | Out-Null

cmake -S "$PSScriptRoot\engine" -B $BuildDir `
    -DCMAKE_TOOLCHAIN_FILE="$VcpkgPath\scripts\buildsystems\vcpkg.cmake" `
    -DVCPKG_INSTALLED_DIR="$VcpkgPath\installed"
	-DCMAKE_BUILD_TYPE=$Config