#pragma once 

#include "CBLT_Util.hpp"
#include "CBLT_File.hpp"

namespace CBLT {


    class FileQueue {
        private:
            std::vector<File> loadedFiles;
            UT::ui32 activeIndex = 0;
        public:
            // Add a file to the queue
            void LoadFileToQueue(const File& f);

            // Remove a file from the queue
            void CloseFile(UT::i32 idx);

            // Set active the next in queue
            void SetActiveNext(void);

            // Set active the prev in the queue
            void SetActivePrev(void);

            // Get the active file, mutable
            File& Active(void);

            // Get the active file, read-only
            const File& Active(void) const ;

            // Get the activeIndex;
            const UT::i32 Index(void) const ;

            // Get the number of loaded files
            UT::llui32 Size(void) const ; 

            // Get the file queue
            std::vector<File>& GetLoadedFiles(void) ;

            // Get the file queue
            const std::vector<File>& GetLoadedFiles(void) const ;

            // Draw the file queue at the bottom of the screen
            void Draw(void);
    }; // File queue class
} // CBLT