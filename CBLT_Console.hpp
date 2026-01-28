#pragma once

#include <string>
#include <algorithm>
#include <filesystem>
#include <bits/stdc++.h> // For transform to lowercase

#include "CBLT_Directive.hpp"
#include "CBLT_Interpolator.hpp"
#include "CBLT_CWDContentToken.hpp"
#include "CBLT_Util.hpp"

#include "CBLT_FileQueue.hpp"

namespace CBLT {
    // Different type of messages the console can display
    enum class ConsoleMessage {
        NONE = -1,                            // No message to display
        INFO,                                 // The console will display info
        DIRECTIVE_ERROR,                      // The console will display an error
        GUIDE                                 // The console will display a guide
    };

    // Results a directive can return
    typedef struct DirectiveResult {
        std::string message;            // Message to return
        ConsoleMessage messageType;     // The type of message
    } DirectiveResult;

    namespace ConsoleWidth {
        const UT::f32 WIDTH_MIN = 20.0f;            // Width min
        const UT::f32 WIDTH_MAX_RATIO = 0.5f;       // Max width ratio relative to screen
    } // Console

    // Console class for executing directives
    class Console {
        private:
            Directive directive;                        // Directive to execute
            DirectiveResult dirRes;                     // Directive result
            UT::b toggled;                              // Console is on or off
            UT::f32 width;                              // Console mutable width
            std::vector<CWDContentToken> cwdContents;   // Current working directory contents
            CursorManager cursor;                       // Cursor position inside the directive, only a primary
            Camera camera;                              // TODO: CWD content camera
        public:
            // Constructor
            Console();
            
            // Destructor
            ~Console();

            // Get cwd contents
            void GetCWDContents(std::string cwd);

            // Toggle the console on or off
            void Toggle(void);

            // Console general updates
            void Update(void);

            // Check if the console is on or off
            UT::b IsOpen(void) const ;

            // Execute the current directive
            void Execute(FileQueue& Q);

            // Draw console window, and CWD contents
            void Draw(std::string cwd);

            // Draw message returned by the console
            void DrawMessage(void);

            // Console directive
            Directive& ConsoleDirective(void);

            // Resize console window
            void Move(UT::f32 offset);

            // Get directive result
            DirectiveResult& Message(void);

            // Get the console cursor
            Cursor& ConsoleCursor(void);

            // Console width
            UT::f32 Width(void) const ;
    }; // Console class
} // CBLT

