#include "process_utils.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>

DWORD ProcessUtils::LaunchProcess(const std::wstring& exePath, const std::wstring& commandLine, bool suspended) {
    SetDebugPrivilege();

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    std::wstring fullCommandLine = exePath;
    if (!commandLine.empty()) {
        fullCommandLine += L" " + commandLine;
    }

    DWORD creationFlags = CREATE_NEW_CONSOLE;
    if (suspended) {
        creationFlags |= CREATE_SUSPENDED;
    }

    if (!CreateProcess(
        exePath.c_str(),
        const_cast<LPWSTR>(fullCommandLine.c_str()),
        nullptr,
        nullptr,
        FALSE,
        creationFlags,
        nullptr,
        nullptr,
        &si,
        &pi
    )) {
        std::wcerr << L"CreateProcess failed with error: " << GetLastError() << std::endl;
        return 0;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    
    return pi.dwProcessId;
}

bool ProcessUtils::WaitForProcessReady(DWORD processId, DWORD timeoutMs) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return false;
    }

    DWORD startTime = GetTickCount();
    bool ready = false;

    while (GetTickCount() - startTime < timeoutMs) {
        // Check if process is still running
        DWORD exitCode;
        if (!GetExitCodeProcess(hProcess, &exitCode) || exitCode != STILL_ACTIVE) {
            break;
        }

        // Check if process has finished initialization (basic check)
        HMODULE hMods[1024];
        DWORD cbNeeded;
        if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
            if (cbNeeded > sizeof(HMODULE)) { // More than just the main module loaded
                ready = true;
                break;
            }
        }

        Sleep(100);
    }

    CloseHandle(hProcess);
    return ready;
}

HANDLE ProcessUtils::OpenProcessByName(const std::wstring& processName) {
    DWORD processId = GetProcessIdByName(processName);
    if (processId == 0) {
        return nullptr;
    }

    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
}

DWORD ProcessUtils::GetProcessIdByName(const std::wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, processName.c_str()) == 0) {
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return 0;
}

bool ProcessUtils::IsProcessRunning(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (!hProcess) {
        return false;
    }

    DWORD exitCode;
    bool running = GetExitCodeProcess(hProcess, &exitCode) && (exitCode == STILL_ACTIVE);
    
    CloseHandle(hProcess);
    return running;
}

bool ProcessUtils::TerminateProcessById(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (!hProcess) {
        return false;
    }

    bool result = TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);
    return result;
}

std::vector<DWORD> ProcessUtils::GetAllProcessIds(const std::wstring& processName) {
    std::vector<DWORD> processIds;
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return processIds;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, processName.c_str()) == 0) {
                processIds.push_back(pe.th32ProcessID);
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return processIds;
}

bool ProcessUtils::AllocateMemoryInProcess(HANDLE hProcess, SIZE_T size, LPVOID* allocatedAddress) {
    *allocatedAddress = VirtualAllocEx(hProcess, nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    return (*allocatedAddress != nullptr);
}

bool ProcessUtils::WriteMemoryToProcess(HANDLE hProcess, LPVOID address, const void* data, SIZE_T size) {
    SIZE_T bytesWritten;
    return WriteProcessMemory(hProcess, address, data, size, &bytesWritten) && (bytesWritten == size);
}

bool ProcessUtils::ReadMemoryFromProcess(HANDLE hProcess, LPCVOID address, void* buffer, SIZE_T size) {
    SIZE_T bytesRead;
    return ReadProcessMemory(hProcess, address, buffer, size, &bytesRead) && (bytesRead == size);
}

bool ProcessUtils::FreeMemoryInProcess(HANDLE hProcess, LPVOID address) {
    return VirtualFreeEx(hProcess, address, 0, MEM_RELEASE);
}

bool ProcessUtils::SetDebugPrivilege() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tokenPrivileges;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return false;
    }

    if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &tokenPrivileges.Privileges[0].Luid)) {
        CloseHandle(hToken);
        return false;
    }

    tokenPrivileges.PrivilegeCount = 1;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    bool result = AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0, nullptr, nullptr);
    
    CloseHandle(hToken);
    return result;
}