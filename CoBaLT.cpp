#include "CBLT_Include.hpp"

// FIXME: Add a font loading fail fallback

// TODO: Add a TODO, FIXME, todo, fixme, bug, BUG word highlighting in comments
// TODO: Add function call highlighting.

// TODO: Add the animator to the console cursor as well
// TODO: Add animators to the filequeue and console resizing and contents movement

// FIXME: Multi-cursor indentation is problematic
// FIXME: Multi-cursor delete at the end of the file, crashes | deletes too many lines

UT::i32 main(int argc, char** argv) {
    CBLT::Win::Init();
    CBLT::Utils::Err::Init();

    std::string resourceDir = CBLT::Sys::ResourcePath();

    ////// Fonts //////
    CBLT::LoadFonts(resourceDir);

    // CBLT::gFont.Load(resourceDir + "/assets/font/DG_One.ttf");
    // CBLT::gFont.size = 17;
    // CBLT::gFont.Config();

    // CBLT::gConsoleFont.Load(resourceDir + "/assets/font/DG_One.ttf");
    // CBLT::gConsoleFont.size = 15;
    // CBLT::gConsoleFont.Config();

    // CBLT::gTopBarFont.Load(resourceDir + "/assets/font/DG_One.ttf");
    // CBLT::gTopBarFont.size = 15;
    // CBLT::gTopBarFont.Config();

    // CBLT::gFileQueueFont.Load(resourceDir + "/assets/font/DG_One.ttf");
    // CBLT::gFileQueueFont.size = 15;
    // CBLT::gFileQueueFont.Config();
    ////// //////

    CBLT::gSound.Load(resourceDir + "/assets/audio/");

    CBLT::gSettings.ReadSettings();
    
    // Controller
    CBLT::Controller ctrl;
    ctrl.InitCWD(CBLT::Sys::WorkingDirectory(argc, argv));
    ctrl.GetKeyboard().AssignTabSize(&CBLT::gSettings.OPTION_TabSize);
    ctrl.GetConsole().GetCWDContents(ctrl.CWD());

    CBLT::gPalette.ReadPaletteFile(CBLT::gSettings.OPTION_Palette);

    // TopBar info, update each frame
    UT::ui32 currentFileLineCount; 
    UT::b currentFileDirt; 
    std::string currentFileName;
    std::string currentFileLangConfName;

    // File cursors
    const UT::ui32 baseX = CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
                    CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES;

    UT::ui32 framesCount = 0;
    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(CBLT::gPalette.background);

            ctrl.Update();

            // Cursor & console references
            CBLT::Console& cnsl = ctrl.GetConsole();
            CBLT::FileQueue& fq = ctrl.LoadedFileQueue();

            // Draw file
            if (fq.Size() > 0) {
                CBLT::File& f = fq.Active();
                CBLT::CursorManager& cm = ctrl.GetActiveCursorManager();
                CBLT::Cursor& c = cm.Primary();

                // Need to retokenize every frame, else non updated tokens with invalid lengths will crash the program
                f.RetokenizeDirtyLines();
                c.AcquireFragment(c.Col(), f.GetCurrentLine(c.Line()));

                cm.DrawCursors(CBLT::gFont, baseX, 0, f.Offs());
                f.Draw(c.renderX, c.renderY, cnsl.IsOpen(), cnsl.Width());
                
                currentFileLineCount        = f.GetLineCount();
                currentFileDirt             = f.Dirt();
                currentFileName             = f.Name();
                currentFileLangConfName     = f.LangConf();

                ctrl.GetConsole().GetCWDContents(ctrl.CWD());
                f.Cam().SetHeight(GetScreenHeight());
                f.Cam().SetWidth(GetScreenWidth());
                cnsl.Cam().SetHeight(GetScreenHeight() - CBLT::gConsoleFont.size - 10);

                CBLT::DrawInfo(c, currentFileLineCount, currentFileDirt, currentFileName, ctrl.CWD(), currentFileLangConfName);
            }

            // Draw guide BEFORE the console, to not overlap with it
            if (fq.Size() == 0) cnsl.DrawGuide();

            if (cnsl.IsOpen()) {
                ctrl.HandleConsoleMouseWheel();
                Rectangle rect = ctrl.HandleConsoleMouseClick();
                cnsl.Draw(fq, rect);
            }

            // Draw file queue
            if (fq.Size() > 0) fq.Draw();

            cnsl.DrawMessage();
        EndDrawing();

        framesCount++;
    }

    CBLT::Win::Destroy();
    
    return UDef::GRACEFUL_EXIT;
}