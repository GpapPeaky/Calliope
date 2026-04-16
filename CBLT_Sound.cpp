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
        
    void SoundSystem::Load(std::string path) {
        // Inflile sounds
        
        soundVector[(UT::llui32)SoundClass::SOUND_INFILE_INSERT]       = LoadSound((path + "infile/insert.wav").c_str()); // C
        soundVector[(UT::llui32)SoundClass::SOUND_INFILE_DELETE]       = LoadSound((path + "infile/del.wav").c_str());    // C
        soundVector[(UT::llui32)SoundClass::SOUND_INFILE_NAV]          = LoadSound((path + "infile/nav.wav").c_str());    // C
        soundVector[(UT::llui32)SoundClass::SOUND_INFILE_RETURN]       = LoadSound((path + "infile/return.wav").c_str()); // C           
        soundVector[(UT::llui32)SoundClass::SOUND_INFILE_SPACE]        = LoadSound((path + "infile/space.wav").c_str());  // C       

        // Console sounds
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_OPEN]        = LoadSound((path + "console/copen.wav").c_str());
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_CLOSE]       = LoadSound((path + "console/cclose.wav").c_str());
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_EXECUTE]     = LoadSound((path + "console/cexec.wav").c_str());
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_FILE_SWITCH] = LoadSound((path + "console/cfswitch.wav").c_str());
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_ERROR]       = LoadSound((path + "console/cerr.wav").c_str());
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_INFO]        = LoadSound((path + "console/cinfo.wav").c_str());
        soundVector[(UT::llui32)SoundClass::SOUND_CONSOLE_GUIDE]       = LoadSound((path + "console/cguide.wav").c_str());

        // File queue sounds
        soundVector[(UT::llui32)SoundClass::SOUND_FQ_TRAVERSE]         = LoadSound((path + "fq/fqnav.wav").c_str());
        soundVector[(UT::llui32)SoundClass::SOUND_FQ_DEQUEUE]          = LoadSound((path + "fq/fqdq.wav").c_str());             

        // Other
        soundVector[(UT::llui32)SoundClass::SOUND_EXIT]                = LoadSound((path + "other/cbltexit.wav").c_str());

        for (UT::llui32 i = 0 ; i < (UT::llui32)SoundClass::EDITOR_SOUNDS_COUNT ; i++) {
            if (!IsSoundValid(soundVector[i])) {
                CBLT::Utils::Err::Log("BAD SOUND DATA AT INDEX " + std::to_string(i) + "\n");
            }
        }
    }
        
    void SoundSystem::Play(CBLT::SoundClass index) {
        if (index >= SoundClass::EDITOR_SOUNDS_COUNT) return;

        // Introduce a pitch variation as well!
        const float pitchVar = 0.67f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (1.45f - 0.8f);
        
        SetSoundPitch(soundVector[(UT::llui32)index], pitchVar);
        PlaySound(soundVector[(UT::llui32)index]);
    }
} // CBLT