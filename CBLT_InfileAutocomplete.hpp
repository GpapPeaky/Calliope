#pragma once

#include "CBLT_File.hpp"

// TODO: Finish and test

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
            void LoadTokens(const std::string& filename);

            // Get suggestions based on current input, cursor fragment
            std::vector<std::string> GetSuggestions(const std::string& fragment);

            // Draw suggestions to the screen
            void DrawSuggestions(void);
    }
}