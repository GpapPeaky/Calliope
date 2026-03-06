#pragma once

#include <unordered_map>
#include <sstream>

#include "CBLT_Util.hpp"

#include "CBLT_raylib.hpp"

namespace CBLT {
    // Palette class for changing colours
    class Palette {
        public:
            std::string key;            // Palette name key 

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
            Color fileQueueColour;      // File queue background colour
            Color openFileColor;        // Current open file color in the CWD list
            Color frag;                 // Current cursor fragment color

            Color lex_id;               // Id colours
            Color lex_comm;             // Comment colours
            Color lex_key;              // keyword colours
            Color lex_num;              // number literal colours
            Color lex_op;               // operator colours
            Color lex_str;              // string literal colours
            Color lex_pun;              // punctuation colours
            Color lex_misc;             // Misc colour based on language

            // Constructor
            Palette(void);

            // Destructor
            ~Palette(void);

            // Check if the given file is the theme options file
            UT::b IsThemeOptions(std::string fname) const ;

            // Read the palette file and assign the correct colours
            UT::b ReadPaletteFile(const std::string& path);
    }; // Palette class

    // Palette global
    extern Palette gPalette;

} // CBLT