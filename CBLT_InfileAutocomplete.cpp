#include "CBLT_InfileAutocomplete.hpp"

namespace CBLT {
    InfileAutocomplete::InfileAutocomplete(void) {}

    InfileAutocomplete::~InfileAutocomplete(void) {}

    void InfileAutocomplete::LoadTokens(File& file) {
        tokens = file.GetUniqueTokens();
    }

    std::vector<std::string> InfileAutocomplete::GetSuggestions(const std::string& fragment) {
        suggestions.clear();
        for (const auto& token : tokens) {
            if (token.find(fragment) == 0) { // Check if token starts with fragment
                suggestions.push_back(token);
            }
        }
        return suggestions;
    }

    void InfileAutocomplete::DrawSuggestions(void) {
        const UT::ui8 margin = 5; // Margin between suggestions

        // Implement drawing logic using CBLT's rendering system
        // This is a placeholder and should be replaced with actual drawing code
        for (UT::llui32 i = 0; i < suggestions.size(); ++i) {
            // Draw each suggestion at the appropriate position
            DrawTextEx(
                gFont.f,
                suggestions[i].c_str(),
                {
                    x,
                    y + (i * gFont.size) + margin
                },
                gFont.size,
                0.0f,
                UF::C(255, 255, 255);         
            );
        }
    }
}