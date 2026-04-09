#pragma once

#include "CBLT_raylib.hpp"
#include "CBLT_Util.hpp"

namespace CBLT {
    // Sound class enumerator, index to the sound vector of the sound system
    enum class SoundClass : UT::llui32 {
        // Infile sounds
        SOUND_INFILE_INSERT = 0,
        SOUND_INFILE_DELETE,
        SOUND_INFILE_SPACE,
        SOUND_INFILE_NAV,
        SOUND_INFILE_RETURN,
        
        // Console sounds
        SOUND_CONSOLE_OPEN,
        SOUND_CONSOLE_CLOSE,
        SOUND_CONSOLE_EXECUTE,      // For ':' context directives
        SOUND_CONSOLE_FILE_SWITCH,  // For file-switch context directives
        SOUND_CONSOLE_ERROR,        // Error message alert sound
        SOUND_CONSOLE_INFO,
        SOUND_CONSOLE_GUIDE,
        
        // File queue sounds
        SOUND_FQ_TRAVERSE,          // Moving left or right inside the file queue
        SOUND_FQ_DEQUEUE,           // Dequeuing a file sound
        
        // Exit sounds
        SOUND_EXIT,                 // Editor exit sound
        
        EDITOR_SOUNDS_COUNT
    }; // Sound class
    
    class SoundSystem {
        private:
            Sound soundVector[(UT::llui32)SoundClass::EDITOR_SOUNDS_COUNT]; // Holds all the sounds
        public:
            // Constructor, init the audio device
            SoundSystem(void);
            
            // Destructor    
            ~SoundSystem(void);
            
            // Load all sounds
            void Load(std::string path);
            
            // Play a sound            
            void Play(CBLT::SoundClass index);
    }; // Sound System class
    
    // Sound system global
    extern SoundSystem gSound;
} // CBLT