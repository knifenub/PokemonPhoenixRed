@echo off
echo Building Pokemon Phoenix Red...
gcc src\*.c -o pokemon.exe -Isrc -l:libraylib.a -l:libglfw3.dll.a -lopengl32 -lgdi32 -lwinmm -Wall -mwindows -O2 2>nul
if %ERRORLEVEL% NEQ 0 (
    gcc src\*.c -o pokemon.exe -Isrc -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -mwindows -O2
)
if %ERRORLEVEL% EQU 0 (
    echo Build succeeded! pokemon.exe is ready.
) else (
    echo Build failed with error code %ERRORLEVEL%.
)
