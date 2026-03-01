#include "CBLT_Palette.hpp"

namespace CBLT {
    Palette gPalette;

    Palette::Palette(void) {
        background          = UF::C(0, 0, 0);
        console             = UF::C(255, 255, 255);
        consoleText         = UF::C(255, 255, 255);
        consoleBackground   = UF::C(0, 0, 0);
        consoleCursor       = UF::C(160, 160, 160);
        cursor              = UF::C(160, 160, 160);
        cursorMode          = UF::C(0, 255, 255);
        textSeperators      = UF::C(255, 255, 255);
        lineInfo            = UF::C(128, 128, 128); 
        file                = UF::C(255, 255, 0);
        cwd                 = UF::C(255, 0, 255);
        cwdDir              = UF::C(96, 96, 96);
        cwdFile             = UF::C(160, 160, 160);
        dirty               = UF::C(192, 192, 192);
        clean               = UF::C(255, 255, 255);
        textBase            = UF::C(192, 192, 192);
        textLines           = UF::C(192, 192, 192);
        
        cursorPosHighlight   = cursor;
        cursorPosHighlight.a = 24;
        
        selectionColor      = UF::C(50, 150, 255);
        selectionColor.a    = 64;
        
        msgBackground       = UF::C(0, 0, 0);
        msgForeground       = UF::C(255, 255, 255); 
        msgErr              = UF::C(255, 64, 64); 
        msgGuide            = UF::C(64, 255, 64);
        msgInfo             = UF::C(64, 64, 255);
        
        fileQueueEntry      = UF::C(64, 64, 64);
        currentFile         = UF::C(192, 192, 192);

        fileQueueColour     = UF::C(0, 0, 0);

        openFileColor       = UF::C(255, 255, 255);

        frag                = UF::C(0, 128, 192);

        lex_id      = UF::C(192, 192, 192); // White: Clear, standard text
        lex_comm    = UF::C(128, 128, 128); // Dark Grey: Pushed to background
        lex_key     = UF::C(64, 128, 255);  // Sky Blue: Distinct from Cyan cursor
        lex_num     = UF::C(255, 128, 64);  // Orange: High visibility for data
        lex_op      = UF::C(255, 255, 0);   // Yellow: Stands out for logic
        lex_str     = UF::C(64, 255, 64);   // Bright Green: Matches 'clean' / standard string color
        lex_pun     = UF::C(255, 255, 255); // Silver: Subtle structure
        lex_misc    = UF::C(255, 64, 255);
    }

    Palette::~Palette(void) {}
    
    void Palette::ReadPaletteFile(void) {
        // TODO: Make the file parser.
    }
} // CBLT