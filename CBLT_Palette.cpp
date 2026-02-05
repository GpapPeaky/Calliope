#include "CBLT_Palette.hpp"

namespace CBLT {
    Palette gPalette;

    Palette::Palette(void) {
        background          = UF::C(0, 0, 0);
        console             = UF::C(255, 255, 255);
        consoleText         = UF::C(255, 255, 255);
        consoleBackground   = UF::C(0, 0, 0);
        consoleCursor       = UF::C(0, 255, 255);
        cursor              = UF::C(0, 255, 255);
        cursorMode          = UF::C(0, 255, 0);
        textSeperators      = UF::C(255, 255, 255);
        lineInfo            = UF::C(0, 255, 255); 
        file                = UF::C(255, 0, 255);
        cwd                 = UF::C(255, 255, 0);
        cwdDir              = UF::C(255, 128, 0);
        cwdFile             = UF::C(255, 255, 0);
        dirty               = UF::C(255, 64, 64);
        clean               = UF::C(64, 255, 64);
        textBase            = UF::C(192, 192, 192);
        textLines           = UF::C(192, 192, 192);
        
        cursorPosHighlight   = cursor;
        cursorPosHighlight.a = 32;
        
        selectionColor      = UF::C(50, 150, 255);
        selectionColor.a    = 32;
        
        msgBackground       = UF::C(0, 0, 0);
        msgForeground       = UF::C(255, 255, 255); 
        msgErr              = UF::C(255, 64, 64); 
        msgGuide            = UF::C(64, 255, 64);
        msgInfo             = UF::C(64, 64, 255);
        
        fileQueueEntry      = UF::C(64, 64, 64);
        currentFile         = UF::C(192, 192, 192);

        fileQueueColour     = UF::C(0, 0, 0);

        lex_id    = UF::C(200, 200, 255);
        lex_comm  = UF::C(128, 128, 128);
        lex_key   = UF::C(255, 128, 0);
        lex_num   = UF::C(0, 200, 200);
        lex_op    = UF::C(255, 255, 255);
        lex_str   = UF::C(0, 200, 0); 
        lex_pun   = UF::C(128, 128, 128);

        // Image load        

        img_textBackground        = LoadImage("assets/image/placeholder.png");
        img_CWDContentsBackground = LoadImage("assets/image/placeholder.png");
        img_consoleBackground     = LoadImage("assets/image/placeholder.png");
        img_topBarBackground      = LoadImage("assets/image/placeholder.png");
    }

    Palette::~Palette(void) {
        UnloadImage(img_textBackground);
        UnloadImage(img_CWDContentsBackground);
        UnloadImage(img_consoleBackground);
        UnloadImage(img_topBarBackground);
    }
} // CBLT