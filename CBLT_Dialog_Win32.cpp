#include "CBLT_Dialog.hpp"

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <shlobj.h>
    #include <commdlg.h>

namespace CBLT {
    Dialog::Dialog(void) {}
        
    Dialog::~Dialog(void) {}

    std::string Dialog::RunCommand(const std::string& com) {
        return com; // No reason for it to exist at win32
    }

    std::string Dialog::OpenFolderPicker(void) {
        // Peaky: Win32 fucking sucks
        
        HRESULT hr;
        IFileDialog* pDialog = nullptr;
    
        // Initialize COM (safe to call multiple times)
        hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr)) {
            return "";
        }
    
        // Create the File Open Dialog
        hr = CoCreateInstance(
            CLSID_FileOpenDialog,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&pDialog)
        );
    
        if (FAILED(hr)) {
            CoUninitialize();
            return "";
        }
    
        // Set options: pick folders instead of files
        DWORD options;
        pDialog->GetOptions(&options);
        pDialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
    
        // Show dialog
        hr = pDialog->Show(nullptr);
        if (FAILED(hr)) {
            pDialog->Release();
            CoUninitialize();
            return "";
        }
    
        // Get the result
        IShellItem* pItem = nullptr;
        hr = pDialog->GetResult(&pItem);
        if (FAILED(hr)) {
            pDialog->Release();
            CoUninitialize();
            return "";
        }
    
        // Extract the filesystem path
        PWSTR widePath = nullptr;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &widePath);
    
        std::string result;
        if (SUCCEEDED(hr) && widePath) {
            int size = WideCharToMultiByte(
                CP_UTF8, 0,
                widePath, -1,
                nullptr, 0,
                nullptr, nullptr
            );
    
            result.resize(size - 1);
            WideCharToMultiByte(
                CP_UTF8, 0,
                widePath, -1,
                result.data(), size,
                nullptr, nullptr
            );
    
            CoTaskMemFree(widePath);
        }
    
        pItem->Release();
        pDialog->Release();
        CoUninitialize();
    
        return result;
    }

    Dialog gDialog;
} // CBLT

#endif // _WIN32