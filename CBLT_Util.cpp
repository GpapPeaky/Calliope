#include "CBLT_Util.hpp"

namespace CBLT {
    namespace Utils {
        namespace Err {
            void Log(std::string msg) {
                std::cerr << "CBLT_ERR: " << msg << "\n";
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
        } // Func
    } // Utils
} // CBLT