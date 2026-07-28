#include "CBLT_Cursor.hpp"

namespace CBLT {
    Cursor::Cursor(UT::ui32 col, UT::ui32 ln) :
        column(col),
        line(ln),
        startSelectColumn(col),
        finalSelectColumn(col),
        startSelectLine(ln),
        finalSelectLine(ln),
        m(CursorMode::INSERT),                                     // Default
        fragment(""),
        animator(),
        charWidth(MeasureTextEx(gFont.f, "A", gFont.size, 0.0f).x) // Measure once
    {
        animator.SetProfile(&gSettings.OPTION_ANIM_FileCursor);
        cursorSymbol = &gSettings.OPTION_CursorSymbol;
        animator.Snap(0.0f, ln * gFont.size);
    }

    Cursor::~Cursor(void) {}

    std::string Cursor::Fragment(void) const {
        return fragment;
    }

    void Cursor::AcquireFragment(UT::ui32 c, std::string& line) {
        fragment = "";
    
        if (line.empty()) return;
    
        UT::ui32 pos = c;
    
        if (pos >= line.size()) pos = line.size() - 1;
    
        // Step back to find the start of the current word-like token,
        // stopping at whitespace OR bracket boundaries
        UT::i32 start = static_cast<UT::i32>(pos);
    
        // If we're sitting on whitespace, no fragment
        if (Classify(line[start]) == CharClass::WHITESPACE) {
            fragment = "";
            return;
        }
    
        // Walk left while still in the same INWORD class, stop at symbols/brackets/whitespace
        while (start > 0 && Classify(line[start - 1]) == CharClass::INWORD) {
            start--;
        }
    
        // Walk right from pos while still INWORD
        UT::llui32 end = pos;
        while (end < line.size() && Classify(line[end]) == CharClass::INWORD) {
            end++;
        }
    
        if (end <= static_cast<UT::llui32>(start)) {
            fragment = "";
            return;
        }
    
        fragment = line.substr(start, end - start);
    }

    UT::ui32 Cursor::Col(void) const {
        return this->column;
    }

    UT::ui32 Cursor::Line(void) const {
        return this->line;
    }

    void Cursor::SetAt(UT::ui32 col, UT::ui32 line, const std::string& targetLine) {
        // Clamping, just in case
        if (col > targetLine.size()) {
            col = targetLine.size();
        }
        
        this->column = col;
        this->line = line;

        // Animation trigger here, might need some tinkering later specifically with cursor x calculation
        UT::f32 targetX = GetCursorX(targetLine, gFont.size);
        UT::f32 targetY = line * gFont.size;

        animator.MoveTo(targetX, targetY);
    }

    CursorMode Cursor::GetMode(void) const {
        return this->m;
    }

    void Cursor::SetMode(CursorMode m) {
        this->m = m;
    }

    void Cursor::Up(const std::string& targetLine) {
        if(this->line > 0) {
            SetAt(this->column, this->line - 1, targetLine); // Set at used everywhere since the animator is triggered there!
        }
    }

    void Cursor::Down(const std::string& targetLine) {
        SetAt(this->column, this->line + 1, targetLine);
    }

    void Cursor::Left(const std::string& targetLine) {
        if(this->column > 0) {
            SetAt(this->column - 1, this->line, targetLine);
        }
    }

    void Cursor::Right(const std::string& targetLine) {
        SetAt(this->column + 1, this->line, targetLine);
    }

    void Cursor::DrawSelection(EditorFont& font, UT::ui32 xOff, UT::ui32 yOff, Offset& off) {
        UT::ui32 startColumn = SSCol();
        UT::ui32 startLine   = SSLine();
        UT::ui32 endColumn   = SFCol();
        UT::ui32 endLine     = SFLine();
    
        // Normalize selection (important!)
        if (startLine > endLine || (startLine == endLine && startColumn > endColumn)) {
            std::swap(startLine, endLine);
            std::swap(startColumn, endColumn);
        }
    
        // UT::i32 baseY = CBLT::FileMargins::UI::TOP_BAR_HEIGHT + off.y;
        UT::i32 baseY = off.y + CBLT::FileMargins::UI::TOP_BAR_HEIGHT * 2;
        UT::i32 baseX = xOff + off.x;
    
        for (UT::ui32 line = startLine; line <= endLine; ++line) {
            // UT::i32 y = baseY + line * font.size + font.size;
            UT::i32 y = baseY + line * gFont.size;
    
            if (line == startLine && line == endLine) {
                // Single line selection
                DrawRectangle(
                    baseX + startColumn * charWidth,
                    y,
                    (endColumn - startColumn) * charWidth,
                    font.size,
                    gPalette.selectionColor
                );
            }
            else if (line == startLine) {
                // First line
                DrawRectangle(
                    baseX + startColumn * charWidth,
                    y,
                    GetScreenWidth() - (baseX + startColumn * charWidth),
                    font.size,
                    gPalette.selectionColor
                );
            }
            else if (line == endLine) {
                // Last line
                DrawRectangle(
                    baseX,
                    y,
                    endColumn * charWidth,
                    font.size,
                    gPalette.selectionColor
                );
            }
            else {
                // Middle lines
                DrawRectangle(
                    baseX,
                    y,
                    GetScreenWidth(),
                    font.size,
                    gPalette.selectionColor
                );
            }
        }
    }

