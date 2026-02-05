#include "CBLT_Token.hpp"

namespace CBLT {
    Token::Token(TokenClass tc, UT::ui32 line, UT::ui32 col, UT::ui32 len) {
        this->type = tc; 
        this->line = line; 
        this->col = col; 
        this->len = len; 
    }
    
    Token::~Token(void) {}

    Color Token::TokenColor(void) {
        switch (this->type) {
            case TokenClass::ID:
                return CBLT::gPalette.lex_id;
            case TokenClass::COMMENT:
                return CBLT::gPalette.lex_comm;
            case TokenClass::KEYWORD:
                return CBLT::gPalette.lex_key;
            case TokenClass::NUM:
                return CBLT::gPalette.lex_num;
            case TokenClass::OPERATOR:
                return CBLT::gPalette.lex_op;
            case TokenClass::STRING:
                return CBLT::gPalette.lex_str;
            case TokenClass::PUNCTUATION:
                return CBLT::gPalette.lex_pun;
            default:
                return {0, 0, 0, 0}; // Nothing 
        }
    }

    UT::ui32 Token::GetCursorX(const std::string& lineText, UT::ui32 fontSize, UT::ui32 column) {
        // Scale based on font size vs base font size
        UT::f32 scale = static_cast<UT::f32>(fontSize) / gFont.f.baseSize;
    
        UT::ui32 width = 0;
    
        // Convert the line to codepoints (handles UTF-8)
        auto codepoints = CBLT::gFont.Utf8ToCodepoints(lineText);
    
        // Iterate over each codepoint up to the target column
        for (size_t i = 0; i < column && i < codepoints.size(); i++) {
            UT::i32 cp = codepoints[i];
            UT::i32 glyphIndex = -1;
    
            // Lookup glyph in the font
            for (UT::i32 g = 0; g < gFont.f.glyphCount; g++) {
                if (gFont.f.glyphs[g].value == cp) {
                    glyphIndex = g;
                    break;
                }
            }
    
            if (glyphIndex >= 0) {
                // Add glyph advance
                width += gFont.f.glyphs[glyphIndex].advanceX;
            } else {
                // Fallback width for missing glyphs
                width += fontSize / 2;
            }
        }
    
        return static_cast<UT::ui32>(width * scale);
    }
} // CBLT
