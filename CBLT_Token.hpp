#pragma once

#include "CBLT_raylib.hpp"

#include "CBLT_Util.hpp"
#include "CBLT_Palette.hpp"
#include "CBLT_Font.hpp"

#include <string>

namespace CBLT {
    enum class TokenClass : UT::ui8 {
        ID,
        KEYWORD,
        NUM,
        STRING,
        COMMENT,
        OPERATOR,
        WHITESPACE,
        PUNCTUATION,
        MISC,
        UNKOWN,
        FUNCTION
    }; // Token type

    class Token {
        public:
            TokenClass type; // Token type
            UT::ui32 line;   // Token at line
            UT::ui32 col;    // Token at column   
            UT::ui32 len;    // Token length

            // Constructor
            Token(TokenClass tc, UT::ui32 line, UT::ui32 col, UT::ui32 len);
            
            // Destructor
            ~Token(void);

            // Select the token colour
            Color TokenColor(void);

            // For token advancing
            UT::ui32 GetCursorX(const std::string_view lineText, UT::ui32 fontSize, UT::ui32 column);
    }; // Token class
} // CBLT