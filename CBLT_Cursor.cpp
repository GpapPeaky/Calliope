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
        cursorSymbol(CursorSymbol::NON_ASCII_BOX)           // Default
    {}

   Cursor::~Cursor(void) {}

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

    void Cursor::Draw(const std::string& lineText, UT::llui32 cursorId) {
        UT::i32 x = GetCursorX(lineText, gFont.size);
        UT::i32 y = line * gFont.size;

        // Apply camera offsets
        x += CBLT::gOffsets.x;
        y += CBLT::gOffsets.y;

        // Draw a transparent rectangle, to show where the cursor is
        DrawRectangle(
            0,
            y + CBLT::UI::TOP_BAR_HEIGHT + gFont.size,
            GetScreenWidth(),
            gFont.size,
            Color{255, 255, 255, 45}
        );

        const UT::i32 horizontalFix = 2;

        // Hash on the symbol
        switch (cursorSymbol) {
            case CursorSymbol::NON_ASCII_BOX:
                DrawRectangle(
                    x + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
                    CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
                    y + CBLT::UI::TOP_BAR_HEIGHT + gFont.size,
                    charWidth,
                    gFont.size,
                    Color{255, 100, 0, 255}
                );

                return;
            case CursorSymbol::NON_ASCII_HOLLOW_BOX:
                // Outter
                DrawRectangleLines(
                    x + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
                    CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
                    y + CBLT::UI::TOP_BAR_HEIGHT + gFont.size,
                    charWidth,
                    gFont.size,
                    Color{255, 100, 0, 255}
                );
                
                // Inner
                DrawRectangleLines(
                    x + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
                    CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES +
                    1,
                    y + CBLT::UI::TOP_BAR_HEIGHT + gFont.size + 1,
                    charWidth - 2,
                    gFont.size - 2,
                    Color{255, 100, 0, 255}
                );

                return;
            case CursorSymbol::NON_ASCII_LINE:
                DrawRectangle(
                    x + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
                    CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES - horizontalFix,
                    y + CBLT::UI::TOP_BAR_HEIGHT + gFont.size,
                    1,
                    gFont.size,
                    Color{255, 100, 0, 255}
                );

                return;
            case CursorSymbol::NON_ASCII_UNDERSCORE:
                DrawRectangle(
                    x + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
                    CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
                    y + CBLT::UI::TOP_BAR_HEIGHT + 2 * gFont.size,
                    charWidth,
                    1,
                    Color{255, 100, 0, 255}
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

    const CharClass Cursor::Classify(UT::cui8 c) const {
        if (c == ' ' || c == '\n') {
            return CharClass::WHITESPACE;
        }

        if (std::isalnum(c) || c == '_') {
            return CharClass::INWORD;
        }

        return CharClass::SYMBOL;
    }
    
    void Cursor::SetToWordBoundary(const std::string& lineText, CursorDirection dir, File f) {
        UT::ui32 col  = static_cast<UT::ui32>(Col());
        UT::ui32 line = static_cast<UT::ui32>(Line());
        UT::ui32 len  = static_cast<UT::ui32>(lineText.size());
    
        if (dir == CursorDirection::RIGHT) {
            if (col >= len) {
                if (line == f.GetLineCount() - 1) {
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

    void Cursor::ClampToCamera(Camera& cam, File& f) {
        // Clamp vertically
        UT::i32 camMarginY = cam.MarginY();
        UT::i32 camHeight  = cam.Height();

        UT::i32 cursorY = static_cast<UT::i32>(line * gFont.size) + CBLT::gOffsets.y;

        if (cursorY < camMarginY) {
            gOffsets.y += camMarginY - cursorY;
        } else if (cursorY + gFont.size > camHeight - camMarginY) {
            gOffsets.y -= (cursorY + gFont.size) - (camHeight - camMarginY);
        }

        // Clamp horizontally
        UT::i32 camMarginX = cam.MarginX();
        UT::i32 camWidth   = cam.Width();

        UT::i32 cursorX = static_cast<UT::i32>(GetCursorX(f.GetCurrentLine(line), gFont.size)) + CBLT::gOffsets.x;

        if (cursorX < camMarginX) {
            gOffsets.x += camMarginX - cursorX;
        } else if (cursorX + charWidth > camWidth - camMarginX) {
            gOffsets.x -= (cursorX + charWidth) - (camWidth - camMarginX);
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

    void CursorManager::DrawCursors(CBLT::File& openFile) {
        for (UT::llui32 i = 0 ; i < activeCursors.size() ; i++) {
            const std::string& lineText = openFile.GetCurrentLine(activeCursors[i].Line());
            activeCursors[i].Draw(lineText, i);
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

    void CursorManager::HandlePendingRequests(File& file) {
        // Reset
        if (requestReset) {
            RemoveSecondaries();
            requestReset = false;
        }
    
        // Add cursors down (lead)
        if (requestLead) {
            Cursor base = activeCursors[0];
            for (auto& c : activeCursors)
                if (c.Line() > base.Line())
                    base = c;
        
            if (base.Line() + 1 < file.GetLineCount())
                AddCursorAt(base.Col(), base.Line() + 1);
        
            requestLead = false;
        }
    
        // Add cursors up (trail)
        if (requestTrail) {
            Cursor base = activeCursors[0];
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
        return activeCursors[0];
    }

    const Cursor CursorManager::Primary() const {
        return activeCursors[0];
    }
} // CBLT