@echo off
echo Building StellaSora Complete Package...
echo =======================================

REM Check if MSBuild is available
where msbuild >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: MSBuild not found in PATH
    echo Please run this from Visual Studio Developer Command Prompt
    pause
    exit /b 1
)

REM Build the solution (both projects)
echo Building Release x64 configuration...
echo Projects: StellaSora-Tool.dll + StellaSora-Launcher.exe
echo.
msbuild StellaSora-Tool.sln /p:Configuration=Release /p:Platform=x64 /m

if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo.
echo Build outputs location: bin\Release-x64\
echo.

REM Check if files exist and show file sizes
if exist "bin\Release-x64\StellaSora-Tool.dll" (
    for %%F in ("bin\Release-x64\StellaSora-Tool.dll") do (
        echo ✓ StellaSora-Tool.dll found - %%~zF bytes
    )
) else (
    echo ✗ StellaSora-Tool.dll not found
)

if exist "bin\Release-x64\StellaSora-Launcher.exe" (
    for %%F in ("bin\Release-x64\StellaSora-Launcher.exe") do (
        echo ✓ StellaSora-Launcher.exe found - %%~zF bytes
    )
) else (
    echo ✗ StellaSora-Launcher.exe not found
)

echo.
echo === Usage Instructions ===
echo.
echo 1. Run the launcher:
echo    bin\Release-x64\StellaSora-Launcher.exe
echo.
echo 2. With game path:
echo    bin\Release-x64\StellaSora-Launcher.exe "C:\Games\YourGame.exe"
echo.
echo 3. With custom DLL path:
echo    bin\Release-x64\StellaSora-Launcher.exe "C:\Games\YourGame.exe" "custom\StellaSora-Tool.dll"
echo.
echo IMPORTANT: Run as Administrator for injection to work properly!
echo.
pause