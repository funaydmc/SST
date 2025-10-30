#include "process_utils.h"
#include <iostream>
#include <Psapi.h>

namespace StellaLauncher {

std::vector<DWORD> ProcessUtils::GetProcessesByName(const std::wstring& processName) {
    std::vector<DWORD> processIds;
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return processIds;
    }
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (_wcsicmp(pe32.szExeFile, processName.c_str()) == 0) {
                processIds.push_back(pe32.th32ProcessID);
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    return processIds;
}

DWORD ProcessUtils::GetProcessIdByName(const std::wstring& processName) {
    auto processes = GetProcessesByName(processName);
    return processes.empty() ? 0 : processes[0];
}

std::wstring ProcessUtils::GetProcessNameById(DWORD processId) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return L"";
    }
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (pe32.th32ProcessID == processId) {
                CloseHandle(hSnapshot);
                return std::wstring(pe32.szExeFile);
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    return L"";
}

std::vector<DWORD> ProcessUtils::GetAllProcessIds() {
    std::vector<DWORD> processIds;
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return processIds;
    }
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            processIds.push_back(pe32.th32ProcessID);
        } while (Process32NextW(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    return processIds;
}

bool ProcessUtils::IsProcessRunning(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (!hProcess) {
        return false;
    }
    
    DWORD exitCode;
    bool result = GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE;
    
    CloseHandle(hProcess);
    return result;
}

bool ProcessUtils::IsProcessRunning(const std::wstring& processName) {
    return GetProcessIdByName(processName) != 0;
}

std::wstring ProcessUtils::GetProcessPath(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return L"";
    }
    
    wchar_t path[MAX_PATH];
    DWORD size = MAX_PATH;
    
    if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) {
        CloseHandle(hProcess);
        return std::wstring(path);
    }
    
    CloseHandle(hProcess);
    return L"";
}

DWORD ProcessUtils::GetProcessParentId(DWORD processId) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (pe32.th32ProcessID == processId) {
                CloseHandle(hSnapshot);
                return pe32.th32ParentProcessID;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    return 0;
}

bool ProcessUtils::TerminateProcessById(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (!hProcess) {
        return false;
    }
    
    bool result = TerminateProcess(hProcess, 0) != 0;
    CloseHandle(hProcess);
    return result;
}

bool ProcessUtils::TerminateProcessByName(const std::wstring& processName) {
    auto processes = GetProcessesByName(processName);
    bool allTerminated = true;
    
    for (DWORD processId : processes) {
        if (!TerminateProcessById(processId)) {
            allTerminated = false;
        }
    }
    
    return allTerminated && !processes.empty();
}

bool ProcessUtils::SuspendProcess(DWORD processId) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);
    
    bool result = true;
    
    if (Thread32First(hSnapshot, &te32)) {
        do {
            if (te32.th32OwnerProcessID == processId) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                if (hThread) {
                    if (SuspendThread(hThread) == (DWORD)-1) {
                        result = false;
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hSnapshot, &te32));
    }
    
    CloseHandle(hSnapshot);
    return result;
}

bool ProcessUtils::ResumeProcess(DWORD processId) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);
    
    bool result = true;
    
    if (Thread32First(hSnapshot, &te32)) {
        do {
            if (te32.th32OwnerProcessID == processId) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                if (hThread) {
                    if (ResumeThread(hThread) == (DWORD)-1) {
                        result = false;
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hSnapshot, &te32));
    }
    
    CloseHandle(hSnapshot);
    return result;
}

HWND ProcessUtils::FindMainWindow(DWORD processId) {
    EnumWindowsData data;
    data.processId = processId;
    data.hwnd = nullptr;
    
    EnumWindows(EnumWindowsProc, (LPARAM)&data);
    return data.hwnd;
}

HWND ProcessUtils::FindWindowByTitle(const std::wstring& title) {
    return FindWindowW(nullptr, title.c_str());
}

bool ProcessUtils::IsWindowVisible(HWND hwnd) {
    return IsWindowVisible(hwnd) != 0;
}

bool ProcessUtils::BringWindowToFront(HWND hwnd) {
    if (!hwnd) {
        return false;
    }
    
    return SetForegroundWindow(hwnd) && ShowWindow(hwnd, SW_RESTORE);
}

std::vector<HMODULE> ProcessUtils::GetProcessModules(DWORD processId) {
    std::vector<HMODULE> modules;
    
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return modules;
    }
    
    HMODULE hMods[1024];
    DWORD cbNeeded;
    
    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        DWORD moduleCount = cbNeeded / sizeof(HMODULE);
        for (DWORD i = 0; i < moduleCount; i++) {
            modules.push_back(hMods[i]);
        }
    }
    
    CloseHandle(hProcess);
    return modules;
}

HMODULE ProcessUtils::GetModuleHandle(DWORD processId, const std::wstring& moduleName) {
    auto modules = GetProcessModules(processId);
    
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return nullptr;
    }
    
    for (HMODULE hMod : modules) {
        wchar_t modName[MAX_PATH];
        if (GetModuleBaseNameW(hProcess, hMod, modName, sizeof(modName) / sizeof(wchar_t))) {
            if (_wcsicmp(modName, moduleName.c_str()) == 0) {
                CloseHandle(hProcess);
                return hMod;
            }
        }
    }
    
    CloseHandle(hProcess);
    return nullptr;
}

