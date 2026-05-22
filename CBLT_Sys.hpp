#pragma once

#include <string>

#if defined(__linux__) || defined(__APPLE__)
    #include <unistd.h>
    #include <limits.h>
#endif

namespace CBLT {
    namespace Sys {
        // Return resource path based on operating system
        std::string ResourcePath(void);
    
        // Return current working directory based on operating system
        std::string WorkingDirectory(int argc, char** argv);
    }
} // CBLT