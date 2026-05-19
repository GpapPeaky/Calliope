#pragma once

#include "CBLT_Util.hpp"

namespace CBLT {
    // Cursor symbol options for settings
    enum class CursorSymbol : UT::i32 {
        NON_ASCII_UNDERSCORE  = -4,          // Non-ASCII underscore
        NON_ASCII_LINE        = -3,          // Non-ASCII line
        NON_ASCII_HOLLOW_BOX  = -2,          // Non-ASCII hollow box
        NON_ASCII_BOX         = -1           // Non-ASCII box
    };
} // CBLT
