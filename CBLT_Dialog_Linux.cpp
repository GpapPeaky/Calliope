#include "CBLT_Dialog.hpp"

#if defined(__linux__)
    #include <cstdlib>
    #include <cstdio>
    #include <string>

namespace CBLT {
    std::string Dialog::RunCommand(const std::string& com) {
        char buffer[512];

        std::string result;
        
        FILE* pipe = popen(com.c_str(), "r");
        
        if (!pipe) return "";
        
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        
        pclose(pipe);
        
        if (!result.empty() && result.back() == '\n') result.pop_back();
        
        return result;
    }

    std::string Dialog::OpenFolderPicker(void) {
        return RunCommand(" --file-selection --directory");
    }

    Dialog::Dialog(void) {}
        
    Dialog::~Dialog(void) {}

    Dialog gDialog;
} // CBLT

#endif // Linux