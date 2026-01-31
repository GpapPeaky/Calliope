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

    ///////////////
    // IMPORTANT //
    ///////////////

        // Thes functions are invoked whenever loading a new file onto the loaded file queue
        CBLT::File file;

        file.Load(".tests/editor.c");
        ctrl.LoadedFileQueue().LoadFileToQueue(file);
        
        file.Load(".tests/5k_lines.c");
        ctrl.LoadedFileQueue().LoadFileToQueue(file);
        
        file.Load(".tests/10k_lines.c");
        ctrl.LoadedFileQueue().LoadFileToQueue(file);
        
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
            CBLT::FileQueue& fq = ctrl.LoadedFileQueue();
            CBLT::File& f = fq.Active();
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
                cnsl.Draw();
            }

            fq.Draw();
            
            cnsl.DrawMessage();

            // cam.Draw();

            DrawFPS(950, 0);
        EndDrawing();
    }

    CBLT::Win::Destroy();

    return 0;
}
