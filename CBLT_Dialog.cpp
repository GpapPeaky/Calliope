#include "CBLT_Dialog.hpp"

namespace CBLT {
    Dialog::Dialog(void) {}
        
    Dialog::~Dialog(void) {}

    std::string Dialog::OpenFolderPicker(void) {
        std::string folder;
        
        if (folderPath) {
            folder = std::string(folderPath);
        }

        std::cout << folderPath << "\n";

        return folder;
    }

    Dialog gDialog;
} // CBLT