#pragma once
#include <windows.h>
#include <string>
#include <vector>

class ProcessUtils {
public:
    static DWORD LaunchProcess(const std::wstring& exePath, const std::wstring& commandLine = L"", bool suspended = false);
    static bool WaitForProcessReady(DWORD processId, DWORD timeoutMs = 30000);
    static HANDLE OpenProcessByName(const std::wstring& processName);
    static DWORD GetProcessIdByName(const std::wstring& processName);
    static bool IsProcessRunning(DWORD processId);
    static bool TerminateProcessById(DWORD processId);
    static std::vector<DWORD> GetAllProcessIds(const std::wstring& processName);
    
    // Memory operations
    static bool AllocateMemoryInProcess(HANDLE hProcess, SIZE_T size, LPVOID* allocatedAddress);
    static bool WriteMemoryToProcess(HANDLE hProcess, LPVOID address, const void* data, SIZE_T size);
    static bool ReadMemoryFromProcess(HANDLE hProcess, LPCVOID address, void* buffer, SIZE_T size);
    static bool FreeMemoryInProcess(HANDLE hProcess, LPVOID address);

private:
    static bool SetDebugPrivilege();
};