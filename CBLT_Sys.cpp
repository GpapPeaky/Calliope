#include "CBLT_Sys.hpp"

namespace CBLT::Sys {
    std::string ResourcePath(void) {
        #if defined(__linux__)
            const char* resource_path = getenv("CBLT_RESOURCES");
            return resource_path ? std::string(resource_path) : ".";
        #elif defined(__APPLE__)
            return "."; // macOS can use relative paths directly
        #elif defined(_WIN32)
            return "."; // Windows can also use relative paths directly
        #else
            return "."; // Default to current directory for unknown platforms
        #endif
    }

    std::string WorkingDirectory(int argc, char** argv) {
        #if defined(__linux__)
            return (argc > 1) ? std::string(argv[1]) : "/home";
        #elif defined(__APPLE__)
            char cwd[PATH_MAX];
            return (getcwd(cwd, sizeof(cwd)) ? cwd : "/");
        #elif defined(_WIN32)
            return "C:/";
        #else
            return ".";
        #endif
    }
}
