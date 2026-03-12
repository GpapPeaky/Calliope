#include "CBLT_InfileAutocomplete.hpp"

// TODO: Reload tokens, try to make it cachable by line like the acutal infile tokens
// TODO: Add autocomplete function!
// TODO: Fix some issues with the cursor fragment

namespace CBLT {
    InfileAutocomplete::InfileAutocomplete(void) {
        current = 0;
    }

    InfileAutocomplete::~InfileAutocomplete(void) {}

    void InfileAutocomplete::LoadTokens(std::vector<std::vector<Token>>& fileTokens, std::vector<std::string>& fileText){
        lineTokens.resize(fileTokens.size());
    
        for (UT::llui32 i = 0; i < fileTokens.size(); i++) {
            for (auto& token : fileTokens[i]) {
                if (token.type != TokenClass::ID) continue;
    
                const std::string& src = fileText[token.line];
    
                if (token.col + token.len > src.size()) continue;
    
                std::string t = src.substr(token.col, token.len);
    
                lineTokens[i].insert(t);
                allTokens.insert(t);
            }
        }
    
        if (!gKeywords.empty())
            allTokens.insert(gKeywords.begin(), gKeywords.end());
    
        tokens.assign(allTokens.begin(), allTokens.end());
    }
    
    void InfileAutocomplete::UpdateLine(UT::ui32 line, std::vector<Token>& tokensLine, const std::string& text) {
        for (auto& t : lineTokens[line]) {
            allTokens.erase(t);
        }
        
        lineTokens[line].clear();
        
        for (auto& token : tokensLine) {
            if (token.type != TokenClass::ID) continue;
    
            if (token.col + token.len > text.size()) continue;
        
            std::string s = text.substr(token.col, token.len);
        
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

        const UT::ui8 margin = 5; // Margin between suggestions

        // Implement drawing logic using CBLT's rendering system
        // This is a placeholder and should be replaced with actual drawing code
        for (UT::llui32 i = 0 ; i < suggestions.size() ; ++i) {
            // Calculate position for each suggestion (this is just an example)
            float x = cursorX; // Example x position
            float y = (i * (gFont.size + margin)) + cursorY - current * gFont.size; // Stack suggestions vertically

            // Draw each suggestion at the appropriate position
            DrawTextEx(
                gFont.f,
                suggestions[i].c_str(),
                {
                    x + 75.0f, // Minor horizontal fix
                    y + gFont.size
                },
                gFont.size,
                0.0f,
                UF::C(255, 255, 255)         
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
}