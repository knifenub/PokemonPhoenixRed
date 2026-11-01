@echo off
echo Building Pokemon Phoenix Red...
gcc src\*.c -o pokemon.exe -Isrc -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -mwindows
if %ERRORLEVEL% EQU 0 (
    echo Build succeeded! pokemon.exe is ready.
) else (
    echo Build failed with error code %ERRORLEVEL%.
)
