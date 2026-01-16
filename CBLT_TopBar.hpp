#pragma once

#include "CBLT_Util.hpp"
#include "CBLT_Font.hpp"

#include "raylib.h"

namespace CBLT {
    namespace UI {
        // Draw top bar and all data that live inside it
        void Draw(UT::ui32 col, UT::ui32 line, UT::ui32 lineCount, UT::b dirty, std::string fname, std::string cwd, UT::i32 mode);

        // Top margin from the top bar, or it's width
        const UT::f32 TOP_BAR_HEIGHT = 50.0f;

        // Top bar seperator Y from height
        const UT::f32 TOP_BAR_SEPERATOR_Y = 17.0f;
    } // UI
} // CBLT
