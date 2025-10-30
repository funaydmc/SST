#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>

namespace StellaLauncher {
    class ProcessUtils {
    public:
        // Process enumeration
        static std::vector<DWORD> GetProcessesByName(const std::wstring& processName);
        static DWORD GetProcessIdByName(const std::wstring& processName);
        static std::wstring GetProcessNameById(DWORD processId);
        static std::vector<DWORD> GetAllProcessIds();

        // Process information
        static bool IsProcessRunning(DWORD processId);
        static bool IsProcessRunning(const std::wstring& processName);
        static std::wstring GetProcessPath(DWORD processId);
        static DWORD GetProcessParentId(DWORD processId);

        // Process manipulation
        static bool TerminateProcessById(DWORD processId);
        static bool TerminateProcessByName(const std::wstring& processName);
        static bool SuspendProcess(DWORD processId);
        static bool ResumeProcess(DWORD processId);

        // Window utilities
        static HWND FindMainWindow(DWORD processId);
        static HWND FindWindowByTitle(const std::wstring& title);
        static bool IsWindowVisible(HWND hwnd);
        static bool BringWindowToFront(HWND hwnd);

        // Module enumeration
        static std::vector<HMODULE> GetProcessModules(DWORD processId);
        static HMODULE GetModuleHandle(DWORD processId, const std::wstring& moduleName);
        static std::wstring GetModulePath(DWORD processId, HMODULE hModule);

        // Memory utilities
        static SIZE_T GetProcessMemoryUsage(DWORD processId);
        static bool ReadProcessMemory(DWORD processId, LPCVOID address, LPVOID buffer, SIZE_T size);
        static bool WriteProcessMemory(DWORD processId, LPVOID address, LPCVOID buffer, SIZE_T size);

        // Privilege utilities
        static bool EnableDebugPrivilege();
        static bool SetProcessPrivilege(const std::wstring& privilege, bool enable);

        // Launch utilities
        static PROCESS_INFORMATION LaunchProcess(const std::wstring& executable, const std::wstring& arguments = L"", const std::wstring& workingDirectory = L"");
        static PROCESS_INFORMATION LaunchProcessSuspended(const std::wstring& executable, const std::wstring& arguments = L"", const std::wstring& workingDirectory = L"");

    private:
        // Helper functions
        static bool EnablePrivilege(HANDLE hToken, LPCWSTR privilege);
        static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

        // Structure for window enumeration
        struct EnumWindowsData {
            DWORD processId;
            HWND hwnd;
        };
    };
}
