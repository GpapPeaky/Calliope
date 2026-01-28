#pragma once

#include "CBLT_Util.hpp"

#include "raylib.h"

namespace CBLT {
    // Palette class for changing colours
    class Palette {
        public:
            Image textBackground;           // Text background image
            Image CWDContentsBackground;    // CWD list background image
            Image consoleBackground;        // Console backgorund image
            Image topBarBackground;         // Top bar Image

            Color background;           // Background color
            Color console;              // Console window colour
            Color consoleText;          // Console text
            Color consoleBackground;    // Console background
            Color consoleCursor;        // Console cursor
            Color cursor;               // Cursor
            Color cursorMode;           // Cursor mode display
            Color textSeperators;       // Text seperators
            Color lineInfo;             // Top left line info
            Color file;                 // File display colour
            Color cwd;                  // CWD display colour
            Color cwdDir;               // Console CWD content dir
            Color cwdFile;              // Console CWD content file
            Color dirty;                // Dirty file
            Color clean;                // Clean file
            Color textLines;            // Text lines
            Color textBase;             // Text
            Color cursorPosHighlight;   // Cursor pos highlight box
            Color selectionColor;       // Select mode colour
            Color msgBackground;        // Msg box background
            Color msgForeground;        // Msg box foreground
            Color msgErr;               // Error msg colour
            Color msgGuide;             // Gude msg colour
            Color msgInfo;              // Info msg colour

            // Constructor
            Palette();

            // Destructor
            ~Palette();

    }; // Palette class

    // Palette global
    extern Palette gPalette;
} // CBLT