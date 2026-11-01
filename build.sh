#!/bin/bash
cd -- "$(dirname -- "$0")" || exit 1

gcc src/*.c -o pokemon -Isrc -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 &&
./pokemon
