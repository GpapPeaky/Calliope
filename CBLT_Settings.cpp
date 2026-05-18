#include "CBLT_Settings.hpp"

namespace CBLT {
    static std::string Trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        size_t end   = str.find_last_not_of(" \t\r\n");

        return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    void EditorSettings::ReadSettings(void) {
        namespace fs = std::filesystem;
    
        std::string installationPath;

        #if defined(__linux__)
            const char* resource_path = getenv("CBLT_RESOURCES");
            installationPath = resource_path ? std::string(resource_path) : ".";
        #endif

        std::string fname = "settings.conf";

        fs::path settingsPath = fs::path(installationPath) / "options" / fname;

        std::cerr << ("READING SETTINGS FROM " + settingsPath.string() + " \n");
    
        if (!fs::exists(settingsPath)) {
            // Optional log
            std::cerr << ("NO SETTINGS FILE FOUND " + fname + "\n");
            return;
        }
    
        std::ifstream in(settingsPath);
    
        if (!in.is_open()) {
            std::cerr << ("FAILED TO OPEN SETTINGS FILE: " + settingsPath.string() + "\n");
            return;
        }
    
        std::string line;
        bool inBlock = false;
    
        while (std::getline(in, line)) {
            line = Trim(line);
    
            if (line.empty()) continue;

            if (line.at(0) == '!') continue;
            
            // Block seperator
            if (line == "%settings") {
                if (!inBlock) {
                    inBlock = true;
                } else {
                    break;
                }
                
                continue;
            }

            // Delimeter
            size_t delimPos = line.find(':');

            // Token
            std::string token = "";

            std::string value = "";

            // Valid string
            if (delimPos != std::string().npos) {
                token = Trim(line.substr(0, delimPos));
                value = Trim(line.substr(delimPos + 1, line.length()));

                if (token == "USER_TERMINAL_POSIX") {
                    this->OPTION_POSIX_Term = value;

                    std::cerr << ("READ SETTING " + token + " AS " + value + "\n");
                }
                else if (token == "USER_TERMINAL_WIN32") {
                    this->OPTION_WIN32_Term = value;

                    std::cerr << ("READ SETTING " + token + " AS " + value + "\n");
                }
                else if (token == "PALETTE") {
                    this->OPTION_Palette = value;

                    std::cerr << ("READ SETTING " + token + " AS " + value + "\n");
                }
                else if (token == "CURSOR_SYMBOL") {
                    // this->OPTION_CursorSymbol = value;

                    std::cerr << ("READ SETTING " + token + " AS " + value + "\n");
                    
                    if (value == "UNDERSCORE")  { this->OPTION_CursorSymbol = -4; }
                    else if (value == "BOX")    { this->OPTION_CursorSymbol = -1; }
                    else if (value == "LINE")   { this->OPTION_CursorSymbol = -3; }
                    else if (value == "HOLLOW") { this->OPTION_CursorSymbol = -2; }
                    else {
                        std::cerr << ("INVALID CURSOR SYMBOL TOKEN AT LINE: " + line + " SELECT ONE OF THESE: \'BOX\', \'LINE\', \'HOLLOW\', \'UNDERSCORE\'\n");
                    }
                }
                else if (token == "ANIM_FILE_CURSOR_PROFILE") {
                    std::cerr << ("READ SETTING " + token + " AS " + value + "\n");
                    
                    this->OPTION_ANIM_FileCursor = ReadAnimationFile(value);
                }
                else if (token == "ANIM_CONSOLE_CURSOR_PROFILE") {
                    std::cerr << ("READ SETTING " + token + " AS " + value + "\n");
                    
                    this->OPTION_ANIM_ConsoleCursor = ReadAnimationFile(value);
                }
                else if (token == "ANIM_FQ_PROFILE") {
                    std::cerr << ("READ SETTING " + token + " AS " + value + "\n");
                    
                    this->OPTION_ANIM_FQ = ReadAnimationFile(value);
                }
                else if (token == "ANIM_CONSOLE_PROFILE") {
                    std::cerr << ("READ SETTING " + token + " AS " + value + "\n");
                    
                    this->OPTION_ANIM_Console = ReadAnimationFile(value);
                }
                else {
                    std::cerr <<  ("INVALID SETTINGS TOKEN AT LINE: " + line + "\n");
                } 
            } else {
                std::cerr << ("INVALID SETTINGS SYNTAX AT LINE: " + line + "\n");
            }
            
            if (!inBlock) continue;
        }
    }

    EditorSettings gSettings;
}