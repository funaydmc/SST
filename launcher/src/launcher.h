#pragma once
#include <Windows.h>
#include <string>

namespace StellaLauncher {
    class Launcher {
    public:
        Launcher();
        ~Launcher();

        // Main launcher functions
        bool Initialize();
        bool LaunchGame();
        bool InjectDLL();
        void Cleanup();

        // Configuration
        void SetGamePath(const std::wstring& path);
        void SetDLLPath(const std::wstring& path);
        void SetAutoInject(bool autoInject);

        // Status
        bool IsGameRunning();
        DWORD GetGameProcessId();

    private:
        std::wstring m_gamePath;
        std::wstring m_dllPath;
        bool m_autoInject;
        DWORD m_gameProcessId;
        bool m_initialized;

        // Helper functions
        bool ValidateGamePath();
        bool ValidateDLLPath();
        bool WaitForGameWindow();
    };
}
