#include "CoBaLT_INCLUDES.hpp"

// Minor POSIX system fix for easier use
#if defined(__linux__) || defined(__APPLE__)
    #include <unistd.h>
    #include <limits.h>  // For PATH_MAX
#endif

// TODO: Finish palette handling and palette editor along with animations
// TODO: Finish AnimationProfile editor 

UT::i32 main(int argc, char** argv) {
    CBLT::Win::Init();

    CBLT::Utils::Err::Init();
    CBLT::InitNAF();


    // ----------------- Previous / Linux asset handling -----------------
#if defined(__linux__)
    CBLT::Controller ctrl;

    std::string userDir = (argc > 1) ? std::string(argv[1]) : "/home";
    ctrl.InitCWD(userDir);

    const char* resource_path = getenv("CBLT_RESOURCES");
    std::string resourceDir = resource_path ? std::string(resource_path) : ".";

    CBLT::gFont.Load(resourceDir + "/assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gFont.Config();
    CBLT::gFont.size = 23;

    CBLT::gConsoleFont.Load(resourceDir + "/assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gConsoleFont.Config();
    CBLT::gConsoleFont.size = 20;

    CBLT::gSound.Load(resourceDir + "/assets/audio/"); // <--- pass resourceDir here
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
    CBLT::gFont.Config();
    CBLT::gFont.size = 23;

    CBLT::gConsoleFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gConsoleFont.Config();
    CBLT::gConsoleFont.size = 20;

    CBLT::gSound.Load();

#elif defined(_WIN32)
    CBLT::Controller ctrl; // Controller instance
    ctrl.InitCWD("C:/");

    // Previous asset loading (Windows)
    CBLT::gFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gFont.Config();
    CBLT::gFont.size = 23;

    CBLT::gConsoleFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gConsoleFont.Config();
    CBLT::gConsoleFont.size = 20;

    CBLT::gSound.Load("assets/audio");

#endif
// -------------------------------------------------------------------

    ctrl.GetConsole().GetCWDContents(ctrl.CWD());

    UT::ui32 currentFileLineCount; 
    UT::b currentFileDirt; 
    std::string currentFileName;

    // Default palette
    CBLT::gPalette.ReadPaletteFile("Stockholm");

    UT::ui32 framesCount = 0;
    while(!WindowShouldClose()) {
        BeginDrawing();

            ClearBackground(CBLT::gPalette.background);

            ctrl.Update();

            // Cursor & console references
            CBLT::CursorManager& cm = ctrl.GetActiveCursorManager();
            CBLT::Cursor& c = cm.Primary();
            CBLT::Console& cnsl = ctrl.GetConsole();
            CBLT::FileQueue& fq = ctrl.LoadedFileQueue();
            CBLT::File& f = fq.Active();
            CBLT::Camera& cam = ctrl.GetCamera();

            // Draw file
            if (fq.Size() > 0) {
                if (framesCount % 10 == 0) {
                    f.RetokenizeDirtyLines();
                    c.AcquireFragment(c.Col(), f.GetCurrentLine(c.Line()));
                }
                cm.DrawCursors(f.GetLines());
                f.Draw(cam, c.renderX, c.renderY, cnsl.IsOpen(), cnsl.Width());
                currentFileLineCount = f.GetLineCount();
                currentFileDirt      = f.Dirt();
                currentFileName      = f.Name();
            } else {
                currentFileLineCount = 0;
                currentFileDirt      = false;
                currentFileName      = "";
            }

            CBLT::DrawInfo(c, currentFileLineCount, currentFileDirt, currentFileName, ctrl.CWD());

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
    return 0;
}