    void Cursor::Draw(EditorFont& font, UT::ui32 xOff, UT::ui32 yOff, Offset& off) {
        // Limit from topbar
        BeginScissorMode(
                CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + 
                CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
            CBLT::FileMargins::UI::TOP_BAR_HEIGHT * 2,                                                  // TOP_BAR_HEIGHT * 2, since we have 2 lines
            GetScreenWidth(),
            GetScreenHeight()
        );

        // DrawRectangle(
        //     0,
        //     CBLT::FileMargins::UI::TOP_BAR_HEIGHT * 2, 
        //     GetScreenWidth(),
        //     GetScreenHeight(),
        //     RED
        // );

        if (GetMode() == CursorMode::SELECT) DrawSelection(font, xOff, yOff, off);

        animator.Update();
    
        UT::i32 x = static_cast<UT::i32>(animator.tx) + off.x;
        UT::i32 y = static_cast<UT::i32>(animator.ty) + off.y + CBLT::FileMargins::UI::TOP_BAR_HEIGHT * 2;
    
        // Get the base from the offset
        const UT::i32 base = xOff;

        // Needed for the suggestions vector draw call
        renderX = x;
        renderY = y;
    
        DrawRectangle(0, y, GetScreenWidth(), font.size, gPalette.cursorPosHighlight);
    
        switch (*cursorSymbol) {
            case CursorSymbol::NON_ASCII_BOX:
                DrawRectangle(x + base, y, charWidth, font.size, gPalette.cursor);
                EndScissorMode();
                return;
            case CursorSymbol::NON_ASCII_HOLLOW_BOX:
                DrawRectangleLines(x + base, y, charWidth, font.size, gPalette.cursor);
                EndScissorMode();
                return;
            case CursorSymbol::NON_ASCII_LINE:
                DrawRectangle(x + base - 2, y, 1, font.size, gPalette.cursor);
                EndScissorMode();
                return;
            case CursorSymbol::NON_ASCII_UNDERSCORE:
                DrawRectangle(x + base, y + 2 * font.size, charWidth, 1, gPalette.cursor);
                EndScissorMode();
                return;
        }

        EndScissorMode();
    }

    void Cursor::StartSelection(void) {
        startSelectColumn = column;
        startSelectLine = line;
    }

    void Cursor::StopSelection(void) {
        finalSelectColumn = column;
        finalSelectLine = line;
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

    void Cursor::ResetSelection(void) {
        startSelectColumn = column;
        finalSelectColumn = column;
        startSelectLine = line;
        finalSelectLine = line;
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
                    SetAt(len, line, lineText);
                } else {
                    SetAt(len, line + 1, lineText);
                }

                return;
            }

            while (col < len && Classify(lineText[col]) == CharClass::WHITESPACE) col++;

            if (col >= len) {
                SetAt(len, line, lineText);
                return;
            }

            CharClass cc = Classify(lineText[col]);

            // Skip current class
            while (col < len && cc == Classify(lineText[col])) col++;

            SetAt(col, line, lineText);
            return;
        }
    
        if (dir == CursorDirection::LEFT) {
            if (col <= 0 && line > 0) {
                SetAt(0, line - 1, lineText);
                return;
            }
    
            int i = col - 1;
    
            while (i >= 0 && Classify(lineText[i]) == CharClass::WHITESPACE) i--;
    
            if (i < 0) {
                SetAt(0, line, lineText);
                return;
            }
    
            CharClass cc = Classify(lineText[i]);
            while (i >= 0 && Classify(lineText[i]) == cc) i--;
    
            SetAt(i + 1, line, lineText);

            const UT::i32 newLine = static_cast<UT::i32>(Line());

            if (newLine < 0) { // Safety check
                SetAt(0, 0, lineText);
                return;
            }
            
            return;
        }
    }

    void Cursor::ClampToCamera(Camera& cam, std::string currentLine, Offset& off) {
        const UT::i32 camTop = cam.Origin().y + cam.MarginY();
        const UT::i32 camBottom = cam.Origin().y + cam.Height() - cam.MarginY();
        const UT::i32 camLeft = cam.Origin().x + cam.MarginX();
        const UT::i32 camRight = cam.Origin().x + cam.Width() - cam.MarginX();
        
        const UT::f32 lineHeight = gFont.size;
        
        const UT::i32 textBaseX = CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI + 
                                CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + 
                                CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES
                                + 31.0f;

        const UT::i32 textBaseY = CBLT::FileMargins::UI::TOP_BAR_HEIGHT * 2;
        
        UT::i32 cursorWorldX = textBaseX + GetCursorX(currentLine, gFont.size);
        UT::i32 cursorWorldY = textBaseY + line * lineHeight;
        
        UT::i32 cursorScreenX = cursorWorldX + off.x;
        UT::i32 cursorScreenY = cursorWorldY + off.y;
        
        const UT::i32 charWidth = static_cast<UT::i32>(gFont.size);
        const UT::i32 charHeight = static_cast<UT::i32>(gFont.size);
        
        if (cursorScreenY < camTop) {
            off.y += camTop - cursorScreenY;
        }
        else if (cursorScreenY + charHeight > camBottom) {
            off.y -= (cursorScreenY + charHeight) - camBottom;
        }

        // Never let the top-of-file scroll below its natural resting position
        if (off.y > 0) off.y = 0;
        
        if (cursorScreenX < camLeft) {
            off.x += camLeft - cursorScreenX;
        }
        else if (cursorScreenX + charWidth > camRight) {
            off.x -= (cursorScreenX + charWidth) - camRight;
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

    void CursorManager::DrawCursors(EditorFont& font, UT::ui32 xOff, UT::ui32 yOff, Offset& off) {
        for (UT::llui32 i = 0 ; i < activeCursors.size() ; i++) {
            activeCursors[i].Draw(font, xOff, yOff, off);
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