#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

namespace CBLT {
    class EditorSettings {
        public:
            std::string OPTION_WIN32_Term = ""; // Terminal for win32
            std::string OPTION_POSIX_Term = ""; // Terminal for posix
            std::string OPTION_Palette = "";    // Palette option

            // Read options/settings.conf
            void ReadSettings(void);
    }; // Settings class

    // Settings global
    extern EditorSettings gSettings;
} // CBLT