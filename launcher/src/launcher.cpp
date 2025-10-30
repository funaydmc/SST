#include "launcher.h"
#include "anticheat_bypass.h"
#include "dll_injector.h"
#include "process_utils.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>

namespace StellaLauncher {

Launcher::Launcher() 
    : m_autoInject(false)
    , m_gameProcessId(0)
    , m_initialized(false)
{
}

Launcher::~Launcher() {
    Cleanup();
}

bool Launcher::Initialize() {
    std::wcout << L"[INFO] Initializing launcher..." << std::endl;
    
    // Enable debug privileges for process manipulation
    if (!ProcessUtils::EnableDebugPrivilege()) {
        std::wcout << L"[WARNING] Could not enable debug privileges" << std::endl;
    }
    
    m_initialized = true;
    std::wcout << L"[SUCCESS] Launcher initialized" << std::endl;
    return true;
}

bool Launcher::LaunchGame() {
    if (!m_initialized) {
        std::wcout << L"[ERROR] Launcher not initialized" << std::endl;
        return false;
    }
    
    if (!ValidateGamePath()) {
        std::wcout << L"[ERROR] Invalid game path: " << m_gamePath << std::endl;
        return false;
    }
    
    // Check if game is already running
    DWORD existingPid = ProcessUtils::GetProcessIdByName(L"starrail.exe");
    if (existingPid != 0) {
        std::wcout << L"[WARNING] Game process already running (PID: " << existingPid << L")" << std::endl;
        m_gameProcessId = existingPid;
        return true;
    }
    
    std::wcout << L"[INFO] Starting anti-cheat bypass..." << std::endl;
    
    // Initialize anti-cheat bypass before launching game
    AntiCheatBypass bypass;
    if (!bypass.InitializeBypass()) {
        std::wcout << L"[ERROR] Failed to initialize anti-cheat bypass" << std::endl;
        return false;
    }
    
    std::wcout << L"[SUCCESS] Anti-cheat bypass initialized" << std::endl;
    
    // Launch the game process
    std::wstring gameDir = std::filesystem::path(m_gamePath).parent_path().wstring();
    PROCESS_INFORMATION procInfo = ProcessUtils::LaunchProcess(m_gamePath, L"", gameDir);
    
    if (procInfo.hProcess == nullptr) {
        std::wcout << L"[ERROR] Failed to launch game process" << std::endl;
        return false;
    }
    
    m_gameProcessId = procInfo.dwProcessId;
    
    std::wcout << L"[SUCCESS] Game process started (PID: " << m_gameProcessId << L")" << std::endl;
    
    // Wait for the process to initialize
    std::wcout << L"[INFO] Waiting for game process to initialize..." << std::endl;
    Sleep(2000);
    
    // Apply anti-cheat patches to the running process
    if (!bypass.PatchACEBinary(m_gameProcessId)) {
        std::wcout << L"[WARNING] Failed to patch ACE in game process" << std::endl;
    } else {
        std::wcout << L"[SUCCESS] ACE bypass applied to game process" << std::endl;
    }
    
    // Clean up process handles
    CloseHandle(procInfo.hProcess);
    CloseHandle(procInfo.hThread);
    
    return true;
}

bool Launcher::InjectDLL() {
    if (!m_initialized) {
        std::wcout << L"[ERROR] Launcher not initialized" << std::endl;
        return false;
    }
    
    if (!ValidateDLLPath()) {
        std::wcout << L"[ERROR] Invalid DLL path: " << m_dllPath << std::endl;
        return false;
    }
    
    if (m_gameProcessId == 0 || !ProcessUtils::IsProcessRunning(m_gameProcessId)) {
        std::wcout << L"[ERROR] Game process not running" << std::endl;
        return false;
    }
    
    // Wait for game window if needed
    if (!WaitForGameWindow()) {
        std::wcout << L"[WARNING] Game window not found, proceeding with injection..." << std::endl;
    }
    
    // Perform DLL injection
    DLLInjector injector;
    
    std::wcout << L"[INFO] Attempting DLL injection..." << std::endl;
    
    if (injector.InjectDLL(m_gameProcessId, m_dllPath)) {
        std::wcout << L"[SUCCESS] Standard DLL injection successful" << std::endl;
        return true;
    }
    
    std::wcout << L"[WARNING] Standard injection failed, trying manual mapping..." << std::endl;
    
    if (injector.ManualMapDLL(m_gameProcessId, m_dllPath)) {
        std::wcout << L"[SUCCESS] Manual map injection successful" << std::endl;
        return true;
    }
    
    std::wcout << L"[ERROR] All injection methods failed" << std::endl;
    return false;
}

void Launcher::Cleanup() {
    if (!m_initialized) {
        return;
    }
    
    std::wcout << L"[INFO] Cleaning up launcher..." << std::endl;
    
    m_gameProcessId = 0;
    m_initialized = false;
    
    std::wcout << L"[INFO] Cleanup completed" << std::endl;
}

void Launcher::SetGamePath(const std::wstring& path) {
    m_gamePath = path;
}

void Launcher::SetDLLPath(const std::wstring& path) {
    m_dllPath = path;
}

void Launcher::SetAutoInject(bool autoInject) {
    m_autoInject = autoInject;
}

bool Launcher::IsGameRunning() {
    if (m_gameProcessId == 0) {
        return false;
    }
    
    return ProcessUtils::IsProcessRunning(m_gameProcessId);
}

DWORD Launcher::GetGameProcessId() {
    return m_gameProcessId;
}

bool Launcher::ValidateGamePath() {
    if (m_gamePath.empty()) {
        return false;
    }
    
    return std::filesystem::exists(m_gamePath) && std::filesystem::is_regular_file(m_gamePath);
}

bool Launcher::ValidateDLLPath() {
    if (m_dllPath.empty()) {
        return false;
    }
    
    return std::filesystem::exists(m_dllPath) && std::filesystem::is_regular_file(m_dllPath);
}

bool Launcher::WaitForGameWindow() {
    std::wcout << L"[INFO] Waiting for game window..." << std::endl;
    
    // Wait up to 30 seconds for the game window
    for (int i = 0; i < 30; i++) {
        HWND gameWindow = ProcessUtils::FindMainWindow(m_gameProcessId);
        if (gameWindow != nullptr && ProcessUtils::IsWindowVisible(gameWindow)) {
            std::wcout << L"[SUCCESS] Game window found" << std::endl;
            return true;
        }
        
        // Also check for common game window titles
        std::vector<std::wstring> windowTitles = {
            L"Honkai: Star Rail",
            L"崩坏：星穹铁道",
            L"Star Rail",
            L"starrail"
        };
        
        for (const auto& title : windowTitles) {
            HWND window = ProcessUtils::FindWindowByTitle(title);
            if (window != nullptr) {
                std::wcout << L"[SUCCESS] Game window found by title: " << title << std::endl;
                return true;
            }
        }
        
        Sleep(1000);
    }
    
    std::wcout << L"[WARNING] Game window not found after 30 seconds" << std::endl;
    return false;
}

} // namespace StellaLauncher
