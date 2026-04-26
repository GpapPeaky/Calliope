#include "CoBaLT_INCLUDES.hpp"

// Minor POSIX system fix for easier use
#if defined(__linux__) || defined(__APPLE__)
    #include <unistd.h>
    #include <limits.h>  // For PATH_MAX
#endif

// FIXME: for some reason pressing enter after '}' doesn't do anything, till the cursor moves again
// TODO: Add line swap with Alt + arrowKeys
// TODO: Make handling and handle function a bit more uniform in Controller::Update, both modes and the console IsOpen(...) block
// TODO: Write the HandleSelectionSpecial(...) function
// TODO: Finish AnimationProfile editor 
// FIXME: Multi-cursor indentation is problematic
// FIXME: Multi-cursor delete at the end of the file, crashes | deletes too many lines
// FIXME: Very weird bug that breaks backspace indentation after pressing '}' again?
// TODO: Add a token type for function call
// FIXME: Multiline comments and strings are a bit iffy
// FIXME: Move the line-comment checking in the file lexer to a function
// TODO: Finish the sound samples, replace them, and add them where needed

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
    CBLT::gFont.Config();
    CBLT::gFont.size = 23;

    CBLT::gConsoleFont.Load(resourceDir + "/assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gConsoleFont.Config();
    CBLT::gConsoleFont.size = 20;

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
    CBLT::gFont.Config();
    CBLT::gFont.size = 23;

    CBLT::gConsoleFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gConsoleFont.Config();
    CBLT::gConsoleFont.size = 20;

    CBLT::gSound.Load("assets/audio/");

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

    CBLT::gSound.Load("assets/audio/");

#endif

    CBLT::gSettings.ReadSettings();

    ctrl.GetConsole().GetCWDContents(ctrl.CWD());

    UT::ui32 currentFileLineCount; 
    UT::b currentFileDirt; 
    std::string currentFileName;

    // Default palette
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

                cm.DrawCursors(f.GetLines());
                f.Draw(cam, c.renderX, c.renderY, cnsl.IsOpen(), cnsl.Width());
                
                currentFileLineCount = f.GetLineCount();
                currentFileDirt      = f.Dirt();
                currentFileName      = f.Name();

                CBLT::DrawInfo(c, currentFileLineCount, currentFileDirt, currentFileName, ctrl.CWD());
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