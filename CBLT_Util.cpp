#include "CBLT_Util.hpp"

namespace CBLT {
    namespace Utils {
        namespace Err {
            void Log(std::string msg) {
                std::cerr << "CBLT_ERR: " << msg << "\n";
            }
        } // Err

        std::string Func::TrimLeadingColon(std::string& str) {
            if (!str.empty() && str[0] == ':') return str.substr(1);
            return str;
        }

        Color Func::C(Types::ui8 r, Types::ui8 g, Types::ui8 b) {
            return Color {r, g, b, 255};
        }

    } // Utils
} // CBLT