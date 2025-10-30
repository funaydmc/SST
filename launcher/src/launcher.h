#pragma once
#include <windows.h>
#include <string>

class Launcher {
public:
    Launcher();
    ~Launcher();

    bool Initialize();
    bool LaunchGameWithInjection(const std::wstring& gamePath, const std::wstring& dllPath);

private:
    bool m_initialized;
    std::wstring m_gameProcessName;
    std::wstring m_targetDllPath;

    // Internal methods
    bool ValidateFiles(const std::wstring& gamePath, const std::wstring& dllPath);
    void ShowConsole();
    void LogMessage(const std::string& message);
};