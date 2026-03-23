#include "CBLT_InfileMark.hpp"

namespace CBT {
    UT::ui32 gMarkIDFactory = 0;

    UT::ui32 InfileMark::Col(void) const {
        return col;
    }
    
    UT::ui32 InfileMark::Line(void) const {
        return line;
    }
    
    UT::ui8 InfileMark::Id(void) const {
        return id;
    }
    
    void InfileMark::Draw(void) {
        return;
    }
    
    InfileMark::InfileMark(void) {
        this->id = gMarkIDFactory++;        
    }
       
    InfileMark::~InfileMark(void) {}    
} // CBLT