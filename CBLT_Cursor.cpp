#include "CBLT_Cursor.hpp"

namespace CBLT {
    Cursor::Cursor(UT::ui32 col, UT::ui32 ln) :
        column(col),
        line(ln),
        startSelectColumn(col),
        finalSelectColumn(col),
        startSelectLine(ln),
        finalSelectLine(ln),
        m(CursorMode::INSERT),                              // Default
        fragment(""),
        charWidth(MeasureText("A", CBLT::gFont.size)),      // Measure once
        cursorSymbol(CursorSymbol::NON_ASCII_HOLLOW_BOX)           // Default
    {}

   Cursor::~Cursor(void) {}

    std::string Cursor::Fragment(void) const {
        return fragment;
    }

    void Cursor::AcquireFragment(UT::ui32 c, std::string& line) { // FIXME: Problematic
        std::string frag = "";
        
        // Get the leading part
        for (UT::llui32 i = c ; i < line.size() ; i++) {
            if (!isalpha(line.at(i))) {
                break;
            }

            frag.push_back(line.at(i));
        }

        for (UT::ui32 i = c - 1 ; i == 0 ; i--) {
            if (!isalpha(line.at(i))) {
                break;
            }

            frag.insert(i, 1, line.at(i));
        }
        
        fragment = frag;
    }

    UT::ui32 Cursor::Col(void) const {
        return this->column;
    }

    UT::ui32 Cursor::Line(void) const {
        return this->line;
    }

    void Cursor::SetAt(UT::ui32 col, UT::ui32 line) {
        this->column = col;
        this->line = line;
    }

    CursorMode Cursor::GetMode(void) const {
        return this->m;
    }

    void Cursor::SetMode(CursorMode m) {
        this->m = m;
    }

    void Cursor::Up(void) {
        if(this->line > 0) {
            this->line -= 1;
        }
    }

    void Cursor::Down(void) {
        this->line += 1;
    }

    void Cursor::Left(void) {
        if(this->column > 0) {
            this->column -= 1;
        }
    }

    void Cursor::Right(void) {
        this->column += 1;
    }

    void Cursor::Draw(const std::string& lineText) {
        UT::i32 x = GetCursorX(lineText, gFont.size);
        UT::i32 y = line * gFont.size;

        // Apply camera offsets
        x += CBLT::gOffsets.x;
        y += CBLT::gOffsets.y;

        // Draw a transparent rectangle, to show where the cursor is
        DrawRectangle(
            0,
            y + gFont.size,
            GetScreenWidth(),
            gFont.size,
            gPalette.cursorPosHighlight
        );

        const UT::i32 horizontalFix = 2;

        // Hash on the symbol
        switch (cursorSymbol) {
            case CursorSymbol::NON_ASCII_BOX:
                DrawRectangle(
                    x + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
                    CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
                    y + gFont.size,
                    charWidth,
                    gFont.size,
                    gPalette.cursor
                );

                return;
            case CursorSymbol::NON_ASCII_HOLLOW_BOX:
                // Outter
                DrawRectangleLines(
                    x + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
                    CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
                    y + gFont.size,
                    charWidth,
                    gFont.size,
                    gPalette.cursor
                );
                
                return;
            case CursorSymbol::NON_ASCII_LINE:
                DrawRectangle(
                    x + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
                    CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES - horizontalFix,
                    y + gFont.size,
                    1,
                    gFont.size,
                    gPalette.cursor
                );

                return;
            case CursorSymbol::NON_ASCII_UNDERSCORE:
                DrawRectangle(
                    x + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
                    CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
                    y + 2 * gFont.size,
                    charWidth,
                    1,
                    gPalette.cursor
                );

                return;
        }
    }

    void Cursor::StartSelection(void) {
        startSelectColumn = column;
        startSelectLine = line;

        m = CursorMode::SELECT;
    }

