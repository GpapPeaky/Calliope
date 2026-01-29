#include "CBLT_FileQueue.hpp"

namespace CBLT {
    void FileQueue::LoadFileToQueue(const File& f) {
        loadedFiles.push_back(f);
        activeIndex = Size() - 1;
    }

    void FileQueue::CloseFile(UT::i32 idx) {
        if (idx < 0 || idx >= (UT::i32)loadedFiles.size()) return;
    
        loadedFiles.erase(loadedFiles.begin() + idx);
    
        // Adjust activeIndex
        if (loadedFiles.empty()) {
            activeIndex = 0;
        } else if (activeIndex >= (UT::i32)loadedFiles.size()) {
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

    const UT::i32 FileQueue::Index(void) const  {
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
} // CBLT