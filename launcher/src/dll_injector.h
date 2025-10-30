#pragma once
#include <Windows.h>
#include <string>

namespace StellaLauncher {
    class DLLInjector {
    public:
        DLLInjector();
        ~DLLInjector();

        // Injection methods
        bool InjectDLL(DWORD processId, const std::wstring& dllPath);
        bool InjectDLL(const std::wstring& processName, const std::wstring& dllPath);
        
        // Manual mapping injection (more advanced)
        bool ManualMapDLL(DWORD processId, const std::wstring& dllPath);

        // Memory allocation utilities
        LPVOID AllocateMemoryInProcess(HANDLE hProcess, SIZE_T size);
        bool WriteMemoryToProcess(HANDLE hProcess, LPVOID address, const void* data, SIZE_T size);
        bool FreeMemoryInProcess(HANDLE hProcess, LPVOID address);

        // Process utilities
        HANDLE OpenTargetProcess(DWORD processId);
        bool IsProcessValid(HANDLE hProcess);

    private:
        // Helper functions for manual mapping
        bool RelocateImage(LPVOID imageBase, LPVOID targetBase, PIMAGE_NT_HEADERS ntHeaders);
        bool ResolveImports(HANDLE hProcess, LPVOID imageBase, PIMAGE_NT_HEADERS ntHeaders);
        HMODULE GetRemoteModuleHandle(HANDLE hProcess, const std::wstring& moduleName);
        FARPROC GetRemoteProcAddress(HANDLE hProcess, HMODULE hModule, const char* procName);
    };
}
