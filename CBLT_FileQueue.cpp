#include "CBLT_FileQueue.hpp"

namespace CBLT {
    void FileQueue::LoadFileToQueue(const File& f) {
        loadedFiles.push_back(f);
        activeIndex = Size() - 1;
    }

    void FileQueue::CloseFile(UT::llui32 idx) {
        if (idx >= loadedFiles.size()) return;
    
        loadedFiles.erase(loadedFiles.begin() + idx);
    
        // Adjust activeIndex
        if (loadedFiles.empty()) {
            activeIndex = 0;
        } else if (activeIndex >= loadedFiles.size()) {
            activeIndex = loadedFiles.size() - 1;
        }
    }

    void FileQueue::SetActiveNext(void) {
        if (activeIndex < Size() - 1) {
            activeIndex = activeIndex + 1;
        } else {
            activeIndex = 0;
        }
    }

    void FileQueue::SetActivePrev(void) {
        if (activeIndex > 0) {
            activeIndex = activeIndex - 1;
        } else {
            activeIndex = Size() - 1;
        }
    }

    File& FileQueue::Active(void) {
        if(Size() > 0)
            return loadedFiles.at(activeIndex);

        return CBLT::gNAF;
    }

    const File& FileQueue::Active(void) const {
        if(Size() > 0)
            return loadedFiles.at(activeIndex);
    
        return CBLT::gNAF;    
    }

    UT::llui32 FileQueue::Index(void) const  {
        return activeIndex;
    }

    UT::llui32 FileQueue::Size(void) const {
        return loadedFiles.size();
    }

    std::vector<File>& FileQueue::GetLoadedFiles(void) {
        return loadedFiles;
    }

    const std::vector<File>& FileQueue::GetLoadedFiles(void) const {
        return loadedFiles;
    }

    // FIXME: Add file-queue camera
    void FileQueue::Draw(void) {
        UT::ui32 index = 0;
        const UT::i32 fontSize = 21;
       
        // Background
        DrawRectangle(
            0,
            GetScreenHeight() - (fontSize + 7), // Add a small margin!
            GetScreenWidth(),
            fontSize + 7,
            gPalette.fileQueueColour
        );

        // Seperator
        DrawLine(
            0,
            GetScreenHeight() - (fontSize + 7),
            GetScreenWidth(),
            GetScreenHeight() - (fontSize + 7),
            gPalette.textSeperators
        );

        UT::f32 x = 5;
        const UT::f32 itemMargin = 5;

        UT::f32 barTop = (UT::f32)(GetScreenHeight() - (fontSize + 9));
        UT::f32 barBottom = (UT::f32)(GetScreenHeight() + 1);

        for (index = 0 ; index < Size() ; index++) {
            std::string construct = "[" + std::to_string(index) + "] " + std::string(loadedFiles.at(index).Name());
            UT::f32 constructWidth = MeasureTextEx(gFont.f, construct.c_str(), gFont.size, 0.0f).x;

            if (index == activeIndex) {
                DrawTextEx(
                    gFont.f,
                    construct.c_str(),
                    { 
                        x,
                        (UT::f32)(GetScreenHeight() - fontSize) - 2.5f
                    },
                    fontSize,
                    0.0f,
                    gPalette.currentFile
                );
            } else {
                DrawTextEx(
                    gFont.f,
                    construct.c_str(),
                    {
                        x,
                        (UT::f32)(GetScreenHeight() - fontSize) - 2.5f,
                    },
                    fontSize,
                    0.0f,
                    gPalette.fileQueueEntry
                );
            }

            // Draw separator AFTER this entry (if not last)
            if (index < Size() - 1) {
                UT::f32 sepX = x + constructWidth + (itemMargin * 0.5f);
        
                DrawLine(
                    (UT::ui32)sepX,
                    (UT::ui32)(barTop + 2),
                    (UT::ui32)sepX,
                    (UT::ui32)(barBottom - 2),
                    gPalette.textSeperators
                );
            }

            x += constructWidth + itemMargin;
        }
    }
} // CBLT