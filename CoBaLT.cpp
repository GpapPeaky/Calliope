// 1. Autocomplete with TAB
// 2. Add click at infile
// 3. Split, font size++/--
// 4. Fullscreen/resizable window
// 5. Selection system
// 6. Ctrl G -> Goto line
// 7. Ctrl F -> Find symbol

#include "CoBaLT_INCLUDES.hpp"

UT::i32 main() {
    CBLT::Win::Init();

    CBLT::gFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gFont.size = 23;

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
                cm.DrawCursors(f);
                f.Draw(cam);
                currentFileLineCount = f.GetLineCount();
                currentFileDirt      = f.Dirt();
                currentFileName      = f.Name();
            } else { // Safety
                currentFileLineCount = 0;
                currentFileDirt      = false;
                currentFileName      = "";
            }

            ctrl.DrawSelection(c);
            CBLT::UI::Draw(c.Col(), c.Line(), currentFileLineCount, currentFileDirt, currentFileName, ctrl.CWD(), (UT::i32)c.GetMode());
            
            if (cnsl.IsOpen()) {
                cnsl.Draw();
            }

            if (fq.Size() > 0) {
                fq.Draw();
            }

            cnsl.DrawMessage();

            // cam.Draw();

            DrawFPS(950, 0);
        EndDrawing();

        if (framesCount % 120 == 0) { // Every 120 frames, update the cwd contents
            ctrl.GetConsole().GetCWDContents(ctrl.CWD());
        }

        framesCount++;
    }

    CBLT::Win::Destroy();

    return 0;
}
