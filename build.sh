#!/bin/bash
cd -- "$(dirname -- "$0")" || exit 1

echo "Building Pokemon Phoenix Red for Linux..."

# Try compiling with pkg-config if available, otherwise fallback to standard linking
if command -v pkg-config >/dev/null 2>&1 && pkg-config --exists raylib; then
    gcc src/*.c -o pokemon -Isrc $(pkg-config --cflags --libs raylib) -lm
else
    gcc src/*.c -o pokemon -Isrc -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
fi

if [ $? -eq 0 ]; then
    echo "Build succeeded! Launching game..."
    ./pokemon
else
    echo "Build failed."
    echo "Make sure Raylib is installed on your Linux distribution:"
    echo "  Ubuntu/Debian: sudo apt install libraylib-dev"
    echo "  Arch Linux:    sudo pacman -S raylib"
    echo "  Fedora:        sudo dnf install raylib-devel"
    exit 1
fi
