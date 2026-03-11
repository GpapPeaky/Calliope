#pragma once

#include "CBLT_Token.hpp"
#include "CBLT_Language.hpp"
#include "CBLT_Util.hpp"

#include <unordered_set>

namespace CBLT {
    class InfileAutocomplete {
        private:
            std::vector<std::string> tokens;        // File's tokens
            std::vector<std::string> suggestions;   // Current suggestions
        public:
            // Constructor
            InfileAutocomplete(void);

            // Destructor
            ~InfileAutocomplete(void);

            // Load tokens from a file
            void LoadTokens(std::vector<std::vector<Token>>& fileTokens, std::vector<std::string>& fileText);

            // Get suggestions based on current input, cursor fragment
            void GetSuggestions(const std::string& fragment);

            // Draw suggestions to the screen
            void DrawSuggestions(void);
    }; // Autocomplete class
} // CBLT