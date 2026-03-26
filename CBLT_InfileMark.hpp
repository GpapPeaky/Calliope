#include <vector>
#include <string>

#include "CBLT_raylib.hpp"
#include "CBLT_Font.hpp"
#include "CBLT_Palette.hpp"
#include "CBLT_Util.hpp"

namespace CBLT {
    extern UT::ui32 gMarkIDfactory;

    class InfileMark {
        private:
            UT::ui32 id;    // Mark identifier
            UT::ui32 col;   // Mark is placed in column
            UT::ui32 line;  // Mark is placed in line
        public:
            // Get the mark column
            UT::ui32 Col(void) const ;
        
            // Get the mark line
            UT::ui32 Line(void) const ;
            
            // Get the mark identifier
            UT::ui32 Id(void) const ;
        
            // Draw the mark
            void Draw(UT::ui32 renderX, UT::ui32 renderY);
        
            // Constructor
            InfileMark(UT::ui32 c, UT::ui32 l);
            
            // Destructor
            ~InfileMark(void);
     }; // Infile mark class for marking files 
} // CBLT