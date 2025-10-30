#include <Windows.h>
#include <iostream>
#include <string>
#include <filesystem>
#include "launcher.h"
#include "process_utils.h"

void PrintBanner() {
    std::wcout << L"╔══════════════════════════════════════════════════════════════╗" << std::endl;
    std::wcout << L"║                    StellaSora Launcher                       ║" << std::endl;
    std::wcout << L"║              Game Launcher with ACE Bypass                  ║" << std::endl;
    std::wcout << L"║                                                              ║" << std::endl;
    std::wcout << L"║  Features:                                                   ║" << std::endl;
    std::wcout << L"║  • Start Game Process                                       ║" << std::endl;
    std::wcout << L"║  • Disable AntiCheatExpert (ACE)                           ║" << std::endl;
    std::wcout << L"║  • Memory Allocation & Protection                           ║" << std::endl;
    std::wcout << L"║  • Automatic DLL Injection                                  ║" << std::endl;
    std::wcout << L"╚══════════════════════════════════════════════════════════════╝" << std::endl;
    std::wcout << std::endl;
}

void PrintUsage() {
    std::wcout << L"Usage:" << std::endl;
    std::wcout << L"  StellaSora-Launcher.exe [options]" << std::endl;
    std::wcout << L"" << std::endl;
    std::wcout << L"Options:" << std::endl;
    std::wcout << L"  -g, --game <path>     Path to game executable" << std::endl;
    std::wcout << L"  -d, --dll <path>      Path to StellaSora DLL" << std::endl;
    std::wcout << L"  -a, --auto-inject     Automatically inject DLL after game launch" << std::endl;
    std::wcout << L"  -w, --wait            Wait for game window before injection" << std::endl;
    std::wcout << L"  -h, --help            Show this help message" << std::endl;
    std::wcout << std::endl;
    std::wcout << L"Example:" << std::endl;
    std::wcout << L"  StellaSora-Launcher.exe -g \"C:\\Game\\Game.exe\" -d \"StellaSora-Tool.dll\" -a" << std::endl;
}

bool ParseArguments(int argc, wchar_t* argv[], std::wstring& gamePath, std::wstring& dllPath, bool& autoInject, bool& waitForWindow) {
    for (int i = 1; i < argc; i++) {
        std::wstring arg = argv[i];
        
        if (arg == L"-g" || arg == L"--game") {
            if (i + 1 < argc) {
                gamePath = argv[++i];
            } else {
                std::wcout << L"Error: --game requires a path argument" << std::endl;
                return false;
            }
        }
        else if (arg == L"-d" || arg == L"--dll") {
            if (i + 1 < argc) {
                dllPath = argv[++i];
            } else {
                std::wcout << L"Error: --dll requires a path argument" << std::endl;
                return false;
            }
        }
        else if (arg == L"-a" || arg == L"--auto-inject") {
            autoInject = true;
        }
        else if (arg == L"-w" || arg == L"--wait") {
            waitForWindow = true;
        }
        else if (arg == L"-h" || arg == L"--help") {
            PrintUsage();
            return false;
        }
        else {
            std::wcout << L"Error: Unknown argument: " << arg << std::endl;
            return false;
        }
    }
    
    return true;
}

std::wstring FindGameExecutable() {
    // Common game paths to search
    std::vector<std::wstring> searchPaths = {
        L"C:\\Program Files\\miHoYo\\games\\",
        L"C:\\Program Files (x86)\\miHoYo\\games\\",
        L"D:\\Program Files\\miHoYo\\games\\",
        L"E:\\Program Files\\miHoYo\\games\\",
        L"C:\\Games\\",
        L"D:\\Games\\",
        L"E:\\Games\\"
    };
    
    // Common executable names
    std::vector<std::wstring> exeNames = {
        L"starrail.exe",
        L"StarRail.exe",
        L"honkai_star_rail.exe",
        L"game.exe"
    };
    
    for (const auto& basePath : searchPaths) {
        for (const auto& exeName : exeNames) {
            std::wstring fullPath = basePath + exeName;
            if (std::filesystem::exists(fullPath)) {
                return fullPath;
            }
        }
    }
    
    return L"";
}

