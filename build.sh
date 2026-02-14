#!/bin/bash

# Build script for Brain PW-SH2 Apps
# Requires CeGCC environment

SOURCE="main.cpp"
OUTPUT="./Example/AppMain.exe"

# Create output directory if it doesn't exist
dirname "$OUTPUT" | xargs mkdir -p

echo "Building $SOURCE for Windows CE..."

arm-mingw32ce-g++ -Wall -Wextra -O3 -mcpu=arm926ej-s -static -s -o "$OUTPUT" "$SOURCE" -D_WIN32_IE=0x0400

if [ $? -eq 0 ]; then
    echo "Build successful! Output: $OUTPUT"
    
    # Deploy to SD Card
    # Get current directory name (e.g., SnakeGame)
    PROJECT_NAME=$(basename "$PWD")
    DEPLOY_DIR="/Volumes/SDCARD/アプリ/$PROJECT_NAME"
    
    echo "Deploying to $DEPLOY_DIR..."
    
    # Ensure deployment directory is clean and exists
    # We want to copy the CONTENTS of Example to DEPLOY_DIR, effectively making DEPLOY_DIR a copy of Example
    if [ -d "$DEPLOY_DIR" ]; then
        rm -rf "$DEPLOY_DIR"
    fi
    
    # Copy Example folder as the new project folder
    cp -r ./Example "$DEPLOY_DIR"
    
    if [ $? -eq 0 ]; then
        echo "Deployment successful!"
    else
        echo "Deployment failed."
    fi
else
    echo "Build failed."
fi
