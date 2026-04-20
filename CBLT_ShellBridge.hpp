#pragma once

// #include "CBLT_Util.hpp" // Raylib being included absolutely destroyed everything

#include <string>
#include <iostream>
#include <filesystem>

namespace CBLT {
    // Cross platform function
    #define __CROSS

    class ShellBridge {
        public:
            #if defined(__linux__)
                std::string term = "kitty";
            #endif  // Linux terminal name, can be configured by the user

            // Run a command based on the OS
            __CROSS std::string Execute(const std::string& com, std::string& cwd);
            
            // Constructor
            ShellBridge(void);
        
            // Destructor
            ~ShellBridge(void);
    }; // ShellBridge class

    extern ShellBridge gShellBridge;
} // CBLT