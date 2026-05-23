#include "CBLT_Camera.hpp"

namespace CBLT {
    Camera::Camera(void) {
        originX = 1;
        originY = CBLT::FileMargins::UI::TOP_BAR_HEIGHT + CBLT::FileMargins::UI::TOP_BAR_SEPERATOR_Y;
        clampMarginX = 90;
        clampMarginY = 60;
        width = GetScreenWidth() - 1;
        height = GetScreenHeight() - (CBLT::FileMargins::UI::TOP_BAR_HEIGHT + CBLT::FileMargins::UI::TOP_BAR_SEPERATOR_Y) - 1;
    }

    Camera::~Camera(void) {}

    void Camera::Set(UT::f32 x, UT::f32 y, Offset& off) {
        off.x = x;
        off.y = y;
    }

    void Camera::Move(UT::f32 dx, UT::f32 dy, Offset& off) {
        off.x += dx;
        off.y += dy;
    }

    UT::b Camera::Contains(UT::f32 x, UT::f32 y, UT::f32 w, UT::f32 h) const {
        return (x + w > this->originX && x < this->originX + width &&
                y + h > this->originY && y < this->originY + height);
    }

    UT::b Camera::Contains(UT::i32 x, UT::i32 y, UT::i32 w, UT::i32 h) const {
        return (x + w > this->originX && x < this->originX + width &&
                y + h > this->originY && y < this->originY + height);
    }

    UT::i32 Camera::Width(void) {
        return width;
    }

    UT::i32 Camera::Height(void) {
        return height;
    }

    void Camera::Draw(void) {
        // Draw file contents inside this box
        DrawRectangleLines(originX, originY, width, height, Color{255, 0, 0, 255});
        
        DrawRectangleLines(originX + CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI
            , originY, width, height, Color{0, 255, 255, 255});

        // Draw clamping margins
        // If the cursor moves outside this box, the global offsets move accordingly
        DrawRectangleLines(
            originX + clampMarginX,
            originY + clampMarginY,
            width - (clampMarginX * 2),
            height - (clampMarginY * 2),
            Color{0, 0, 255, 255}
        );
    }

    UT::i32 Camera::MarginX(void) {
        return clampMarginX;
    }

    UT::i32 Camera::MarginY(void) {
        return clampMarginY;
    }

    CameraOrigin Camera::Origin(void) {
        return {
            originX,
            originY
        };
    }

    void Camera::SetHeight(UT::ui32 h) {
        this->height = h;
    }

    void Camera::SetWidth(UT::ui32 w) {
        this->width = w;
    }

    void Camera::SetOrigin(UT::ui32 x, UT::ui32 y) {
        this->originX = x;
        this->originY = y;
    }
} // CBLT