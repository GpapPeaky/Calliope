#include "CoBaLT_INCLUDES.hpp"

// TODO: Add file tokenization
// TODO: Add tokenization drawing

// FIXME: Performance issues at large files

UT::i32 main() {
    CBLT::Win::Init();

    CBLT::gFont.Load("assets/font/IBMPlexMono-Regular.ttf");
    CBLT::gFont.size = 23;

    // CBLT::Controller ctrl; // Automatically calls the constructor
    // CBLT::Controller ctrl(fields...); // Automatically calls the constructor
    CBLT::Controller ctrl; // Program controller

    ///////////////
    // IMPORTANT //
    ///////////////

        // Thes functions are invoked whenever loading a new file onto the loaded file queue
        CBLT::File f;

        f.Load(".tests/message_passing.c");
        ctrl.LoadedFileQueue().LoadFileToQueue(f);
        
        f.Load(".tests/5k_lines.c");
        ctrl.LoadedFileQueue().LoadFileToQueue(f);
        
        f.Load(".tests/10k_lines.c");
        ctrl.LoadedFileQueue().LoadFileToQueue(f);
        
        f.Load(".tests/new.c");
        ctrl.LoadedFileQueue().LoadFileToQueue(f);

        // These functions should be invoked right after a  valid 'cd' directive is executed.
        // or when calling the system's native file explorer
        ctrl.FindCWD();
        ctrl.GetConsole().GetCWDContents(ctrl.CWD());

    while(!WindowShouldClose()) {
        BeginDrawing();

            ClearBackground(CBLT::gPalette.background);

            ctrl.Update();

            // No heap alloc, no copies, no runtime cost
            CBLT::CursorManager& cm = ctrl.GetCursorManager();
            CBLT::Cursor& c = cm.Primary();
            CBLT::Console& cnsl = ctrl.GetConsole();
            CBLT::File& f = ctrl.GetFile();
            CBLT::Camera& cam = ctrl.GetCamera();

            cm.DrawCursors(f);
            
            // Draw open file
            f.Draw(cam);

            DrawRectangleLines(
                0,
                0,
                CBLT::gOffsets.x,
                CBLT::gOffsets.y,
                WHITE
            );

            ctrl.DrawSelection(c);
            CBLT::UI::Draw(c.Col(), c.Line(), f.GetLineCount(), f.Dirt(), f.Name(), ctrl.CWD(), (UT::i32)c.GetMode());
            
            if (cnsl.IsOpen()) {
                cnsl.Draw(f.Name());
            }
            
            cnsl.DrawMessage();

            // cam.Draw();

            DrawFPS(950, 0);
        EndDrawing();
    }

    CBLT::Win::Destroy();

    return 0;
}
