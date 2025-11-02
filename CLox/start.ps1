# Paths
$buildDir = Join-Path $PSScriptRoot 'build\profile-debug'
$exe      = Join-Path $buildDir 'CLox.exe'
$etl      = Join-Path $env:TEMP 'heap_trace.etl'

# 1) Configure + build (idempotent)
cmake --preset profile-debug
if ($LASTEXITCODE) { throw "CMake configure failed." }

cmake --build --preset profile-debug -j
if ($LASTEXITCODE) { throw "Build failed." }

if (-not (Test-Path $exe)) {
    Write-Warning "CLox.exe not found at $exe. Scanning build dir..."
    $found = Get-ChildItem -Path $buildDir -Recurse -Filter 'CLox.exe' -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($null -eq $found) { throw "CLox.exe not found under $buildDir." }
    $exe = $found.FullName
}

try {
    & $exe @args
    $exitCode = $LASTEXITCODE
} finally {}

Write-Host "CLox exit code $exitCode"
