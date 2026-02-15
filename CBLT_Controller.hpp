#pragma once

#include "CBLT_Cursor.hpp"          // Cursor manager
#include "CBLT_Keyboard.hpp"        // Keyboard for input
#include "CBLT_File.hpp"            // Document/File type
#include "CBLT_Console.hpp"         // Console for directives
#include "CBLT_Camera.hpp"          // Camera for view offset
#include "CBLT_FileQueue.hpp"
#include "CBLT_Sound.hpp"

#include <cmath>                    // min()/max() ...
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

namespace CBLT {
    // Basic controller and input handling
    class Controller {
        private:
            CBLT::CursorManager cursorManager;            // Cursor manager
            CBLT::Keyboard keyboard;                      // Keyboard
            CBLT::FileQueue Q;                            // Current open document
            CBLT::Console console;                        // Console
            CBLT::Camera camera;                          // Camera
            std::string cwd;                              // Current working directory

            // Handle basic cursor movement
            void HandleMovement(Cursor& cursor, File* fileOverride = nullptr);

            // Handle special cursor movement, return true if special movement was handled
            UT::b HandleSpecialMovement(Cursor& cursor);

            // Handle indentation
            UT::b HandleIndentation(Cursor& cursor);

            // Handle presses like TAB, RETURN and BACKSPACE
            void HandleSpecials(Cursor& cursor);

            // Handle input at insert mode
            UT::b HandleInsert(Cursor& cursor, std::vector<char>& keyQueue);

            // Handle input at console mode
            UT::b HandleConsole(void);

            // Handle shorcuts, based on the active keyboard modifiers and cursor mode, return true if a shortcut was handled
            UT::b HandleShorcuts(Cursor& cursor);

        public:
            // Controller update, most functions are called here
            void Update(void);

            // Get the current open file, read only
            const File& GetFile(void) const;

            // Get the current open file, mutable access
            File& GetFile(void);

            // Get the console, read only
            const Console& GetConsole(void) const;

            // Get the console, mutable access
            Console& GetConsole(void);

            // Get the cursor manager, read only
            const CBLT::CursorManager& GetCursorManager(void) const;

            // Get the cursor manager, mutable access
            CBLT::CursorManager& GetCursorManager(void);

            // Check line and file lengths to limit cursor movement
            void ClampCursor(Cursor& cursor);

            // Get pressed key queue so as to not consume instantly anything
            std::vector<char> GetKeyQueue(void);

            // Get identation depth, check for unmatched '{' before the cursor
            UT::ui32 GetIndentation(UT::ui32 line);

            // Get the camera, read only
            const Camera& GetCamera(void) const;

            // Get the camera, mutable access
            Camera& GetCamera(void);

            // Initialise CWD
            void InitCWD(const std::string& p);

            // Get CWD
            const std::string& CWD(void) const;

            // Get the file queue
            FileQueue& LoadedFileQueue(void);

            // Move the cursor in file
            void HandleMouseWheel(void);

            // Place cursor at mose position
            void HandleMouseClick(void);

            // Handle font resizing
            void ResizeHandling(void);

            // Constructor
            Controller(void);

            // Destructor
            ~Controller(void);
            
    }; // Controller class
} // CBLT
