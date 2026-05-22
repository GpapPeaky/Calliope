#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "CBLT_Animation.hpp"    // For animation profiles
#include "CBLT_CursorSymbol.hpp" // For cursor symbol enum

namespace CBLT {
    class EditorSettings {
        public:
            UT::ui32 OPTION_TabSize = 4;        // Tab size in spaces   

            std::string OPTION_WIN32_Term = ""; // Terminal for win32
            std::string OPTION_POSIX_Term = ""; // Terminal for posix
            std::string OPTION_Palette    = ""; // Palette option

            CursorSymbol OPTION_CursorSymbol = CursorSymbol::NON_ASCII_UNDERSCORE; // Cursor symbol option

            AnimationProfile OPTION_ANIM_FileCursor;                               // Animation profile for file cursors
            AnimationProfile OPTION_ANIM_FQ;                                       // Animation profile for file queue movement
            AnimationProfile OPTION_ANIM_Console;                                  // Animation profile for console movement
            AnimationProfile OPTION_ANIM_ConsoleCursor;                            // Animation profile for console cursor
            AnimationProfile OPTION_ANIM_ConsoleContent;                           // Animation profile for console content panning

            // Read options/settings.cbltconf
            void ReadSettings(void);
    }; // Settings class

    // Settings global
    extern EditorSettings gSettings;
} // CBLT