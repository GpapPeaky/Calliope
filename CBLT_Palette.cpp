#include "CBLT_Palette.hpp"

namespace CBLT {
    Palette::Palette() {
        background          = UF::C(0, 0, 0);
        console             = UF::C(128, 128, 128);
        consoleText         = UF::C(128, 128, 128);
        consoleBackground   = UF::C(128, 128, 128);
        consoleCursor       = UF::C(255, 255, 255);
        cursor              = UF::C(255, 255, 255);
        cursorMode          = UF::C(0, 255, 0);
        textSeperators      = UF::C(128, 128, 128);
        lineInfo            = UF::C(0, 255, 255); 
        file                = UF::C(255, 0, 255);
        cwd                 = UF::C(255, 255, 0);
        dirty               = UF::C(255, 128, 0);
        clean               = UF::C(128, 255, 0);
        textLines           = UF::C(128, 128, 128);
        textBase            = UF::C(128, 128, 128);
    }

    Palette::~Palette() {}

    Palette gPalette;
} // CBLT