#pragma once

#include "CBLT_Util.hpp"

#include <string>

// Windows system
#if defined(_WIN32) 
    #include <windows.h>

    #define OS 0
#endif

// Linux
#if defined(__linux__)
    #include <cstdio.h>

    #define OS 1
#endif

// MacOS
#if defined(__APPLE__)
    #include <cstdio>

    define OS 2
#endif

namespace CBLT {
    // Connect to the native shell
    class ClipBridge {
        public:
            // Get clipboard text
            std::string GetClipboardText(void) const;

            // Set clipboard text
            void SetClipboardText(const std::string& text) const;

    }; // ClipBridge class
} // CBLT
