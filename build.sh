#!/bin/bash
cd -- "$(dirname -- "$0")" || exit 1

gcc main.c pokemon_database.c moves.c type_chart.c battle.c -o pokemon -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 &&
./pokemon
