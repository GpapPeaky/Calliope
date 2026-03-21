#pragma once

#include "CBLT_Interpolator.hpp"
#include "CBLT_Util.hpp"

namespace CBLT {
    class Animator {
        public: 
            UT::f32 x = 0.0f;       // X position
            UT::f32 y = 0.0f;       // Y position
            Interpolator interp;    // Interpolator tied to animation line

            // Constructor 
            Animator(void);
            
            // Destructor
            ~Animator(void);

            // Update animation positions function
            void Update(void);

            // Check if there is an active animation
            UT::b IsMoving(void) const ;

            // Move to function, initiate movement animation
            void MoveTo(UT::f32 toX, UT::f32 toY, UT::f32 speed = 0.25f);
    }; // Animator class
} // CBLT