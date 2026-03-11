#include "CBLT_InfileAutocomplete.hpp"

// TODO: Reload tokens, try to make it cachable by line like the acutal infile tokens
// TODO: Add autocomplete function!
// TODO: Fix some issues with the cursor fragment

namespace CBLT {
    InfileAutocomplete::InfileAutocomplete(void) {
        tokens.clear();
        suggestions.clear();
    }

    InfileAutocomplete::~InfileAutocomplete(void) {}

    void InfileAutocomplete::LoadTokens(std::vector<std::vector<Token>>& fileTokens, std::vector<std::string>& fileText) {
        std::unordered_set<std::string> uniqueTokens;

        for (auto& line : fileTokens) {
            for (auto& token : line) {
                if (token.type == TokenClass::ID) {
                    if (token.line >= fileText.size()) continue;
                
                    const std::string& src = fileText[token.line];
                
                    if (token.col >= src.size()) continue;
                    if (token.col + token.len > src.size()) continue;
                
                    std::string tokenExtract = src.substr(token.col, token.len);
                
                    uniqueTokens.insert(tokenExtract);
                }
            }
        }

        // So this unique token get, has to be done once the file and extension is recognized
        // This is because the file extension keywords need to be included in the unique token
        // list for them to show up in the autocomplete suggestions, and the file extension is
        // required for language support and tokenization
        if (!gKeywords.empty()) {
            uniqueTokens.insert(gKeywords.begin(), gKeywords.end());
        }

        // Keep in mind this is done once
        // we need to replace disturbed tokens once per insertion

        tokens = std::vector<std::string>(uniqueTokens.begin(), uniqueTokens.end());
    }

    void InfileAutocomplete::GetSuggestions(const std::string& fragment) {
        if (fragment.empty()) {
            suggestions.clear();
            return;
        }

        suggestions.clear();
        for (const auto& token : tokens) {
            if (token.find(fragment) == 0) { // Check if token starts with fragment
                suggestions.push_back(token);
            }
        }
    }

    void InfileAutocomplete::DrawSuggestions(UT::ui32 cursorX, UT::ui32 cursorY) {
        if (suggestions.empty()) return;

        const UT::ui8 margin = 5; // Margin between suggestions

        // Implement drawing logic using CBLT's rendering system
        // This is a placeholder and should be replaced with actual drawing code
        for (UT::llui32 i = 0; i < suggestions.size(); ++i) {
            // Calculate position for each suggestion (this is just an example)
            float x = cursorX; // Example x position
            float y = (i * (gFont.size + margin)) + cursorY; // Stack suggestions vertically

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
}