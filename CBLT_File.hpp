#pragma once

#include "CBLT_raylib.hpp"              // for rendering
#include "CBLT_Util.hpp"                // for types
#include "CBLT_Font.hpp"                // for Font global
#include "CBLT_Camera.hpp"              // for camera offsets
#include "CBLT_FIleExtension.hpp"       // For extensions
#include "CBLT_Palette.hpp"             // Pallete colours
#include "CBLT_Token.hpp"               // Tokens storage class
#include "CBLT_Language.hpp"            // Tokens for tokenizing
#include "CBLT_Camera.hpp"
#include "CBLT_InfileAutocomplete.hpp"
#include "CBLT_InfileMark.hpp"
#include "CBLT_Lex.hpp"                 // Einai aseveia rr

#include <cctype>                       // for tokenizing
#include <unordered_set>                // for std::unordered_set<> ...
#include <vector>                       // for std::vector<> ...
#include <string>                       // for std::string ...
#include <fstream>                      // File stream
#include <filesystem>                   // For CWD

namespace CBLT {
    // Basic document/file class
    class File {
        private:
            std::vector<std::string> lines;                     // Most elemental storage class of a file/document
            std::vector<std::vector<Token>> tokens;             // File tokens
            std::unordered_set<UT::ui32> dirtyLines;            // Lines to retokenize infile change
            std::vector<LexerState> lineStates;                 // Line states for lexer
            std::string path;                                   // File path, will include its name and parent folder
            std::string name;                                   // Only the name
            UT::b dirty;                                        // File's original contents have been changed and have not been saved
            InfileAutocomplete autocomplete;                    // Autocomplete system for the file
            std::vector<InfileMark> marks;                      // Infile markings
            UT::ui32 markIdFactory;                             // Mark id factory
            CursorManager cursors;                              // Cursor manager for the file, handles cursor position and movement
            Language lang = Language(FileExtension::TXT);       // File's language loaded via the extension
            std::string langConf;                               // Language conf file name
            Camera cam;                                         // Camera for rendering the file, will be used for panning and zooming the file view     
        public:
            // Constructor
            File(void);

            // Destructor
            ~File(void);

            // Tokenize file contents
            void Tokenize(void);

            // Load file from name
            UT::b Load(const std::string& fpath, const std::string& cwd);
          
            // Save file
            UT::b Save(void);

            // Clear file?
            UT::b Clear(void);

            // Insert character at a specific line and column
            void InsertChar(UT::ui32 col, UT::ui32 line, UT::i32 c);

            // Get current file path
            const std::string& GetPath(void) const;

            // Draw the document
            void Draw(UT::ui32 cursorX, UT::ui32 cursorY, UT::b consoleOpen, UT::ui32 consoleWidth);

            // Get line count
            UT::ui32 GetLineCount(void) const;

            // Get column count in line
            UT::ui32 GetLineLength(UT::ui32 line) const;

            // Create a line below the provided index
            void CreateLine(UT::ui32 line);

            // Create a line below the provided index, with specified content
            void CreateLine(UT::ui32 line, std::string content);

            // Splits a line into two strings from a specific column, returns the right fragment
            std::string SplitLine(UT::ui32 line, UT::ui32 col);

            // Delete a line at a specific index
            void DeleteLine(UT::ui32 line);

            // Moves the remaining line fragment of source line, to the end of the destination line when deleting at column zero
            void PushBackLineFragment(UT::ui32 sourceLine, UT::ui32 destinationLine);

            // Get line at line index, read only
            const std::string& GetCurrentLine(UT::ui32 line) const;

            // Get line at line index, mutable access
            std::string& GetCurrentLine(UT::ui32 line);

            // Get if the file is dirty or not
            UT::b Dirt(void) const;

            // Set the dirt to true or false
            void SetDirt(UT::b stat);

            // Return the name of the file
            const std::string& Name(void) const;

            // Return a string of info relative to the file
            const std::string Info(void) const;

            // Set the file name to something else
            void SetName(std::string name);

            // LANGUAGE AND HIGHLIGHTING

                // Return the language of the file
                Language& FileLanguage(void);
    
                // Set a line as dirty and push it back to the dirty line vector for retokenization
                void InsertDirtyLine(UT::ui32 line);
    
                // Retokenize lines marked as dirty
                void RetokenizeDirtyLines(void);

                // Language conf file
                std::string LangConf(void);

            // LANGUAGE AND HIGHLIGHTING

            // Get all the lines of the file
            std::vector<std::string>& GetLines(void);

            // Get the autocomplete handle
            InfileAutocomplete& Auto(void);

            // Return the token 2D vector of the file
            std::vector<std::vector<Token>>& Tokens(void);
    
            // Add a mark at a specific line
            UT::b AddMark(UT::ui32 l);
            
            // Remove a specific mark via position
            UT::b RemoveMark(UT::ui32 l);

            // Get the file marks
            std::vector<InfileMark>& Marks(void);

            // Mark id factory
            UT::ui32& MarkIdFactory(void);

            // Get the file's cursors
            CursorManager& Cursors(void);

            // Check line and file lengths to limit cursor movement
            void ClampCursor(Cursor& cursor);

            // Save the present marks in the disk, for when revisiting the file it is present, exist at a sidecar file
            void SaveMarks(void) const ;

            // Load marks from the sidecar file
            void LoadMarks(void);

            // Get the file camera
            Camera& Cam(void);

            // Return a string of the file markings and nearby lines
            std::string GetMarksAndNearbyLinesMessageString(void) const ;
    }; // File class
} // CBLT