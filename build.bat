@echo off
echo Building Pokemon Phoenix Red...
gcc main.c pokemon_database.c moves.c type_chart.c battle.c -o pokemon.exe -lraylib -lopengl32 -lgdi32 -lwinmm -Wall
if %ERRORLEVEL% EQU 0 (
    echo Build succeeded!
) else (
    echo Build failed with error code %ERRORLEVEL%.
)
