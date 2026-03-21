#pragma once

#include "CBLT_Util.hpp"

namespace CBLT {
    // Animation line for interpolation
    typedef struct AnimationLine {
        UT::f32 targetX;            // Where to go at x
        UT::f32 targetY;            // Where to go at y
        UT::f32 sourceX;            // Where from at x
        UT::f32 sourceY;            // Where from at y            
    } AnimationLine;

    class Interpolator {
        private:
            AnimationLine al;   // Animation line to follow
            UT::f32 progress;   // 0.0 at source -> 1.0 at target
            UT::f32 speed;      // How fast the interpolation is moving
            UT::b active;       // Interpolation ongoing

        public:
            // Constructor
            Interpolator(void);

            // Start a new interpolation
            void Start(UT::f32 fromX, UT::f32 fromY, UT::f32 toX, UT::f32 toY, UT::f32 speedPerFrame = 0.1f);
    
            // Update interpolation and return current position
            std::pair<UT::f32, UT::f32> Update(void);
    
            // Check if interpolation is active
            UT::b IsActive(void) const;
    
            // Stop or reset interpolation
            void Stop(void);
    }; // Interpolator class

    typedef struct {
        UT::f32 ax;         // Animation X
        UT::f32 ay;         // Animation Y
    } Interpolator2DHelper; // Interpolator animation 2D vector helper
} // CBLT