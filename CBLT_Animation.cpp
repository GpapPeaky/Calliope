#include "CBLT_Animation.hpp"

namespace CBLT {
    Animator::Animator(void) : x(0.0f), y(0.0f), interp() {}

    Animator::~Animator(void) {}

    void Animator::MoveTo(UT::f32 toX, UT::f32 toY, UT::f32 speed) {
        interp.Start(x, y, toX, toY, speed);
    }

    void Animator::Update(void) {
        if (interp.IsActive()) {
            auto [nx, ny] = interp.Update();
            x = nx;
            y = ny;
        }
    }

    UT::b Animator::IsMoving(void) const {
        return interp.IsActive();
    }
} // CBLT