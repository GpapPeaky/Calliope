#include "CBLT_TopBar.hpp"

void CBLT::UI::Draw(UT::ui32 col, UT::ui32 line, UT::ui32 lineCount, UT::b dirty, std::string fname, std::string cwd, UT::i32 mode) {
    const UT::ui32 topBarFontSize = 25;
    const UT::ui32 topBarSeperatorY = 17;
    const UT::ui32 topBarInfoVerticalShift = 12;
    const UT::ui32 topBarInfoHorizontalShift = 4;
    const UT::ui32 topBarSecondColumnX = 150;
    const UT::ui32 topBarCWDFilePathSeperatorX = 300;
    const UT::ui32 topBarThirdColumnX = 300;
    const UT::ui32 modeMargin = 200;
    
    // String to notify the user if the file is dirty (modified/unsaved) or clean (saved)    
    std::string dirtyFile;
    Color dirtyColour;
    if (dirty) {
        dirtyFile = std::string("dirty");
        dirtyColour = {255, 128, 0, 255};
    } else {
        dirtyFile = std::string("clean");
        dirtyColour = {0, 255, 128, 255};
    }
    
    // Seperators

    // Draw top bar seperator
    DrawLine(
        0,
        CBLT::UI::TOP_BAR_HEIGHT + topBarSeperatorY,
        static_cast<UT::f32>(GetScreenWidth()),
        CBLT::UI::TOP_BAR_HEIGHT + topBarSeperatorY, 
        Color{0, 255, 0, 255}
    );

    // Draw vertical seperator for cwd and filepath
    DrawLine(
        topBarCWDFilePathSeperatorX,
        0,
        topBarCWDFilePathSeperatorX,
        CBLT::UI::TOP_BAR_HEIGHT + topBarSeperatorY,
        Color{0, 255, 0, 255}
    );

    // Draw cursor column and line data
    DrawTextEx(
        gFont.f,
        (std::string("c: ") + std::to_string(col)).c_str(),
        {topBarInfoHorizontalShift, 0},
        topBarFontSize,
        0.0f,
        Color{0, 255, 255, 255}
    );
    
    DrawTextEx(
        gFont.f,
        (std::string("l: ") + std::to_string(line)).c_str(),
        {topBarInfoHorizontalShift, static_cast<UT::f32>(topBarFontSize) + topBarInfoVerticalShift},
        topBarFontSize,
        0.0f,
        Color{0, 255, 255, 255}
    );
    
    DrawTextEx(
        gFont.f,
        (std::string("lc: ") + std::to_string(lineCount)).c_str(),
        {topBarInfoHorizontalShift + topBarSecondColumnX, 0},
        topBarFontSize,
        0.0f,
        Color{0, 255, 255, 255}
    );

    DrawTextEx(
        gFont.f,
        (std::string("d: ") + dirtyFile).c_str(),
        {topBarInfoHorizontalShift + topBarSecondColumnX, static_cast<UT::f32>(topBarFontSize) + topBarInfoVerticalShift},
        topBarFontSize,
        0.0f,
        dirtyColour
    );

    // Draw current mode
    DrawTextEx(
        gFont.f,
        std::to_string(mode).c_str(),
        {topBarInfoHorizontalShift + topBarThirdColumnX + modeMargin, 0},
        gFont.size,
        0.0f,
        Color{255, 64, 64, 255}
    );

    DrawTextEx(
        gFont.f,
        fname.c_str(),
        {topBarInfoHorizontalShift + topBarThirdColumnX, 0},
        topBarFontSize,
        0.0f,
        Color{255, 0, 255, 255}
    );

    DrawTextEx(
        gFont.f,
        cwd.c_str(),
        {topBarInfoHorizontalShift + topBarThirdColumnX, static_cast<UT::f32>(topBarFontSize) + topBarInfoVerticalShift},
        topBarFontSize,
        0.0f,
        Color{255, 0, 255, 255}
    );
}
