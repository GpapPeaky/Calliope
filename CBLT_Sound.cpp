#include "CBLT_Sound.hpp"

namespace CBLT {
    SoundSystem gSound;

    SoundSystem::SoundSystem(void) {
        InitAudioDevice(); // Raylib.
    }
    
    SoundSystem::~SoundSystem(void) {
        for (UT::llui32 i = 0 ; i < (UT::llui32)SoundClass::EDITOR_SOUNDS_COUNT ; i++) {
            UnloadSound(soundVector[i]);       
        }
        
        CloseAudioDevice();
    }
        
    void SoundSystem::Load(void) {
        // Inflile sounds
        
        // TODO: Finish the samples, replace them, and add them where needed

        soundVector[(UT::llui32)SoundClass::SOUND_INFILE_INSERT] = LoadSound("assets/audio/infile/insert.wav"); // C
        soundVector[(UT::llui32)SoundClass::SOUND_INFILE_DELETE] = LoadSound("assets/audio/infile/del.wav");    // C
        soundVector[(UT::llui32)SoundClass::SOUND_INFILE_NAV]    = LoadSound("assets/audio/infile/nav.wav");    // C
        soundVector[(UT::llui32)SoundClass::SOUND_INFILE_RETURN] = LoadSound("assets/audio/infile/return.wav"); // C           
        soundVector[(UT::llui32)SoundClass::SOUND_INFILE_SPACE]  = LoadSound("assets/audio/infile/space.wav");  // C       

        // Console sounds
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_OPEN]        = LoadSound("assets/audio/console/copen.wav");
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_CLOSE]       = LoadSound("assets/audio/console/cclose.wav");
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_EXECUTE]     = LoadSound("assets/audio/console/cexec.wav");
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_FILE_SWITCH] = LoadSound("assets/audio/console/cfswitch.wav");
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_ERROR]       = LoadSound("assets/audio/console/cerr.wav");
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_INFO]        = LoadSound("assets/audio/console/cinfo.wav");
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_GUIDE]       = LoadSound("assets/audio/console/cguide.wav");

        // File queue sounds
        soundVector[(UT::llui32)SoundClass::SOUND_FQ_TRAVERSE] = LoadSound("assets/audio/fq/fqnav.wav");
        soundVector[(UT::llui32)SoundClass::SOUND_FQ_DEQUEUE]  = LoadSound("assets/audio/fq/fqdq.wav");             

        // Other
        soundVector[(UT::llui32)SoundClass::SOUND_EXIT] = LoadSound("assets/audio/other/cbltexit.wav");

        for (UT::llui32 i = 0 ; i < (UT::llui32)SoundClass::EDITOR_SOUNDS_COUNT ; i++) {
            if (!IsSoundValid(soundVector[i])) {
                CBLT::Utils::Err::Log("BAD SOUND DATA AT INDEX " + std::to_string(i) + "\n");
            }
        }
    }
        
    void SoundSystem::Play(CBLT::SoundClass index) {
        if (index >= SoundClass::EDITOR_SOUNDS_COUNT) return;

        // Introduce a pitch variation as well!
        const float pitchVar = 0.8f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (1.45f - 0.8f);
        
        SetSoundPitch(soundVector[(UT::llui32)index], pitchVar);
        PlaySound(soundVector[(UT::llui32)index]);
    }
} // CBLT