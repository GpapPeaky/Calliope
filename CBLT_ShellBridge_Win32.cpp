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


    namespace CBLT {
        std::string ShellBridge::Execute(const std::string& com, std::string& cwd) {
            // std::array<char, 128> buffer;
            // std::string result;
        
            // // Wrap command to run in PowerShell
            // // We need to execute at our vitual path of course.
            // std::string cmd =
            //     "powershell -NoProfile -Command \""
            //     "Set-Location -LiteralPath '" + cwd + "'; " + com +
            //     "\"";
        
            // std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
        
            // if (!pipe) {
            //     throw std::runtime_error("Failed to run command");
            // }
        
            // while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            //     result += buffer.data();
            // }
        
            return com;
        }
                
        ShellBridge::ShellBridge(void) {}
        
        ShellBridge::~ShellBridge(void) {}
    
        ShellBridge gShellBridge;
    } // CBLT


#endif // _WIN32