param(
    [switch]$Release
)

$BuildDir = "$PSScriptRoot\engine\build"
$Config = if ($Release) { "Release" } else { "Debug" }

cmake --build $BuildDir --config $Config