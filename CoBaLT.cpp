#include "CoBaLT_INCLUDES.hpp"

// TODO: Add file tokenization
// TODO: Add tokenization drawing

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

            DrawRectangleLines(
                0,
                0,
                CBLT::gOffsets.x,
                CBLT::gOffsets.y,
                WHITE
            );

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

            // DrawFPS(950, 0);
        EndDrawing();
    }

    CBLT::Win::Destroy();

    return 0;
}
