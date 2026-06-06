@echo off
echo Compiling the project...

g++ main.cpp Engine.cpp camera.cpp lighting.cpp basic.cpp objloader.cpp -o main.exe -I "C:\Users\varsh\OneDrive\Desktop\sdl\i686-w64-mingw32\include" -L "C:\Users\varsh\OneDrive\Desktop\sdl\i686-w64-mingw32\lib" -lSDL3

if %errorlevel% equ 0 (
    echo Compilation successful! Running main.exe...
    main.exe
) else (
    echo.
    echo COMPILATION FAILED! Look at the errors above.
    pause
)