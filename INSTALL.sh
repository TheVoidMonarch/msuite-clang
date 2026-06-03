#!/bin/bash

# Exit immediately if a command fails
set -e

# Define colors
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Handle "clean" option
if [ "$1" == "clean" ]; then
    echo "Cleaning build artifacts..."
    rm -f masjidsuite src/*.o
    echo -e "${GREEN}SUCCESS:${NC} Clean complete."
    exit 0
fi

echo "Updating package lists..."
if ! sudo apt-get update; then
    echo -e "${RED}ERROR:${NC} Failed to update package lists. Exiting."
    exit 1
fi

echo "Installing dependencies..."
if ! sudo apt-get install -y \
    libcurl4-openssl-dev \
    libcjson-dev \
    libsdl2-dev \
    libsdl2-ttf-dev \
    libsdl2-mixer-dev \
    libsdl2-image-dev \
    build-essential; then
    echo -e "${RED}ERROR:${NC} Failed to install dependencies. Exiting."
    exit 1
fi

echo "Checking dependencies..."
for pkg in libcurl4-openssl-dev libcjson-dev libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev libsdl2-image-dev build-essential; do
    if ! dpkg -s "$pkg" &> /dev/null; then
        echo -e "${RED}ERROR:${NC} Missing dependency: $pkg"
        exit 1
    fi
done

echo -e "${GREEN}SUCCESS:${NC} All dependencies installed."

echo "Compiling project..."
if ! gcc -o masjidsuite \
    src/main.c \
    src/api_handler.c \
    src/sdl_handler.c \
    src/logger.c \
    -Isrc \
    -lcurl \
    -lcjson \
    -lSDL2 \
    -lSDL2_ttf \
    -lSDL2_mixer \
    -lSDL2_image \
    -I/usr/include/cjson; then
    echo -e "${RED}ERROR:${NC} Compilation failed. Exiting."
    exit 1
fi

echo -e "${GREEN}SUCCESS:${NC} Build complete! Run ./masjidsuite to start."
