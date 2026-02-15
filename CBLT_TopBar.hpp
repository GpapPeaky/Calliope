#pragma once

#include "CBLT_raylib.hpp"
#include "CBLT_Util.hpp"
#include "CBLT_Font.hpp"
#include "CBLT_Palette.hpp"
#include "CBLT_Cursor.hpp"

namespace CBLT {
    // Draw top bar and all data that live inside it
    void DrawInfo(CBLT::Cursor& c, UT::ui32 lineCount, UT::b dirty, std::string fname, std::string cwd);
} // CBLT
