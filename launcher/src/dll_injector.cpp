#include "dll_injector.h"
#include "process_utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <Psapi.h>

namespace StellaLauncher {

DLLInjector::DLLInjector() {
}

DLLInjector::~DLLInjector() {
}

bool DLLInjector::InjectDLL(DWORD processId, const std::wstring& dllPath) {
    std::wcout << L"[INFO] Attempting standard DLL injection..." << std::endl;
    
    // Open target process
    HANDLE hProcess = OpenTargetProcess(processId);
    if (!hProcess) {
        std::wcout << L"[ERROR] Failed to open target process" << std::endl;
        return false;
    }
    
    // Allocate memory in target process for DLL path
    SIZE_T pathSize = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID remoteMemory = AllocateMemoryInProcess(hProcess, pathSize);
    
    if (!remoteMemory) {
        std::wcout << L"[ERROR] Failed to allocate memory in target process" << std::endl;
        CloseHandle(hProcess);
        return false;
    }
    
    // Write DLL path to target process
    if (!WriteMemoryToProcess(hProcess, remoteMemory, dllPath.c_str(), pathSize)) {
        std::wcout << L"[ERROR] Failed to write DLL path to target process" << std::endl;
        FreeMemoryInProcess(hProcess, remoteMemory);
        CloseHandle(hProcess);
        return false;
    }
    
    // Get LoadLibraryW address
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    FARPROC loadLibraryAddr = GetProcAddress(hKernel32, "LoadLibraryW");
    
    if (!loadLibraryAddr) {
        std::wcout << L"[ERROR] Failed to get LoadLibraryW address" << std::endl;
        FreeMemoryInProcess(hProcess, remoteMemory);
        CloseHandle(hProcess);
        return false;
    }
    
    // Create remote thread to load DLL
    HANDLE hThread = CreateRemoteThread(
        hProcess,
        nullptr,
        0,
        (LPTHREAD_START_ROUTINE)loadLibraryAddr,
        remoteMemory,
        0,
        nullptr
    );
    
    if (!hThread) {
        std::wcout << L"[ERROR] Failed to create remote thread" << std::endl;
        FreeMemoryInProcess(hProcess, remoteMemory);
        CloseHandle(hProcess);
        return false;
    }
    
    // Wait for thread to complete
    DWORD waitResult = WaitForSingleObject(hThread, 5000); // 5 second timeout
    
    if (waitResult == WAIT_TIMEOUT) {
        std::wcout << L"[ERROR] DLL injection timed out" << std::endl;
        TerminateThread(hThread, 0);
        FreeMemoryInProcess(hProcess, remoteMemory);
        CloseHandle(hThread);
        CloseHandle(hProcess);
        return false;
    }
    
    // Check thread exit code
    DWORD exitCode;
    GetExitCodeThread(hThread, &exitCode);
    
    // Clean up
    FreeMemoryInProcess(hProcess, remoteMemory);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    
    if (exitCode == 0) {
        std::wcout << L"[ERROR] LoadLibraryW returned 0 (DLL not loaded)" << std::endl;
        return false;
    }
    
    std::wcout << L"[SUCCESS] Standard DLL injection completed" << std::endl;
    return true;
}

bool DLLInjector::InjectDLL(const std::wstring& processName, const std::wstring& dllPath) {
    DWORD processId = ProcessUtils::GetProcessIdByName(processName);
    if (processId == 0) {
        std::wcout << L"[ERROR] Process not found: " << processName << std::endl;
        return false;
    }
    
    return InjectDLL(processId, dllPath);
}

bool DLLInjector::ManualMapDLL(DWORD processId, const std::wstring& dllPath) {
    std::wcout << L"[INFO] Attempting manual map injection..." << std::endl;
    
    // Open target process
    HANDLE hProcess = OpenTargetProcess(processId);
    if (!hProcess) {
        std::wcout << L"[ERROR] Failed to open target process for manual mapping" << std::endl;
        return false;
    }
    
    // Read DLL file into memory
    std::ifstream file(dllPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::wcout << L"[ERROR] Failed to open DLL file: " << dllPath << std::endl;
        CloseHandle(hProcess);
        return false;
    }
    
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<BYTE> dllBuffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(dllBuffer.data()), fileSize)) {
        std::wcout << L"[ERROR] Failed to read DLL file" << std::endl;
        CloseHandle(hProcess);
        return false;
    }
    file.close();
    
    // Parse PE headers
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)dllBuffer.data();
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        std::wcout << L"[ERROR] Invalid DOS signature" << std::endl;
        CloseHandle(hProcess);
        return false;
    }
    
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)(dllBuffer.data() + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        std::wcout << L"[ERROR] Invalid NT signature" << std::endl;
        CloseHandle(hProcess);
        return false;
    }
    
    // Allocate memory in target process
    DWORD imageSize = ntHeaders->OptionalHeader.SizeOfImage;
    LPVOID targetBase = VirtualAllocEx(hProcess, nullptr, imageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    
    if (!targetBase) {
        std::wcout << L"[ERROR] Failed to allocate memory for manual mapping" << std::endl;
        CloseHandle(hProcess);
        return false;
    }
    
    std::wcout << L"[INFO] Allocated memory at: " << std::hex << targetBase << std::endl;
    
    // Copy headers
    if (!WriteProcessMemory(hProcess, targetBase, dllBuffer.data(), ntHeaders->OptionalHeader.SizeOfHeaders, nullptr)) {
        std::wcout << L"[ERROR] Failed to write headers" << std::endl;
        VirtualFreeEx(hProcess, targetBase, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }
    
    // Copy sections
    PIMAGE_SECTION_HEADER sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
    for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
        if (sectionHeader[i].SizeOfRawData > 0) {
            LPVOID sectionDest = (LPVOID)((DWORD_PTR)targetBase + sectionHeader[i].VirtualAddress);
            LPVOID sectionSrc = (LPVOID)(dllBuffer.data() + sectionHeader[i].PointerToRawData);
            
            if (!WriteProcessMemory(hProcess, sectionDest, sectionSrc, sectionHeader[i].SizeOfRawData, nullptr)) {
                std::wcout << L"[ERROR] Failed to write section: " << sectionHeader[i].Name << std::endl;
                VirtualFreeEx(hProcess, targetBase, 0, MEM_RELEASE);
                CloseHandle(hProcess);
                return false;
            }
        }
    }
    
    // Perform relocations
    if (!RelocateImage(dllBuffer.data(), targetBase, ntHeaders)) {
        std::wcout << L"[WARNING] Relocation failed, but continuing..." << std::endl;
    }
    
    // Resolve imports
    if (!ResolveImports(hProcess, targetBase, ntHeaders)) {
        std::wcout << L"[ERROR] Failed to resolve imports" << std::endl;
        VirtualFreeEx(hProcess, targetBase, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }
    
    // Call DLL entry point
    DWORD_PTR entryPoint = (DWORD_PTR)targetBase + ntHeaders->OptionalHeader.AddressOfEntryPoint;
    
    // Create remote thread to call DllMain
    typedef BOOL(WINAPI* DllMainFunc)(HINSTANCE, DWORD, LPVOID);
    
    HANDLE hThread = CreateRemoteThread(
        hProcess,
        nullptr,
        0,
        (LPTHREAD_START_ROUTINE)entryPoint,
        targetBase, // hInstance parameter
        0,
        nullptr
    );
    
    if (!hThread) {
        std::wcout << L"[ERROR] Failed to create thread for DllMain" << std::endl;
        VirtualFreeEx(hProcess, targetBase, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }
    
    // Wait for DllMain to complete
    WaitForSingleObject(hThread, 5000);
    
    DWORD exitCode;
    GetExitCodeThread(hThread, &exitCode);
    
    CloseHandle(hThread);
    CloseHandle(hProcess);
    
    if (exitCode == 0) {
        std::wcout << L"[ERROR] DllMain returned FALSE" << std::endl;
        return false;
    }
    
    std::wcout << L"[SUCCESS] Manual map injection completed" << std::endl;
    return true;
}

LPVOID DLLInjector::AllocateMemoryInProcess(HANDLE hProcess, SIZE_T size) {
    return VirtualAllocEx(hProcess, nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

bool DLLInjector::WriteMemoryToProcess(HANDLE hProcess, LPVOID address, const void* data, SIZE_T size) {
    SIZE_T bytesWritten;
    return WriteProcessMemory(hProcess, address, data, size, &bytesWritten) && bytesWritten == size;
}

bool DLLInjector::FreeMemoryInProcess(HANDLE hProcess, LPVOID address) {
    return VirtualFreeEx(hProcess, address, 0, MEM_RELEASE) != 0;
}

HANDLE DLLInjector::OpenTargetProcess(DWORD processId) {
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
}

bool DLLInjector::IsProcessValid(HANDLE hProcess) {
    if (!hProcess || hProcess == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    DWORD exitCode;
    return GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE;
}

bool DLLInjector::RelocateImage(LPVOID imageBase, LPVOID targetBase, PIMAGE_NT_HEADERS ntHeaders) {
    DWORD_PTR deltaBase = (DWORD_PTR)targetBase - ntHeaders->OptionalHeader.ImageBase;
    
    if (deltaBase == 0) {
        return true; // No relocation needed
    }
    
    PIMAGE_DATA_DIRECTORY relocDir = &ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    if (relocDir->Size == 0) {
        return deltaBase == 0; // No relocations and delta is non-zero
    }
    
    PIMAGE_BASE_RELOCATION relocData = (PIMAGE_BASE_RELOCATION)((DWORD_PTR)imageBase + relocDir->VirtualAddress);
    
    while (relocData->VirtualAddress > 0) {
        LPVOID dest = (LPVOID)((DWORD_PTR)imageBase + relocData->VirtualAddress);
        WORD* relInfo = (WORD*)((DWORD_PTR)relocData + sizeof(IMAGE_BASE_RELOCATION));
        DWORD numEntries = (relocData->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
        
        for (DWORD i = 0; i < numEntries; i++) {
            WORD type = (relInfo[i] >> 12) & 0xF;
            WORD offset = relInfo[i] & 0xFFF;
            
            if (type == IMAGE_REL_BASED_DIR64) {
                DWORD_PTR* patchAddr = (DWORD_PTR*)((DWORD_PTR)dest + offset);
                *patchAddr += deltaBase;
            }
            else if (type == IMAGE_REL_BASED_HIGHLOW) {
                DWORD* patchAddr = (DWORD*)((DWORD_PTR)dest + offset);
                *patchAddr += (DWORD)deltaBase;
            }
        }
        
        relocData = (PIMAGE_BASE_RELOCATION)((DWORD_PTR)relocData + relocData->SizeOfBlock);
    }
    
    return true;
}

bool DLLInjector::ResolveImports(HANDLE hProcess, LPVOID imageBase, PIMAGE_NT_HEADERS ntHeaders) {
    PIMAGE_DATA_DIRECTORY importDir = &ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (importDir->Size == 0) {
        return true; // No imports
    }
    
    PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD_PTR)imageBase + importDir->VirtualAddress);
    
    while (importDesc->Name != 0) {
        char* moduleName = (char*)((DWORD_PTR)imageBase + importDesc->Name);
        std::string moduleNameStr(moduleName);
        std::wstring moduleNameWStr(moduleNameStr.begin(), moduleNameStr.end());
        
        HMODULE hModule = GetRemoteModuleHandle(hProcess, moduleNameWStr);
        if (!hModule) {
            std::wcout << L"[ERROR] Failed to get remote module: " << moduleNameWStr.c_str() << std::endl;
            return false;
        }
        
        PIMAGE_THUNK_DATA thunk = nullptr;
        PIMAGE_THUNK_DATA funcRef = nullptr;
        
        if (importDesc->OriginalFirstThunk != 0) {
            thunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)imageBase + importDesc->OriginalFirstThunk);
            funcRef = (PIMAGE_THUNK_DATA)((DWORD_PTR)imageBase + importDesc->FirstThunk);
        } else {
            thunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)imageBase + importDesc->FirstThunk);
            funcRef = (PIMAGE_THUNK_DATA)((DWORD_PTR)imageBase + importDesc->FirstThunk);
        }
        
        while (thunk->u1.AddressOfData != 0) {
            FARPROC funcAddr = nullptr;
            
            if (thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
                // Import by ordinal
                WORD ordinal = IMAGE_ORDINAL(thunk->u1.Ordinal);
                funcAddr = GetRemoteProcAddress(hProcess, hModule, (char*)ordinal);
            } else {
                // Import by name
                PIMAGE_IMPORT_BY_NAME importByName = (PIMAGE_IMPORT_BY_NAME)((DWORD_PTR)imageBase + thunk->u1.AddressOfData);
                funcAddr = GetRemoteProcAddress(hProcess, hModule, importByName->Name);
            }
            
            if (!funcAddr) {
                std::wcout << L"[ERROR] Failed to resolve import from " << moduleNameWStr.c_str() << std::endl;
                return false;
            }
            
            funcRef->u1.Function = (DWORD_PTR)funcAddr;
            
            thunk++;
            funcRef++;
        }
        
        importDesc++;
    }
    
    return true;
}

HMODULE DLLInjector::GetRemoteModuleHandle(HANDLE hProcess, const std::wstring& moduleName) {
    // In a real implementation, you would enumerate modules in the remote process
    // For now, we'll assume the module is already loaded and return the local handle
    return GetModuleHandleW(moduleName.c_str());
}

FARPROC DLLInjector::GetRemoteProcAddress(HANDLE hProcess, HMODULE hModule, const char* procName) {
    // In a real implementation, you would parse the export table of the remote module
    // For now, we'll return the local address (this works for system DLLs)
    if ((DWORD_PTR)procName < 0x10000) {
        // Ordinal
        return GetProcAddress(hModule, procName);
    } else {
        // Name
        return GetProcAddress(hModule, procName);
    }
}

} // namespace StellaLauncher
