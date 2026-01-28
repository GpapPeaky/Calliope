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
        cwdDir              = UF::C(255, 128, 0);
        cwdFile             = UF::C(255, 255, 0);
        dirty               = UF::C(255, 64, 64);
        clean               = UF::C(64, 255, 64);
        textLines           = UF::C(255, 255, 255);
        textBase            = UF::C(255, 255, 255);
        cursorPosHighlight  = cursor;                    cursorPosHighlight.a = 32;
        selectionColor      = UF::C(50, 150, 255);       selectionColor.a = 32;
        msgBackground       = UF::C(0, 0, 0);
        msgForeground       = UF::C(255, 255, 255); 
        msgErr              = UF::C(255, 64, 64); 
        msgGuide            = UF::C(64, 255, 64);
        msgInfo             = UF::C(64, 64, 255);

        // Image load        

        // img_textBackground        = LoadImage("");
        // img_CWDContentsBackground = LoadImage("");
        // img_consoleBackground     = LoadImage("");
        // img_topBarBackground      = LoadImage("");
    }

    Palette::~Palette() {}

    Palette gPalette;
} // CBLT