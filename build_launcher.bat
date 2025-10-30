@echo off
setlocal enabledelayedexpansion

echo.
echo ================================================================
echo                    StellaSora Launcher Builder
echo ================================================================
echo.

:: Check for Visual Studio installation
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo [ERROR] Visual Studio installer not found!
    echo Please install Visual Studio 2022 with C++ development tools.
    pause
    exit /b 1
)

:: Find Visual Studio installation
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    set "VSINSTALLDIR=%%i"
)

if not defined VSINSTALLDIR (
    echo [ERROR] Visual Studio with C++ tools not found!
    echo Please install Visual Studio 2022 with C++ development tools.
    pause
    exit /b 1
)

echo [INFO] Found Visual Studio at: %VSINSTALLDIR%

:: Setup build environment
set "VCVARSALL=%VSINSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat"
if not exist "%VCVARSALL%" (
    echo [ERROR] vcvarsall.bat not found!
    pause
    exit /b 1
)

:: Determine build configuration
set BUILD_CONFIG=Release
set BUILD_PLATFORM=x64

if "%1"=="debug" set BUILD_CONFIG=Debug
if "%1"=="x86" set BUILD_PLATFORM=x86
if "%1"=="win32" set BUILD_PLATFORM=x86

echo [INFO] Building configuration: %BUILD_CONFIG%
echo [INFO] Building platform: %BUILD_PLATFORM%
echo.

:: Create output directory
if not exist "bin\%BUILD_CONFIG%-%BUILD_PLATFORM%" (
    mkdir "bin\%BUILD_CONFIG%-%BUILD_PLATFORM%"
    echo [INFO] Created output directory: bin\%BUILD_CONFIG%-%BUILD_PLATFORM%
)

:: Setup Visual Studio environment
echo [INFO] Setting up Visual Studio environment...
call "%VCVARSALL%" %BUILD_PLATFORM% >nul 2>&1

if errorlevel 1 (
    echo [ERROR] Failed to setup Visual Studio environment!
    pause
    exit /b 1
)

:: Build the solution
echo [INFO] Building StellaSora Launcher...
echo.

msbuild "StellaSora-Tool.sln" /p:Configuration=%BUILD_CONFIG% /p:Platform=%BUILD_PLATFORM% /m /v:minimal

if errorlevel 1 (
    echo.
    echo [ERROR] Build failed!
    echo Check the output above for errors.
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Build completed successfully!
echo.
echo Output files:
dir "bin\%BUILD_CONFIG%-%BUILD_PLATFORM%" /b 2>nul

echo.
echo ================================================================
echo                      Build Instructions
echo ================================================================
echo.
echo The launcher has been built successfully. To use:
echo.
echo 1. Copy StellaSora-Tool.dll to the same directory as the launcher
echo 2. Run StellaSora-Launcher.exe as Administrator
echo 3. Use -h flag for help and options
echo.
echo Example usage:
echo   StellaSora-Launcher.exe -a
echo   StellaSora-Launcher.exe -g "C:\Game\starrail.exe" -d "StellaSora-Tool.dll" -a
echo.
echo ================================================================

pause
