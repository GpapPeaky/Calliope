#include "CBLT_ShellBridge.hpp"

#if defined(__APPLE__) || defined(__linux__)

    #include <cstdio>
    #include <memory>
    #include <array>
    #include <stdexcept>
    #include <unistd.h>
    #include <sys/wait.h>

    namespace CBLT {
        std::string ShellBridge::Execute(const std::string& com, std::string& cwd) {
            // Use bash for consistent behavior
            pid_t pid = fork();

            if (pid == 0) {
                // Child process
                execlp("sh", "sh", (char*)NULL);
            } else {
                // Parent waits
                wait(NULL);
            }

            return com;
        }
    
        ShellBridge::ShellBridge(void) {}

        ShellBridge::~ShellBridge(void) {}
    
        ShellBridge gShellBridge;
    
    } // CBLT

#endif // __APPLE__ || __linux__