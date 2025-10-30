#include "launcher.h"
#include "process_utils.h"
#include "anticheat_bypass.h"
#include "dll_injector.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <tlhelp32.h>

Launcher::Launcher() : m_initialized(false) {
    ShowConsole();
}

Launcher::~Launcher() {
    // Cleanup if needed
}

bool Launcher::Initialize() {
    LogMessage("Initializing launcher...");
    
    // Set console title
    SetConsoleTitle(L"StellaSora Launcher");
    
    m_initialized = true;
    LogMessage("Launcher initialized successfully");
    return true;
}

bool Launcher::LaunchGameWithInjection(const std::wstring& gamePath, const std::wstring& dllPath) {
    if (!m_initialized) {
        LogMessage("Error: Launcher not initialized");
        return false;
    }

    // Validate files exist
    if (!ValidateFiles(gamePath, dllPath)) {
        return false;
    }

    LogMessage("Step 1: Disabling AntiCheat systems...");
    if (!AntiCheatBypass::DisableAntiCheatExpert()) {
        LogMessage("Warning: Could not fully disable AntiCheat (may still work)");
    } else {
        LogMessage("AntiCheat systems disabled successfully");
    }

    // Wait a moment for anti-cheat to be fully disabled
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    LogMessage("Step 2: Launching game process...");
    DWORD processId = ProcessUtils::LaunchProcess(gamePath, L"", true); // Launch suspended
    if (processId == 0) {
        LogMessage("Error: Failed to launch game process");
        return false;
    }

    LogMessage("Game process launched (PID: " + std::to_string(processId) + ")");

    LogMessage("Step 3: Applying anti-cheat bypass to process...");
    if (!AntiCheatBypass::BypassProcessProtection(processId)) {
        LogMessage("Warning: Could not apply all process protections");
    }

    LogMessage("Step 4: Allocating memory and injecting DLL...");
    DllInjector injector;
    if (!injector.InjectDll(processId, dllPath, InjectionMethod::LoadLibrary)) {
        LogMessage("Error: DLL injection failed");
        ProcessUtils::TerminateProcessById(processId);
        return false;
    }

    LogMessage("DLL injected successfully");

    LogMessage("Step 5: Resuming game process...");
    // Resume the main thread of the suspended process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (hProcess) {
        // Find and resume the main thread
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (hSnapshot != INVALID_HANDLE_VALUE) {
            THREADENTRY32 te;
            te.dwSize = sizeof(THREADENTRY32);
            
            if (Thread32First(hSnapshot, &te)) {
                do {
                    if (te.th32OwnerProcessID == processId) {
                        HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                        if (hThread) {
                            ResumeThread(hThread);
                            CloseHandle(hThread);
                            break; // Resume first thread found
                        }
                    }
                } while (Thread32Next(hSnapshot, &te));
            }
            CloseHandle(hSnapshot);
        }
        CloseHandle(hProcess);
    }

    LogMessage("Game launched successfully with DLL injection!");
    return true;
}

bool Launcher::ValidateFiles(const std::wstring& gamePath, const std::wstring& dllPath) {
    if (!std::filesystem::exists(gamePath)) {
        LogMessage("Error: Game executable not found: " + std::string(gamePath.begin(), gamePath.end()));
        return false;
    }

    if (!std::filesystem::exists(dllPath)) {
        LogMessage("Error: DLL not found: " + std::string(dllPath.begin(), dllPath.end()));
        return false;
    }

    return true;
}

void Launcher::ShowConsole() {
    // Allocate console for this GUI application
    if (!AllocConsole()) {
        // Console might already exist
        return;
    }

    // Redirect stdout, stdin, stderr to console
    FILE* pCout;
    FILE* pCin;
    FILE* pCerr;
    
    freopen_s(&pCout, "CONOUT$", "w", stdout);
    freopen_s(&pCin, "CONIN$", "r", stdin);
    freopen_s(&pCerr, "CONOUT$", "w", stderr);

    // Make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    std::ios::sync_with_stdio(true);
    std::wcout.clear();
    std::cout.clear();
    std::wcerr.clear();
    std::cerr.clear();
    std::wcin.clear();
    std::cin.clear();
}

void Launcher::LogMessage(const std::string& message) {
    std::cout << "[LAUNCHER] " << message << std::endl;
}