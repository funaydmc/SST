@echo off
echo Verifying StellaSora Build Configuration...
echo ==========================================

echo.
echo Checking solution file...
if exist "StellaSora-Tool.sln" (
    echo ✓ Solution file found
) else (
    echo ✗ Solution file missing: StellaSora-Tool.sln
    goto :error
)

echo.
echo Checking project files...
if exist "StellaSora.vcxproj" (
    echo ✓ Main project found: StellaSora.vcxproj
) else (
    echo ✗ Main project missing: StellaSora.vcxproj
    goto :error
)

if exist "StellaSora-Launcher.vcxproj" (
    echo ✓ Launcher project found: StellaSora-Launcher.vcxproj
) else (
    echo ✗ Launcher project missing: StellaSora-Launcher.vcxproj
    goto :error
)

echo.
echo Checking source files...
if exist "launcher\src\main.cpp" (
    echo ✓ Launcher source found
) else (
    echo ✗ Launcher source missing: launcher\src\main.cpp
    goto :error
)

if exist "src\main.cpp" (
    echo ✓ Main DLL source found
) else (
    echo ✗ Main DLL source missing: src\main.cpp
    goto :error
)

echo.
echo Checking workflow file...
if exist ".github\workflows\manual-build.yml" (
    echo ✓ GitHub Actions workflow found
    
    REM Check if launcher is mentioned in the workflow
    findstr /i "launcher" ".github\workflows\manual-build.yml" >nul
    if %errorlevel% equ 0 (
        echo ✓ Launcher integration found in workflow
    ) else (
        echo ✗ Launcher not integrated in workflow
        goto :error
    )
) else (
    echo ✗ GitHub Actions workflow missing
    goto :error
)

echo.
echo Checking dependencies...
if exist "vendor\detours\detours.h" (
    echo ✓ Detours library found
) else (
    echo ! Detours library not found (may cause build issues)
)

if exist "vendor\ImGui\imgui.h" (
    echo ✓ ImGui library found
) else (
    echo ! ImGui library not found (may cause build issues)
)

echo.
echo ========================================
echo ✓ Configuration verification completed!
echo.
echo The build should work with:
echo - Local build: build_launcher.bat
echo - GitHub Actions: Manual Build workflow
echo.
echo Both projects will be built:
echo 1. StellaSora-Tool.dll (Main cheat library)
echo 2. StellaSora-Launcher.exe (Game launcher and injector)
echo ========================================
pause
exit /b 0

:error
echo.
echo ========================================
echo ✗ Configuration has issues!
echo Please fix the missing files before building.
echo ========================================
pause
exit /b 1