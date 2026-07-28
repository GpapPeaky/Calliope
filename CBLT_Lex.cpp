#include "CBLT_Lex.hpp"

CBLT::LexerState CBLT::LexLine(const std::string& s, UT::ui32 line, LexerState ls, Language& lang, std::vector<Token>& tokens) {
    LexerState state = ls;
    const auto& block = lang.commentBlock;
    const auto& lineC = lang.commentLine;
    const auto& str   = lang.stringDelim;
    const auto& ops   = lang.operators;
    const auto& puns  = lang.punctuations;

    UT::ui32 i = 0;

    // Cool one!
    auto push = [&](TokenClass tc, UT::ui32 start, UT::ui32 end) {
        tokens.emplace_back(tc, line, start, end - start);
    };

    while (i < s.size()) {
        bool matched = false;

        // Continuation of a block comment from a previous line
        if (state == LexerState::INBLOCK) {
            UT::ui32 j = i;
            bool closed = false;
            while (j < s.size()) {
                bool atClose = false;
                for (const auto& [bstart, bend] : block) {
                    if (s.compare(j, bend.size(), bend) == 0) {
                        UT::llui32 end = j + bend.size();
                        UT::llui32 notePos = std::string::npos;

                        for (const auto& note : lang.commentNotes) {
                            auto p = s.find(note, i);
                            if (p != std::string::npos &&
                                p < end &&
                                (notePos == std::string::npos || p < notePos))
                                notePos = p;
                        }

                        if (notePos == std::string::npos) {
                            push(TokenClass::COMMENT, i, end);
                        } else {
                            push(TokenClass::COMMENT, i, notePos);
                            push(TokenClass::MISC, notePos, end);
                        }

                        i = j + bend.size();
                        state = LexerState::NONE;
                        closed = true;
                        atClose = true;
                        break;
                    }
                }
                if (atClose) break;
                ++j;
            }

            if (!closed) {
                UT::llui32 pos = std::string::npos;

                for (const auto& note : lang.commentNotes) {
                    auto p = s.find(note, i);
                    if (p != std::string::npos) {
                        pos = p;
                        break;
                    }
                }

                if (pos != std::string::npos) {
                    if (pos > i)
                        push(TokenClass::COMMENT, i, pos);

                    push(TokenClass::MISC, pos, s.size());
                } else {
                    push(TokenClass::COMMENT, i, s.size());
                }
                
                return state;
            }
            continue;
        }

        // Block comment open
        for (const auto& [bstart, bend] : block) {
            if (s.compare(i, bstart.size(), bstart) == 0) {
                UT::ui32 j = i + bstart.size();
                bool closed = false;
                while (j < s.size()) {
                    if (s.compare(j, bend.size(), bend) == 0) {
                        UT::llui32 end = j + bend.size();
                        UT::llui32 pos = std::string::npos;

                        for (const auto& note : lang.commentNotes) {
                            auto p = s.find(note, i);
                            if (p != std::string::npos && p < end) {
                                pos = p;
                                break;
                            }
                        }

                        if (pos != std::string::npos) {
                            if (pos > i)
                                push(TokenClass::COMMENT, i, pos);

                            push(TokenClass::MISC, pos, end);
                        } else {
                            push(TokenClass::COMMENT, i, end);
                        }

                        i = j + bend.size();
                        closed = true;
                        break;
                    }
                    ++j;
                }

                if (!closed) {
                    UT::llui32 pos = std::string::npos;

                    for (const auto& note : lang.commentNotes) {
                        auto p = s.find(note, i);
                        if (p != std::string::npos) {
                            pos = p;
                            break;
                        }
                    }

                    if (pos != std::string::npos) {
                        if (pos > i)
                            push(TokenClass::COMMENT, i, pos);

                        push(TokenClass::MISC, pos, s.size());
                    } else {
                        push(TokenClass::COMMENT, i, s.size());
                    }

                    state = LexerState::INBLOCK;
                    return state;
                }
                matched = true;
                break;
            }
        }
        if (matched) continue;

        // Line comment
        for (const auto& lc : lineC) {
            if (s.compare(i, lc.size(), lc) == 0) {
                UT::llui32 notePos = std::string::npos;

                for (const auto& note : lang.commentNotes) {
                    auto p = s.find(note, i + lc.size());

                    if (p != std::string::npos &&
                        (notePos == std::string::npos || p < notePos))
                        notePos = p;
                }

                if (notePos == std::string::npos) {
                    push(TokenClass::COMMENT, i, s.size());
                } else {
                    push(TokenClass::COMMENT, i, notePos);
                    push(TokenClass::MISC, notePos, s.size());
                }

                return state;
            }
        }

        // Macro
        for (const auto& prefix : lang.macros) {
            if (s.compare(i, prefix.size(), prefix) == 0) {
                push(TokenClass::MISC, i, s.size());
                return state;
            }
        }

        // Annotations are coloured with the same color as macros
        for (const auto& prefix : lang.annotations) {
            if (s.compare(i, prefix.size(), prefix) == 0) {
                push(TokenClass::MISC, i, s.size());
                return state;
            }
        }

        // String
        for (const auto& [open, close] : str) {
            if (s.compare(i, open.size(), open) == 0) {
                UT::ui32 j = i + open.size();
                while (j < s.size()) {
                    if (lang.settings.escapeSequences && s[j] == '\\') { j += 2; continue; }
                    if (s.compare(j, close.size(), close) == 0) break;
                    ++j;
                }
                j = (j < s.size()) ? j + close.size() : s.size();
                push(TokenClass::STRING, i, j);
                i = j;
                matched = true;
                break;
            }
        }
        if (matched) continue;

        // Operator
        for (const auto& op : ops) {
            if (s.compare(i, op.size(), op) == 0) {
                push(TokenClass::OPERATOR, i, i + op.size());
                i += op.size();
                matched = true;
                break;
            }
        }
        if (matched) continue;

        // Punctuation
        for (const auto& pun : puns) {
            if (s.compare(i, pun.size(), pun) == 0) {
                push(TokenClass::PUNCTUATION, i, i + pun.size());
                i += pun.size();
                matched = true;
                break;
            }
        }
        if (matched) continue;

        // Identifier / keyword
        if (std::isalpha(s[i]) || s[i] == '_') {
            UT::ui32 j = i + 1;
            while (j < s.size() && (std::isalnum(s[j]) || s[j] == '_'))
                ++j;
            std::string_view word(&s[i], j - i);
            push(lang.IsKeyword(std::string(word)) ? TokenClass::KEYWORD : TokenClass::ID, i, j);
            i = j;
            continue;
        }

        // Number
        if (std::isdigit(s[i])) {
            UT::ui32 j = i + 1;
            while (j < s.size() && std::isdigit(s[j]))
                ++j;
            push(TokenClass::NUM, i, j);
            i = j;
            continue;
        }

        // Fallthrough
        push(TokenClass::MISC, i, i + 1);
        ++i;
    }

    return state;
}