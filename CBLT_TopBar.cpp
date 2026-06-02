#include "CBLT_TopBar.hpp"

void CBLT::DrawInfo(CBLT::Cursor& c, UT::ui32 lineCount, UT::b dirty, std::string fname, std::string cwd, std::string conf) {
    const UT::ui32 topBarSeperatorY = (UT::ui32)CBLT::FileMargins::UI::TOP_BAR_SEPERATOR_Y;

    const UT::ui32 topBarInfoVerticalShift = 12;
    const UT::ui32 topBarInfoHorizontalShift = 4;

    const UT::ui32 columnGap = 20;
    const UT::ui32 sectionGap = 40;
    const UT::ui32 filenameToModeMargin = 10;
    const UT::ui32 fragmentMarginFromMode = 10;

    // Lambda to measure text width for the top bar font
    auto TextWidth = [&](const std::string& str) -> UT::ui32 {
        return static_cast<UT::ui32>(
            MeasureTextEx(
                gTopBarFont.f,
                str.c_str(),
                static_cast<float>(gTopBarFont.size),
                0.0f
            ).x
        );
    };

    const UT::i32 mode = static_cast<UT::i32>(c.GetMode());

    const UT::ui32 col = c.Col();
    const UT::ui32 line = c.Line();

    const std::string cursorFragment = c.Fragment();

    // Calculate after deciding font-relative strings and measurements
    CBLT::FileMargins::UI::TOP_BAR_HEIGHT = topBarSeperatorY + topBarInfoVerticalShift;

    // Dirty state

    std::string dirtyFile;
    Color dirtyColour;

    if (dirty) {
        dirtyFile = "dirty";
        dirtyColour = gPalette.dirty;
    } else {
        dirtyFile = "clean";
        dirtyColour = gPalette.clean;
    }

    // Mode string

    std::string modeString;

    if (mode == 0) {
        modeString = "INSERT";
    } else if (mode == 1) {
        modeString = "SELECT";
    } else {
        modeString = "UNKNOWN";
    }

    // Strings

    const std::string colStr =
        "c: " + std::to_string(col);

    const std::string lineStr =
        "l: " + std::to_string(line);

    const std::string lcStr =
        "lc: " + std::to_string(lineCount);

    const std::string dirtyStr =
        "d: " + dirtyFile;

    const std::string fnameAndConf =
        fname + " " + conf;

    // Measurements

    const UT::ui32 colWidth   = TextWidth(colStr);
    const UT::ui32 lineWidth  = TextWidth(lineStr);

    const UT::ui32 lcWidth    = TextWidth(lcStr);
    const UT::ui32 dirtyWidth = TextWidth(dirtyStr);

    const UT::ui32 fileWidth =
        TextWidth(fnameAndConf);

    const UT::ui32 modeWidth =
        TextWidth(modeString);

    const UT::ui32 fragmentWidth =
        TextWidth(cursorFragment);

    // Column widths

    const UT::ui32 firstColumnWidth =
        std::max(colWidth, lineWidth);

    const UT::ui32 secondColumnWidth =
        std::max(lcWidth, dirtyWidth);

    // Dynamic positions

    const UT::ui32 secondColumnX =
        topBarInfoHorizontalShift +
        firstColumnWidth +
        columnGap;

    const UT::ui32 fileSectionX =
        secondColumnX +
        secondColumnWidth +
        sectionGap;

    const UT::ui32 modeX =
        fileSectionX +
        fileWidth +
        filenameToModeMargin;

    const UT::ui32 fragmentX =
        modeX +
        modeWidth +
        fragmentMarginFromMode;

    // Vertical separator after left information columns

    const UT::ui32 cwdSeparatorX =
        fileSectionX - (sectionGap / 2);

    // Separators

    DrawLine(
        0,
        CBLT::FileMargins::UI::TOP_BAR_HEIGHT + topBarSeperatorY,
        static_cast<float>(GetScreenWidth()),
        CBLT::FileMargins::UI::TOP_BAR_HEIGHT + topBarSeperatorY,
        gPalette.textSeperators
    );

    DrawLine(
        cwdSeparatorX,
        0,
        cwdSeparatorX,
        CBLT::FileMargins::UI::TOP_BAR_HEIGHT + topBarSeperatorY,
        gPalette.textSeperators
    );

    // Left information columns

    DrawTextEx(
        gTopBarFont.f,
        colStr.c_str(),
        {
            static_cast<float>(topBarInfoHorizontalShift),
            0.0f
        },
        gTopBarFont.size,
        0.0f,
        gPalette.lineInfo
    );

    DrawTextEx(
        gTopBarFont.f,
        lineStr.c_str(),
        {
            static_cast<float>(topBarInfoHorizontalShift),
            static_cast<float>(gTopBarFont.size + topBarInfoVerticalShift)
        },
        gTopBarFont.size,
        0.0f,
        gPalette.lineInfo
    );

    DrawTextEx(
        gTopBarFont.f,
        lcStr.c_str(),
        {
            static_cast<float>(secondColumnX),
            0.0f
        },
        gTopBarFont.size,
        0.0f,
        gPalette.lineInfo
    );

    DrawTextEx(
        gTopBarFont.f,
        dirtyStr.c_str(),
        {
            static_cast<float>(secondColumnX),
            static_cast<float>(gTopBarFont.size + topBarInfoVerticalShift)
        },
        gTopBarFont.size,
        0.0f,
        dirtyColour
    );

    // File information

    DrawTextEx(
        gTopBarFont.f,
        fnameAndConf.c_str(),
        {
            static_cast<float>(fileSectionX),
            0.0f
        },
        gTopBarFont.size,
        0.0f,
        gPalette.file
    );

    DrawTextEx(
        gTopBarFont.f,
        modeString.c_str(),
        {
            static_cast<float>(modeX),
            0.0f
        },
        gTopBarFont.size,
        0.0f,
        gPalette.cursorMode
    );

    DrawTextEx(
        gTopBarFont.f,
        cursorFragment.c_str(),
        {
            static_cast<float>(fragmentX),
            0.0f
        },
        gTopBarFont.size,
        0.0f,
        gPalette.frag
    );

    DrawTextEx(
        gTopBarFont.f,
        cwd.c_str(),
        {
            static_cast<float>(fileSectionX),
            static_cast<float>(gTopBarFont.size + topBarInfoVerticalShift)
        },
        gTopBarFont.size,
        0.0f,
        gPalette.cwd
    );
}