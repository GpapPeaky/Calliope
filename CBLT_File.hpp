#pragma once

#include <vector>               // for std::vector<> ...
#include <string>               // for std::string ...
#include <fstream>              // File stream
#include <filesystem>           // For CWD

#include "CBLT_raylib.hpp"             // for rendering

#include "CBLT_Util.hpp"            // for types
#include "CBLT_Font.hpp"            // for Font global
#include "CBLT_TopBar.hpp"          // for vertical margins
#include "CBLT_Camera.hpp"          // for camera offsets
#include "CBLT_FileExtension.hpp"   // For extensions
#include "CBLT_Palette.hpp"

namespace CBLT {
    // Basic document/file class
    class File {
        private:
            std::vector<std::string> lines;  // Most elemental storage class of a file/document
            std::string path;                // File path, will include its name and parent folder
            // fixme: Maybe move CWD to controller, makes more sense
            UT::b dirty;                     // File's original contents have been changed and have not been saved
            FileExtension ext;               // File's extension, required for language support and tokenization
        public:
            // Constructor
            File(void);

            // Load file from name
            UT::b Load(const std::string& fpath);
            
            // Save file
            UT::b Save(void);

            // Clear file?
            UT::b Clear(void);

            // Insert character at a specific line and column
            void InsertChar(UT::ui32 col, UT::ui32 line, UT::i32 c);

            // Get current file path
            const std::string& GetPath(void) const;

            // Draw the document
            void Draw(Camera& cam);

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
            const UT::b Dirt(void) const;

            // Set the dirt to true or false
            void SetDirt(UT::b stat);

            // Return the name of the file
            const std::string& Name(void) const;

            // Return a string of info relative to the file
            const std::string Info(void) const;

            // Return the extesion of the file
            const FileExtension Extension(void) const;

        }; // File class
        
    // Positioning margins
    namespace FileMargins {
        namespace Text {
            // Left margin from the window y position
            const UT::f32 LEFT_FROM_FILE_LINES_UI = 5.0f;
        } // Text

        namespace Lines {
            // Left marging from the window y position
            const UT::f32 LEFT_FROM_WINDOW_Y = 5.0f;
        } // Lines

        namespace UI {
            // Left margin from the file lines
            const UT::f32 LEFT_FROM_FILE_LINES = 50.0f;
        } // UI
    } // Margins
} // CBLT
