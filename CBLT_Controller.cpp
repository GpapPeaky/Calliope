#include "CBLT_Controller.hpp"

namespace CBLT {
    Controller::Controller(void) {} // Field members' constructors are automatically called!
    
    Controller::~Controller(void) {}

    UT::b Controller::HandleSpecialMovement(Cursor& cursor) {
        const UT::ui32 line = cursor.Line();

        if (keyboard.m.ctrl && (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT))) {
            File& f = Q.Active();

            gSound.Play(SoundClass::SOUND_INFILE_NAV);
            cursor.SetToWordBoundary(f.GetCurrentLine(line), CursorDirection::RIGHT, f.GetLineCount());

            return true;
        } else if (keyboard.m.ctrl && (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT))) {
            File& f = Q.Active();

            gSound.Play(SoundClass::SOUND_INFILE_NAV);
            cursor.SetToWordBoundary(f.GetCurrentLine(line), CursorDirection::LEFT, f.GetLineCount());

            return true;
        }

        return false;
    }

    void Controller::HandleMovement(Cursor& cursor, File* fileOverride) {
        // Need a reference, else it copies it, bad for performance
        File& f = fileOverride ? *fileOverride : Q.Active(); // Override if required
        
        // We need to check specifics AND THEN check for general key presses
        if (HandleSpecialMovement(cursor)) return; // Already handled movement, via LCtrl, skip applying any more movement
        
        const UT::ui32 line = cursor.Line();
        const UT::ui32 col  = cursor.Col();
        const UT::ui32 len  = f.GetLineLength(line);
        
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
            if (col > 0) {
                gSound.Play(SoundClass::SOUND_INFILE_NAV);
                cursor.Left(f.GetCurrentLine(line));
            } else if (line > 0) {
                gSound.Play(SoundClass::SOUND_INFILE_NAV);
                cursor.SetAt(f.GetLineLength(line - 1), line - 1, f.GetCurrentLine(line - 1));
            }

            f.Auto().Close(); // Close in infile navigation
        } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
            if (col < f.GetLineLength(line)) {
                gSound.Play(SoundClass::SOUND_INFILE_NAV);
                cursor.Right(f.GetCurrentLine(line));
            } else if (line + 1 < f.GetLineCount()) {
                gSound.Play(SoundClass::SOUND_INFILE_NAV);
                cursor.SetAt(0, line + 1, f.GetCurrentLine(line + 1));
            }

            f.Auto().Close(); // Close in infile navigation
        } else if (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)) {
            File& f = Q.Active();
            
            if (!f.Auto().IsOpen()) {
                if (line > 0 && col != len + 1) {
                    UT::ui32 newLine = line - 1;
                    UT::ui32 newCol  = std::min(
                        col,
                        f.GetLineLength(newLine)
                    );
                    gSound.Play(SoundClass::SOUND_INFILE_NAV);
                    cursor.SetAt(newCol, newLine, f.GetCurrentLine(newLine));
                } else if (line > 0) {
                    UT::ui32 newLine = line - 1;
                    UT::ui32 newCol  = std::max(
                        col,
                        f.GetLineLength(newLine)
                    );
                    gSound.Play(SoundClass::SOUND_INFILE_NAV);
                    cursor.SetAt(newCol, newLine, f.GetCurrentLine(newLine));
                }
            } else {
                f.Auto().Up();
            }
        } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) {
            File& f = Q.Active();

            if (!f.Auto().IsOpen()) {
                if (line + 1 < f.GetLineCount() && col != len + 1) {
                    UT::ui32 newLine = line + 1;
                    UT::ui32 newCol  = std::min(
                        col,
                        f.GetLineLength(newLine)
                    );
                    gSound.Play(SoundClass::SOUND_INFILE_NAV);
                    cursor.SetAt(newCol, newLine, f.GetCurrentLine(newLine));
                } else if (line + 1 < f.GetLineCount()) {
                    UT::ui32 newLine = line + 1;
                    UT::ui32 newCol  = std::max(
                        col,
                        f.GetLineLength(newLine)
                    );
                    gSound.Play(SoundClass::SOUND_INFILE_NAV);
                    cursor.SetAt(newCol, newLine, f.GetCurrentLine(newLine));
                }
            } else {
                f.Auto().Down();
            }
        }
    }

    UT::b Controller::HandleIndentation(Cursor& cursor) {
        UT::ui32 col = cursor.Col();
        File& f = Q.Active();

        if (col == 0) return false; // No identation to check

        const std::string& line = f.GetCurrentLine(cursor.Line());
        
        if (line.at(cursor.Col() - 1) == '{') {
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
            f.CreateLine(innerLine, inner);
            
            // Set cursor at the inner line
            cursor.SetAt(inner.size(), innerLine, f.GetCurrentLine(innerLine));
            
            // Closer after the inner line
            f.CreateLine(innerLine + 1, closer);

            return true;
        }

        return false;
    }

    void Controller::HandleSpecials(Cursor& cursor) {
        // Escape
        if (IsKeyPressed(KEY_ESCAPE)) {
            Q.Active().Auto().Dismiss();
        }

        // Backspace
        if (IsKeyPressedRepeat(KEY_BACKSPACE) || IsKeyPressed(KEY_BACKSPACE)) {
            gSound.Play(SoundClass::SOUND_INFILE_DELETE);

            if (cursor.Col() > 0) {
                std::string& line = Q.Active().GetCurrentLine(cursor.Line());
                UT::i32 col = cursor.Col();
                UT::i32 tabSize = keyboard.tabSize;
        
                // If previous char is space -> delete indentation block
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
                    cursor.SetAt(startCol, cursor.Line(), Q.Active().GetCurrentLine(cursor.Line()));
                    Q.Active().InsertDirtyLine(cursor.Line());

                } else { // Normal character delete
                    line.erase(col - 1, 1);
                    cursor.Left(line);
                    Q.Active().InsertDirtyLine(cursor.Line());
                }
            } else if (cursor.Col() == 0 && cursor.Line() > 0) {
                UT::ui32 originalLine = cursor.Line(); // capture
                UT::ui32 prevLineLen  = Q.Active().GetLineLength(originalLine - 1);
                UT::b    isEmpty      = Q.Active().GetCurrentLine(originalLine).empty();
            
                cursor.SetAt(prevLineLen, originalLine - 1, Q.Active().GetCurrentLine(originalLine - 1));
            
                if (isEmpty) {
                    Q.Active().DeleteLine(originalLine);
                } else {
                    Q.Active().PushBackLineFragment(originalLine, originalLine - 1);
                    Q.Active().DeleteLine(originalLine);
                }
            }

            Q.Active().SetDirt(true);
        }

        // FIXME: Multi-cursor indentation is problematic
        // Return
        if (IsKeyPressedRepeat(KEY_ENTER) || IsKeyPressed(KEY_ENTER)) {
            if (cursor.Col() == 0) {
                Q.Active().CreateLine(cursor.Line()); 

                cursor.Down(Q.Active().GetCurrentLine(cursor.Line() + 1));
            } else if (cursor.Col()) {
                UT::ui32 originalLine = cursor.Line(); // capture
            
                std::string fragment = Q.Active().SplitLine(originalLine, cursor.Col());
            
                UT::b indentationHandle = HandleIndentation(cursor);
                if (indentationHandle) return;
            
                UT::ui32 indent = GetIndentation(originalLine);
            
                if (Q.Active().GetCurrentLine(originalLine).at(cursor.Col() - 1) == '}') {
                    indent--;
                }
            
                std::string indentString(indent * keyboard.tabSize, ' ');
                std::string indentedFragment = indentString + fragment;
            
                Q.Active().CreateLine(originalLine + 1, indentedFragment); // insert first
                cursor.SetAt(indentString.size(), originalLine + 1, Q.Active().GetCurrentLine(originalLine + 1));       // then move
            }

            gSound.Play(SoundClass::SOUND_INFILE_RETURN);
            Q.Active().SetDirt(true);
        }

        // Tab
        if (IsKeyPressedRepeat(KEY_TAB) || IsKeyPressed(KEY_TAB)) {
            File& f = Q.Active();
        
            if (f.Auto().IsOpen()) {  // Only try to autocomplete if open
                std::string suggestion = f.Auto().GetCurrentSuggestion();
        
                if (!suggestion.empty()) {
                    UT::ui32 line = cursor.Line();
                    UT::i32 col = cursor.Col();
                    
                    const std::string& fragment = cursor.Fragment();
                    if (!fragment.empty()) {
                        UT::i32 start = std::max(0, col - static_cast<UT::i32>(fragment.size()));
                        
                        std::string& lineStr = f.GetCurrentLine(line);
                        UT::ui32 lineSize = static_cast<UT::ui32>(lineStr.size());
                        
                        if (static_cast<UT::ui32>(start) <= lineSize) {
                            UT::ui32 eraseCount = std::min<UT::ui32>(
                                static_cast<UT::ui32>(fragment.size()),
                                lineSize - start
                            );
                            
                            lineStr.erase(start, eraseCount);
                            lineStr.insert(start, suggestion);
                            
                            cursor.SetAt(start + suggestion.size(), line, lineStr);
                            f.InsertDirtyLine(line);
                        }
                    }
        
                    f.Auto().Close();
                    f.SetDirt(true);

                    return; // Don't fall through to indent
                }
            }
        
            // Normal tab indent — only reached if autocomplete is closed or empty
            UT::ui8 remainingSpace;
            
            if (cursor.Col() % keyboard.tabSize == 0) {
                remainingSpace = keyboard.tabSize;
            } else if (cursor.Col() > keyboard.tabSize) {
                remainingSpace = cursor.Col() % keyboard.tabSize;
            } else {
                remainingSpace = keyboard.tabSize - cursor.Col();
            }
        
            for (UT::ui8 i = 0; i < remainingSpace; i++) {
                f.InsertChar(cursor.Col(), cursor.Line(), ' ');
                cursor.Right(f.GetCurrentLine(cursor.Line()));
                // f.Auto().Close(); // Autocomplete and close
            }
        
            f.SetDirt(true);
        }
    }

    UT::b Controller::HandleInsert(Cursor& cursor, std::vector<char>& keyQueue) {        
        File& f = Q.Active();
        std::string& line = f.GetCurrentLine(cursor.Line());
        
        // Insert the queued input
        for (UT::c32 typed : keyQueue) {
            if (isspace(typed)) {
                gSound.Play(SoundClass::SOUND_INFILE_SPACE);
            } else {
                gSound.Play(SoundClass::SOUND_INFILE_INSERT); // Per queue element
            }

            // Closers omit
            if (typed == '}') {
                if (cursor.Col() >= line.length() || line.at(cursor.Col()) != '}') {
                    f.InsertChar(cursor.Col(), cursor.Line(), '}');
                    f.SetDirt(true);
                }
                
                cursor.Right(f.GetCurrentLine(cursor.Line()));
                f.InsertDirtyLine(cursor.Line());

                f.Auto().Close();

                continue;
            }

            else if (typed == ']') {
                if (cursor.Col() >= line.length() || line.at(cursor.Col()) != ']') {
                    f.InsertChar(cursor.Col(), cursor.Line(), ']');
                    f.SetDirt(true);
                }
                
                cursor.Right(f.GetCurrentLine(cursor.Line()));
                f.InsertDirtyLine(cursor.Line());

                f.Auto().Close();

                continue;
            }

            else if (typed == ')') {
                if (cursor.Col() >= line.length() || line.at(cursor.Col()) != ')') {
                    f.InsertChar(cursor.Col(), cursor.Line(), ')');
                    f.SetDirt(true);
                }

                cursor.Right(f.GetCurrentLine(cursor.Line()));
                f.InsertDirtyLine(cursor.Line());

                f.Auto().Close();

                continue;
            }

            // Openers/closers
            if (typed == '{') {
                f.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    typed
                );

                cursor.Right(f.GetCurrentLine(cursor.Line()));

                f.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    '}'
                );

                f.SetDirt(true); // Mark file as dirty
                f.InsertDirtyLine(cursor.Line());

                f.Auto().Close();

                return true;
            }

            else if (typed == '(') {
                f.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    typed
                );

                cursor.Right(f.GetCurrentLine(cursor.Line()));

                f.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    ')'
                );

                f.SetDirt(true); // Mark file as dirty
                f.InsertDirtyLine(cursor.Line());

                f.Auto().Close();

                return true;
            }

            else if (typed == '[') {
                f.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    typed
                );

                cursor.Right(f.GetCurrentLine(cursor.Line()));

                f.InsertChar(
                    cursor.Col(),
                    cursor.Line(),
                    ']'
                );

                f.SetDirt(true); // Mark file as dirty
                f.InsertDirtyLine(cursor.Line());

                f.Auto().Close();

                return true;
            } 
            
            // Normal insert
            else {
                f.InsertChar(cursor.Col(), cursor.Line(), typed);
            
                InfileAutocomplete& ia = f.Auto();
            
                cursor.Right(f.GetCurrentLine(cursor.Line()));
            
                // Only bother with suggestions if the character can be part of a word
                if (!isalnum(typed) && typed != '_') {
                    ia.Close();
                } else {
                    cursor.AcquireFragment(cursor.Col(), f.GetCurrentLine(cursor.Line()));
                    std::string frag = cursor.Fragment();
            
                    if (!frag.empty()) {
                        ia.GetSuggestions(frag);
                        std::vector<std::string> sugg = ia.GetCurrentSuggestions();
            
                        if (sugg.empty()) {
                            ia.Close();
                        } else if (sugg.size() == 1 && sugg[0] == frag) {
                            ia.Close();
                        } else {
                            ia.Valid(); // Reset the dismissed field when a valid character is typed
                            ia.Open();
                        }
                    } else {
                        ia.Close();
                    }
                }
            }

            f.SetDirt(true); // Mark file as dirty
            
            return true;
        }

        return false;
    }

    UT::b Controller::HandleShorcuts(Cursor& cursor) {
        // ESCAPE ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        
        // Close message
        if (IsKeyPressed(KEY_ESCAPE) && console.Message().messageType != ConsoleMessage::NONE) {
            console.Message().messageType = ConsoleMessage::NONE;
        } 

        // LSHIFT + LALT ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

        // Copy current line and move cursor down
        if (keyboard.m.shift && keyboard.m.alt && (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN))) {
            Q.Active().CreateLine(cursor.Line(), Q.Active().GetCurrentLine(cursor.Line()));
            Q.Active().SetDirt(true);

            cursor.Down(Q.Active().GetCurrentLine(cursor.Line() + 1));

            return true;
        }

        // Copy current line
        if (keyboard.m.shift && keyboard.m.alt && (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP))) {
            Q.Active().CreateLine(cursor.Line(), Q.Active().GetCurrentLine(cursor.Line()));
            Q.Active().SetDirt(true);

            return true;
        }

        // LCTRL + LALT ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

        // Create cursors down
        if (keyboard.m.ctrl && keyboard.m.alt && (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN))) {
            gSound.Play(SoundClass::SOUND_INFILE_NAV);
            GetActiveCursorManager().RequestLead();

            return true;
        }

        // Create cursors up
        if (keyboard.m.ctrl && keyboard.m.alt && (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP))) {
            gSound.Play(SoundClass::SOUND_INFILE_NAV);
            GetActiveCursorManager().RequestTrail();

            return true;
        } // FIXME: They are not clipped and show on top of the topbar

        // LCTRl ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

        // Delete current line
        if (keyboard.m.ctrl && (IsKeyPressed(KEY_X) || IsKeyPressedRepeat(KEY_X))) { // FIXME: Multi-cursor delete at the end of the file, crashes | deletes too many lines
            File& f = Q.Active();
            UT::ui32 lineToDel = cursor.Line();

            gSound.Play(SoundClass::SOUND_INFILE_DELETE);
            
            SetClipboardText(f.GetCurrentLine(lineToDel).c_str());
            
            for (auto& im : f.Marks()) {
                if (lineToDel == im.Line()) {
                    f.RemoveMark(lineToDel); // Matching line to mark -> delete mark and ReIndex all other marks
                    break;
                }
            }
            
            // Delete the actual line after removing any marks
            f.DeleteLine(lineToDel);

            if (cursor.Line() > 0 && cursor.Line() < Q.Active().GetLineCount()) {
                cursor.SetAt(cursor.Col(), cursor.Line(), Q.Active().GetCurrentLine(cursor.Line()));
            } else if (cursor.Line() == Q.Active().GetLineCount()) {
                cursor.SetAt(cursor.Col(), cursor.Line() - 1, Q.Active().GetCurrentLine(cursor.Line() - 1));
            } else {
                cursor.SetAt(0, 0, Q.Active().GetCurrentLine(0));
            }

            Q.Active().SetDirt(true);

            return true;
        }

        // Fragment removal kit thing
        if (keyboard.m.ctrl && (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE))) {
            File& f = Q.Active();
            Cursor& c = f.Cursors().Primary();
            std::string frag = c.Fragment();
            UT::ui32 col  = c.Col();
            UT::ui32 line = c.Line();
            std::string& lineStr = f.GetCurrentLine(line);

            // FIXME: Fix issue with deleting one extra character
        
            if (!frag.empty()) {
                // Clamp col to line length, same way AcquireFragment does
                UT::ui32 clampedCol = (col >= lineStr.size() && !lineStr.empty())
                                    ? (UT::ui32)lineStr.size()
                                    : col;
        
                // Find where the fragment actually starts in the line
                size_t fragStart = lineStr.rfind(frag, clampedCol);
        
                if (fragStart != std::string::npos && fragStart + frag.size() <= lineStr.size()) {
                    lineStr.erase(fragStart, frag.size());
                    c.SetAt((UT::ui32)fragStart, line, lineStr);
                    f.InsertDirtyLine(line);
                    f.SetDirt(true);
                }
            }
        
            return true;
        }

        // Mark add/remove
        if (keyboard.m.ctrl && IsKeyPressed(KEY_M)) {
            if (!Q.Active().AddMark(cursor.Line())) {
                Q.Active().RemoveMark(cursor.Line());
            }

            Q.Active().SaveMarks();
        }

        // Exit
        if (keyboard.m.ctrl && IsKeyPressed(KEY_E)) {
            exit(UDef::GRACEFUL_EXIT);
        }

        // Write and Exit
        if (keyboard.m.ctrl && IsKeyPressed(KEY_W)) {
            Q.Active().Save();

            exit(UDef::GRACEFUL_EXIT);
        }

        // Write file contents
        if (keyboard.m.ctrl && IsKeyPressed(KEY_S)) {
            Q.Active().Save(); // Automatically cleans the "dirt"

            return true;
        }

        // Reset to primary cursor
        if (keyboard.m.ctrl && IsKeyPressed(KEY_P)) {
            GetActiveCursorManager().RequestReset();

            return true;
        }

        // Guide pull-up
        if (keyboard.m.ctrl && IsKeyPressed(KEY_H)) {
            // Call the console to execute the directive
            console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE) = ":h";
             console.Execute(Q, cwd);

            return true;
        }

        // Guide pull-up
        if (keyboard.m.ctrl && IsKeyPressed(KEY_G)) {
            // Call the console to execute the directive
            console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE) = ":ge";
             console.Execute(Q, cwd);

            return true;
        }

        // Open folder picker
        if (keyboard.m.ctrl && IsKeyPressed(KEY_O)) {
            // Call the console to execute the directive
            console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE) = ":o";
             console.Execute(Q, cwd);

            return true;
        }

        // Current file info
        if (keyboard.m.ctrl && IsKeyPressed(KEY_I)) {
            // Call the console to execute the directive
            console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE) = ":i";
             console.Execute(Q, cwd);

            return true;
        }

        // Comment out/in line
        if (keyboard.m.ctrl && IsKeyPressed(KEY_SLASH)) {
            std::string& line = Q.Active().GetCurrentLine(cursor.Line()); 

            UT::llui32 idx = line.find("//");
            
            if (line.empty()) {
                line.append("//");

                cursor.SetAt(2, cursor.Line(), line);
            } else if (idx != std::string::npos) {
                line.erase(idx, 2);

                cursor.SetAt(std::max(0U, cursor.Col() - 2), cursor.Line(), line);
            } else if (idx == std::string::npos) {
                line.insert(0, "//");

                cursor.SetAt(cursor.Col() + 2, cursor.Line(), line);
            }

            Q.Active().InsertDirtyLine(cursor.Line());

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

                Q.Active().InsertDirtyLine(lineIdx);
                Q.Active().CreateLine(lineIdx++, line);
                linesPasted++;
            }

            cursor.SetAt(cursor.Col(), cursor.Line() + linesPasted - 1, Q.Active().GetCurrentLine(cursor.Line() + linesPasted - 1));

            return true;
        }

        // Go to next loaded file
        if (keyboard.m.ctrl && IsKeyPressed(KEY_PERIOD)) {
            // UT::ui32 currentLine = cursor.Line();
            
            Q.SetActiveNext();
            
            // File& newFile = Q.Active();
            // UT::ui32 newLineCount = newFile.GetLineCount();
            
            // if (currentLine >= newLineCount && newLineCount > 0) {
            //     cursor.SetAt(0, newLineCount - 1, newFile.GetCurrentLine(newLineCount - 1));
            // } else {
            //     cursor.SetAt(0, currentLine, newFile.GetCurrentLine(currentLine));
            // }

            return true;
        }

        // Go to previous loaded file
        if (keyboard.m.ctrl && IsKeyPressed(KEY_COMMA)) {
            // UT::ui32 currentLine = cursor.Line();
            
            Q.SetActivePrev();
            
            // File& newFile = Q.Active();
            // UT::ui32 newLineCount = newFile.GetLineCount();
            
            // if (currentLine >= newLineCount && newLineCount > 0) {
            //     cursor.SetAt(0, newLineCount - 1, newFile.GetCurrentLine(newLineCount - 1));
            // } else {
            //     cursor.SetAt(0, currentLine, newFile.GetCurrentLine(currentLine));
            // }
        
            return true;
        }

        // Dequeue from loaded files and close current file, doesn't write to file
        if (keyboard.m.ctrl && IsKeyPressed(KEY_Q)) {
            // Call the console to execute the directive
            console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE) = ":q";
             console.Execute(Q, cwd);
        
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

                if (c == ' ') {
                    gSound.Play(SoundClass::SOUND_INFILE_SPACE);
                } else {
                    gSound.Play(SoundClass::SOUND_INFILE_INSERT);
                }

                cc.Right(df.GetCurrentLine(DIRECTIVE_FILE_LINE)); // Move cursor forward after inserting
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
                console.Execute(Q, cwd);
            }

            // Delete
            if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
                gSound.Play(SoundClass::SOUND_INFILE_DELETE);

                if (console.ConsoleCursor().Col() > 0) {
                    File& df = console.ConsoleDirective().DirectiveFile();
                    std::string& line = df.GetCurrentLine(DIRECTIVE_FILE_LINE);
                    
                    if (!line.empty() && console.ConsoleCursor().Col() <= line.size()) {
                        line.erase(console.ConsoleCursor().Col() - 1, 1);
                        console.ConsoleCursor().Left(line);
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
                console.Cam().SetOrigin(GetScreenWidth() - console.Width(), gConsoleFont.size + 10); // Keep the camera anchored to the right side of the screen           
            }
            
            if (keyboard.m.shift && (IsKeyPressed(KEY_RIGHT))) {
                console.Move(-50.0f);
                console.Cam().SetOrigin(GetScreenWidth() - console.Width(), gConsoleFont.size + 10); // Keep the camera anchored to the right side of the screen           
            }

            // Move camera offsets
            if (keyboard.m.shift && (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN))) {
                console.Scroll(-gConsoleFont.size); // Scroll by line height
            }
            
            if (keyboard.m.shift && (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP))) {
                console.Scroll(+gConsoleFont.size);
            }

            // Remove the console message
            if (IsKeyPressed(KEY_ESCAPE) && console.Message().messageType != ConsoleMessage::NONE) {
                console.Message().messageType = ConsoleMessage::NONE;
            } 

            // Console autocomplete
            if (IsKeyPressed(KEY_TAB)) {
                Cursor& cc = console.ConsoleCursor();
                std::string autocmp = console.Autocomplete();
                console.ConsoleDirective().Becomes(autocmp);
                cc.SetAt(autocmp.size(), cc.Line(), console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE));
            }

            return;
        }

        if (keyboard.m.ctrl && IsKeyPressed(KEY_GRAVE)) {
            console.Toggle();
            return;
        }

        // Mouse (stoopid af)
        HandleMouseWheel();
        HandleMouseClick();

        // Get pressed keys
        std::vector<char> keyQueue = GetKeyQueue();

        GetActiveCursorManager().HandlePendingRequests(Q.Active().GetLineCount());

        // Snapshot before possible file switch, need to check to avoid some annoying clamping
        UT::llui32 previousIndex = Q.Index();

        // These will have to finish their jobs of the active file
        // file switching with the HandleSpecials will change the active file
        // but NOT the cursors inside this loop
        // we will avoid the cursor based clamping that references the previous active file
        // via checking the new index. It works since the next frame will update the activeFile cursors
        // but for the first frame of the new active file, where the cursors are not updated in the loop
        // and reference the old ones, we avoid clamping!
        for(auto& c : GetActiveCursorManager().activeCursors) {
            // HandleShorcuts();
            CBLT::CursorMode m = c.GetMode();

            // Handling booleans
            UT::b handledShort;

            switch(m) {
                case CBLT::CursorMode::SELECT:
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

                    // Clamp if we didn't switch files.
                    if (Q.Index() == previousIndex) {
                        Q.Active().ClampCursor(c); // Clamp cursor inside file bounds
                        c.ClampToCamera(camera, Q.Active().GetCurrentLine(c.Line()));
                    }

                    break;
                default:
                    UE::Log("Unknown cursor mode");

                    break;
            }
        }
    }

    const File& Controller::GetFile(void) const {
        return Q.Active();
    }

    File& Controller::GetFile(void) {
        return Q.Active();
    }

    const Console& Controller::GetConsole(void) const {
        return console;
    }

    Console& Controller::GetConsole(void) {
        return console;
    }

    CBLT::CursorManager& Controller::GetActiveCursorManager(void) {
        return Q.Active().Cursors();
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
            const std::string& l = Q.Active().GetCurrentLine(i);
    
            for (char c : l) {
                if (c == '{') ++depth;
                else if (c == '}' && depth > 0) --depth;
            }
        }
    
        return depth;
    }

    void Controller::InitCWD(const std::string& p) {
        cwd = p;
    }

    const std::string& Controller::CWD(void) const {
        return cwd;
    }

    const Camera& Controller::GetCamera(void) const {
        return camera;
    }

    Camera& Controller::GetCamera(void) {
        return camera;
    }

    FileQueue& Controller::LoadedFileQueue(void) {
        return Q;
    }

    void Controller::HandleMouseWheel(void) {
        UT::i32 scroll = (UT::i32)GetMouseWheelMove();
        File& f = Q.Active();
    
        for (auto& cursor : GetActiveCursorManager().activeCursors) {
            if (scroll < 0 && cursor.Line() + 1 < f.GetLineCount()) {
                cursor.SetAt(cursor.Col(), cursor.Line() + 1, f.GetCurrentLine(cursor.Line() + 1));
            } else if (scroll > 0 && cursor.Line() > 0) {
                cursor.SetAt(cursor.Col(), cursor.Line() - 1, f.GetCurrentLine(cursor.Line() - 1));
            }

            f.ClampCursor(cursor);
        }
    }

    void Controller::HandleMouseClick(void) {
        if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            return;
    
        if (Q.Size() == 0)
            return;
    
        Vector2 mouse = GetMousePosition();
    
        // Only allow clicking inside text camera region
        if (!camera.Contains((UT::ui32)mouse.x, (UT::ui32)mouse.y, camera.Width(), camera.Height()))
            return;
    
        File& f = Q.Active();
        Cursor& c = GetActiveCursorManager().Primary();
    
        // Base offsets (same ones used in DrawSelection / File::Draw)
        UT::ui32 baseX =
            CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI +
            CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y +
            CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES;
    
        UT::ui32 baseY = CBLT::FileMargins::UI::TOP_BAR_HEIGHT;
    
        // Account for scrolling offsets
        UT::f32 localY = mouse.y - baseY - CBLT::gOffsets.y + gFont.size;
        UT::f32 localX = mouse.x - baseX - CBLT::gOffsets.x - 2.5f;
    
        if (localY < 0)
            return;
    
        // Calculate line
        UT::ui32 line = static_cast<UT::ui32>(localY / gFont.size);
    
        if (line >= f.GetLineCount())
            line = f.GetLineCount() - 1;
    
        const std::string& lineText = f.GetCurrentLine(line);
    
        // Calculate column from glyph widths
        UT::ui32 col = 0;
        UT::f32 accumulated = 0.0f;
    
        auto cps = CBLT::gFont.Utf8ToCodepoints(lineText);
        UT::f32 scale = (UT::f32)gFont.size / gFont.f.baseSize;
    
        for (UT::llui32 i = 0 ; i < cps.size() ; i++) {
            UT::i32 cp = cps[i];
    
            UT::i32 glyphIndex = -1;
            for (UT::i32 g = 0 ; g < gFont.f.glyphCount ; g++) {
                if (gFont.f.glyphs[g].value == cp) {
                    glyphIndex = g;
                    break;
                }
            }
    
            UT::f32 advance = (glyphIndex >= 0)
                ? gFont.f.glyphs[glyphIndex].advanceX
                : gFont.size / 2;
    
            advance *= scale;
    
            if (localX < accumulated + advance * 0.5f)
                break;
    
            accumulated += advance;
            col++;
        }
    
        if (col > f.GetLineLength(line))
            col = f.GetLineLength(line);
    
        // Reset other cursors
        GetActiveCursorManager().RemoveSecondaries();
    
        c.SetAt(col, line, f.GetCurrentLine(line));
        c.StopSelection(); // ensure we exit select mode
    }
} // CBLT