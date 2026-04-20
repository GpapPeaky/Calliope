#include "CBLT_Settings.hpp"

namespace CBLT {
    void Settings::ReadSettings(void) {
        namespace fs = std::filesystem;
    
        std::string installationPath;

        #if defined(__linux__)
            const char* resource_path = getenv("CBLT_RESOURCES");
            installationPath = resource_path ? std::string(resource_path) : ".";
        #endif

        std::string fname = "settings.conf";

        fs::path settingsPath = fs::path(installationPath) / "options" / fname;
    
        if (!fs::exists(settingsPath)) {
            // Optional log
            CBLT::Utils::Err::Log("NO SETTINGS FILE FOUND " + fname);
            return;
        }
    
        std::ifstream in(settingsPath);
    
        if (!in.is_open()) {
            CBLT::Utils::Err::Log("FAILED TO OPEN SETTINGS FILE: " + settingsPath.string());
            return;
        }
    
        std::string line;
        bool inBlock = false;
    
        while (std::getline(in, line)) {
            line = UF::Trim(line);
    
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
            UT::llui32 delimPos = line.find(':');

            // Token
            std::string token = "";

            std::string value = "";

            // Valid string
            if (delimPos != std::string().npos) {
                token = Utils::Func::Trim(line.substr(0, delimPos));
                value = Utils::Func::Trim(line.substr(delimPos + 1, line.length()));

                if (token == "USER_TERMINAL_POSIX") {
                    this->OPTION_POSIX_Term = value;

                    CBLT::Utils::Err::Log("READ SETTING " + token + " AS " + value);
                }
                else if (token == "USER_TERMINAL_WIN32") {
                    this->OPTION_WIN32_Term = value;

                    CBLT::Utils::Err::Log("READ SETTING " + token + " AS " + value);
                }
                else if (token == "PALETTE") {
                    this->OPTION_Palette = value;

                    CBLT::Utils::Err::Log("READ SETTING " + token + " AS " + value);
                }
                else {
                    CBLT::Utils::Err::Log ("INVALID SETTINGS TOKEN AT LINE: " + line);
                } 
            } else {
                CBLT::Utils::Err::Log("INVALID SETTINGS SYNTAX AT LINE: " + line);
            }
            
            if (!inBlock) continue;
        }
    }

    Settings gSettings;
}