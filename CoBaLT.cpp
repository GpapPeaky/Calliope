#include "includes.hpp"

// TODO: Add file tokenization
// TODO: Add camera occlusion
// TODO: Add tokenization drawing

UT::i32 main() {
    CBLT::Win::Init();

    CBLT::gFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gFont.size = 23;

    // CBLT::Controller ctrl; // Automatically calls the constructor
    // CBLT::Controller ctrl(fields...); // Automatically calls the constructor
    CBLT::Controller ctrl; // Program controller
    ctrl.GetFile().Load(".tests/new.c");

    while(!WindowShouldClose()) {
        BeginDrawing();

            ClearBackground(BLACK);

            ctrl.Update();

            // No heap alloc, no copies, no runtime cost
            CBLT::CursorManager& cm = ctrl.GetCursorManager();
            CBLT::Cursor& c = cm.Primary();
            CBLT::Console& cnsl = ctrl.GetConsole();
            CBLT::File& f = ctrl.GetFile();
            CBLT::Camera& cam = ctrl.GetCamera();

            // Draw open file
            cm.DrawCursors(f);
            f.Draw(cam);
            ctrl.DrawSelection(c);
            CBLT::UI::Draw(c.Col(), c.Line(), f.GetLineCount(), f.Dirt(), f.Name(), f.CWD(), (UT::i32)c.GetMode());
            
            if (cnsl.IsOpen()) {
                cnsl.Draw(f.Name());
            }
            
            cnsl.DrawMessage();

            cam.Draw();

            DrawFPS(950, 0);
        EndDrawing();
    }

    CBLT::Win::Destroy();

    return 0;
}
