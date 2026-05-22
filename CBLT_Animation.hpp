#pragma once

#include "CBLT_Interpolator.hpp"
#include "CBLT_Util.hpp"
#include "CBLT_Sys.hpp"

#include <cmath>

namespace CBLT {
    enum class AnimationEase : UT::i32 {
        NONE,         // Instant, no interpolation
        LINEAR,       // Constant speed
        EASE_OUT,     // Fast start, slow finish
        EASE_IN,      // Slow start, fast finish  
        ELASTIC,      // Overshoot and bounce back
        BOUNCE        // Snappy with a little pop
    }; // Animation eases

    typedef struct AnimationProfile {
        AnimationEase ease;         // Ease type
        UT::f32       speed;        // Speed (progress per frame)
        UT::f32       overshoot;    // Elastic/bounce overshoot amount
        UT::f32       stiffness;    // How snappy the return is
        UT::f32       damping;      // How quickly oscillation dies
    } AnimationProfile; // Animation profile, how to apply different speeds, ease in/out and scaling

    class Animator {
        public: 
            UT::f32 tx = 0.0f;       // X position translate
            UT::f32 ty = 0.0f;       // Y position translate
            UT::b init = false;      // Initialised position
            Interpolator interp;     // Interpolator tied to animation line
            AnimationProfile* anim;  // Animator profile pointer, always read from settings global

            // Set the animation type of the 
            void SetProfile(AnimationProfile* profile);

            // Constructor 
            Animator(void);
            
            // Destructor
            ~Animator(void);

            // Update animation positions function
            void Update(void);

            // Check if there is an active animation
            UT::b IsMoving(void) const ;

            // Snap function
            void Snap(UT::f32 toX, UT::f32 toY);

            // Move to function, initiate movement animation
            void MoveTo(UT::f32 toX, UT::f32 toY);
    }; // Animator class

    // Read animation profile file, animation profiles for console, fq and cursor
    AnimationProfile ReadAnimationFile(const std::string& path);
} // CBLT