# Brain Snake Game

A simple Snake Game for Sharp Brain PW-SH2 electronic dictionaries.

## Prerequisites

- **CeGCC Compiler**: You need a cross-compilation environment for Windows CE.
  - Linux or WSL (Windows Subsystem for Linux) is recommended.
  - See [BrainWiki Development Environment](https://brain.fandom.com/ja/wiki/%E9%96%8B%E7%99%BA%E7%92%B0%E5%A2%83%E3%83%BBSDK) for setup instructions.

## Building

1.  Open your terminal in this directory.
2.  Run the build script:
    ```bash
    ./build.sh
    ```
    This will generate `AppMain.exe`.

## Installation on Brain

1.  Connect your Brain device or SD card to your PC.
2.  Create a folder named `アプリ` (Apps) in the root directory if it doesn't exist.
3.  Inside `アプリ`, create a folder for this game, e.g., `SnakeGame`.
4.  Copy the generated `AppMain.exe` into this `SnakeGame` folder.
5.  Create an empty file named `index.din` inside the `SnakeGame` folder.
    - On Linux/Mac: `touch index.din`
    - On Windows: Create a text file and rename it to `index.din`.
6.  (Optional for newer models) Create an empty `AppMain.cfg` for high-resolution support.

Structure:
```
SD_CARD/
└── アプリ/
    └── SnakeGame/
        ├── AppMain.exe
        └── index.din
```

## How to Play

1.  On your Brain device, go to **Accessories (アクセサリー)** -> **Add-on Apps (追加アプリ・動画)**.
2.  Select **SnakeGame**.
3.  **Controls**:
    - **Arrow Keys**: Move the snake.
    - **Enter**: Start / Restart game.
    - **Esc / Back**: Exit game.
