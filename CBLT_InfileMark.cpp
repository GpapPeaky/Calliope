#include "CBLT_InfileMark.hpp"

namespace CBLT {
    UT::ui32 gMarkIDFactory = 0;

    UT::ui32 InfileMark::Col(void) const {
        return col;
    }
    
    UT::ui32 InfileMark::Line(void) const {
        return line;
    }
    
    UT::ui32 InfileMark::Id(void) const {
        return id;
    }
    
    void InfileMark::Draw(UT::ui32 renderX, UT::ui32 renderY) {
        // Mark rect
        DrawRectangle(renderX, renderY, gFont.size, gFont.size, gPalette.frag);

        // Mark id
        DrawText(std::to_string(this->id).c_str(), renderX, renderY, gFont.size, gPalette.cursor);
    }
    
    InfileMark::InfileMark(UT::ui32 c, UT::ui32 l) {
        this->id = gMarkIDFactory++;        

        this->col = c;
        this->line = l;
    }
       
    InfileMark::~InfileMark(void) {}    
} // CBLT