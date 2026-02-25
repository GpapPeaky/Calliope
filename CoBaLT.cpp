#include "CoBaLT_INCLUDES.hpp"

UT::i32 main() {
    CBLT::Win::Init();

    CBLT::Utils::Err::Init();
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
                if (framesCount % 60) { // Every 60 frames
                    f.RetokenizeDirtyLines(); // Will tokenize only dirty lines
                    c.AcquireFragment(c.Col(), f.GetCurrentLine(c.Line()));
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
            } else { // No open fles, display the guide message
                cnsl.Message().messageType = CBLT::ConsoleMessage::GUIDE;
                cnsl.Message().message =
                    "Co.Ba.L.T Console Help Guide:\n"
                    ":e      - Exit Co.Ba.L.T\n"
                    ":we     - Write and exit Co.Ba.L.T\n"
                    ":w      - Write current file\n"
                    ":wq     - Write and close current file\n"
                    ":wqa    - Write and close all files\n"
                    ":i      - Display file info and metadata\n"
                    ":h      - Display this help guide\n"
                    ":c      - Create a file\n"
                    ":r      - Delete a file\n"
                    ":m      - Create a directory\n"
                    ":d      - Delete a directory\n"
                    ":cd     - Change directory\n"
                    ":o      - Open native folder picker\n"
                    ":g      - Go to line in file\n"
                    ":gs     - Go to start of file\n"
                    ":ge     - Go to end of file\n"
                    ":q      - Close current file\n"
                    ":qa     - Close all files\n"
                    ":qas    - Close all clean files\n";
            }
            
            cnsl.DrawMessage();

            // cam.Draw();

            // DrawFPS(950, 0);
        EndDrawing();

        if (framesCount % 120 == 0) { // Every 120 frames
            ctrl.GetConsole().GetCWDContents(ctrl.CWD());
            cam.SetHeight(GetScreenHeight());
            cnsl.Cam().SetHeight(GetScreenHeight() - CBLT::gConsoleFont.size - 10);
        }

        framesCount++;
    }

    CBLT::Win::Destroy();

    return 0;
}
