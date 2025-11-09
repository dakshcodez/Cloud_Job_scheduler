@echo off
REM Build script for Windows (MinGW/MSYS2)
REM Usage: build.bat

echo ========================================
echo Cloud Job Scheduler Simulator - Build
echo ========================================
echo.

REM Check if gcc is available
where gcc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: gcc not found in PATH!
    echo.
    echo Please install MinGW-w64 or MSYS2 and add it to your PATH.
    echo Or use WSL (Windows Subsystem for Linux).
    echo.
    pause
    exit /b 1
)

echo Compiling source files...
echo.

REM Compile all source files
gcc -Wall -Wextra -std=c11 -g -o scheduler.exe ^
    main.c ^
    priority_queue.c ^
    hash_table.c ^
    node_list.c ^
    job_list.c ^
    scheduler.c ^
    persistence.c

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Build successful!
    echo ========================================
    echo.
    echo Executable: scheduler.exe
    echo.
    echo To run: scheduler.exe
    echo.
) else (
    echo.
    echo ========================================
    echo Build failed!
    echo ========================================
    echo.
    pause
    exit /b 1
)

REM Ask if user wants to run
set /p RUN="Run scheduler now? (y/n): "
if /i "%RUN%"=="y" (
    echo.
    echo Starting scheduler...
    echo.
    scheduler.exe
)

pause


