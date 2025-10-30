# StellaSora Launcher

A sophisticated game launcher with integrated anti-cheat bypass and automatic DLL injection capabilities.

## Features

- **Game Process Launch**: Automatically launches the target game executable
- **AntiCheatExpert (ACE) Bypass**: Disables and bypasses ACE protection systems
- **Memory Management**: Advanced memory allocation and protection mechanisms
- **DLL Injection**: Multiple injection methods including standard and manual mapping
- **Process Monitoring**: Real-time monitoring of game processes
- **Auto-Detection**: Automatically finds game and DLL files

## Architecture

### Core Components

1. **Launcher (`launcher.cpp/h`)**
   - Main orchestration class
   - Manages game launching and DLL injection workflow
   - Provides high-level API for launcher operations

2. **AntiCheat Bypass (`anticheat_bypass.cpp/h`)**
   - Implements ACE detection and bypass techniques
   - API hooking for filesystem and process queries
   - Memory protection and signature patching
   - Based on techniques from [Honkai-StarRail-ACE-B](https://github.com/gmh5225/Honkai-StarRail-ACE-B)

3. **DLL Injector (`dll_injector.cpp/h`)**
   - Standard DLL injection via CreateRemoteThread
   - Manual mapping for advanced scenarios
   - Memory allocation and process manipulation
   - Import resolution and relocation handling

4. **Process Utilities (`process_utils.cpp/h`)**
   - Process enumeration and manipulation
   - Window finding and management
   - Module enumeration and information
   - Privilege management and debug access

## Usage

### Command Line Interface

```bash
StellaSora-Launcher.exe [options]

Options:
  -g, --game <path>     Path to game executable
  -d, --dll <path>      Path to StellaSora DLL
  -a, --auto-inject     Automatically inject DLL after game launch
  -w, --wait            Wait for game window before injection
  -h, --help            Show help message
```

### Example Usage

```bash
# Auto-detect game and DLL, with automatic injection
StellaSora-Launcher.exe -a

# Specify custom paths
StellaSora-Launcher.exe -g "C:\Game\starrail.exe" -d "StellaSora-Tool.dll" -a

# Manual injection mode
StellaSora-Launcher.exe -g "C:\Game\starrail.exe" -d "StellaSora-Tool.dll" -w
```

### Auto-Detection

The launcher can automatically detect:
- **Game Executable**: Searches common installation paths for Star Rail
- **StellaSora DLL**: Looks for DLL files in the current directory

## Anti-Cheat Bypass

### ACE Bypass Techniques

1. **API Hooking**
   - `CreateFileW`: Blocks ACE device access (`\\\\.\\ACE-BASE`)
   - `NtQuerySystemInformation`: Hides debugger processes
   - `NtQueryInformationProcess`: Masks debugging flags

2. **Memory Patching**
   - Signature-based detection bypass
   - Runtime code modification
   - Protection flag manipulation

3. **Process Hiding**
   - Debugger process concealment
   - Memory scanning evasion
   - File system check bypass

### Implementation Details

The bypass system works in two phases:

1. **Pre-Launch**: Install hooks and prepare bypass mechanisms
2. **Post-Launch**: Apply patches to the running game process

## DLL Injection Methods

### Standard Injection
- Uses `CreateRemoteThread` + `LoadLibraryW`
- Compatible with most scenarios
- Easier detection by anti-cheat systems

### Manual Mapping
- Direct memory mapping without Windows loader
- Harder to detect
- Full control over loading process
- Custom import resolution

## Build Requirements

- Visual Studio 2022 (v143 toolset)
- Windows 10 SDK
- C++20 standard support
- Administrator privileges (for process injection)

## Project Structure

```
launcher/
├── src/
│   ├── main.cpp              # Entry point and CLI parsing
│   ├── launcher.cpp/h        # Main launcher logic
│   ├── anticheat_bypass.cpp/h # ACE bypass implementation
│   ├── dll_injector.cpp/h    # DLL injection methods
│   └── process_utils.cpp/h   # Process manipulation utilities
└── README.md                 # This file
```

## Security Considerations

- Requires administrator privileges for process manipulation
- Uses debug privileges for memory access
- Implements anti-cheat evasion techniques
- **For educational and research purposes only**

## Dependencies

- Windows API (Kernel32, Ntdll, Psapi)
- Tool Help API for process enumeration
- Process and Thread API for manipulation

## Troubleshooting

### Common Issues

1. **Access Denied**: Run as administrator
2. **Process Not Found**: Check game installation path
3. **DLL Not Found**: Verify StellaSora DLL location
4. **Injection Failed**: Try different injection method

### Debug Information

The launcher provides detailed logging:
- `[INFO]`: General information
- `[SUCCESS]`: Operation completed successfully
- `[WARNING]`: Non-fatal issues
- `[ERROR]`: Critical failures

## References

- [Honkai StarRail ACE Bypass](https://github.com/gmh5225/Honkai-StarRail-ACE-B)
- [Microsoft Detours](https://github.com/microsoft/Detours)
- [Windows API Documentation](https://docs.microsoft.com/en-us/windows/win32/api/)

## Disclaimer

This software is provided for educational and research purposes only. The authors are not responsible for any misuse or damage caused by this software. Use at your own risk and ensure compliance with applicable laws and terms of service.
