#include "CBLT_Palette.hpp"

namespace CBLT {
    Palette gPalette;

    Palette::Palette(void) {
        key = "Stockholm";
        
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
        lex_comm    = UF::C(0, 255, 64);
        lex_key     = UF::C(64, 128, 255);  // Sky Blue: Distinct from Cyan cursor
        lex_num     = UF::C(255, 128, 64);  // Orange: High visibility for data
        lex_op      = UF::C(255, 255, 0);   // Yellow: Stands out for logic
        lex_str     = UF::C(0, 255, 255);
        lex_pun     = UF::C(255, 255, 255); // Silver: Subtle structure
        lex_misc    = UF::C(255, 64, 255);
    }

    Palette::~Palette(void) {}

    UT::b Palette::ReadPaletteFile(const std::string& path) {  // The key to the palette file is the .pal file name without the extension, located in options/palettes/
        std::string fullPath = "options/palettes/" + path + ".pal";

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
            {"lex_misc", &lex_misc}
        };

        std::string line;
        bool inPalBlock = false;
    
        while (std::getline(f, line)) {
            line = UF::Trim(line);
    
            if (line.empty()) continue;
            if (line[0] == '!') continue;
    
            if (line == "%pal") {
                inPalBlock = !inPalBlock;
                continue;
            }

            if (!inPalBlock) continue;
    
            size_t colonPos = line.find(':');
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
                *fieldMap[key] = UF::C(r, g, b);
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