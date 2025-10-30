#include <iostream>
#include <filesystem>
#include "launcher.h"
#include "process_utils.h"
#include "anticheat_bypass.h"
#include "dll_injector.h"

int main(int argc, char* argv[]) {
    std::wcout << L"=== StellaSora Launcher v1.0 ===" << std::endl;
    std::wcout << L"Game Launcher & DLL Injector" << std::endl;
    std::wcout << L"===============================" << std::endl;

    // Default paths - modify these as needed
    std::wstring gamePath = L"";
    std::wstring dllPath = L"bin\\Release-x64\\StellaSora-Tool.dll";

    // Parse command line arguments
    if (argc >= 2) {
        // Convert to wide string
        std::string gamePathStr(argv[1]);
        gamePath = std::wstring(gamePathStr.begin(), gamePathStr.end());
    }

    if (argc >= 3) {
        // Convert to wide string
        std::string dllPathStr(argv[2]);
        dllPath = std::wstring(dllPathStr.begin(), dllPathStr.end());
    }

    // Prompt for game path if not provided
    if (gamePath.empty()) {
        std::wcout << L"Enter game executable path: ";
        std::getline(std::wcin, gamePath);
    }

    // Check if DLL exists
    if (!std::filesystem::exists(dllPath)) {
        std::wcerr << L"Error: DLL not found at: " << dllPath << std::endl;
        std::wcout << L"Press Enter to exit...";
        std::wcin.get();
        return 1;
    }

    // Initialize launcher
    Launcher launcher;
    if (!launcher.Initialize()) {
        std::wcerr << L"Failed to initialize launcher!" << std::endl;
        std::wcout << L"Press Enter to exit...";
        std::wcin.get();
        return 1;
    }

    // Launch game with injection
    std::wcout << L"Launching game and injecting DLL..." << std::endl;
    if (launcher.LaunchGameWithInjection(gamePath, dllPath)) {
        std::wcout << L"Successfully launched game with DLL injection!" << std::endl;
        std::wcout << L"Game is running with StellaSora-Tool loaded." << std::endl;
    } else {
        std::wcerr << L"Failed to launch game or inject DLL!" << std::endl;
        std::wcout << L"Press Enter to exit...";
        std::wcin.get();
        return 1;
    }

    std::wcout << L"Launcher completed. Press Enter to exit...";
    std::wcin.get();
    return 0;
}