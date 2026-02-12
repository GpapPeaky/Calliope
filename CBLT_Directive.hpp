#pragma once

#include "CBLT_Util.hpp"
#include "CBLT_Cursor.hpp"
#include "CBLT_File.hpp"
#include "CBLT_Palette.hpp"

#include <string>

namespace CBLT {
    // Directives to execute in the console

    const UT::ui8 DIRECTIVE_FILE_LINE = 0; // Line index of directive file

    class Directive {
        private:
            File f;                // Directive to execute, one line per directive
        public:
            // Constructor
            Directive();

            // Destructor
            ~Directive();

            // Clear the current directive for after execution
            void Clear(void);

            // Draw the directive when console is open
            void Draw(UT::ui32 x, UT::ui32 y);

            // Get directive string, read only 
            const File& DirectiveFile(void) const;

            // Get directive string, mutable access
            File& DirectiveFile(void);

            // Set the directive to something specific
            void Becomes(std::string src);
    }; // Directive class

    // Console text margins
    namespace DirectiveMargins {
        const UT::ui32 directiveMarginFromConsoleX = 5; // X coord margin from console
        const UT::ui32 CWDContentMargin = 10; // X coord margin from console
        const UT::ui32 directiveMarginFromConsoleY = 5; // Y coord margin from console
    } // ConsoleTextMargins
    
    namespace UI {
        const UT::ui32 directiveFontSize = gConsoleFont.size; // Directive font size
    }

} // CBLT
