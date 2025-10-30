#pragma once
#include <windows.h>
#include <string>

enum class InjectionMethod {
    LoadLibrary,
    ManualMap,
    ThreadHijacking
};

class DllInjector {
public:
    DllInjector();
    ~DllInjector();

    bool InjectDll(DWORD processId, const std::wstring& dllPath, InjectionMethod method = InjectionMethod::LoadLibrary);
    bool InjectDll(HANDLE hProcess, const std::wstring& dllPath, InjectionMethod method = InjectionMethod::LoadLibrary);

private:
    // LoadLibrary injection
    bool InjectViaLoadLibrary(HANDLE hProcess, const std::wstring& dllPath);
    
    // Manual mapping injection
    bool InjectViaManualMap(HANDLE hProcess, const std::wstring& dllPath);
    
    // Thread hijacking injection
    bool InjectViaThreadHijacking(HANDLE hProcess, const std::wstring& dllPath);
    
    // Helper functions
    bool ValidateDll(const std::wstring& dllPath);
    LPVOID GetProcAddressRemote(HANDLE hProcess, HMODULE hModule, const char* procName);
    bool ResolveImports(HANDLE hProcess, LPVOID imageBase, PIMAGE_NT_HEADERS ntHeaders);
    bool CallDllMain(HANDLE hProcess, LPVOID imageBase, DWORD reason);
};