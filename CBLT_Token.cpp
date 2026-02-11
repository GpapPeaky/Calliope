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
            case TokenClass::MISC:
                return CBLT::gPalette.lex_misc;
            default:
                return {0, 0, 0, 0}; // Nothing 
        }
    }

    UT::ui32 Token::GetCursorX(std::string_view lineText, UT::ui32 fontSize, UT::ui32 column) {
        float scale = static_cast<float>(fontSize) / gFont.f.baseSize;
        UT::ui32 width = 0;
    
        // Convert UTF-8 to codepoints
        auto codepoints = CBLT::gFont.Utf8ToCodepoints(lineText); // make this accept string_view
    
        column = std::min(column, static_cast<UT::ui32>(codepoints.size()));
    
        for (size_t i = 0; i < column; i++) {
            UT::i32 cp = codepoints[i];
    
            // Fast lookup using precomputed map
            UT::i32 advance = fontSize / 2; // fallback
            auto it = gFont.advanceMap.find(cp);
            if (it != gFont.advanceMap.end()) advance = it->second;
    
            width += advance;
        }
    
        return static_cast<UT::ui32>(width * scale);
    }

} // CBLT
