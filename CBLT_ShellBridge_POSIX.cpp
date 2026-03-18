#include "CBLT_ShellBridge.hpp"

#if defined(__APPLE__) || defined(__linux__)

    #include <cstdio>
    #include <memory>
    #include <array>
    #include <stdexcept>

    namespace CBLT {
    
        std::string ShellBridge::Execute(const std::string& com, std::string& cwd) {
            std::array<char, 256> buffer;
            std::string result;
    
            // Use bash for consistent behavior
            std::string cmd =
                "/bin/bash -c \"cd '" + cwd + "' && " + com + "\"";
    
            std::unique_ptr<FILE, decltype(&pclose)> pipe(
                popen(cmd.c_str(), "r"), pclose
            );
    
            if (!pipe) {
                throw std::runtime_error("Failed to run command");
            }
    
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                result += buffer.data();
            }
    
            return result;
        }
    
        ShellBridge::ShellBridge(void) {}
        ShellBridge::~ShellBridge(void) {}
    
        ShellBridge gShellBridge;
    
    } // CBLT

#endif // __APPLE__ || __linux__