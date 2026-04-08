#pragma once

// #include "CBLT_Util.hpp" // Raylib being included absolutely destroyed everything

#include <string>
#include <iostream>
#include <filesystem>

namespace CBLT {

    // Cross platform function
    #define __CROSS
    class Dialog {
        private:
            // Run a command based on the OS
            __CROSS std::string RunCommand(const std::string& com);

        public:
            // Constructor
            Dialog(void);
        
            // Destructor
            ~Dialog(void);

            // Dialog helpers for linux
            #if defined(__linux__) 
                std::string OpenFolderPickerPortal(void);
                std::string OpenFolderPickerFallback(void);
            #endif

            // Open the native folder explorer,
            __CROSS std::string OpenFolderPicker(void);
    }; // Dialog box class

    // Global dialog class
    extern Dialog gDialog;
} // CBLT