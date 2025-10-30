# StellaSora Project Structure

## Overview
The StellaSora project now consists of two subprojects:

1. **StellaSora-Tool** (DLL) - The main cheat/tool library
2. **StellaSora-Launcher** (EXE) - Game launcher and DLL injector

## Project Files

### Solution & Project Files
- `StellaSora-Tool.sln` - Main solution file containing both projects
- `StellaSora.vcxproj` - Main DLL project (existing)
- `StellaSora-Launcher.vcxproj` - New launcher executable project

### Launcher Project Structure
```
launcher/
├── src/
│   ├── main.cpp              # Entry point, command line handling
│   ├── launcher.h/cpp        # Main launcher orchestration
│   ├── process_utils.h/cpp   # Process creation and memory management
│   ├── anticheat_bypass.h/cpp# Anti-cheat system bypassing
│   └── dll_injector.h/cpp    # DLL injection methods
└── README.md                 # Launcher documentation
```

## Build Configuration

### Launcher Project Settings
- **Type**: Console Application (.exe)
- **Target**: Windows x64
- **Dependencies**: 
  - `psapi.lib` (Process Status API)
  - `ntdll.lib` (NT Layer DLL)
- **Privileges**: Requires Administrator rights

### Build Outputs
- **Main DLL**: `bin\Release-x64\StellaSora-Tool.dll`
- **Launcher**: `bin\Release-x64\StellaSora-Launcher.exe`

## Functionality

### StellaSora-Launcher.exe Features

1. **Game Process Management**
   - Launch target game executable
   - Create process in suspended state
   - Resume after injection complete

2. **Anti-Cheat Bypassing**
   - Terminate AntiCheatExpert processes
   - Disable anti-cheat services
   - Apply process protection bypass
   - Hook NT functions to hide presence

3. **Memory Operations**
   - Allocate executable memory in target process
   - Write injection payloads
   - Manage memory cleanup

4. **DLL Injection Methods**
   - **LoadLibrary**: Standard injection via CreateRemoteThread
   - **Manual Map**: Advanced stealth injection (framework ready)
   - **Thread Hijacking**: Context manipulation injection (framework ready)

## Usage

### Command Line Usage
```bash
# Interactive mode (prompts for game path)
StellaSora-Launcher.exe

# Specify game path
StellaSora-Launcher.exe "C:\Games\Game.exe"

# Specify both game and DLL paths
StellaSora-Launcher.exe "C:\Games\Game.exe" "custom\path\StellaSora-Tool.dll"
```

### Build Instructions
```bash
# Option 1: Use provided batch file
build_launcher.bat

# Option 2: Manual MSBuild
msbuild StellaSora-Tool.sln /p:Configuration=Release /p:Platform=x64
```

## Security Considerations

⚠️ **Important Notes:**
- Requires Administrator privileges
- May trigger anti-virus detection
- Educational/research use only
- Bypasses game protection systems

## Process Flow

1. **Initialization**
   - Validate file paths
   - Enable debug privileges
   - Initialize console output

2. **Anti-Cheat Bypass**
   - Terminate known anti-cheat processes
   - Disable anti-cheat services
   - Prepare bypass mechanisms

3. **Game Launch**
   - Create game process (suspended)
   - Apply process protection bypass
   - Allocate memory for injection

4. **DLL Injection**
   - Write DLL path to target memory
   - Create remote thread for LoadLibrary
   - Verify injection success

5. **Process Resume**
   - Resume main game thread
   - Monitor injection status
   - Provide user feedback

## Integration with Main Project

The launcher is designed to work seamlessly with the existing StellaSora-Tool:
- Automatically locates the main DLL in default build output
- Compatible with existing cheat features (HitEffect, NoCD, etc.)
- Maintains ImGui integration for in-game interface
- Preserves IL2CPP game compatibility

## Future Enhancements

Planned improvements for the launcher:
- GUI interface option
- Configuration file support
- Multiple injection method selection
- Game-specific launch profiles
- Enhanced anti-cheat detection and bypassing