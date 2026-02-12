#include "CBLT_Directive.hpp"

namespace CBLT {
    Directive::Directive() {}

    Directive::~Directive() {}

    void Directive::Clear(void) {
        f.GetCurrentLine(DIRECTIVE_FILE_LINE).clear();
    }

    void Directive::Draw(UT::ui32 x, UT::ui32 y) {
        DrawTextEx(
            gConsoleFont.f,
            f.GetCurrentLine(DIRECTIVE_FILE_LINE).c_str(), // Only one line reserved for commands
            {
                (UT::f32)x + DirectiveMargins::directiveMarginFromConsoleX,
                (UT::f32)y + DirectiveMargins::directiveMarginFromConsoleY 
            }, 
            UI::directiveFontSize,                                         // Make it remain as is for now
            0.0f,
            gPalette.consoleText 
        );
    }

    const File& Directive::DirectiveFile(void) const {
        return f;
    }

    File& Directive::DirectiveFile(void) {
        return f;
    }

    void Directive::Becomes(std::string src) {
        f.GetCurrentLine(DIRECTIVE_FILE_LINE).clear();
        f.GetCurrentLine(DIRECTIVE_FILE_LINE).append(src);
    }
} // CBLT 
