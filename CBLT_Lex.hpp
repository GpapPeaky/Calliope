#pragma once

#include "CBLT_Util.hpp"
#include "CBLT_Language.hpp"
#include "CBLT_Token.hpp"

namespace CBLT {
    enum class LexerState {
        NONE,
        INBLOCK,            // Block comment
        INCOMMENT,          // Line comment
        INMACRO,
        INANNOTATION,
        INSTRING,
        COMMENT_NOTE
    }; // Lexer current state

    // Run the lexer through the line, returns if the line is in a comment block
    LexerState LexLine(const std::string& s, UT::ui32 line, LexerState ls, Language& lang, std::vector<Token>& tokens);
} //  CBLT