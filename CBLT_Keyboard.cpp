#include "CBLT_Keyboard.hpp"

namespace CBLT {
        Keyboard::Keyboard(void):
            m { false, false, false } // Modifiers set to false
        {
            if (!tabSize) {
                tab = " ";

                return;
            }

            for (UT::llui32 i = 0 ; i < *tabSize ; i++) {
                tab.append(" ");
            } 
        }
    
        Keyboard::~Keyboard(void) {}

        UT::i32 Keyboard::GetKey(void) {
            return GetCharPressed(); // Via raylib
        }

        void Keyboard::UpdateModifiers(void) {
            this->m.ctrl = IsKeyDown(KEY_LEFT_CONTROL);
            this->m.shift = IsKeyDown(KEY_LEFT_SHIFT);
            this->m.alt = IsKeyDown(KEY_LEFT_ALT);
        }

        UT::b Keyboard::CtrlActive(void) const {
            return this->m.ctrl;
        }

        UT::b Keyboard::AltActive(void) const {
            return this->m.alt;
        }

        UT::b Keyboard::ShiftActive(void) const {
            return this->m.shift;
        }

        void Keyboard::AssignTabSize(UT::ui32* size) {
            if (*size == 0 || *size > 255){
                CBLT::Utils::Err::Log("INVALID TAB SIZE: " + std::to_string(*size) + " (MUST BE BETWEEN 1 AND 255) DEFAULTING TO 4\n");
                *size = 4;
            }

            this->tabSize = size;
            this->tab.clear();

            // Construct the tab
            for (UT::llui32 i = 0 ; i < *tabSize ; i++) {
                this->tab.append(" ");
            }
        }
} // CBLT
