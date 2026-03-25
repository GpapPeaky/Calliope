#include "CBLT_InfileMark.hpp"

namespace CBT {
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
    
    void InfileMark::Draw(void) {
        return;
    }
    
    InfileMark::InfileMark(UT::ui32 c, UT::ui32 l) {
        this->id = gMarkIDFactory++;        

        this->col = c;
        this->line = l;
    }
       
    InfileMark::~InfileMark(void) {}    
} // CBLT