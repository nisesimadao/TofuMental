#!/bin/bash

# Build script for Windows 10 (Desktop)
# Requires mingw-w64 environment (x86_64-w64-mingw32-g++)

SOURCE="main.cpp"
OUTPUT_DIR="./dist_win10"
OUTPUT="$OUTPUT_DIR/AppMain_win10.exe"

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

echo "Building App for Windows 10..."

# Compile with x86_64 mingw compiler
# -DUNICODE -D_UNICODE for wide string support
# -mwindows for GUI app (no console)
# -municode for wWinMain support
# -static for standalone executable
x86_64-w64-mingw32-g++ -Wall -Wextra -O3 -mwindows -municode -static -s -o "$OUTPUT" "$SOURCE" -DUNICODE -D_UNICODE

if [ $? -eq 0 ]; then
    echo "Build successful! Output: $OUTPUT"
    
    echo "Preparing assets..."
    cp *.wav "$OUTPUT_DIR/"
    
    echo "------------------------------------------------"
    echo "Ready! You can find the executable and wav files in $OUTPUT_DIR"
    echo "------------------------------------------------"
else
    echo "Build failed."
    exit 1
fi
