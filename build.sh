#!/bin/bash
cd -- "$(dirname -- "$0")" || exit 1

gcc main.c -o pokemon -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 &&
./pokemon