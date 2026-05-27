#include "CBLT_Include.hpp"

// TODO: Add the animator to the console cursor as well
// TODO: Add animators to the filequeue and console resizing and contents movement

// FIXME: Multi-cursor indentation is problematic
// FIXME: Multi-cursor delete at the end of the file, crashes | deletes too many lines

UT::i32 main(int argc, char** argv) {
    CBLT::Win::Init();
    CBLT::Utils::Err::Init();

    std::string resourceDir = CBLT::Sys::ResourcePath();

    ////// Fonts //////
    CBLT::gFont.Load(resourceDir + "/assets/font/TerminessNerdFontMono-Regular.ttf");
    CBLT::gFont.size = 23;
    CBLT::gFont.Config();

    CBLT::gConsoleFont.Load(resourceDir + "/assets/font/TerminessNerdFontMono-Regular.ttf");
    CBLT::gConsoleFont.size = 20;
    CBLT::gConsoleFont.Config();

    CBLT::gTopBarFont.Load(resourceDir + "/assets/font/TerminessNerdFontMono-Regular.ttf");
    CBLT::gTopBarFont.size = 23;
    CBLT::gTopBarFont.Config();

    CBLT::gFileQueueFont.Load(resourceDir + "/assets/font/TerminessNerdFontMono-Regular.ttf");
    CBLT::gFileQueueFont.size = 21;
    CBLT::gFileQueueFont.Config();
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

                // File cursors
                UT::ui32 baseX = CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
                    CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES;

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

            if (cnsl.IsOpen()) {
                ctrl.HandleConsoleMouseWheel();
                Rectangle rect = ctrl.HandleConsoleMouseClick();
                cnsl.Draw(fq, rect);
            }

            if (fq.Size() > 0) fq.Draw();
            else cnsl.DrawGuide();

            cnsl.DrawMessage();
        EndDrawing();

        framesCount++;
    }

    CBLT::Win::Destroy();
    
    return UDef::GRACEFUL_EXIT;
}