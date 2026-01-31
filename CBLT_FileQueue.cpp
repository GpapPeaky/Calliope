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
        const UT::i32 fontSize = 20;

        for (index = 0 ; index < Size() ; index++) {
            std::string construct = "[" + std::to_string(index) + "] " + std::string(loadedFiles[index].Name());

            if (index == activeIndex) {
                DrawText(
                    construct.c_str(),
                    index * 400,
                    GetScreenHeight() - fontSize,
                    fontSize,
                    gPalette.currentFile
                );
            } else {
                DrawText(
                    construct.c_str(),
                    index * 400,
                    GetScreenHeight() - fontSize,
                    fontSize,
                    gPalette.fileQueueEntry
                );
            }
        }
    }
} // CBLT