#include "CBLT_TopBar.hpp"

void CBLT::DrawInfo(CBLT::Cursor& c, UT::ui32 lineCount, UT::b dirty, std::string fname, std::string cwd, std::string conf) {
    const UT::ui32 topBarFontSize = 25;
    const UT::ui32 topBarSeperatorY = (UT::ui32)CBLT::FileMargins::UI::TOP_BAR_SEPERATOR_Y;
    const UT::ui32 topBarInfoVerticalShift = 12;
    const UT::ui32 topBarInfoHorizontalShift = 4;
    const UT::ui32 topBarSecondColumnX = 100;
    const UT::ui32 topBarCWDFilePathSeperatorX = 210;
    const UT::ui32 topBarThirdColumnX = 220;

    const std::string fnameAndConf = fname + " " + conf;
    // We also render the file's lang .conf name
    const UT::ui32 filenameLen = MeasureTextEx(gTopBarFont.f, fnameAndConf.c_str(), topBarFontSize, 0.0f).x;
    // const UT::ui32 CWDLen = MeasureTextEx(gTopBarFont.f, cwd.c_str(), topBarFontSize, 0.0f).x;

    const UT::ui32 filenameToModeMargin = 10;
    const UT::ui32 modePosition = filenameLen + filenameToModeMargin; 

    const UT::i32 mode = (UT::i32)c.GetMode();
    const UT::ui32 col = c.Col();
    const UT::ui32 line = c.Line();
    const std::string cursorFragment = c.Fragment();
    const UT::ui32 fragmentMarginFromMode = 5;

    // String to notify the user if the file is dirty (modified/unsaved) or clean (saved)    
    std::string dirtyFile;
    Color dirtyColour;
    if (dirty) {
        dirtyFile = std::string("dirty");
        dirtyColour = gPalette.dirty;
    } else {
        dirtyFile = std::string("clean");
        dirtyColour = gPalette.clean;
    }

    std::string modeString;
    if (mode == 0) {
        modeString = std::string("INSERT");
    } else if (mode == 1) {
        modeString = std::string("SELECT");
    }

    const UT::ui32 modeAndFilenameWidth = modePosition + MeasureTextEx(gTopBarFont.f, modeString.c_str(), topBarFontSize, 0.0f).x;
    
    // Seperators

    // Draw top bar seperator
    DrawLine(
        0,
        CBLT::FileMargins::UI::TOP_BAR_HEIGHT + topBarSeperatorY,
        static_cast<UT::f32>(GetScreenWidth()),
        CBLT::FileMargins::UI::TOP_BAR_HEIGHT + topBarSeperatorY, 
        gPalette.textSeperators
    );

    // Draw vertical seperator for cwd and filepath
    DrawLine(
        topBarCWDFilePathSeperatorX,
        0,
        topBarCWDFilePathSeperatorX,
        CBLT::FileMargins::UI::TOP_BAR_HEIGHT + topBarSeperatorY,
        gPalette.textSeperators
    );

    // Draw cursor column and line data
    DrawTextEx(
        gTopBarFont.f,
        (std::string("c: ") + std::to_string(col)).c_str(),
        {topBarInfoHorizontalShift, 0},
        topBarFontSize,
        0.0f,
        gPalette.lineInfo
    );
    
    DrawTextEx(
        gTopBarFont.f,
        (std::string("l: ") + std::to_string(line)).c_str(),
        {topBarInfoHorizontalShift, static_cast<UT::f32>(topBarFontSize) + topBarInfoVerticalShift},
        topBarFontSize,
        0.0f,
        gPalette.lineInfo
    );
    
    DrawTextEx(
        gTopBarFont.f,
        (std::string("lc: ") + std::to_string(lineCount)).c_str(),
        {topBarInfoHorizontalShift + topBarSecondColumnX, 0},
        topBarFontSize,
        0.0f,
        gPalette.lineInfo
    );

    DrawTextEx(
        gTopBarFont.f,
        (std::string("d: ") + dirtyFile).c_str(),
        {topBarInfoHorizontalShift + topBarSecondColumnX, static_cast<UT::f32>(topBarFontSize) + topBarInfoVerticalShift},
        topBarFontSize,
        0.0f,
        dirtyColour
    );

    // Draw current mode
    DrawTextEx(
        gTopBarFont.f,
        modeString.c_str(),
        {(UT::f32)(topBarInfoHorizontalShift + topBarThirdColumnX + modePosition), 0.0f},
        topBarFontSize,
        0.0f,
        gPalette.cursorMode
    );

    // Current file
    DrawTextEx(
        gTopBarFont.f,
        (fnameAndConf).c_str(),
        {(UT::f32)(topBarInfoHorizontalShift + topBarThirdColumnX), 0},
        topBarFontSize,
        0.0f,
        gPalette.file
    );

    // Cursor fragment
    DrawTextEx(
        gTopBarFont.f,
        cursorFragment.c_str(),
        {(UT::f32)(topBarInfoHorizontalShift + topBarThirdColumnX + modeAndFilenameWidth + fragmentMarginFromMode), 0},
        topBarFontSize,
        0.0f,
        gPalette.frag
    );

    // CWD
    DrawTextEx(
        gTopBarFont.f,
        cwd.c_str(),
        {topBarInfoHorizontalShift + topBarThirdColumnX, static_cast<UT::f32>(topBarFontSize) + topBarInfoVerticalShift},
        topBarFontSize,
        0.0f,
        gPalette.cwd
    );
}
