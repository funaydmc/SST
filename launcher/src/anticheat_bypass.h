#pragma once
#include <Windows.h>
#include <string>

namespace StellaLauncher {
    class AntiCheatBypass {
    public:
        AntiCheatBypass();
        ~AntiCheatBypass();

        // Main bypass functions
        bool InitializeBypass();
        bool DisableAntiCheatExpert();
        bool PatchACEProtections();
        bool SetupMemoryProtections();

        // Hook management
        bool InstallHooks();
        bool RemoveHooks();

        // ACE-specific bypass methods
        bool BypassACEFileSystemChecks();
        bool BypassACEMemoryScanning();
        bool BypassACEProcessChecks();

        // Utility functions
        bool IsACEActive();
        bool PatchACEBinary(DWORD processId);

    private:
        bool m_hooksInstalled;
        bool m_bypassActive;

        // Hook function pointers
        static HANDLE(WINAPI* OriginalCreateFileW)(
            LPCWSTR lpFileName,
            DWORD dwDesiredAccess,
            DWORD dwShareMode,
            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
            DWORD dwCreationDisposition,
            DWORD dwFlagsAndAttributes,
            HANDLE hTemplateFile
        );

        static NTSTATUS(NTAPI* OriginalNtQuerySystemInformation)(
            ULONG SystemInformationClass,
            PVOID SystemInformation,
            ULONG SystemInformationLength,
            PULONG ReturnLength
        );

        static NTSTATUS(NTAPI* OriginalNtQueryInformationProcess)(
            HANDLE ProcessHandle,
            ULONG ProcessInformationClass,
            PVOID ProcessInformation,
            ULONG ProcessInformationLength,
            PULONG ReturnLength
        );

        // Hooked functions
        static HANDLE WINAPI HookedCreateFileW(
            LPCWSTR lpFileName,
            DWORD dwDesiredAccess,
            DWORD dwShareMode,
            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
            DWORD dwCreationDisposition,
            DWORD dwFlagsAndAttributes,
            HANDLE hTemplateFile
        );

        static NTSTATUS NTAPI HookedNtQuerySystemInformation(
            ULONG SystemInformationClass,
            PVOID SystemInformation,
            ULONG SystemInformationLength,
            PULONG ReturnLength
        );

        static NTSTATUS NTAPI HookedNtQueryInformationProcess(
            HANDLE ProcessHandle,
            ULONG ProcessInformationClass,
            PVOID ProcessInformation,
            ULONG ProcessInformationLength,
            PULONG ReturnLength
        );

        // Helper functions
        bool PatchMemoryRegion(LPVOID address, const BYTE* patch, SIZE_T size);
        bool SetMemoryProtection(LPVOID address, SIZE_T size, DWORD protection);
        LPVOID FindPattern(HMODULE module, const char* pattern, const char* mask);
    };
}
