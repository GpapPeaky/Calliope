#pragma once

#include "CBLT_Token.hpp"
#include "CBLT_Language.hpp"
#include "CBLT_Util.hpp"
#include "CBLT_Palette.hpp"

#include <unordered_set>

namespace CBLT {
    class InfileAutocomplete {
        private:
            std::vector<std::unordered_set<std::string>> lineTokens;        // Tokens per line
            std::unordered_set<std::string> allTokens;                      // Set of all tokens
            std::vector<std::string> tokens;                                // Final token vector
            std::vector<std::string> suggestions;                           // Suggestions based on the cursor fragment
            UT::ui32 current;                                               // Current suggestion index for navigation
            UT::b open;                                                     // Suggestions are open  
            UT::b dismissed;                                                // Suggestions are dismissed
        public:
            // Constructor
            InfileAutocomplete(void);

            // Destructor
            ~InfileAutocomplete(void);

            // Load tokens from a file
            void LoadTokens(std::vector<std::vector<Token>>& fileTokens, std::vector<std::string>& fileText);

            // Update a specific line's tokens, notably the dirty ones
            void UpdateLine(UT::ui32 line, std::vector<Token>& tokensLine, const std::string& text);

            // Get suggestions based on current input, cursor fragment
            void GetSuggestions(const std::string& fragment);

            // Draw suggestions to the screen
            void DrawSuggestions(UT::ui32 cursorX, UT::ui32 cursorY);

            // Get the current suggestion string
            std::string GetCurrentSuggestion(void) const ;

            // Get the suggestions vector
            std::vector<std::string> GetCurrentSuggestions(void) const ;

            // Move the index up
            void Up(void);

            // Move the index down
            void Down(void);

            // Reset the current index
            void Reset(void);

            // Close suggestions
            void Close(void);

            // Open suggestions
            void Open(void);

            // Dismiss current suggestions 
            void Dismiss(void);

            // Reset the dismiss, valid character was inserted, we can open suggestions after that
            void Valid(void);

            // Is the suggestion part open
            UT::b IsOpen(void) const ;
    }; // Autocomplete class
} // CBLT