    void Cursor::StopSelection(void) {
        finalSelectColumn = column;
        finalSelectLine = line;

        m = CursorMode::INSERT;
    }

    UT::ui32 Cursor::SFCol() const {
        return finalSelectColumn;
    }

    UT::ui32 Cursor::SFLine() const {
        return finalSelectLine;
    }

    UT::ui32 Cursor::SSCol() const {
        return startSelectColumn;
    }

    UT::ui32 Cursor::SSLine() const {
        return startSelectLine;
    }

    UT::ui32 Cursor::GetCursorX(const std::string& lineText, UT::ui32 fontSize){
        UT::f32 scale = (UT::f32)fontSize / gFont.f.baseSize;
        UT::ui32 width = 0;
        auto cps = CBLT::gFont.Utf8ToCodepoints(lineText);

        for(size_t i = 0; i < column && i < cps.size(); i++) {
            UT::i32 cp = cps[i];
            UT::i32 glyphIndex = -1;

            for(UT::i32 g = 0; g < gFont.f.glyphCount; g++) {
                if(gFont.f.glyphs[g].value == cp){
                    glyphIndex = g;
                    break;
                }
            }

            if (glyphIndex >= 0) {
                width += gFont.f.glyphs[glyphIndex].advanceX;
            } else {
                width += fontSize / 2;
            }
        }
        
        return (UT::ui32)(width * scale);
    }

    CharClass Cursor::Classify(UT::cui8 c) const {
        if (c == ' ' || c == '\n') {
            return CharClass::WHITESPACE;
        }

        if (std::isalnum(c) || c == '_') {
            return CharClass::INWORD;
        }

        return CharClass::SYMBOL;
    }
    
    void Cursor::SetToWordBoundary(const std::string& lineText, CursorDirection dir, UT::ui32 lineCount) {
        UT::ui32 col  = static_cast<UT::ui32>(Col());
        UT::ui32 line = static_cast<UT::ui32>(Line());
        UT::ui32 len  = static_cast<UT::ui32>(lineText.size());
    
        if (dir == CursorDirection::RIGHT) {
            if (col >= len) {
                if (line == lineCount - 1) {
                    SetAt(len, line);
                } else {
                    SetAt(len, line + 1);
                }

                return;
            }

            while (col < len && Classify(lineText[col]) == CharClass::WHITESPACE) col++;

            if (col >= len) {
                SetAt(len, line);
                return;
            }

            CharClass cc = Classify(lineText[col]);

            // Skip current class
            while (col < len && cc == Classify(lineText[col])) col++;

            SetAt(col, line);
            return;
        }
    
        if (dir == CursorDirection::LEFT) {
            if (col <= 0 && line > 0) {
                SetAt(0, line - 1);
                return;
            }
    
            int i = col - 1;
    
            while (i >= 0 && Classify(lineText[i]) == CharClass::WHITESPACE) i--;
    
            if (i < 0) {
                SetAt(0, line);
                return;
            }
    
            CharClass cc = Classify(lineText[i]);
            while (i >= 0 && Classify(lineText[i]) == cc) i--;
    
            SetAt(i + 1, line);

            const UT::i32 newLine = static_cast<UT::i32>(Line());

            if (newLine < 0) { // Safety check
                SetAt(0, 0);
                return;
            }
            
            return;
        }
    }

