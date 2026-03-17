#include "CBLT_Dialog.hpp"

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <shlobj.h>
    #include <commdlg.h>

#endif // _WIN32