#include "CBLT_Camera.hpp"

namespace CBLT {
    Offset gOffsets = { 0.0f, 0.0f };

    Camera::Camera(void) {
        originX = 1;
        originY = CBLT::UI::TOP_BAR_HEIGHT + CBLT::UI::TOP_BAR_SEPERATOR_Y;
        width = GetScreenWidth() - 1;
        height = GetScreenHeight() - (CBLT::UI::TOP_BAR_HEIGHT + CBLT::UI::TOP_BAR_SEPERATOR_Y) - 1;
    }

    Camera::~Camera(void) {}

    void Camera::Set(UT::f32 x, UT::f32 y) {
        gOffsets.x = x;
        gOffsets.y = y;
    }

    void Camera::Move(UT::f32 dx, UT::f32 dy) {
        gOffsets.x += dx;
        gOffsets.y += dy;
    }

    UT::b Camera::Contains(UT::f32 x, UT::f32 y, UT::f32 w, UT::f32 h) const {
        return (x + w > gOffsets.x && x < gOffsets.x + width &&
                y + h > gOffsets.y && y < gOffsets.y + height);
    }

    UT::b Camera::Contains(UT::i32 x, UT::i32 y, UT::i32 w, UT::i32 h) const {
        return (x + w > gOffsets.x && x < gOffsets.x + width &&
                y + h > gOffsets.y && y < gOffsets.y + height);
    }

    UT::i32 Camera::Width(void) {
        return width;
    }

    UT::i32 Camera::Height(void) {
        return height;
    }

    void Camera::Draw(void) {
        DrawRectangleLines(originX, originY, width, height, Color{255, 0, 0, 255});
    }
} // CBLT