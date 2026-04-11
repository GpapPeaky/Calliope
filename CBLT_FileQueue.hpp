#pragma once 

#include "CBLT_Util.hpp"
#include "CBLT_File.hpp"

#include <execinfo.h>

namespace CBLT {
    class FileQueue {
        private:
            std::vector<File> loadedFiles;   // Loaded files queue
            UT::llui32 activeIndex = 0;      // Active file index
            Camera cam;                      // Camera for the file queue, applying offsets in controller
            Offset camOffset;                // Camera offset for smooth movement when switching files
        public:
            // Add a file to the queue
            void LoadFileToQueue(const File& f);

            // Remove a file from the queue
            void CloseFile(UT::llui32 idx);

            // Set active the next in queue
            void SetActiveNext(void);

            // Set active the prev in the queue
            void SetActivePrev(void);

            // Get the active file, mutable
            File& Active(void);

            // Get the active file, read-only
            const File& Active(void) const ;

            // Get the activeIndex;
            UT::llui32 Index(void) const ;

            // Get the number of loaded files
            UT::llui32 Size(void) const ; 

            // Get the file queue
            std::vector<File>& GetLoadedFiles(void) ;

            // Get the file queue
            const std::vector<File>& GetLoadedFiles(void) const ;

            // Draw the file queue at the bottom of the screen
            void Draw(void);

            // File queue camera
            Camera& Cam(void);

            // Camera offsets
            Offset& CameraOffset(void);

            // Scroll the file queue camera by a specific offset, only in the x axis
            void Scroll(UT::f32 dx);

            // Constructor
            FileQueue(void);

            // Destructor
            ~FileQueue(void);
    }; // File queue class
} // CBLT