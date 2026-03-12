#include "CBLT_InfileAutocomplete.hpp"

// TODO: Reload tokens, try to make it cachable by line like the acutal infile tokens
// TODO: Add autocomplete function!
// TODO: Fix some issues with the cursor fragment

namespace CBLT {
    InfileAutocomplete::InfileAutocomplete(void) {
        current = 0;
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
        if (suggestions.empty()) return;
    
        const UT::ui8 margin = 5;
        const UT::f32 lineHeight = gFont.size + margin;
    
        for (UT::llui32 i = 0 ; i < suggestions.size() ; ++i) {
            UT::i32 offset = (UT::i32)i - (UT::i32)current;
    
            UT::f32 x = cursorX;
            UT::f32 y = cursorY + (offset * lineHeight);
    
            DrawTextEx(
                gFont.f,
                suggestions[i].c_str(),
                {
                    x + 75.0f,
                    y + gFont.size
                },
                gFont.size,
                0.0f,
                UF::C(255,255,255)
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

    std::string InfileAutocomplete::GetCurrentSuggestion(void) {
        if (suggestions.empty() || current >= suggestions.size()) {
            return "";
        }

        current = 0; // Reset to the first suggestion after returning the string
        return suggestions[current];
    }
} // CBLT