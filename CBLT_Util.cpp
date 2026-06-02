#include "CBLT_Util.hpp"

namespace CBLT {
    namespace Utils {
        std::ofstream gLogFile;

        namespace Err {
            // Initialize the log file
            void Init(void) {
                namespace fs = std::filesystem;
                        
                fs::path logDir = Sys::ResourcePath();
                logDir /= "log";
                        
                fs::create_directories(logDir);
                        
                fs::path logFile = logDir / "dir.log";
                        
                gLogFile = std::ofstream(logFile, std::ios::app);
            }

            void Log(std::string msg) {
                gLogFile << "CBLT_LOG: " << msg << "\n";
            }
        } // Err

        namespace Func {
            std::string TrimLeadingColon(const std::string& str) {
                if (!str.empty() && str[0] == ':') return str.substr(1);
                return str;
            }

            Types::cblt_color C(Types::ui8 r, Types::ui8 g, Types::ui8 b) {
                return Types::cblt_color {r, g, b, 255};
            }

            std::string Trim(const std::string& str) {
                size_t start = str.find_first_not_of(" \t\r\n");
                size_t end   = str.find_last_not_of(" \t\r\n");

                return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
            }

            std::string PathToMarkFileName(const std::string& path) {
                std::string result = path;
                for (char& c : result) {
                    if (c == '/' || c == '\\' || c == ':') {
                        c = '_';
                    }
                }

                return result;
            }
            
            std::string MarkFileEncode(const std::string& path) {
                std::ostringstream oss;
            
                for (unsigned char c : path) {
                    oss << std::hex
                        << std::setw(2)
                        << std::setfill('0')
                        << static_cast<int>(c);
                }
            
                return oss.str();
            }
        } // Func
    } // Utils

    CBLT::Utils::Types::f32 CBLT::FileMargins::UI::TOP_BAR_HEIGHT = 0.0F;
} // CBLT