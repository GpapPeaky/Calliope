#include "CBLT_Palette.hpp"

namespace CBLT {
    Palette::Palette() {
        background          = UF::C(0, 0, 0);
        console             = UF::C(255, 255, 255);
        consoleText         = UF::C(255, 255, 255);
        consoleBackground   = UF::C(0, 0, 0);
        consoleCursor       = UF::C(0, 64, 255);
        cursor              = UF::C(0, 64, 255);
        cursorMode          = UF::C(0, 255, 0);
        textSeperators      = UF::C(255, 255, 255);
        lineInfo            = UF::C(0, 255, 255); 
        file                = UF::C(255, 0, 255);
        cwd                 = UF::C(255, 255, 0);
        dirty               = UF::C(255, 64, 64);
        clean               = UF::C(64, 255, 64);
        textLines           = UF::C(255, 255, 255);
        textBase            = UF::C(255, 255, 255);
    }

    Palette::~Palette() {}

    Palette gPalette;
} // CBLT