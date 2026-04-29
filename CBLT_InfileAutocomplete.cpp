#include "CBLT_InfileAutocomplete.hpp"

namespace CBLT {
    InfileAutocomplete::InfileAutocomplete(void) {
        current = 0;
        open = false;
    }

    InfileAutocomplete::~InfileAutocomplete(void) {}

    void InfileAutocomplete::LoadTokens(std::vector<std::vector<Token>>& fileTokens, std::vector<std::string>& fileText, Language& lang) {
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
    
        if (!lang.keywords.controlFlow.empty())
            allTokens.insert(lang.keywords.controlFlow.begin(), lang.keywords.controlFlow.end());

        if (!lang.keywords.dataTypes.empty())
            allTokens.insert(lang.keywords.dataTypes.begin(), lang.keywords.dataTypes.end());
    
        if (!lang.keywords.storageClass.empty())
            allTokens.insert(lang.keywords.storageClass.begin(), lang.keywords.storageClass.end());

        if (!lang.keywords.typeQualifiers.empty())
            allTokens.insert(lang.keywords.typeQualifiers.begin(), lang.keywords.typeQualifiers.end());

        if (!lang.keywords.userDefinedClass.empty())
            allTokens.insert(lang.keywords.userDefinedClass.begin(), lang.keywords.userDefinedClass.end());

        if (!lang.keywords.utility.empty())
            allTokens.insert(lang.keywords.utility.begin(), lang.keywords.utility.end());

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
    
        // How many suggestions to show at once
        const UT::ui32 maxVisible = 6;
        const UT::ui32 visible = std::min((UT::ui32)suggestions.size(), maxVisible);
    
        const UT::f32 flipTriggerMargin = 40.0f; // small margin to trigger the flip sooner
        // Flip above cursor if box would clip screen bottom
        const UT::f32 screenBottom = (UT::f32)GetScreenHeight() - flipTriggerMargin;

        // Scroll window — current stays centered if possible
        UT::ui32 windowStart = 0;
        if (current >= visible) {
            windowStart = current - visible + 1;
        }
    
        const UT::f32 boxX = cursorX + 90.0f;
        UT::f32 boxY = cursorY + gFont.size;
        const UT::f32 boxW = 200.0f;
        const UT::f32 boxH = lineHeight * visible;
    
        if (boxY + boxH > screenBottom) {
            boxY = cursorY - boxH; // Flip
        }

        // Background outline
        DrawRectangleLines(
            boxX - 1.0f,
            boxY - 1.0f,
            boxW + 2.0f,
            boxH + 2.0f,
            { gPalette.console.r, gPalette.console.g, gPalette.console.b, 255 }
        );
    
        // Foreground fill
        DrawRectangle(
            boxX,
            boxY,
            boxW,
            boxH,
            { gPalette.consoleBackground.r, gPalette.consoleBackground.g, gPalette.consoleBackground.b, 255 }
        );
    
        // Draw only the visible window
        for (UT::ui32 i = 0; i < visible; i++) {
            UT::ui32 suggIndex = windowStart + i;
            if (suggIndex >= suggestions.size()) break;
    
            const UT::f32 x = boxX + margin;
            const UT::f32 y = boxY + i * lineHeight;
    
            // Highlight the currently selected entry
            if (suggIndex == current) {
                DrawRectangle(
                    boxX,
                    y,
                    boxW,
                    gFont.size,
                    { gPalette.console.r, gPalette.console.g, gPalette.console.b, 80 }
                );
            }
    
            DrawTextEx(
                gFont.f,
                suggestions[suggIndex].c_str(),
                { x, y },
                gFont.size,
                0.0f,
                { gPalette.consoleText.r, gPalette.consoleText.g, gPalette.consoleText.b, 255 }
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

    void InfileAutocomplete::Valid(void) {
        dismissed = false;
    }

    void InfileAutocomplete::Dismiss(void) {
        dismissed = true;
        open = false;
    }

    UT::b InfileAutocomplete::IsOpen(void) const {
        return open && !dismissed;
    }
} // CBLT