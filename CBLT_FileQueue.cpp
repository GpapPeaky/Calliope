#include "CBLT_FileQueue.hpp"

namespace CBLT {
    void FileQueue::LoadFileToQueue(const File& f) {
        // FIXME: Add a validation check here
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
        return loadedFiles[activeIndex];
    }

    const File& FileQueue::Active(void) const {
        return loadedFiles[activeIndex];
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

        for (index = 0 ; index < Size() ; index++) {
            std::string construct = "[" + std::to_string(index) + "] " + std::string(loadedFiles[index].Name());

            if (index == activeIndex) {
                DrawTextEx(
                    gFont.f,
                    construct.c_str(),
                    { 
                        (UT::f32)(5 + index * 400),
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
                        (UT::f32)(5 + index * 400),
                        (UT::f32)(GetScreenHeight() - fontSize) - 2.5f,
                    },
                    fontSize,
                    0.0f,
                    gPalette.fileQueueEntry
                );
            }
        }
    }
} // CBLT