std::wstring ProcessUtils::GetModulePath(DWORD processId, HMODULE hModule) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return L"";
    }
    
    wchar_t modPath[MAX_PATH];
    if (GetModuleFileNameExW(hProcess, hModule, modPath, sizeof(modPath) / sizeof(wchar_t))) {
        CloseHandle(hProcess);
        return std::wstring(modPath);
    }
    
    CloseHandle(hProcess);
    return L"";
}

SIZE_T ProcessUtils::GetProcessMemoryUsage(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return 0;
    }
    
    PROCESS_MEMORY_COUNTERS pmc;
    SIZE_T memoryUsage = 0;
    
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        memoryUsage = pmc.WorkingSetSize;
    }
    
    CloseHandle(hProcess);
    return memoryUsage;
}

bool ProcessUtils::ReadProcessMemory(DWORD processId, LPCVOID address, LPVOID buffer, SIZE_T size) {
    HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return false;
    }
    
    SIZE_T bytesRead;
    bool result = ::ReadProcessMemory(hProcess, address, buffer, size, &bytesRead) && bytesRead == size;
    
    CloseHandle(hProcess);
    return result;
}

bool ProcessUtils::WriteProcessMemory(DWORD processId, LPVOID address, LPCVOID buffer, SIZE_T size) {
    HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, processId);
    if (!hProcess) {
        return false;
    }
    
    SIZE_T bytesWritten;
    bool result = ::WriteProcessMemory(hProcess, address, buffer, size, &bytesWritten) && bytesWritten == size;
    
    CloseHandle(hProcess);
    return result;
}

bool ProcessUtils::EnableDebugPrivilege() {
    return SetProcessPrivilege(L"SeDebugPrivilege", true);
}

bool ProcessUtils::SetProcessPrivilege(const std::wstring& privilege, bool enable) {
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return false;
    }
    
    bool result = EnablePrivilege(hToken, privilege.c_str());
    CloseHandle(hToken);
    return result;
}

PROCESS_INFORMATION ProcessUtils::LaunchProcess(const std::wstring& executable, const std::wstring& arguments, const std::wstring& workingDirectory) {
    PROCESS_INFORMATION pi = {};
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    
    std::wstring cmdLine = executable;
    if (!arguments.empty()) {
        cmdLine += L" " + arguments;
    }
    
    // Make a mutable copy of the command line
    std::vector<wchar_t> cmdLineBuffer(cmdLine.begin(), cmdLine.end());
    cmdLineBuffer.push_back(L'\0');
    
    const wchar_t* workDir = workingDirectory.empty() ? nullptr : workingDirectory.c_str();
    
    if (!CreateProcessW(
        executable.c_str(),
        cmdLineBuffer.data(),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        workDir,
        &si,
        &pi)) {
        
        // Clear the structure on failure
        memset(&pi, 0, sizeof(pi));
    }
    
    return pi;
}

PROCESS_INFORMATION ProcessUtils::LaunchProcessSuspended(const std::wstring& executable, const std::wstring& arguments, const std::wstring& workingDirectory) {
    PROCESS_INFORMATION pi = {};
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    
    std::wstring cmdLine = executable;
    if (!arguments.empty()) {
        cmdLine += L" " + arguments;
    }
    
    // Make a mutable copy of the command line
    std::vector<wchar_t> cmdLineBuffer(cmdLine.begin(), cmdLine.end());
    cmdLineBuffer.push_back(L'\0');
    
    const wchar_t* workDir = workingDirectory.empty() ? nullptr : workingDirectory.c_str();
    
    if (!CreateProcessW(
        executable.c_str(),
        cmdLineBuffer.data(),
        nullptr,
        nullptr,
        FALSE,
        CREATE_SUSPENDED,
        nullptr,
        workDir,
        &si,
        &pi)) {
        
        // Clear the structure on failure
        memset(&pi, 0, sizeof(pi));
    }
    
    return pi;
}

bool ProcessUtils::EnablePrivilege(HANDLE hToken, LPCWSTR privilege) {
    TOKEN_PRIVILEGES tp;
    LUID luid;
    
    if (!LookupPrivilegeValueW(nullptr, privilege, &luid)) {
        return false;
    }
    
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    
    return AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr) != 0;
}

BOOL CALLBACK ProcessUtils::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    EnumWindowsData* data = (EnumWindowsData*)lParam;
    
    DWORD windowProcessId;
    GetWindowThreadProcessId(hwnd, &windowProcessId);
    
    if (windowProcessId == data->processId && IsWindowVisible(hwnd) && GetWindow(hwnd, GW_OWNER) == nullptr) {
        data->hwnd = hwnd;
        return FALSE; // Stop enumeration
    }
    
    return TRUE; // Continue enumeration
}

} // namespace StellaLauncher