std::wstring FindStellasoraDLL() {
    // Get current directory and look for the DLL
    wchar_t currentPath[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, currentPath);
    
    std::vector<std::wstring> dllNames = {
        L"StellaSora-Tool.dll",
        L"StellaSora.dll",
        L"stellasora.dll"
    };
    
    for (const auto& dllName : dllNames) {
        std::wstring fullPath = std::wstring(currentPath) + L"\\" + dllName;
        if (std::filesystem::exists(fullPath)) {
            return fullPath;
        }
    }
    
    return L"";
}

int wmain(int argc, wchar_t* argv[]) {
    // Set console to UTF-8 for proper Unicode output
    SetConsoleOutputCP(CP_UTF8);
    
    PrintBanner();
    
    // Enable debug privileges for process injection
    if (!StellaLauncher::ProcessUtils::EnableDebugPrivilege()) {
        std::wcout << L"[WARNING] Failed to enable debug privilege. Some features may not work." << std::endl;
    }
    
    // Parse command line arguments
    std::wstring gamePath, dllPath;
    bool autoInject = false;
    bool waitForWindow = false;
    
    if (!ParseArguments(argc, argv, gamePath, dllPath, autoInject, waitForWindow)) {
        return 1;
    }
    
    // Auto-detect game path if not provided
    if (gamePath.empty()) {
        std::wcout << L"[INFO] Searching for game executable..." << std::endl;
        gamePath = FindGameExecutable();
        if (gamePath.empty()) {
            std::wcout << L"[ERROR] Could not find game executable. Please specify with -g option." << std::endl;
            PrintUsage();
            return 1;
        }
        std::wcout << L"[INFO] Found game at: " << gamePath << std::endl;
    }
    
    // Auto-detect DLL path if not provided
    if (dllPath.empty()) {
        std::wcout << L"[INFO] Searching for StellaSora DLL..." << std::endl;
        dllPath = FindStellasoraDLL();
        if (dllPath.empty()) {
            std::wcout << L"[ERROR] Could not find StellaSora DLL. Please specify with -d option." << std::endl;
            PrintUsage();
            return 1;
        }
        std::wcout << L"[INFO] Found DLL at: " << dllPath << std::endl;
    }
    
    // Initialize launcher
    StellaLauncher::Launcher launcher;
    
    if (!launcher.Initialize()) {
        std::wcout << L"[ERROR] Failed to initialize launcher" << std::endl;
        return 1;
    }
    
    // Configure launcher
    launcher.SetGamePath(gamePath);
    launcher.SetDLLPath(dllPath);
    launcher.SetAutoInject(autoInject);
    
    std::wcout << L"[INFO] Configuration:" << std::endl;
    std::wcout << L"       Game Path: " << gamePath << std::endl;
    std::wcout << L"       DLL Path:  " << dllPath << std::endl;
    std::wcout << L"       Auto Inject: " << (autoInject ? L"Yes" : L"No") << std::endl;
    std::wcout << std::endl;
    
    // Launch the game
    std::wcout << L"[INFO] Launching game..." << std::endl;
    
    if (!launcher.LaunchGame()) {
        std::wcout << L"[ERROR] Failed to launch game" << std::endl;
        return 1;
    }
    
    std::wcout << L"[SUCCESS] Game launched successfully!" << std::endl;
    
    // Wait for game window if requested
    if (waitForWindow) {
        std::wcout << L"[INFO] Waiting for game window..." << std::endl;
        // Implementation will be in launcher.cpp
    }
    
    // Inject DLL if auto-inject is enabled
    if (autoInject) {
        std::wcout << L"[INFO] Injecting StellaSora DLL..." << std::endl;
        
        // Wait a bit for the game to initialize
        Sleep(3000);
        
        if (launcher.InjectDLL()) {
            std::wcout << L"[SUCCESS] DLL injected successfully!" << std::endl;
            std::wcout << L"[INFO] StellaSora tools are now active in the game." << std::endl;
        } else {
            std::wcout << L"[ERROR] Failed to inject DLL. You may need to inject manually." << std::endl;
        }
    }
    
    // Keep launcher alive to monitor the game
    std::wcout << L"[INFO] Launcher is now monitoring the game process..." << std::endl;
    std::wcout << L"[INFO] Press Ctrl+C to exit or close this window." << std::endl;
    
    // Monitor game process
    while (launcher.IsGameRunning()) {
        Sleep(1000);
    }
    
    std::wcout << L"[INFO] Game process has ended. Cleaning up..." << std::endl;
    launcher.Cleanup();
    
    return 0;
}
