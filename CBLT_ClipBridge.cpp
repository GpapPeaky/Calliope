#include "CBLT_ClipBridge.hpp"

namespace CBLT {
    #if (OS == 0) // Windows
        std::string ClipBridge::GetClipboardText(void) const {
            if (!OpenClipboard(nullptr))
                return "";

            HANDLE hData = GetClipboardData(CF_TEXT);
            if (!hData) {
                CloseClipboard();
                return "";
            }

            char* pszText = static_cast<char*>(GlobalLock(hData));
            if (!pszText) {
                CloseClipboard();
                return "";
            }

            std::string text(pszText);

            GlobalUnlock(hData);
            CloseClipboard();

            return text;
        }

        void ClipBridge::SetClipboardText(const std::string& text) const {
            if (!OpenClipboard(nullptr))
                return;

            EmptyClipboard();

            HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
            if (!hGlob) {
                CloseClipboard();
                return;
            }

            memcpy(GlobalLock(hGlob), text.c_str(), text.size() + 1);
            GlobalUnlock(hGlob);

            SetClipboardData(CF_TEXT, hGlob);
            CloseClipboard();
        }    
    #elif (OS == 1) // Linux
        std::string ClipBridge::GetClipboardText(void) const {
            FILE* pipe = popen(
                "wl-paste 2>/dev/null || xclip -selection clipboard -o 2>/dev/null",
                "r"
            );
            if (!pipe) return "";
        
            char buffer[256];
            std::string result;
        
            while (fgets(buffer, sizeof(buffer), pipe))
                result += buffer;
        
            pclose(pipe);
            return result;
        }
        
        void ClipBridge::SetClipboardText(const std::string& text) const {
            FILE* pipe = popen(
                "wl-copy 2>/dev/null || xclip -selection clipboard",
                "w"
            );
            if (!pipe) return;
        
            fwrite(text.c_str(), sizeof(char), text.size(), pipe);
            pclose(pipe);
        }
    #elif (OS == 2) // MacOS
        std::string ClipBridge::GetClipboardText(void) const {
            FILE* pipe = popen("pbpaste", "r");
            if (!pipe) return "";
        
            char buffer[256];
            std::string result;
        
            while (fgets(buffer, sizeof(buffer), pipe))
                result += buffer;
        
            pclose(pipe);
            return result;
        }
        
        void ClipBridge::SetClipboardText(const std::string& text) const {
            FILE* pipe = popen("pbcopy", "w");
            if (!pipe) return;
        
            fwrite(text.c_str(), sizeof(char), text.size(), pipe);
            pclose(pipe);
        }
    #endif
}