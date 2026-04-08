#include "CBLT_Dialog.hpp"

#if defined(__linux__)

#include <cstdlib>
#include <cstdio>
#include <string>
#include <array>

namespace CBLT {
    Dialog::Dialog(void) {}
    Dialog::~Dialog(void) {}

    // Helpers

    static bool CommandExists(const char* cmd) {
        std::string check = "command -v ";
        check += cmd;
        check += " > /dev/null 2>&1";
        return system(check.c_str()) == 0;
    }

    static bool HasDisplay() {
        return std::getenv("DISPLAY") || std::getenv("WAYLAND_DISPLAY");
    }

    static std::string GetHome() {
        const char* home = std::getenv("HOME");
        return home ? std::string(home) : std::string(".");
    }

    std::string Dialog::RunCommand(const std::string& com) {
        std::array<char, 512> buffer{};
        std::string result;

        FILE* pipe = popen(com.c_str(), "r");
        if (!pipe) return "";

        while (fgets(buffer.data(), buffer.size(), pipe)) {
            result += buffer.data();
        }

        pclose(pipe);

        // Trim trailing newline
        if (!result.empty() && result.back() == '\n')
            result.pop_back();

        return result;
    }

    // Public API

    std::string Dialog::OpenFolderPicker(void) {

        std::string home = GetHome();

        // GUI dialogs
        if (HasDisplay()) {

            if (CommandExists("zenity")) {
                std::string res = RunCommand(
                    "zenity --file-selection --directory "
                    "--title='Select Folder' "
                    "2>/dev/null"
                );
                if (!res.empty()) return res;
            }

            if (CommandExists("kdialog")) {
                std::string res = RunCommand(
                    "kdialog --getexistingdirectory \"" + home + "\" "
                    "2>/dev/null"
                );
                if (!res.empty()) return res;
            }

            if (CommandExists("yad")) {
                std::string res = RunCommand(
                    "yad --file --directory "
                    "--title='Select Folder' "
                    "2>/dev/null"
                );
                if (!res.empty()) return res;
            }
        }

        // Terminal dialogs
        if (CommandExists("dialog")) {
            std::string res = RunCommand(
                "dialog --stdout --title 'Select Folder' "
                "--dselect \"" + home + "\" 20 60"
            );
            if (!res.empty()) return res;
        }

        if (CommandExists("whiptail")) {
            // whiptail has no directory picker fallback to input box, procs for archlinux
            std::string res = RunCommand(
                "whiptail --inputbox 'Enter folder path:' 10 60 \"" + home + "\" "
                "3>&1 1>&2 2>&3"
            );
            if (!res.empty()) return res;
        }

        if (CommandExists("xdg-open")) {
            system(("xdg-open \"" + home + "\" >/dev/null 2>&1").c_str());
        }

        return "";
    }

    Dialog gDialog;

} // namespace CBLT

#endif