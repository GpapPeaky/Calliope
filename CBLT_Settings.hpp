#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "CBLT_Animation.hpp" // For animation profiles

namespace CBLT {
    class EditorSettings {
        public:
            std::string OPTION_WIN32_Term = ""; // Terminal for win32
            std::string OPTION_POSIX_Term = ""; // Terminal for posix
            std::string OPTION_Palette = "";    // Palette option

            UT::i32 OPTION_CursorSymbol = -4;   // Cursor symbol option, raw

            AnimationProfile OPTION_ANIM_FileCursor;                      // Animation profile for file cursors
            AnimationProfile OPTION_ANIM_FQ;                              // Animation profile for file queue movement
            AnimationProfile OPTION_ANIM_Console;                         // Animation profile for console movement
            AnimationProfile OPTION_ANIM_ConsoleCursor;                   // Animation profile for console cursor

            // Read options/settings.conf
            void ReadSettings(void);
    }; // Settings class

    // Settings global
    extern EditorSettings gSettings;
} // CBLT