    void Cursor::ClampToCamera(Camera& cam, std::string currentLine) {
        const UT::i32 camTop = cam.Origin().y + cam.MarginY();
        const UT::i32 camBottom = cam.Origin().y + cam.Height() - cam.MarginY();
        const UT::i32 camLeft = cam.Origin().x + cam.MarginX();
        const UT::i32 camRight = cam.Origin().x + cam.Width() - cam.MarginX();
        
        const UT::f32 lineHeight = gFont.size;
        
        const UT::i32 textBaseX = CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI + 
                                CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + 
                                CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES
                                + 31.0f;

        const UT::i32 textBaseY = CBLT::FileMargins::UI::TOP_BAR_HEIGHT;
        
        UT::i32 cursorWorldX = textBaseX + GetCursorX(currentLine, gFont.size);
        UT::i32 cursorWorldY = textBaseY + line * lineHeight + lineHeight;
        
        UT::i32 cursorScreenX = cursorWorldX + CBLT::gOffsets.x;
        UT::i32 cursorScreenY = cursorWorldY + CBLT::gOffsets.y;
        
        const UT::i32 charWidth = static_cast<UT::i32>(gFont.size);
        const UT::i32 charHeight = static_cast<UT::i32>(gFont.size);
        
        if (cursorScreenY < camTop) {
            CBLT::gOffsets.y += camTop - cursorScreenY;
        }
        else if (cursorScreenY + charHeight > camBottom) {
            CBLT::gOffsets.y -= (cursorScreenY + charHeight) - camBottom;
        }
        
        if (cursorScreenX < camLeft) {
            CBLT::gOffsets.x += camLeft - cursorScreenX;
        }
        else if (cursorScreenX + charWidth > camRight) {
            CBLT::gOffsets.x -= (cursorScreenX + charWidth) - camRight;
        }
    }

    CursorManager::CursorManager() {
        activeCursors.emplace_back(0, 0); // Initialize one cursor at 0,1
        
        // Requests
        requestLead = 0;
        requestTrail = 0;
        requestReset = false;
    }

    CursorManager::~CursorManager() {}

    void CursorManager::AddCursorAt(UT::ui32 col, UT::ui32 line) {
        this->activeCursors.emplace_back(col, line); // Constructs directly inside the vector
    }

    void CursorManager::RemoveCursorAt(UT::ui32 col, UT::ui32 line) {
        for(size_t i = 0 ; i < activeCursors.size() ; i++) {
            if(activeCursors.at(i).Col() == col && activeCursors.at(i).Line() == line) {
                activeCursors.erase(activeCursors.begin() + i); // Remove the matching cursor 
            }
        }
    }

    void CursorManager::RemoveSecondaries(void) {
        if (activeCursors.size() > 1) {
            activeCursors.erase(
                activeCursors.begin() + 1,
                activeCursors.end()
            );
        }
    }

    void CursorManager::DrawCursors(std::vector<std::string>& lines) {
        for (UT::llui32 i = 0 ; i < activeCursors.size() ; i++) {
            const std::string& lineText = lines.at(activeCursors[i].Line());
            activeCursors[i].Draw(lineText);
        }
    }

    void CursorManager::RequestReset(void) {
        requestReset = true;
    }

    void CursorManager::RequestTrail(void) {
        requestTrail = true;
    }
            
    void CursorManager::RequestLead(void) {
        requestLead = true;
    }

    void CursorManager::HandlePendingRequests(UT::ui32 lineCount) {
        // Reset
        if (requestReset) {
            RemoveSecondaries();
            requestReset = false;
        }
    
        // Add cursors down (lead)
        if (requestLead) {
            Cursor base = activeCursors.at(0);
            for (auto& c : activeCursors)
                if (c.Line() > base.Line())
                    base = c;
        
            if (base.Line() + 1 < lineCount)
                AddCursorAt(base.Col(), base.Line() + 1);
        
            requestLead = false;
        }
    
        // Add cursors up (trail)
        if (requestTrail) {
            Cursor base = activeCursors.at(0);
            for (auto& c : activeCursors) {
                if (c.Line() < base.Line())
                    base = c;
            }
    
            if (base.Line() > 0) {
                AddCursorAt(base.Col(), base.Line() - 1);
            }

            requestTrail = false;
        }
    }

    Cursor& CursorManager::Primary() {
        return activeCursors.at(0);
    }

    const Cursor CursorManager::Primary() const {
        return activeCursors.at(0);
    }
} // CBLT