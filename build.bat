@echo off
setlocal
cd /d %~dp0

set PATH=C:\raylib\w64devkit\bin;%PATH%

echo Compiling Segfault Engine...

gcc src/main.c src/terminal.c src/dive.c src/story.c src/options.c src/episodes/ep1.c -o game.exe -O2 -Wall -I C:\raylib\raylib\src -L C:\raylib\raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm

if %errorlevel% equ 0 (
    echo.
    echo -------------------------
    echo BUILD SUCCESSFUL
    echo -------------------------
    game.exe
) else (
    echo.
    echo -------------------------
    echo BUILD FAILED
    echo -------------------------
    pause
)
endlocal