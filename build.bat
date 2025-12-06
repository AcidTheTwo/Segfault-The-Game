@echo off
setlocal
cd /d %~dp0
set PATH=C:\raylib\w64devkit\bin;%PATH%

echo Compiling Modular Segfault...

:: Note: We compile main.c AND terminal.c AND dive.c
gcc src/main.c src/terminal.c src/dive.c -o game.exe -O2 -Wall -I C:\raylib\raylib\src -L C:\raylib\raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm

if %errorlevel% equ 0 (
    echo Build Successful! Running...
    game.exe
) else (
    echo Build Failed.
    pause
)
endlocal