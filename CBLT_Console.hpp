#pragma once

#include "CBLT_Dialog.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>          // Directive history double linked list

#include "CBLT_Directive.hpp"
#include "CBLT_Interpolator.hpp"
#include "CBLT_CWDContentToken.hpp"
#include "CBLT_Util.hpp"
#include "CBLT_FileQueue.hpp"
#include "CBLT_ShellBridge.hpp"

namespace CBLT {
    // Different type of messages the console can display
    enum class ConsoleMessage {
        NONE = -1,                            // No message to display
        INFO,                                 // The console will display info
        DIRECTIVE_ERROR,                      // The console will display an error
        GUIDE,                                // The console will display a guide
        SHELL_BRIDGE,                         // The console will display shell-bridge output
        FILE_MARKS,                           // The console will display file markings info 
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

    namespace ConsoleParameters {
        const UT::ui8 MAX_HISTORY_ENTRIES = 100; // Max number of entries in the directive history
    }

    // Console class for executing directives
    class Console {
        private:
            Directive directive;                        // Directive to execute
            DirectiveResult dirRes;                     // Directive result
            UT::b toggled;                              // Console is on or off
            UT::f32 width;                              // Console mutable width
            std::vector<CWDContentToken> cwdContents;   // Current working directory contents
            CursorManager cursor;                       // Cursor position inside the directive, only a primary
            Camera camera;                              // CWD content camera
            Offset cameraOffset;                        // CWD content camera offset for smooth scrolling
            std::list<std::string> directiveHistory;    // History of executed directives, up/down arrow navigation
            std::list<std::string>::iterator historyIt; // History list Iterator
        public:
            // Constructor
            Console();
            
            // Destructor
            ~Console();

            // Get cwd contents from the cwd
            void GetCWDContents(std::string cwd);

            // Get the CWDContents field
            std::vector<CWDContentToken>& CWDEntries(void);

            // Toggle the console on or off
            void Toggle(void);

            // Console general updates
            void Update(void);

            // Check if the console is on or off
            UT::b IsOpen(void) const ;

            // Execute the current directive
            void Execute(FileQueue& Q, std::string& cwd);

            // Draw console window, and CWD contents
            void Draw(FileQueue& Q, Rectangle rect);

            // Draw message returned by the console
            void DrawMessage(void);

            // Draw a simple message without any background/outline, used for the guide message when there is no open file
            void DrawGuide(void);

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

            // Return the best matching CWD strings
            std::string Autocomplete(void) const ;

            // Scroll the CWD content camera by a specific offset, only in the y axis
            void Scroll(UT::i32 dy);

            // Get the console camera object
            Camera& Cam(void);

            // Get camera offsets
            Offset CameraOffset(void) const ;

            // Get console history, read-only, immutable
            std::list<std::string>& History(void);

            // Get history iterator, works like an index basically
            std::list<std::string>::iterator& HistoryIt(void);
    }; // Console class
} // CBLT

