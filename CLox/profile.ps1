# profile.ps1 — build (profile preset) + WPR heap trace + run CLox
# Usage: pwsh -ExecutionPolicy Bypass -File .\profile.ps1 .\scripts\problem.lox

# --- Auto-elevate ---
$principal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    $argsEscaped = ($args | ForEach-Object { '"{0}"' -f $_ }) -join ' '
    $argList = "-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`" $argsEscaped"
    Start-Process powershell -Verb RunAs -ArgumentList $argList
    exit
}

# Always work from the script directory (NOT system32)
Set-Location -LiteralPath $PSScriptRoot

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

# 2) Record heap profile around the run
wpr -cancel *>$null 2>&1  # ignore if nothing running

wpr -HeapTracingConfig CLox.exe enable
wpr -start Heap.Verbose -filemode
try {
    & $exe @args
    $exitCode = $LASTEXITCODE
} finally {
    # Always stop profiling even if CLox crashes
    wpr -stop $etl
}
wpr -HeapTracingConfig CLox.exe disable
Write-Host "Profile saved to $etl (CLox exit code $exitCode) ✅"

wpa C:\Users\Darki\AppData\Local\Temp\heap_trace.etl
