#include "dll_injector.h"
#include "process_utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>

DllInjector::DllInjector() {
}

DllInjector::~DllInjector() {
}

bool DllInjector::InjectDll(DWORD processId, const std::wstring& dllPath, InjectionMethod method) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess) {
        std::wcout << L"[INJECTOR] Error: Could not open target process (PID: " << processId << L")" << std::endl;
        return false;
    }

    bool result = InjectDll(hProcess, dllPath, method);
    CloseHandle(hProcess);
    return result;
}

bool DllInjector::InjectDll(HANDLE hProcess, const std::wstring& dllPath, InjectionMethod method) {
    if (!ValidateDll(dllPath)) {
        return false;
    }

    std::wcout << L"[INJECTOR] Starting DLL injection..." << std::endl;
    std::wcout << L"[INJECTOR] DLL Path: " << dllPath << std::endl;

    bool result = false;
    switch (method) {
        case InjectionMethod::LoadLibrary:
            std::wcout << L"[INJECTOR] Using LoadLibrary injection method" << std::endl;
            result = InjectViaLoadLibrary(hProcess, dllPath);
            break;

        case InjectionMethod::ManualMap:
            std::wcout << L"[INJECTOR] Using Manual Map injection method" << std::endl;
            result = InjectViaManualMap(hProcess, dllPath);
            break;

        case InjectionMethod::ThreadHijacking:
            std::wcout << L"[INJECTOR] Using Thread Hijacking injection method" << std::endl;
            result = InjectViaThreadHijacking(hProcess, dllPath);
            break;

        default:
            std::wcout << L"[INJECTOR] Error: Invalid injection method" << std::endl;
            return false;
    }

    if (result) {
        std::wcout << L"[INJECTOR] DLL injection completed successfully!" << std::endl;
    } else {
        std::wcout << L"[INJECTOR] DLL injection failed!" << std::endl;
    }

    return result;
}

bool DllInjector::InjectViaLoadLibrary(HANDLE hProcess, const std::wstring& dllPath) {
    std::wcout << L"[INJECTOR] Injecting via LoadLibrary..." << std::endl;

    // Get LoadLibraryW address
    HMODULE hKernel32 = GetModuleHandle(L"kernel32.dll");
    if (!hKernel32) {
        std::wcout << L"[INJECTOR] Error: Could not get kernel32.dll handle" << std::endl;
        return false;
    }

    LPVOID pLoadLibraryW = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryW");
    if (!pLoadLibraryW) {
        std::wcout << L"[INJECTOR] Error: Could not get LoadLibraryW address" << std::endl;
        return false;
    }

    // Allocate memory for DLL path in target process
    SIZE_T pathSize = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID pDllPath = nullptr;
    
    if (!ProcessUtils::AllocateMemoryInProcess(hProcess, pathSize, &pDllPath)) {
        std::wcout << L"[INJECTOR] Error: Could not allocate memory in target process" << std::endl;
        return false;
    }

    // Write DLL path to target process
    if (!ProcessUtils::WriteMemoryToProcess(hProcess, pDllPath, dllPath.c_str(), pathSize)) {
        std::wcout << L"[INJECTOR] Error: Could not write DLL path to target process" << std::endl;
        ProcessUtils::FreeMemoryInProcess(hProcess, pDllPath);
        return false;
    }

    // Create remote thread to call LoadLibraryW
    HANDLE hThread = CreateRemoteThread(
        hProcess,
        nullptr,
        0,
        (LPTHREAD_START_ROUTINE)pLoadLibraryW,
        pDllPath,
        0,
        nullptr
    );

    if (!hThread) {
        std::wcout << L"[INJECTOR] Error: Could not create remote thread (Error: " << GetLastError() << L")" << std::endl;
        ProcessUtils::FreeMemoryInProcess(hProcess, pDllPath);
        return false;
    }

    // Wait for the thread to complete
    std::wcout << L"[INJECTOR] Waiting for LoadLibrary to complete..." << std::endl;
    DWORD waitResult = WaitForSingleObject(hThread, 10000); // 10 second timeout
    
    if (waitResult != WAIT_OBJECT_0) {
        std::wcout << L"[INJECTOR] Error: LoadLibrary thread timed out or failed" << std::endl;
        TerminateThread(hThread, 0);
        CloseHandle(hThread);
        ProcessUtils::FreeMemoryInProcess(hProcess, pDllPath);
        return false;
    }

    // Get the return value (HMODULE of loaded DLL)
    DWORD exitCode;
    GetExitCodeThread(hThread, &exitCode);
    
    CloseHandle(hThread);
    ProcessUtils::FreeMemoryInProcess(hProcess, pDllPath);

    if (exitCode == 0) {
        std::wcout << L"[INJECTOR] Error: LoadLibrary failed in target process" << std::endl;
        return false;
    }

    std::wcout << L"[INJECTOR] LoadLibrary injection successful! Module base: 0x" << std::hex << exitCode << std::dec << std::endl;
    return true;
}

