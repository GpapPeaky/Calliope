#include "CBLT_CWDContentToken.hpp"

namespace CBLT {
    CWDContentToken::CWDContentToken(Color clr, std::string name, UT::b dir) {
        c = clr;
        n = name;
        isDir = dir;
    }

    CWDContentToken::~CWDContentToken(void) {}
} // CBLT