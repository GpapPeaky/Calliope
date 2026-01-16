#include "CBLT_Controller.hpp"

namespace CBLT {
    Controller::Controller(void) {} // Field members' constructors are automatically called!
    
    Controller::~Controller(void) {}

    void Controller::HandleSelect(void) { // Without using shift, toggle
        Cursor& c = cursorManager.Primary();

        // Entry
        if (keyboard.m.ctrl && IsKeyPressed(KEY_K) && c.GetMode() != CursorMode::SELECT) {
            c.StartSelection();
            cursorManager.RemoveSecondaries();
        }

        // Copy and Exit
        else if (keyboard.m.ctrl && (IsKeyPressed(KEY_K) || IsKeyPressed(KEY_C)) && c.GetMode() != CursorMode::INSERT) {
            c.StopSelection();

            SetClipboardText(CopySelectedText().c_str());
        }
    }

    UT::b Controller::HandleSpecialMovement(Cursor& cursor) {
        const UT::ui32 line = cursor.Line();

        if (keyboard.m.ctrl && (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT))) {
            cursor.SetToWordBoundary(file.GetCurrentLine(line), CursorDirection::RIGHT, file);

            return true;
        } else if (keyboard.m.ctrl && (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT))) {
            cursor.SetToWordBoundary(file.GetCurrentLine(line), CursorDirection::LEFT, file);
        
            return true;
        }

        return false;
    }

    void Controller::HandleMovement(Cursor& cursor, File* fileOverride) {
        File& f = fileOverride ? *fileOverride : file; // Override if required

        // We need to check specifics AND THEN check for general key presses
        if (HandleSpecialMovement(cursor)) return; // Already handled movement, via LCtrl, skip applying any more movement
        
        const UT::ui32 line = cursor.Line();
        const UT::ui32 col  = cursor.Col();
        const UT::ui32 len  = f.GetLineLength(line);
    
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
            if (col > 0) {
                cursor.Left();
            } else if (line > 0) {
                cursor.SetAt(
                    f.GetLineLength(line - 1),
                    line - 1
                );
            }
        } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
            if (col < f.GetLineLength(line)) {
                cursor.Right();
            } else if (line + 1 < f.GetLineCount()) {
                cursor.SetAt(0, line + 1);
            }
        } else if (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)) {
            if (line > 0 && col != len + 1) {
                UT::ui32 newLine = line - 1;
                UT::ui32 newCol  = std::min(
                    col,
                    f.GetLineLength(newLine)
                );
          
                cursor.SetAt(newCol, newLine);
            } else if (line > 0) {
                UT::ui32 newLine = line - 1;
                UT::ui32 newCol  = std::max(
                    col,
                    f.GetLineLength(newLine)
                );
          
                cursor.SetAt(newCol, newLine);
            }
        } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) {
            if (line + 1 < f.GetLineCount() && col != len + 1) {
                UT::ui32 newLine = line + 1;
                UT::ui32 newCol  = std::min(
                    col,
                    f.GetLineLength(newLine)
                );
          
                cursor.SetAt(newCol, newLine);
            } else if (line + 1 < f.GetLineCount()) {
                UT::ui32 newLine = line + 1;
                UT::ui32 newCol  = std::max(
                    col,
                    f.GetLineLength(newLine)
                );
          
                cursor.SetAt(newCol, newLine);
            }
        }
    }

    UT::b Controller::HandleIndentation(File& file, Cursor& cursor) {
        if (cursor.Col() == 0) return false; // No identation to check

        const std::string& line = file.GetCurrentLine(cursor.Line());
        
        if (line.at(cursor.Col() - 1) == '{') { // FIXME: Might need to add the closer in this check, it crashes when done though?
            UT::ui32 currentIndent = GetIndentation(cursor.Line());
            UT::ui32 innerIndent = currentIndent + 1;
            UT::ui32 closerIndent = currentIndent;

            // Inner line
            std::string inner(innerIndent * keyboard.tabSize, ' ');
    
            // Closing line
            std::string closer(closerIndent * keyboard.tabSize, ' ');
            closer += '}';

            UT::ui32 innerLine = cursor.Line() + 1;

            // Insert inner indented line
            file.CreateLine(innerLine, inner);

            // Set cursor at the inner line
            cursor.SetAt(inner.size(), innerLine);

            // Closer after the inner line
            file.CreateLine(innerLine + 1, closer);

            return true;
        }

        return false;
    }

    void Controller::HandleSpecials(Cursor& cursor) {
        // Backspace
        if (IsKeyPressedRepeat(KEY_BACKSPACE) || IsKeyPressed(KEY_BACKSPACE)) {
            if (cursor.Col() > 0) {
                std::string& line = file.GetCurrentLine(cursor.Line());
                UT::i32 col = cursor.Col();
                UT::i32 tabSize = keyboard.tabSize;
        
                // If previous char is space → delete indentation block
                if (line.at(col - 1) == ' ') {
                    UT::i32 deleteCount = 0;
                    UT::i32 startCol = col;
        
                    // Walk left while:
                    // still spaces
                    // not past column 0
                    // not past a tab stop
                    while (startCol > 0 &&
                        line.at(startCol - 1) == ' ' &&
                        ((startCol - 1) % tabSize != 0)) {
                        startCol--;
                        deleteCount++;
                    }
        
                    // Always delete at least one space
                    if (deleteCount == 0) {
                        startCol--;
                        deleteCount = 1;
                    }
        
                    line.erase(startCol, deleteCount);
                    cursor.SetAt(startCol, cursor.Line());
                } else { // Normal character delete
                    line.erase(col - 1, 1);
                    cursor.Left();
                }
            } else if (cursor.Col() == 0 && cursor.Line() > 0) {
                std::string& previousLine = file.GetCurrentLine(cursor.Line() - 1);
                std::string& line = file.GetCurrentLine(cursor.Line());
                
                // Move one line up
                cursor.SetAt(previousLine.length(), cursor.Line() - 1);
                
                if (line.empty()){
                    file.DeleteLine(cursor.Line() + 1);
                } else {
                    // Move the rest of the line to the previous one
                    file.PushBackLineFragment(cursor.Line() + 1, cursor.Line());

                    // THEN delete the line
                    file.DeleteLine(cursor.Line() + 1);
                }
            }

            file.SetDirt(true);
        }

        // FIXME: Multi-cursor indentation is problematic
        // Return
        if (IsKeyPressedRepeat(KEY_ENTER) || IsKeyPressed(KEY_ENTER)) {
            if (cursor.Col() == 0) {
                file.CreateLine(cursor.Line()); 
                
                cursor.Down();
            } else if (cursor.Col()){
                std::string fragment = file.SplitLine(cursor.Line(), cursor.Col());
                
                // Check for indentation
                UT::b indentationHandle = HandleIndentation(file, cursor);

                if (indentationHandle) return;

                UT::ui32 indent = GetIndentation(cursor.Line());

                if (file.GetCurrentLine(cursor.Line()).at(cursor.Col() - 1) == '}') {
                    indent--; // Suppress it if the cursor is right after a closer
                }

                std::string indentString(indent * keyboard.tabSize, ' ');
                std::string indentedFragment = indentString + fragment;
                
                cursor.SetAt(indentString.size(), cursor.Line() + 1);
                
                file.CreateLine(cursor.Line(), indentedFragment);
            }

            file.SetDirt(true);
        }

        // Tab
        if (IsKeyPressedRepeat(KEY_TAB) || IsKeyPressed(KEY_TAB)) {
            UT::ui8 remainingSpace;
            
            if (cursor.Col() % keyboard.tabSize == 0) {
                remainingSpace = keyboard.tabSize;
            } else if (cursor.Col() > keyboard.tabSize) {
                remainingSpace = cursor.Col() % keyboard.tabSize;
            } else {
                remainingSpace = keyboard.tabSize - cursor.Col();
            }

            for (UT::ui8 i = 0 ; i < remainingSpace ; i++) {
                file.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    ' '
                );

                cursor.Right();
            }

            file.SetDirt(true);
        }
    }

    UT::b Controller::HandleInsert(Cursor& cursor, std::vector<char>& keyQueue) {        
        std::string& line = file.GetCurrentLine(cursor.Line());
        
        // Insert the queued input
        for (UT::c32 typed : keyQueue) {
            
            // Closers omit
            if (typed == '}') {
                if (cursor.Col() >= line.length() || line.at(cursor.Col()) != '}') {
                    file.InsertChar(cursor.Col(), cursor.Line(), '}');
                    file.SetDirt(true);
                }
                
                cursor.Right();

                continue;
            }

            else if (typed == ']') {
                if (cursor.Col() >= line.length() || line.at(cursor.Col()) != ']') {
                    file.InsertChar(cursor.Col(), cursor.Line(), ']');
                    file.SetDirt(true);
                }
                
                cursor.Right();

                continue;
            }

            else if (typed == ')') {
                if (cursor.Col() >= line.length() || line.at(cursor.Col()) != ')') {
                    file.InsertChar(cursor.Col(), cursor.Line(), ')');
                    file.SetDirt(true);
                }

                cursor.Right();

                continue;
            }

            // Openers/closers
            if (typed == '{') {
                file.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    typed
                );

                cursor.Right();

                file.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    '}'
                );

                file.SetDirt(true); // Mark file as dirty

                return true;
            }

            else if (typed == '(') {
                file.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    typed
                );

                cursor.Right();

                file.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    ')'
                );

                file.SetDirt(true); // Mark file as dirty

                return true;
            }

            else if (typed == '[') {
                file.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    typed
                );

                cursor.Right();

                file.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    ']'
                );

                file.SetDirt(true); // Mark file as dirty

                return true;
            } 
            
            // Normal insert
            else {
                file.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    typed
                );

                cursor.Right();
            }

            file.SetDirt(true); // Mark file as dirty
            
            return true;
        }

        return false;
    }

    UT::b Controller::HandleShorcuts(Cursor& cursor) {
        // LCTRL + LALT ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

        // Create cursors down
        if (keyboard.m.ctrl && keyboard.m.alt && (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN))) {
            cursorManager.RequestLead();

            return true;
        }

        // Create cursors up
        if (keyboard.m.ctrl && keyboard.m.alt && (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP))) {
            cursorManager.RequestTrail();

            return true;
        }

        // LCTRl ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

        // Delete current line
        if (keyboard.m.ctrl && (IsKeyPressed(KEY_X) || IsKeyPressedRepeat(KEY_X))) { // FIXME: Multi-cursor delete at the end of the file, crashes | deletes too many lines
            SetClipboardText(file.GetCurrentLine(cursor.Line()).c_str());
            
            file.DeleteLine(cursor.Line());

            if (cursor.Line() > 0 && cursor.Line() < file.GetLineCount()) {
                cursor.SetAt(cursor.Col(), cursor.Line());
            } else if (cursor.Line() == file.GetLineCount()) {
                cursor.SetAt(cursor.Col(), cursor.Line() - 1);
            } else {
                cursor.SetAt(0, 0);
            }

            file.SetDirt(true);

            return true;
        }

        // Copy current line
        if (keyboard.m.ctrl && (IsKeyPressed(KEY_D) || IsKeyPressedRepeat(KEY_D))) {
            file.CreateLine(cursor.Line(), file.GetCurrentLine(cursor.Line()));
            file.SetDirt(true);

            return true;
        }

        // Exit
        if (keyboard.m.ctrl && IsKeyPressed(KEY_E)) {
            exit(UDef::GRACEFUL_EXIT);
        }

        // Save and Exit
        if (keyboard.m.ctrl && IsKeyPressed(KEY_Q)) {
            file.Save();

            exit(UDef::GRACEFUL_EXIT);
        }

        // Console toggle
        if (keyboard.m.ctrl && IsKeyPressed(KEY_GRAVE)) {
            console.Toggle();

            return true;
        }

        // Save file contents
        if (keyboard.m.ctrl && IsKeyPressed(KEY_S)) {
            file.Save(); // Automatically cleans the "dirt"

            return true;
        }

        // Reset to primary cursor
        if (keyboard.m.ctrl && IsKeyPressed(KEY_P)) {
            cursorManager.RequestReset();

            return true;
        }

        if (keyboard.m.ctrl && IsKeyPressed(KEY_K)) {
                        

            return true;
        }

        // Comment out/in line
        if (keyboard.m.ctrl && IsKeyPressed(KEY_SLASH)) {
            std::string& line = file.GetCurrentLine(cursor.Line()); 

            UT::llui32 idx = line.find("//");
            
            if (line.empty()) {
                line.append("//");

                cursor.SetAt(2, cursor.Line());
            } else if (idx != std::string::npos) {
                line.erase(idx, 2);

                cursor.SetAt(std::max(0U, cursor.Col() - 2), cursor.Line());
            } else if (idx == std::string::npos) {
                line.insert(0, "//");

                cursor.SetAt(cursor.Col() + 2, cursor.Line());
            }

            return true;
        }

        // Paste from clipboard
        if (keyboard.m.ctrl && IsKeyPressed(KEY_V)) {
            UT::ui32 linesPasted = 0;

            std::string clipboard = GetClipboardText();

            if (clipboard.empty()) return false;

            // Automatically splits per line
            std::stringstream ss(clipboard);
            std::string line;
            UT::ui32 lineIdx = cursor.Line();
            std::vector<std::string> lines;

            while (std::getline(ss, line)) {
                // Remove trailing '\r' if coming from Windows clipboard
                if (!line.empty() && line.back() == '\r')
                    line.pop_back();

                file.CreateLine(lineIdx++, line);
                linesPasted++;
            }

            cursor.SetAt(cursor.Col(), cursor.Line() + linesPasted - 1);

            return true;
        }

        // Go to next file
        // TODO: multi file support
        if (keyboard.m.ctrl && IsKeyPressed(KEY_PERIOD)) {

            return true;
        }

        // Go to previous file
        // TODO: multi file support
        if (keyboard.m.ctrl && IsKeyPressed(KEY_COMMA)) {

            return true;
        }

        return false;
    }

    UT::b Controller::HandleConsole(void) {
        // Directive file
        File& df = console.ConsoleDirective().DirectiveFile();
        
        // Console cursor
        Cursor& cc = console.ConsoleCursor();

        UT::i32 c = 0;                // Consumed instantly by the primary cursor 
        
        // Character recording
        while ((c = keyboard.GetKey()) > 0) {
            if (c >= 32 && c <= 126) { // Allow only ASCII
                df.InsertChar(cc.Col(), cc.Line(), c);

                cc.Right(); // Move cursor forward after inserting
            }
        }

        return false;
    }

    void Controller::Update(void) {
        keyboard.UpdateModifiers(); // Update modifiers
        console.Update();           // Update console

        // Console handling
        if (console.IsOpen()) {
            UT::b handleConsole = HandleConsole(); // Input

            if (handleConsole) return; // Input handled, return

            // Overide the file to handle movement at, since without any specifications it will try to write at the current open user file
            HandleMovement(console.ConsoleCursor(), &console.ConsoleDirective().DirectiveFile());

            // Execute written directive
            if (IsKeyPressed(KEY_ENTER)) {
                console.Execute(this->file);
            }

            // Delete
            if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
                if (console.ConsoleCursor().Col() > 0) {
                    File& df = console.ConsoleDirective().DirectiveFile();
                    std::string& line = df.GetCurrentLine(DIRECTIVE_FILE_LINE);
                    
                    if (!line.empty() && console.ConsoleCursor().Col() <= line.size()) {
                        line.erase(console.ConsoleCursor().Col() - 1, 1);
                        console.ConsoleCursor().Left();
                    }
                }
            }

            // Console toggle to get out
            if (keyboard.m.ctrl && IsKeyPressed(KEY_GRAVE)) {
                console.Toggle();
            }

            // Resize console
            if (keyboard.m.shift && (IsKeyPressed(KEY_LEFT))) {
                console.Move(+50.0f);                
            }
            
            if (keyboard.m.shift && (IsKeyPressed(KEY_RIGHT))) {
                console.Move(-50.0f);
            }

            if (IsKeyPressed(KEY_ESCAPE) && console.Message().messageType != ConsoleMessage::NONE) {
                console.Message().messageType = ConsoleMessage::NONE;
            } 

            return;
        }
        
        // Get pressed keys
        std::vector<char> keyQueue = GetKeyQueue();

        cursorManager.HandlePendingRequests(file);

        for(auto& c : cursorManager.activeCursors) {
            // HandleShorcuts();
            CBLT::CursorMode m = c.GetMode();

            HandleSelect(); // Check if we might enter selection
            
            // Handling booleans
            UT::b handledShort;
            
            switch(m) {
                case CBLT::CursorMode::INSERT:
                    handledShort = HandleShorcuts(c);
                    HandleSpecials(c);

                    // Shortcuts include ctrl + arrow key presses so we need to omit movement
                    if (!handledShort) HandleMovement(c);
                    // Shortcuts include letters so it makes sense that we need to omit any leftover I/O's
                    // so they won't spill over to the insert function
                    if (!handledShort) {
                        HandleInsert(c, keyQueue);     // Shortcut was handled, do not insert
                    }

                    // if (!handledInsert) HandleSelect(); // Potential selection entry

                    ClampCursor(c); // Safety check
                
                    // if(keyboard.CtrlActive()) {
                        // DrawText("ctrl", 200, 200, 20, BLACK);
                    // }else if(keyboard.ShiftActive()) {
                        // DrawText("shift", 200, 200, 20, BLACK);
                    // }else if(keyboard.AltActive()) {
                        // DrawText("alt", 200, 200, 20, BLACK);
                    // }else{
                        // DrawText("nomod", 200, 200, 20, BLACK);
                    // }

                    break;
                case CBLT::CursorMode::SELECT:
                    HandleMovement(c); // Selection is limited to the primary cursor either way

                    // Copy and exit is handled at the start of the update function, see Controller::HandleSelect()

                    ClampCursor(c);
                
                    break;
                default:
                    UE::Log("Unknown cursor mode");

                    break;
            }
        }
    }

    const File& Controller::GetFile(void) const {
        return file;
    }

    File& Controller::GetFile(void) {
        return file;
    }

    const Console& Controller::GetConsole(void) const {
        return console;
    }

    Console& Controller::GetConsole(void) {
        return console;
    }

    const CBLT::CursorManager& Controller::GetCursorManager(void) const {
        return cursorManager;
    }

    CBLT::CursorManager& Controller::GetCursorManager(void) {
        return cursorManager;
    }

    void Controller::ClampCursor(Cursor& c) {
        c.SetAt(
            std::min(c.Col(), static_cast<UT::ui32>(file.GetLineLength(c.Line()))),
            std::min(c.Line(), static_cast<UT::ui32>(file.GetLineCount()))
        );
    }

    std::vector<char> Controller::GetKeyQueue(void) {
        std::vector<char> typedChars; // Typed char, in order to save the queue to apply to the other cursors
        UT::i32 c = 0;                // Consumed instantly by the primary cursor 

        // Character recording
        while ((c = keyboard.GetKey()) > 0) {
            if (c >= 32 && c <= 126) { // Allow only ASCII
                typedChars.push_back(static_cast<char>(c)); // Save typed characters
            }
        }

        return typedChars;
    }

    UT::ui32 Controller::GetIndentation(UT::ui32 line) {
        UT::ui32 depth = 0;
    
        for (UT::ui32 i = 0 ; i < line ; ++i) {
            const std::string& l = file.GetCurrentLine(i);
    
            for (char c : l) {
                if (c == '{') ++depth;
                else if (c == '}' && depth > 0) --depth;
            }
        }
    
        return depth;
    }

    // FIXME: Selection drawing is off when selecting backwards (from a greater line to a lesser one)
    void Controller::DrawSelection(Cursor& c) {
        if (c.GetMode() != CursorMode::SELECT)
            return;
        
        UT::ui32 baseX = CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI + CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES;
        UT::ui32 baseY = UI::TOP_BAR_HEIGHT;
        
        // Use CURRENT cursor position as the selection end
        UT::ui32 startLine = std::min(c.SSLine(), c.Line());
        UT::ui32 endLine = std::max(c.SSLine(), c.Line());
        
        for (UT::ui32 l = startLine; l <= endLine; l++) {
            const std::string& lineText = file.GetCurrentLine(l);
            UT::ui32 lineLength = file.GetLineLength(l);
            
            UT::ui32 selStart, selEnd;
    
            if (l == c.SSLine() && l == c.Line()) {
                // Single line selection
                selStart = std::min(c.SSCol(), c.Col());
                selEnd   = std::max(c.SSCol(), c.Col());
            } else if (l == c.SSLine()) {
                // Start line
                selStart = c.SSCol();
                selEnd   = lineLength;
            } else if (l == c.Line()) {
                // End line (current cursor line)
                selStart = 0;
                selEnd   = c.Col();
            } else {
                // Middle lines
                selStart = 0;
                selEnd   = lineLength;
            }
    
            // Calculate actual pixel positions using the same method as GetCursorX
            UT::f32 scale = (UT::f32)gFont.size / gFont.f.baseSize;
            
            // Calculate X position for selStart
            UT::ui32 startX = 0;
            auto cps = CBLT::gFont.Utf8ToCodepoints(lineText);
            for(size_t i = 0; i < selStart && i < cps.size(); i++) {
                UT::i32 cp = cps[i];
                UT::i32 glyphIndex = -1;
                for(UT::i32 g = 0; g < gFont.f.glyphCount; g++) {
                    if(gFont.f.glyphs[g].value == cp){
                        glyphIndex = g;
                        break;
                    }
                }
                if (glyphIndex >= 0) {
                    startX += gFont.f.glyphs[glyphIndex].advanceX;
                } else {
                    startX += gFont.size / 2;
                }
            }
            startX = (UT::ui32)(startX * scale);
            
            // Calculate X position for selEnd
            UT::ui32 endX = 0;
            for(size_t i = 0; i < selEnd && i < cps.size(); i++) {
                UT::i32 cp = cps[i];
                UT::i32 glyphIndex = -1;
                for(UT::i32 g = 0; g < gFont.f.glyphCount; g++) {
                    if(gFont.f.glyphs[g].value == cp){
                        glyphIndex = g;
                        break;
                    }
                }
                if (glyphIndex >= 0) {
                    endX += gFont.f.glyphs[glyphIndex].advanceX;
                } else {
                    endX += gFont.size / 2;
                }
            }
            endX = (UT::ui32)(endX * scale);
    
            Vector2 pos = {
                (float)(baseX + startX), 
                (float)(baseY + l * gFont.size + gFont.size)
            };
    
            float width = (float)(endX - startX);
            float height = (float)gFont.size;
    
            if (width > 0) {
                DrawRectangleV(pos, { width, height }, Color{ 50, 150, 255, 64 });
            }
        }
    }

    std::string Controller::CopySelectedText(void) {
        Cursor& c = cursorManager.Primary();
    
        UT::ui32 sLine = c.SSLine();
        UT::ui32 sCol  = c.SSCol();
        UT::ui32 eLine = c.SFLine();
        UT::ui32 eCol  = c.SFCol();
    
        // Normalize selection (handles reverse selection)
        if (sLine > eLine || (sLine == eLine && sCol > eCol)) {
            std::swap(sLine, eLine);
            std::swap(sCol, eCol);
        }
    
        std::string copied;
    
        for (UT::ui32 l = sLine; l <= eLine; l++) {
            const std::string& lineText = file.GetCurrentLine(l);
            UT::ui32 lineLength = file.GetLineLength(l);
    
            UT::ui32 selStart, selEnd;
    
            if (l == sLine && l == eLine) {
                selStart = sCol;
                selEnd   = eCol;
            }
            else if (l == sLine) {
                selStart = sCol;
                selEnd   = lineLength;
            }
            else if (l == eLine) {
                selStart = 0;
                selEnd   = eCol;
            }
            else {
                selStart = 0;
                selEnd   = lineLength;
            }
    
            copied += lineText.substr(selStart, selEnd - selStart);
    
            if (l != eLine)
                copied += '\n';
        }
    
        return copied;
    }

    const Camera& Controller::GetCamera(void) const {
        return camera;
    }

    Camera& Controller::GetCamera(void) {
        return camera;
    }
} // CBLT
