#pragma once

#include "CBLT_Util.hpp"

#include <vector>
#include <unordered_map>

#include "CBLT_raylib.hpp"

namespace CBLT {
    // Editor font
    class EditorFont {
        private:
            // Build glyph set for the extended ASCII table, no Unicode
            std::vector<UT::i32>BuildGlyphSet(void);
        public:
            // Font size
            UT::i32 size;

            // Usable font
            Font f;

            // Advances map
            std::unordered_map<UT::i32, UT::i32> advanceMap;

            // Constructor
            EditorFont(void);

            // Destructor
            ~EditorFont(void);

            // Load a font from a specific file
            void Load(std::string fontName);

            // Create configurations
            void Config(void);

            // Calculate codepoints from UTF8 encoding
            std::vector<UT::i32> Utf8ToCodepoints(const std::string& str);
            
            // Calculate codepoints from UTF8 encoding
            std::vector<UT::i32> Utf8ToCodepoints(const std::string_view str);
    }; // EditorFont class

    // Current usable font global
    extern EditorFont gFont;
} // CBLT
