#pragma once

#include "CBLT_raylib.hpp"

#include "CBLT_Util.hpp" // Types

namespace CBLT {

    // Keyboard class, keycode reading
    class Keyboard {
        typedef struct {
            UT::b ctrl;
            UT::b shift;
            UT::b alt;
        } Modifiers; // Keyboard modifier struct

        public:
            CBLT::Keyboard::Modifiers m; // Active modifiers for shortcuts

            // Constructor
            Keyboard(void);
    
            // Destructor
            ~Keyboard(void);

            // Keycode getter
            UT::i32 GetKey(void);

            // Update a modifier struct each frame if its key was pressed
            void UpdateModifiers(void);

            // Is ctrl modifier active
            UT::b CtrlActive(void) const;

            // Is alt modifier active
            UT::b AltActive(void) const;

            // Is shift modifier active
            UT::b ShiftActive(void) const;

            // Tab size, parsed from settings
            UT::ui32* tabSize = nullptr;

            // Assign a tab size, constructs the tab string again
            void AssignTabSize(UT::ui32* size);

            // Tab string
            std::string tab;
    }; // Keyboard class
} // CBLT