#include "CBLT_Directive.hpp"

namespace CBLT {
    std::string gGuideMessage = 
        "Co.Ba.L.T Console Help Guide:\n"
                    ":e   - Exit Co.Ba.L.T                 | :mat  - Mark a line\n"
                    ":we  - Write and exit Co.Ba.L.T       | :umat - Unmark a line\n"
                    ":w   - Write current file             | :mcl  - Mark current line\n"
                    ":wq  - Write and close current file   | :umcl - Unmark current line\n"
                    ":wqa - Write and close all files      | :gm   - Go to mark\n"
                    ":i   - Display file info and metadata | :gml  - Go to the last mark\n"
                    ":h   - Display this help guide        | :uml  - Unmark last\n"
                    ":c   - Create a file                  | :uma  - Unmark all\n"
                    ":r   - Delete a file                  | :im   - Display marks\n"
                    ":m   - Create a directory             |               \n"
                    ":d   - Delete a directory             |               \n"
                    ":cd  - Change directory               |               \n"
                    ":up  - Change directory to parent     |               \n"
                    ":o   - Open native folder picker      |               \n"
                    ":g   - Go to line in file             |               \n"
                    ":gs  - Go to start of file            |               \n"
                    ":ge  - Go to end of file              |               \n"
                    ":q   - Close current file             |               \n"
                    ":qa  - Close all files                |               \n"
                    ":qas - Close all clean files          |               \n"
                    ":pal - Switch palette                 |               \n"
                    ":sb  - Execute a shell command        |               \n";

    Directive::Directive(void) {}

    Directive::~Directive(void) {}

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
