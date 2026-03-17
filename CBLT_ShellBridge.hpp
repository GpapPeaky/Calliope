#pragma once

// #include "CBLT_Util.hpp" // Raylib being included absolutely destroyed everything

#include <string>
#include <iostream>
#include <filesystem>

namespace CBLT {

    // Cross platform function
    #define __CROSS
    class ShellBridge {
        private:
            // Run a command based on the OS
            __CROSS std::string Execute(const std::string& com);
            
        public:
            // Constructor
            ShellBridge(void);
        
            // Destructor
            ~ShellBridge(void);

            // Open the native folder explorer,
            __CROSS std::string OpenFolderPicker(void);
    }; // ShellBridge class

    extern ShellBridge gShellBridge;
} // CBLT