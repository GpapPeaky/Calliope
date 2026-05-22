#include "CoBaLT_INCLUDES.hpp"

// Minor POSIX system fix for easier use
#if defined(__linux__) || defined(__APPLE__)
    #include <unistd.h>
    #include <limits.h>  // For PATH_MAX
#endif

// TODO: Add the animator to the console cursor as well
// TODO: Add animators to the filequeue and console resizing and contents movement

// FIXME: Fix some issues with SELECTION MODE bugging out, and selecting from 0,0  
// FIXME: Fix issues with selection pasting cursor position

// FIXME: For some reason pressing enter after '}' or '/' doesn't do anything, till the cursor moves again

// FIXME: Multi-cursor indentation is problematic
// FIXME: Multi-cursor delete at the end of the file, crashes | deletes too many lines

UT::i32 main(int argc, char** argv) {
    CBLT::Win::Init();

    CBLT::Utils::Err::Init();

    //  Previous / Linux asset handling
#if defined(__linux__)
    CBLT::Controller ctrl;

    std::string userDir = (argc > 1) ? std::string(argv[1]) : "/home";
    ctrl.InitCWD(userDir);

    const char* resource_path = getenv("CBLT_RESOURCES");
    std::string resourceDir = resource_path ? std::string(resource_path) : ".";

    CBLT::gFont.Load(resourceDir + "/assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gFont.size = 23;
    CBLT::gFont.Config();

    CBLT::gConsoleFont.Load(resourceDir + "/assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gConsoleFont.size = 20;
    CBLT::gConsoleFont.Config();

    CBLT::gTopBarFont.Load(resourceDir + "/assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gTopBarFont.size = 23;
    CBLT::gTopBarFont.Config();

    CBLT::gFileQueueFont.Load(resourceDir + "/assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gFileQueueFont.size = 21;
    CBLT::gFileQueueFont.Config();

    CBLT::gSound.Load(resourceDir + "/assets/audio/"); // Pass resourceDir here
#elif defined(__APPLE__)
    CBLT::Controller ctrl; // Controller instance
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        ctrl.InitCWD(std::string(cwd));
    } else {
        ctrl.InitCWD("/"); 
    }

    // Previous asset loading (macOS)
    CBLT::gFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gFont.size = 23;
    CBLT::gFont.Config();

    CBLT::gConsoleFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gConsoleFont.size = 20;
    CBLT::gConsoleFont.Config();

    CBLT::gTopBarFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gTopBarFont.size = 20;
    CBLT::gTopBarFont.Config();

    CBLT::gFileQueueFont.Load("/assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gFileQueueFont.size = 21;
    CBLT::gFileQueueFont.Config();

    CBLT::gSound.Load("assets/audio/");
#elif defined(_WIN32)
    CBLT::Controller ctrl; // Controller instance
    ctrl.InitCWD("C:/");

    // Previous asset loading (Windows)
    CBLT::gFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gFont.size = 23;
    CBLT::gFont.Config();

    CBLT::gConsoleFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gConsoleFont.size = 20;
    CBLT::gConsoleFont.Config();

    CBLT::gTopBarFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gTopBarFont.size = 20;
    CBLT::gTopBarFont.Config();

    CBLT::gFileQueueFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gFileQueueFont.size = 21;
    CBLT::gFileQueueFont.Config();

    CBLT::gSound.Load("assets/audio/");
#endif
    CBLT::gSettings.ReadSettings();
    ctrl.GetKeyboard().AssignTabSize(&CBLT::gSettings.OPTION_TabSize);

    ctrl.GetConsole().GetCWDContents(ctrl.CWD());

    UT::ui32 currentFileLineCount; 
    UT::b currentFileDirt; 
    std::string currentFileName;
    std::string currentFileLangConfName;

    // Initialize palette
    CBLT::gPalette.ReadPaletteFile(CBLT::gSettings.OPTION_Palette);

    UT::ui32 framesCount = 0;
    while(!WindowShouldClose()) {
        BeginDrawing();

            ClearBackground(CBLT::gPalette.background);

            ctrl.Update();

            // Cursor & console references
            CBLT::Console& cnsl = ctrl.GetConsole();
            CBLT::FileQueue& fq = ctrl.LoadedFileQueue();
            CBLT::Camera& cam = ctrl.GetCamera();

            // Draw file
            if (fq.Size() > 0) {
                CBLT::File& f = fq.Active();
                CBLT::CursorManager& cm = ctrl.GetActiveCursorManager();
                CBLT::Cursor& c = cm.Primary();

                // Need to retokenize every frame, else non updated tokens with invalid lengths will crash the program
                f.RetokenizeDirtyLines();
                c.AcquireFragment(c.Col(), f.GetCurrentLine(c.Line()));

                // File cursors
                UT::ui32 baseX = CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
                    CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES;

                cm.DrawCursors(CBLT::gFont, baseX, 0);
                f.Draw(cam, c.renderX, c.renderY, cnsl.IsOpen(), cnsl.Width());
                
                currentFileLineCount        = f.GetLineCount();
                currentFileDirt             = f.Dirt();
                currentFileName             = f.Name();
                currentFileLangConfName     = f.LangConf();

                CBLT::DrawInfo(c, currentFileLineCount, currentFileDirt, currentFileName, ctrl.CWD(), currentFileLangConfName);
            }

            if (cnsl.IsOpen()) {
                ctrl.HandleConsoleMouseWheel();
                Rectangle rect = ctrl.HandleConsoleMouseClick();
                cnsl.Draw(fq, rect);
            }

            if (fq.Size() > 0) fq.Draw();
            else cnsl.DrawGuide();

            cnsl.DrawMessage();

        EndDrawing();

        if (framesCount % 120 == 0) {
            ctrl.GetConsole().GetCWDContents(ctrl.CWD());
            cam.SetHeight(GetScreenHeight());
            cam.SetWidth(GetScreenWidth());
            cnsl.Cam().SetHeight(GetScreenHeight() - CBLT::gConsoleFont.size - 10);
        }

        framesCount++;
    }

    CBLT::Win::Destroy();
    
    return UDef::GRACEFUL_EXIT;
}