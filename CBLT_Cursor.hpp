#pragma once

#include "CBLT_Util.hpp"        // Types
#include "CBLT_Font.hpp"        // Font size
#include "CBLT_Palette.hpp"
#include "CBLT_raylib.hpp"      // for MeasureText() ...
#include "CBLT_Camera.hpp"

#include <vector>        // for std::vector<> ...

namespace CBLT {
    // Cursor character representation
    enum class CursorSymbol : UT::i32 {
        NON_ASCII_UNDERSCORE  = -4,          // Non-ASCII underscore
        NON_ASCII_LINE        = -3,          // Non-ASCII line
        NON_ASCII_HOLLOW_BOX  = -2,          // Non-ASCII hollow box
        NON_ASCII_BOX         = -1,          // Non-ASCII box
    };

    // Cursor mode
    enum class CursorMode : UT::i32 {
        INSERT,                 // Insertion mode
        SELECT                  // Selection mode
    }; 

    // Cursor direction class
    enum class CursorDirection : UT::i32 {
        LEFT,                   // Move left
        RIGHT,                  // Move right
        UP,                     // Move up
        DOWN                    // Move down
    };

    // Characters are defined as one of 3 groups, inWord, space, symbol
    enum class CharClass : UT::i32 {
        INWORD,             // Character can be found inside a word <'a' 'b'>
        WHITESPACE,         // Character is a whitespace char <' '>
        SYMBOL              // Character is a symbol <',' '.' '/'>
    };

    // Basic cursor object for both file and console
    class Cursor {
        private:
            UT::ui32 column;                 // Current column the cursor is at
            UT::ui32 line;                   // Current line the cursor is at
            UT::ui32 startSelectColumn;      // Initial cursor column at select mode entry
            UT::ui32 finalSelectColumn;      // Final cursor column at select mode entry
            UT::ui32 startSelectLine;        // Initial cursor line at select mode entry
            UT::ui32 finalSelectLine;        // Final cursor line at select mode entry
            CursorMode m;                    // Current cursor mode
            std::string fragment;            // Text fragment from current cursor position
        
        public:
            UT::ui32 renderX;                // Render X coordinate
            UT::ui32 renderY;                // Render Y coordinate
            UT::ui32 charWidth;              // Monospaced font support ONLY!
            
            // Acquire the cursor fragment, based on cursor positon
            void AcquireFragment(UT::ui32 c, std::string& line);
            
            // Get the cursor fragment
            std::string Fragment(void) const ;
            
            // Constructor
            Cursor(UT::ui32 col, UT::ui32 line);
            
            // Destructor
            ~Cursor(void);

            CursorSymbol cursorSymbol;    // Cursor symbol to draw

            // Get the current column
            UT::ui32 Col(void) const;

            // Get the current line
            UT::ui32 Line(void) const;

            // Set cursor position at a specific column and line
            void SetAt(UT::ui32 col, UT::ui32 line);

            // Get the cursor mode
            CursorMode GetMode(void) const;

            // Set cursor at a specific mode
            void SetMode(CursorMode m);

            // Move cursor one line up, line -= 1
            void Up(void);

            // Move cursor one line down, line += 1
            void Down(void);

            // Move cursor one column left, column -= 1
            void Left(void);

            // Move cursor one column right, column += 1
            void Right(void);

            // Draw cursor
            void Draw(const std::string& lineText);

            // Start selection
            void StartSelection(void);

            // Stop selection
            void StopSelection(void);

            // Select final column
            UT::ui32 SFCol() const;

            // Select final line
            UT::ui32 SFLine() const;

            // Select start column
            UT::ui32 SSCol() const;

            // Select start line
            UT::ui32 SSLine() const;

            // Get cursor x in pixels 
            UT::ui32 GetCursorX(const std::string& lineText, UT::ui32 fontSize);

            // Classify character class
            CharClass Classify(UT::cui8 c) const;

            // Get the distance to a character in the left or right of the cursor and set the cursor there
            void SetToWordBoundary(const std::string& lineText, const CursorDirection dir, UT::ui32 lineCount);

            // Clamp cursor to camera view
            void ClampToCamera(Camera& cam, std::string currentLine);
    }; // Cursor class

    class CursorManager {
        private:
            UT::b requestReset;             // Request cursors reset
            UT::b requestTrail;             // Request cursor up
            UT::b requestLead;              // Request cursor down

        public:
            std::vector<CBLT::Cursor> activeCursors; // Multiline cursor management
            
            // Constructor
            CursorManager(void);

            // Destructor
            ~CursorManager(void);

            // Add a cursor at a specific column and line
            void AddCursorAt(UT::ui32 col, UT::ui32 line);

            // Remove a cursor at a specific column and line
            void RemoveCursorAt(UT::ui32 col, UT::ui32 line);

            // Non primary cursors are removed
            void RemoveSecondaries(void);

            // Draw all active cursors
            void DrawCursors(std::vector<std::string>& lines);

            // Request cursor reset
            void RequestReset(void);

            // Request a trailling cursor
            void RequestTrail(void);
            
            // Request a leading cursor
            void RequestLead(void);

            // Apply pending requests, and reset any flags/counters
            void HandlePendingRequests(UT::ui32 lineCount);

            // Get primary cursor mutable access
            Cursor& Primary();

            // Get primary cursor read only
            const Cursor Primary() const;
    }; // Cursor manager class
} // CBLT

