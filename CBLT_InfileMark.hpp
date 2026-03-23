#include <vector>

#include "CBLT_Utils.hpp"

namespace CBLT {
    extern UT::ui32 gMarkIDfactory;

    class InfileMark {
        private:
            UT::ui8 id;    // Mark identifier   
            UT::ui32 col;  // Mark is placed in column 
            UT::ui32 line; // Mark is placed in line
        public:
            // Get the mark column
            UT::ui32 Col(void) const ;
        
            // Get the mark line
            UT::ui32 Line(void) const ;
            
            // Get the mark identifier
            UT::ui8 Id(void) const ;
        
            // Draw the mark
            void Draw(void);
        
            // Constructor
            InfileMark(void);
            
            // Destructor
            ~InfileMark(void);
     }; // Infile mark class for marking files 
} // CBLT