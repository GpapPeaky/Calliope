#include "CBLT_InfileAutocomplete.hpp"

namespace CBLT {
    InfileAutocomplete::InfileAutocomplete(void) {
        current = 0;
        open = false;
    }

    InfileAutocomplete::~InfileAutocomplete(void) {}

    void InfileAutocomplete::LoadTokens(std::vector<std::vector<Token>>& fileTokens, std::vector<std::string>& fileText) {
        lineTokens.resize(fileTokens.size());
    
        for (UT::llui32 i = 0; i < fileTokens.size(); i++) {
            for (auto& token : fileTokens[i]) {
                if (token.type != TokenClass::ID) continue;
    
                if (token.line  >= fileText.size()) continue;

                const std::string& src = fileText[token.line];
    
                if (token.col >= src.size()) continue;
                if (token.len > src.size() - token.col) continue;
    
                std::string t(src.data() + token.col, token.len);
    
                lineTokens[i].insert(t);
                allTokens.insert(t);
            }
        }
    
        if (!gKeywords.empty())
            allTokens.insert(gKeywords.begin(), gKeywords.end());
    
        tokens.assign(allTokens.begin(), allTokens.end());
    }
    
    void InfileAutocomplete::UpdateLine(UT::ui32 line, std::vector<Token>& tokensLine, const std::string& text) {
        if (line >= lineTokens.size())
            lineTokens.resize(line + 1);

        for (auto& t : lineTokens[line]) {
            allTokens.erase(t);
        }
        
        lineTokens[line].clear();
        
        for (auto& token : tokensLine) {
            if (token.type != TokenClass::ID) continue;
    
            if (token.len > text.size() - token.col) continue;
        
            std::string s(text.data() + token.col, token.len);
        
            lineTokens[line].insert(s);
            allTokens.insert(s);
        }
        
        tokens.assign(allTokens.begin(), allTokens.end());
    }

    void InfileAutocomplete::GetSuggestions(const std::string& fragment) {
        if (fragment.empty()) {
            suggestions.clear();
            return;
        }

        const UT::ui32 suggestionEntriesMax = 27;

        suggestions.clear();
        for (const auto& token : tokens) {
            if (token.compare(0, fragment.size(), fragment) == 0 && suggestions.size() < suggestionEntriesMax) {
                suggestions.push_back(token);
            }
        }
    }

    void InfileAutocomplete::DrawSuggestions(UT::ui32 cursorX, UT::ui32 cursorY) {
        if (!open) return;
        if (suggestions.empty()) return;
    
        const UT::ui8 margin = 5;
        const UT::f32 lineHeight = gFont.size + margin;

        // Background
        DrawRectangleLines(
            cursorX + 90.0f - 1.0f,
            cursorY - 1.0f + gFont.size,
            202.0f,
            lineHeight * suggestions.size() + 2.0f,
            { gPalette.console.r, gPalette.console.g, gPalette.console.b, 255 }
        );

        // Foreground
        DrawRectangle(
            cursorX + 90.0f,
            cursorY + gFont.size,
            200.0f,
            lineHeight * suggestions.size(),
            { gPalette.consoleBackground.r, gPalette.consoleBackground.g, gPalette.consoleBackground.b, 255 }
        );

        for (UT::llui32 i = 0 ; i < suggestions.size() ; ++i) {
            UT::i32 offset = (UT::i32)i - (UT::i32)current;
    
            UT::f32 x = cursorX;
            UT::f32 y = cursorY + (offset * lineHeight);
    
            DrawTextEx(
                gFont.f,
                suggestions[i].c_str(),
                {
                    x + 90.0f + margin,
                    y + gFont.size
                },
                gFont.size,
                0.0f,
                { gPalette.consoleText.r, gPalette.consoleText.b, gPalette.consoleText.g, 255 }
            );
        }
    }

    void InfileAutocomplete::Up(void) {
        if (suggestions.empty()) return;

        if (current > 0) {
            --current;
        }
    }

    void InfileAutocomplete::Down(void) {
        if (suggestions.empty()) return;

        if (current < suggestions.size() - 1) {
            ++current;
        }
    }

    std::string InfileAutocomplete::GetCurrentSuggestion(void) const {
        if (suggestions.empty() || current >= suggestions.size()) {
            return "";
        }

        return suggestions[current];
    }

    void InfileAutocomplete::Reset(void) {
        current = 0;
    }

    std::vector<std::string> InfileAutocomplete::GetCurrentSuggestions(void) const {
        return suggestions;
    }

    void InfileAutocomplete::Close(void) {
        Reset();
        dismissed = false;
        open = false;
    }

    void InfileAutocomplete::Open(void) {
        if (dismissed) return; // Do not touch the open boolean if dismissed
        open = true;
    }

    void InfileAutocomplete::Dismiss(void) {
        dismissed = true;
        open = false;
    }

    UT::b InfileAutocomplete::IsOpen(void) const {
        return open && !dismissed;
    }
} // CBLT