#pragma once

#include "CBLT_FIleExtension.hpp"

#include <unordered_set>

namespace CBLT {
    // Loaded keywords, of a specified language
    extern std::unordered_set<std::string_view> gKeywords;

    // Assign the language keywords to the global set
    void AssignLanguageKeywords(FileExtension fe);

    // Check if a string is containted inside the loaded keyword set
    bool IsKeyword(std::string_view sv);
} // CBLT