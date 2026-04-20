#include "CBLT_ShellBridge.hpp"

#if defined(__APPLE__) || defined(__linux__)    
    #include <cstdio>
    #include <memory>
    #include <array>
    #include <stdexcept>
    #include <unistd.h>
    #include <sys/wait.h>

    #include "CBLT_Settings.hpp"

    namespace CBLT {
        std::string ShellBridge::Execute(const std::string& com, std::string& cwd) {
            // Use bash for consistent behavior
            pid_t pid = fork();

            if (pid == 0) {
                // Child process
                execlp(gSettings.OPTION_POSIX_Term.c_str(), cwd.c_str(), (char*)NULL);
            }

            return com;
        }
    
        ShellBridge::ShellBridge(void) {}

        ShellBridge::~ShellBridge(void) {}
    
        ShellBridge gShellBridge;
    
    } // CBLT

#endif // __APPLE__ || __linux__