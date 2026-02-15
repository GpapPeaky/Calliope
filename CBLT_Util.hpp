#pragma once

#include <string>            // for std::string
#include <iostream>          // for console output
#include <cstdlib>           // for system(...)

#include "CBLT_raylib.hpp"

namespace CBLT {
    namespace Utils {
        // Cross plafrom function attrib
        #define __CROSS
        namespace Types {
            typedef unsigned int ui32; // 32 bit unsigned
            typedef long unsigned int lui32; // long unsigned
            typedef long long unsigned int llui32; // long long unsigned
            typedef const ui32 cui32; // Constant 32 bit unsigned
            typedef int i32; // 32 bit signed
            typedef const i32 ci32; // Constant 32 bit signed
    
            typedef unsigned char ui8; // 8 bit unsigned
            typedef const ui8 cui8; // Constant 8 bit unsigned
            typedef unsigned char i8; // 8 bit signed
            typedef const i8 ci8; // Constant 8 bit signed

            typedef char32_t c32; // 32 bit charater

            typedef bool b; // Boolean
    
            typedef float f32; // Floating point
        } // Types

        namespace Err {
            // Error logging function
            void Log(std::string msg);
        } // Err

        namespace Def {
            const CBLT::Utils::Types::ui32 GRACEFUL_EXIT = 0;

            // Max 4 files can be open inside the queue, if a new one is inserted then it becomes the tail
            const CBLT::Utils::Types::ui8 MAX_FILE_Q_NODES = 16;
        } // Def

        namespace Func {
            // Raylib colour constructor thing
            Color C(Types::ui8 r, Types::ui8 g, Types::ui8 b);
            
            // Trim the semi colon from a string, used in the console to execute ':' commands
            std::string TrimLeadingColon(const std::string& str);
        } // Func
    } // Utils

    // Positioning margins
    namespace FileMargins {
        namespace Text {
            // Left margin from the window y position
            const CBLT::Utils::Types::f32 LEFT_FROM_FILE_LINES_UI = 5.0f;
        } // Text

        namespace Lines {
            // Left marging from the window y position
            const CBLT::Utils::Types::f32 LEFT_FROM_WINDOW_Y = 5.0f;
        } // Lines

        namespace UI {
            // Left margin from the file lines
            const CBLT::Utils::Types::f32 LEFT_FROM_FILE_LINES = 50.0f;

            // Top margin from the top bar
            const CBLT::Utils::Types::f32 TOP_BAR_HEIGHT = 50.0f;
    
            // Top bar seperator Y from height
            const CBLT::Utils::Types::f32 TOP_BAR_SEPERATOR_Y = 17.0f;
        } // UI
    } // Margins
} // CBLT

// Utlity types definition, easier access
#define UT CBLT::Utils::Types

// Utility error logging, easier access
#define UE CBLT::Utils::Err

// Utility general use definitions, easier access
#define UDef CBLT::Utils::Def

// Utility functions, easier access
#define UF CBLT::Utils::Func

// Utility access
#define U CBLT::Utils