#pragma once

#include "CBLT_raylib.hpp"

#include "CBLT_Util.hpp"

#include <string>
#include <filesystem>

namespace CBLT {
    // CWDContent token, name and colour
    class CWDContentToken {
        public:
            Color c;        // Token color
            std::string n;  // Token name

            // Constructor
            CWDContentToken(Color clr, std::string name);

            // Destructor
            ~CWDContentToken(void);
    }; // CWD Content Token flag
} // CBLT
