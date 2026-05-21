#include "CBLT_Palette.hpp"

namespace CBLT {
    Palette gPalette;

    Palette::Palette(void) {
        background          = toCol(UF::C(0, 0, 0));
        console             = toCol(UF::C(255, 255, 255));
        consoleText         = toCol(UF::C(255, 255, 255));
        consoleBackground   = toCol(UF::C(0, 0, 0));
        consoleCursor       = toCol(UF::C(160, 160, 160));
        cursor              = toCol(UF::C(160, 160, 160));
        cursorMode          = toCol(UF::C(0, 255, 255));
        textSeperators      = toCol(UF::C(255, 255, 255));
        lineInfo            = toCol(UF::C(128, 128, 128)); 
        file                = toCol(UF::C(255, 255, 0));
        cwd                 = toCol(UF::C(255, 0, 255));
        cwdDir              = toCol(UF::C(96, 96, 96));
        cwdFile             = toCol(UF::C(160, 160, 160));
        dirty               = toCol(UF::C(192, 192, 192));
        clean               = toCol(UF::C(255, 255, 255));
        textBase            = toCol(UF::C(192, 192, 192));
        textLines           = toCol(UF::C(192, 192, 192));
        
        cursorPosHighlight   = cursor;
        cursorPosHighlight.a = 24;
        
        selectionColor      = toCol(UF::C(50, 150, 255));
        selectionColor.a    = 64;
        
        msgBackground       = toCol(UF::C(0, 0, 0));
        msgForeground       = toCol(UF::C(255, 255, 255)); 
        msgErr              = toCol(UF::C(255, 64, 64)); 
        msgGuide            = toCol(UF::C(64, 255, 64));
        msgInfo             = toCol(UF::C(64, 64, 255));
        
        fileQueueEntry      = toCol(UF::C(64, 64, 64));
        currentFile         = toCol(UF::C(192, 192, 192));

        fileQueueColour     = toCol(UF::C(0, 0, 0));

        openFileColor       = toCol(UF::C(255, 255, 255));

        frag                = toCol(UF::C(0, 128, 192));

        lex_id      = toCol(UF::C(192, 192, 192)); // White: Clear, standard text
        lex_comm    = toCol(UF::C(0, 255, 64));
        lex_key     = toCol(UF::C(64, 128, 255));  // Sky Blue: Distinct from Cyan cursor
        lex_num     = toCol(UF::C(255, 128, 64));  // Orange: High visibility for data
        lex_op      = toCol(UF::C(255, 255, 0));   // Yellow: Stands out for logic
        lex_str     = toCol(UF::C(0, 255, 255));
        lex_pun     = toCol(UF::C(255, 255, 255)); // Silver: Subtle structure
        lex_misc    = toCol(UF::C(255, 64, 255));
        lex_func    = toCol(UF::C(32, 128, 255));
    }

    Palette::~Palette(void) {}

    UT::b Palette::ReadPaletteFile(const std::string& path) {  // The key to the palette file is the .pal file name without the extension, located in options/palettes/
        std::string resourcePath;

        #if defined(__linux__)
            const char* resource_path = getenv("CBLT_RESOURCES");
            resourcePath = resource_path ? std::string(resource_path) : ".";
            resourcePath += '/';
        #endif

        std::string fullPath = resourcePath + "options/palettes/" + path + ".pal";

        std::ifstream f(fullPath);

        if (!f.is_open()) {
            UE::Log("Failed to open palette file: " + fullPath);
            return false;
        }

        std::unordered_map<std::string, Color*> fieldMap = {
            {"background", &background},
            {"console", &console},
            {"console_text", &consoleText},
            {"console_background", &consoleBackground},
            {"console_cursor", &consoleCursor},
            {"cursor", &cursor},
            {"cursor_mode", &cursorMode},
            {"text_seperators", &textSeperators},
            {"line_info", &lineInfo},
            {"file", &file},
            {"cwd", &cwd},
            {"cwd_dir", &cwdDir},
            {"cwd_file", &cwdFile},
            {"dirty", &dirty},
            {"clean", &clean},
            {"text_base", &textBase},
            {"text_lines", &textLines},
            {"selection_colour", &selectionColor},
            {"msg_background", &msgBackground},
            {"msg_foreground", &msgForeground},
            {"msg_err", &msgErr},
            {"msg_guide", &msgGuide},
            {"msg_info", &msgInfo},
            {"file_q_entry", &fileQueueEntry},
            {"current_file_entry", &currentFile},
            {"file_q_color", &fileQueueColour},
            {"open_file_color", &openFileColor},
            {"frag", &frag},
            {"lex_id", &lex_id},
            {"lex_comm", &lex_comm},
            {"lex_key", &lex_key},
            {"lex_num", &lex_num},
            {"lex_op", &lex_op},
            {"lex_str", &lex_str},
            {"lex_pun", &lex_pun},
            {"lex_misc", &lex_misc},
            {"lex_func", &lex_func}
        };

        std::string line;
        bool inPalBlock = false;
    
        while (std::getline(f, line)) {
            line = UF::Trim(line);
    
            if (line.empty()) continue;
            if (line.at(0) == '!') continue;
    
            if (line == "%pal") {
                inPalBlock = !inPalBlock;
                continue;
            }

            if (!inPalBlock) continue;
    
            UT::llui32 colonPos = line.find(':');
            if (colonPos == std::string::npos) continue;
    
            std::string key = UF::Trim(line.substr(0, colonPos));
            std::string value = UF::Trim(line.substr(colonPos + 1));
    
            // Expect [r, g, b]
            if (value.front() != '[' || value.back() != ']')
                continue;
    
            value = value.substr(1, value.size() - 2);
    
            std::stringstream ss(value);
            int r, g, b;
            char comma;
    
            ss >> r >> comma >> g >> comma >> b;
    
            if (fieldMap.count(key)) {
                *fieldMap[key] = toCol(UF::C(r, g, b));
            }
        }
    
        f.close();
    
        // Recalculate derived values
        cursorPosHighlight = cursor;
        cursorPosHighlight.a = 24;
    
        selectionColor.a = 64;

        return true;
    }
} // CBLT