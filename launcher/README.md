# StellaSora Launcher

A game launcher and DLL injector for the StellaSora-Tool project. This executable handles game launching, anti-cheat bypassing, memory allocation, and DLL injection.

## Features

- **Game Process Launching**: Starts the target game executable
- **Anti-Cheat Bypassing**: Disables AntiCheatExpert and similar protection systems
- **Memory Management**: Allocates memory in target process for injection
- **DLL Injection**: Injects the main StellaSora-Tool.dll into the game process
- **Multiple Injection Methods**: Supports LoadLibrary, Manual Map, and Thread Hijacking

## Usage

### Command Line
```bash
StellaSora-Launcher.exe [game_path] [dll_path]
```

### Interactive Mode
If no arguments are provided, the launcher will prompt for the game executable path.

### Example
```bash
# Launch with specific paths
StellaSora-Launcher.exe "C:\Games\MyGame\game.exe" "bin\Release-x64\StellaSora-Tool.dll"

# Interactive mode
StellaSora-Launcher.exe
```

## Build Requirements

- Visual Studio 2022 or later
- Windows 10 SDK
- C++20 standard library

## Build Configuration

The project is configured to build as a console application with the following features:
- **Target**: Windows executable (.exe)
- **Architecture**: x64 (recommended)
- **Dependencies**: psapi.lib, ntdll.lib
- **Privileges**: Requires administrator privileges for process manipulation

## Project Structure

```
launcher/
├── src/
│   ├── main.cpp              # Entry point and command line handling
│   ├── launcher.h/cpp        # Main launcher class
│   ├── process_utils.h/cpp   # Process manipulation utilities
│   ├── anticheat_bypass.h/cpp# Anti-cheat bypassing functionality
│   └── dll_injector.h/cpp    # DLL injection implementation
```

## Functionality Overview

### 1. Process Launch
- Creates the game process in suspended state
- Applies debug privileges for process manipulation
- Validates executable paths before launch

### 2. Anti-Cheat Bypass
- Terminates known anti-cheat processes (AntiCheatExpert, BattlEye, etc.)
- Disables anti-cheat services
- Applies process protection bypass
- Patches detection routines (placeholder implementation)

### 3. Memory Operations
- Allocates executable memory in target process
- Writes injection payloads to target memory
- Manages memory cleanup after injection

### 4. DLL Injection
- **LoadLibrary Method**: Standard injection using CreateRemoteThread
- **Manual Map Method**: Advanced injection without module list entry (placeholder)
- **Thread Hijacking Method**: Stealth injection via thread context manipulation (placeholder)

## Important Notes

⚠️ **Administrator Rights Required**: The launcher needs elevated privileges to manipulate other processes and bypass security systems.

⚠️ **Anti-Virus Detection**: This software may be detected by antivirus programs due to its process manipulation capabilities.

⚠️ **Educational Purpose**: This tool is for educational and research purposes. Use responsibly and in accordance with applicable laws.

## Error Handling

The launcher provides detailed console output for debugging:
- Process creation status
- Anti-cheat bypass results
- Memory allocation success/failure
- DLL injection progress and errors

## Compatibility

- Windows 10/11 (x64)
- Games using AntiCheatExpert protection
- IL2CPP Unity games (primary target)