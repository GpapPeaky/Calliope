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
            pid_t pid = fork();

            if (pid == 0) {
                if (chdir(cwd.c_str()) != 0) {
                    exit(1); // Kill child process since we couldn't change dir
                }

                // We use a C++ Raw String Literal R"(...)" to completely bypass backslash escaping issues.
                // This passes the exact string to bash without C++ modifying the quotes or slashes.
                std::string bashCommand = R"(exec bash --init-file <(echo "[ -f ~/.bashrc ] && source ~/.bashrc; echo -e '\e[33mCBLT CALLED LOCAL SHELL\n\e[0m'"))";

                // Child process
                // Syntax for many terminals: terminal -e bash -c "command"
                execlp(gSettings.OPTION_POSIX_Term.c_str(), gSettings.OPTION_POSIX_Term.c_str(), "-e", "bash", "-c", bashCommand.c_str(), (char*)NULL);
            }
        
            return cwd;
        }
    
        ShellBridge::ShellBridge(void) {}

        ShellBridge::~ShellBridge(void) {}
    
        ShellBridge gShellBridge;
    
    } // CBLT

#endif // __APPLE__ || __linux__