#include "CBLT_Win.hpp"

namespace CBLT {
    std::string Version = std::string("CoBaLT v2.1.1");
    
    namespace Win {

        void Init(void) {
            SetTraceLogLevel(LOG_ERROR);

            Image icon = LoadImage("assets/icon/muse.png");
            
            const UT::ui32 screenWidth = 1720;
            const UT::ui32 screenHeight = 920;
            
            SetConfigFlags(FLAG_WINDOW_RESIZABLE);
            InitWindow(screenWidth, screenHeight, Version.c_str());
            
            SetWindowIcon(icon);
            
            // Free image
            UnloadImage(icon);
            
            SetTargetFPS(180);

            SetExitKey(KEY_NULL); // Disable automatic ESC exit
        }

        void Destroy(void) {
            CloseWindow();
        }

    } // Win
} // CBLT