bool DllInjector::InjectViaManualMap(HANDLE hProcess, const std::wstring& dllPath) {
    std::wcout << L"[INJECTOR] Manual map injection not fully implemented" << std::endl;
    
    // This is a complex injection method that requires:
    // 1. Reading the PE file
    // 2. Mapping sections to memory
    // 3. Resolving imports
    // 4. Applying relocations
    // 5. Calling DLL entry point
    
    // For now, fall back to LoadLibrary method
    return InjectViaLoadLibrary(hProcess, dllPath);
}

bool DllInjector::InjectViaThreadHijacking(HANDLE hProcess, const std::wstring& dllPath) {
    std::wcout << L"[INJECTOR] Thread hijacking injection not fully implemented" << std::endl;
    
    // This method requires:
    // 1. Finding a suitable thread to hijack
    // 2. Suspending the thread
    // 3. Modifying the thread context to point to our shellcode
    // 4. Resuming the thread
    
    // For now, fall back to LoadLibrary method
    return InjectViaLoadLibrary(hProcess, dllPath);
}

bool DllInjector::ValidateDll(const std::wstring& dllPath) {
    // Check if file exists
    if (!std::filesystem::exists(dllPath)) {
        std::wcout << L"[INJECTOR] Error: DLL file not found: " << dllPath << std::endl;
        return false;
    }

    // Check if it's a valid PE file (basic check)
    std::ifstream file(dllPath, std::ios::binary);
    if (!file.is_open()) {
        std::wcout << L"[INJECTOR] Error: Could not open DLL file: " << dllPath << std::endl;
        return false;
    }

    // Read DOS header
    IMAGE_DOS_HEADER dosHeader;
    file.read(reinterpret_cast<char*>(&dosHeader), sizeof(dosHeader));
    
    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
        std::wcout << L"[INJECTOR] Error: Invalid DOS signature in DLL" << std::endl;
        file.close();
        return false;
    }

    // Read NT headers
    file.seekg(dosHeader.e_lfanew);
    IMAGE_NT_HEADERS ntHeaders;
    file.read(reinterpret_cast<char*>(&ntHeaders), sizeof(ntHeaders));
    
    if (ntHeaders.Signature != IMAGE_NT_SIGNATURE) {
        std::wcout << L"[INJECTOR] Error: Invalid NT signature in DLL" << std::endl;
        file.close();
        return false;
    }

    // Check if it's a DLL
    if (!(ntHeaders.FileHeader.Characteristics & IMAGE_FILE_DLL)) {
        std::wcout << L"[INJECTOR] Warning: File is not marked as a DLL" << std::endl;
    }

    file.close();
    std::wcout << L"[INJECTOR] DLL validation passed" << std::endl;
    return true;
}

LPVOID DllInjector::GetProcAddressRemote(HANDLE hProcess, HMODULE hModule, const char* procName) {
    // This is a placeholder for getting procedure address in remote process
    // Would need to implement PE parsing in remote process memory
    return nullptr;
}

bool DllInjector::ResolveImports(HANDLE hProcess, LPVOID imageBase, PIMAGE_NT_HEADERS ntHeaders) {
    // This is a placeholder for import resolution in manual mapping
    // Would need to implement full import table resolution
    return true;
}

bool DllInjector::CallDllMain(HANDLE hProcess, LPVOID imageBase, DWORD reason) {
    // This is a placeholder for calling DllMain in manual mapping
    // Would need to create a remote thread to call the DLL entry point
    return true;
}