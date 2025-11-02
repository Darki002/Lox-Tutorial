cmake --preset profile-debug
cmake --build --preset profile-debug -j

wpr -start Heap -filemode
.\build\profile-debug\lox.exe $args
wpr -stop heap_trace.etl
Write-Host "Profile saved to heap_trace.etl ✅"
