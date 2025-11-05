# profile_heap_newproc.ps1
[CmdletBinding()]
param(
    [string] $EtlOut = $(Join-Path $PSScriptRoot 'heap_performance\heap_trace.etl')
)

$ErrorActionPreference = 'Stop'

function Assert-Admin {
    $id=[Security.Principal.WindowsIdentity]::GetCurrent()
    $p = New-Object Security.Principal.WindowsPrincipal($id)
    if (-not $p.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
        throw "Run elevated (Administrator) for xperf/ETW."
    }
}

Assert-Admin
$null = Get-Command xperf -ErrorAction Stop

# Build as you had it
$buildDir = Join-Path $PSScriptRoot 'build\profile-debug'
$exe      = Join-Path $buildDir 'CLox.exe'

cmake --preset profile-debug
if ($LASTEXITCODE) { throw "CMake configure failed." }
cmake --build --preset profile-debug -j
if ($LASTEXITCODE) { throw "Build failed." }
if (-not (Test-Path $exe)) {
    $found = Get-ChildItem -Path $buildDir -Recurse -Filter 'CLox.exe' -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($null -eq $found) { throw "CLox.exe not found under $buildDir." }
    $exe = $found.FullName
}

# Ensure ETL dir exists
$null = New-Item -ItemType Directory -Force -Path (Split-Path -Parent $EtlOut)

# Clean any stale sessions
try { xperf -stop HeapSession 2>$null | Out-Null } catch {}
try { xperf -stop 2>$null | Out-Null } catch {}

# 1) Start kernel logger with stackwalks here (avoid putting -stackwalk on -heap)
xperf -on PROC_THREAD+LOADER -stackwalk HeapAlloc+HeapRealloc

# 2) Arm a named heap session to auto-attach when THIS image starts (no args)
#    IMPORTANT: give xperf the exact path to the exe you will launch.
xperf -start HeapSession -heap `
      -PidNewProcess $exe `
      -BufferSize 1024 -MinBuffers 128 -MaxBuffers 128

Write-Host "Heap session armed. Launching $exe with NO args (CLox must self-load the script)."

# 3) Launch the process WITHOUT args; your code/env var loads the .lox
$proc = Start-Process -FilePath $exe -PassThru
$proc.WaitForExit()

# 4) Stop & merge in correct order
xperf -stop HeapSession
xperf -stop
xperf -d $EtlOut

Write-Host "ETL written to: $EtlOut"
Write-Host "Open in WPA: wpa `"$EtlOut`""
