// 7. Ctrl F -> Find symbol

#include "CoBaLT_INCLUDES.hpp"

UT::i32 main() {
    CBLT::Win::Init();

    CBLT::InitNAF();

    CBLT::gFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gFont.Config();
    CBLT::gFont.size = 23;

    CBLT::gConsoleFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gConsoleFont.Config();
    CBLT::gConsoleFont.size = 20;

    CBLT::gSound.Load();

    // CBLT::Controller ctrl; // Automatically calls the constructor
    // CBLT::Controller ctrl(fields...); // Automatically calls the constructor
    CBLT::Controller ctrl; // Program controller

    ctrl.InitCWD("C:/"); // Called only once so we do not crash
    ctrl.GetConsole().GetCWDContents(ctrl.CWD());

    UT::ui32 currentFileLineCount; 
    UT::b currentFileDirt; 
    std::string currentFileName;

    UT::ui32 framesCount = 0;
    while(!WindowShouldClose()) {
        BeginDrawing();

            ClearBackground(CBLT::gPalette.background);

            ctrl.Update();

            // No heap alloc, no copies, no runtime cost
            CBLT::CursorManager& cm = ctrl.GetCursorManager();
            CBLT::Cursor& c = cm.Primary();
            CBLT::Console& cnsl = ctrl.GetConsole();
            CBLT::FileQueue& fq = ctrl.LoadedFileQueue();
            CBLT::File& f = fq.Active();
            CBLT::Camera& cam = ctrl.GetCamera();

            // Draw open file
            if (fq.Size() > 0) {
                if (framesCount % 60) { // Tokenize once per 60 frames
                    f.RetokenizeDirtyLines(); // Will tokenize only dirty lines
                }
                cm.DrawCursors(f.GetLines());
                f.Draw(cam);
                currentFileLineCount = f.GetLineCount();
                currentFileDirt      = f.Dirt();
                currentFileName      = f.Name();
            } else { // Safety
                currentFileLineCount = 0;
                currentFileDirt      = false;
                currentFileName      = "";
            }

            CBLT::DrawInfo(c, currentFileLineCount, currentFileDirt, currentFileName, ctrl.CWD());
            
            if (cnsl.IsOpen()) {
                cnsl.Draw(fq);
            }

            if (fq.Size() > 0) {
                fq.Draw();
            }

            cnsl.DrawMessage();

            // cam.Draw();

            // DrawFPS(950, 0);
        EndDrawing();

        if (framesCount % 120 == 0) { // Every 120 frames, update the cwd contents and check camera size
            ctrl.GetConsole().GetCWDContents(ctrl.CWD());
            cam.SetHeight(GetScreenHeight());
        }

        framesCount++;
    }

    CBLT::Win::Destroy();

    return 0;
}
