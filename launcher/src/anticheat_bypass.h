#pragma once
#include <windows.h>
#include <string>

class AntiCheatBypass {
public:
    static bool DisableAntiCheatExpert();
    static bool BypassProcessProtection(DWORD processId);
    static bool PatchAntiCheatModules(HANDLE hProcess);

private:
    static bool TerminateAntiCheatProcesses();
    static bool DisableAntiCheatServices();
    static bool BlockAntiCheatNetworkTraffic();
    static bool PatchKernelCallbacks();
    static bool HookNtFunctions();
    
    // Anti-cheat process names to terminate
    static const std::wstring ANTICHEAT_PROCESSES[];
    static const std::wstring ANTICHEAT_SERVICES[];
};