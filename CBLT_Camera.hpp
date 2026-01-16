#pragma once

#include "CBLT_Util.hpp"
#include "CBLT_TopBar.hpp" // for screen dimensions

namespace CBLT {
    // Offset structure for camera
    typedef struct Offset {
        UT::f32 x;  // X coordinate
        UT::f32 y;  // Y coordinate
    }Offset;

    // Global offsets, will increment via camera movement
    extern Offset gOffsets;

    class Camera {
        private:
            // Origins stay the same
            UT::i32 originX;    // Camera origin x
            UT::i32 originY;    // Camera origin y
            UT::i32 width;      // Camera width
            UT::i32 height;     // Camera height
        public:
            // Constructor
            Camera(void);

            // Destructor
            ~Camera(void);

            // Set global offsets
            void Set(UT::f32 x, UT::f32 y);

            // Apply a delta at the global offsets
            void Move(UT::f32 dx, UT::f32 dy);

            // Check if a rectangle is within the camera view
            UT::b Contains(UT::f32 x, UT::f32 y, UT::f32 w, UT::f32 h) const;

            // Check if a rectangle is within the camera view
            UT::b Contains(UT::i32 x, UT::i32 y, UT::i32 w, UT::i32 h) const;

            // Get the camera width
            UT::i32 Width(void);

            // Get the camera height
            UT::i32 Height(void);

            // Draw the camera rectangle
            void Draw(void);
    }; // Camera class
} // CBLT