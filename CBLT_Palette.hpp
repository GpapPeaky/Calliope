#pragma once

#include "CBLT_Util.hpp"

#include "CBLT_raylib.hpp"

namespace CBLT {
    // Palette class for changing colours
    class Palette {
        public:
            Image img_textBackground;           // Text background image
            Image img_CWDContentsBackground;    // CWD list background image
            Image img_consoleBackground;        // Console backgorund image
            Image img_topBarBackground;         // Top bar Image

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
            Color fileQueueEntry;       // File in queue display colour
            Color currentFile;          // Current file in queue display colour

            // Constructor
            Palette(void);

            // Destructor
            ~Palette(void);

    }; // Palette class

    // Palette global
    extern Palette gPalette;
} // CBLT