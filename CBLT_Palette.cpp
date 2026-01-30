#include "CBLT_Palette.hpp"

namespace CBLT {
    Palette gPalette;

    Palette::Palette(void) {
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
        fileQueueEntry      = UF::C(64, 128, 255);
        currentFile         = UF::C(0, 255, 0);
        cwd                 = UF::C(255, 255, 0);
        cwdDir              = UF::C(255, 128, 0);
        cwdFile             = UF::C(255, 255, 0);
        dirty               = UF::C(255, 64, 64);
        clean               = UF::C(64, 255, 64);
        textLines           = UF::C(255, 255, 255);
        textBase            = UF::C(255, 255, 255);

        cursorPosHighlight   = cursor;
        cursorPosHighlight.a = 32;
        
        selectionColor      = UF::C(50, 150, 255);
        selectionColor.a    = 32;
        
        msgBackground       = UF::C(0, 0, 0);
        msgForeground       = UF::C(255, 255, 255); 
        msgErr              = UF::C(255, 64, 64); 
        msgGuide            = UF::C(64, 255, 64);
        msgInfo             = UF::C(64, 64, 255);

        // --- Debug print: addresses + RGBA ---
        #define PRINT_COLOR(field) \
            printf("%-20s @ %p -> R:%3d G:%3d B:%3d A:%3d\n", \
                #field, (void*)&field, field.r, field.g, field.b, field.a)
    
        printf("Palette object address: %p\n", (void*)this);
    
        PRINT_COLOR(background);
        PRINT_COLOR(console);
        PRINT_COLOR(consoleText);
        PRINT_COLOR(consoleBackground);
        PRINT_COLOR(consoleCursor);
        PRINT_COLOR(cursor);
        PRINT_COLOR(cursorMode);
        PRINT_COLOR(textSeperators);
        PRINT_COLOR(lineInfo);
        PRINT_COLOR(file);
        PRINT_COLOR(fileQueueEntry);
        PRINT_COLOR(currentFile);
        PRINT_COLOR(cwd);
        PRINT_COLOR(cwdDir);
        PRINT_COLOR(cwdFile);
        PRINT_COLOR(dirty);
        PRINT_COLOR(clean);
        PRINT_COLOR(textLines);
        PRINT_COLOR(textBase);
        PRINT_COLOR(cursorPosHighlight);
        PRINT_COLOR(selectionColor);
        PRINT_COLOR(msgBackground);
        PRINT_COLOR(msgForeground);
        PRINT_COLOR(msgErr);
        PRINT_COLOR(msgGuide);
        PRINT_COLOR(msgInfo);
    
        #undef PRINT_COLOR      

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