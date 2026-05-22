#pragma once

#include "CBLT_FIleExtension.hpp"

#include "CBLT_Sys.hpp"

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace CBLT {
    enum class LanguageLoaderSection {
        NONE,
        KEYWORDS,
        CONTROL_FLOW,
        STORAGE_CLASS,
        TYPE_QUALIFIERS,
        USER_DEFINED,
        UTILITY,
        DATA_TYPES,
        OPERATORS,
        COMMENT_BLOCK,
        COMMENT_LINE,
        STRING_DELIM,
        ANNOTATIONS,
        PUNCTUATION,
        MACROS,
        SETTINGS
    };

    enum class LanguageClass {
        CLIKE,                      // C-like languages, higher priority
        SCRIPT,                     // Scripting languages like Javascript
        MARKUP,                     // Languages like markdown
        DATA                        // Hold data
    }; // Language classification

    typedef struct LanguageSettings {
        UT::b multilineStrings = false;          // Can declare multiline strings
        UT::b hexNumerals = false;               // Can declare hex numerals
        UT::b binNumerals = false;               // Can declare bin numerals
        UT::b escapeSequences = true;            // Has escape characters
    } LanguageSettings; // Language settings struct

    typedef struct LanguageKeywords {
        std::unordered_set<std::string> controlFlow;           // if, else
        std::unordered_set<std::string> storageClass;          // static, extern
        std::unordered_set<std::string> typeQualifiers;        // const, volatitle
        std::unordered_set<std::string> userDefinedClass;      // struct, union, enum
        std::unordered_set<std::string> utility;               // sizeof, return
        std::unordered_set<std::string> dataTypes;             // char, int
    } LanguageKeywords; // Language keywords

    class Language {
        public:
            LanguageKeywords                                            keywords;                     // Language keywords
            std::unordered_set<std::string>                             operators;                    // Language operators
            std::vector<std::pair<std::string, std::string>>            commentBlock;                 // Comment block creation
            std::vector<std::string>                                    commentLine;                  // Comment line creation 
            std::vector<std::pair<std::string, std::string>>            stringDelim;                  // String block creation
            std::vector<std::string>                                    annotations;                  // annotations highlighting entry
            std::vector<std::string>                                    macros;                       // Macro highlighting entry 
            std::unordered_set<std::string>                             punctuations;                 // Punctuations
            LanguageSettings                                            settings;                     // Language behavioural settings

            // Constructor
            Language(FileExtension fe);

            // Destructor
            ~Language(void);

            // Read language conf file
            void ReadLangFile(std::string fname);

            // Check if a string is containted inside the loaded keyword set
            UT::b IsKeyword(const std::string s);

            // Check if a string is contained inside the loaded operator set
            UT::b IsOp(const std::string s);
    }; // Language class

    // File extension to .cbltconf name map
    extern std::unordered_map<FileExtension, std::string> gLangFiles;
} // CBLT