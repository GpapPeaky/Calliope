#include "CBLT_InfileMark.hpp"

namespace CBLT {
    UT::ui32 gMarkIDFactory = 0;

    UT::ui32 InfileMark::Line(void) const {
        return line;
    }
    
    UT::ui32 InfileMark::Id(void) const {
        return id;
    }
    
    void InfileMark::Draw(UT::ui32 renderX, UT::ui32 renderY, UT::ui32 width) {
        const UT::ui32 resizedSize = gFont.size / 1.35;

        // Mark rect
        DrawRectangle(renderX, renderY, width - 1, resizedSize, gPalette.textBase);

        // Mark id
        DrawTextEx(
            gFont.f,
            std::to_string(this->id).c_str(),
            {
                (UT::f32)renderX,
                (UT::f32)renderY
            },
            resizedSize,
            0.0f,
            gPalette.background
        );
    }
    
    InfileMark::InfileMark(UT::ui32 l) {
        this->id = gMarkIDFactory++;        

        this->line = l;
    }
       
    InfileMark::~InfileMark(void) {}

    void InfileMark::ReIndex(UT::ui32 i) {
        id = i;
    }
} // CBLT