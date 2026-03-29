#include "CBLT_Util.hpp"

namespace CBLT {
    namespace Utils {
        std::ofstream gLogFile;

        namespace Err {
            // Initialize the log file
            void Init(void) {
                gLogFile = std::ofstream("log/dir.log", std::ios::app); // Open the log file in append mode
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

            Color C(Types::ui8 r, Types::ui8 g, Types::ui8 b) {
                return Color {r, g, b, 255};
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

                return result + ".marks";
            }

            // FIXME
            std::string MarkFileEncode(const std::string& path) {
                int iter = 0;
                
                for (char& s : path.c_str()) {
                    s += iter++ % 255; // Simple obfuscation by adding an incrementing value to each character
                }

                return path;
            }

            // FIXME
            std::string MarkFileDecode(const std::string& path) {
                int iter = 0;
                std::string result = path;

                for (char& s : result.c_str()) {
                    s -= iter++ % 255; // Reverse the obfuscation by subtracting the same incrementing value
                }

                return result;
            }
        } // Func
    } // Utils
} // CBLT