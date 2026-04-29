#include "CBLT_ShellBridge.hpp"

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <shlobj.h>
    #include <commdlg.h>
    #include <cstdio>
    #include <memory>
    #include <stdexcept>
    #include <array>

    #include "CBLT_Settings.hpp"

    namespace CBLT {
        std::string ShellBridge::Execute(const std::string& com, std::string& cwd) {
            STARTUPINFOA si = { sizeof(si) };
            PROCESS_INFORMATION pi;

            std::string term = gSettings.OPTION_WIN32_Term;
            std::string command;

            if (term == "cmd.exe") {
                command = "cmd.exe /K \"cd /d " + cwd + " && " + com + "\"";
            }
            else if (term == "powershell.exe" || term == "pwsh.exe") {
                command = "powershell.exe -NoExit -Command \"Set-Location '" + cwd + "'; " + com + "\"";
            }
            else if (term == "wt.exe") { // Windows Terminal
                command = "wt.exe cmd /K \"cd /d " + cwd + " && " + com + "\"";
            }
            else {
                // fallback: assume it's a direct executable
                command = term + " " + com;
            }

            BOOL success = CreateProcessA(
                NULL,
                command.data(),
                NULL,
                NULL,
                FALSE,
                CREATE_NEW_CONSOLE,
                NULL,
                NULL, // don't pass cwd here, we handle it in command
                &si,
                &pi
            );

            if (!success) {
                return "Failed to launch terminal";
            }

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            return cwd;
        }
                
        ShellBridge::ShellBridge(void) {}
        
        ShellBridge::~ShellBridge(void) {}
    
        ShellBridge gShellBridge;
    } // CBLT

#endif // _WIN32