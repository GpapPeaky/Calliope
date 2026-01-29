#pragma once

#include "CBLT_Util.hpp"

#include <string>
#include <iostream>
#include <filesystem>

namespace CBLT {
    class Dialog {
        // Constructor
        public:
            Dialog(void);
        
            // Destructor
            ~Dialog(void);

            // Open the native folder explorer,
            __CROSS std::string OpenFolderPicker(void);
    }; // Dialog box class

    extern Dialog gDialog;
} // CBLT