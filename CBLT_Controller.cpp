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
            Q.Active().Cursors().RemoveSecondaries(); // Also remove secondaries
        }

        // Backspace
        if (IsKeyPressedRepeat(KEY_BACKSPACE) || IsKeyPressed(KEY_BACKSPACE)) {
            gSound.Play(SoundClass::SOUND_INFILE_DELETE);

            if (cursor.Col() > 0) {
                std::string& line = Q.Active().GetCurrentLine(cursor.Line());
                UT::ui32 col = cursor.Col();
                UT::cui8 tabSize = keyboard.tabSize;

                // Clamp
                if (col > (UT::ui32)line.size()) col = line.size();
        
                // If previous char is space -> delete indentation block
                if (line.at(col - 1) == ' ') {
                    UT::ui32 deleteCount = 0;
                    UT::ui32 startCol = col;
        
                    // Walk left while:
                    // still spaces
                    // not past column 0
                    // not past a tab stop
                    while (startCol > 0 &&
                        startCol <= (UT::ui32)line.size() &&
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
                    cursor.SetAt(startCol, cursor.Line(), line);
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

            // FIXME: Very weird bug that breaks backspace indentation after pressing '}' again?

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
        }

        // Scroll FileQueue camera to the left
        if (keyboard.m.ctrl && keyboard.m.alt && (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT))) {
            const UT::i32 scroll = 25.0f;

            Q.Scroll(-scroll);

            return true;
        }

        // Scroll FileQueue camera to the right
        if (keyboard.m.ctrl && keyboard.m.alt && (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT))) {
            const UT::i32 scroll = 25.0f;

            Q.Scroll(scroll);

            return true;
        }

        // LCtrl + Shift  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

        // Go to the start of the file
        if (keyboard.m.ctrl && keyboard.m.shift && (IsKeyPressed(KEY_G))) {
            console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE) = ":gs";
            console.Execute(Q, cwd);
        
            return true;        
        }

        // LCtrl ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
        // Open token search
        if (keyboard.m.ctrl && (IsKeyPressed(KEY_F))) {
            console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE) = ":f ";
            console.Toggle();
        
            return true;
        }

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

        // Go to end of file
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

            // Maybe some other time
            // if (Q.Size() == 0) {
            //      goto DEQUEUED_LAST_FILE_VIA_SHORTCUT;
            // }
        
            // WOW, CRAZY STUFF
            // if this dequeues the LAST Filequeue node
            // and somewhere after the HandleShorcuts function
            // tries to get the Q.Active()
            // because we haven't exited the normal 
            // no-NAF loop, we simply and ungraciously crash.

            // Using the :q, :qa, :qas directives is safe, since
            // the console is conservative with using the 
            // loaded files, and directives in general ONLY execute
            // when there is an active file

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
        
        UT::b handled = false;

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

                handled = true;
            }
        }

        return handled;
    }

    void Controller::Update(void) {
        keyboard.UpdateModifiers(); // Update modifiers
        console.Update();           // Update console

        // Console handling
        if (console.IsOpen()) {
            // HandleConsoleMouseWheel(); // Mouse
            // HandleConsoleMouseClick();

            UT::b handleConsole = HandleConsole(); // Input

            if (handleConsole) return; // Input handled, return

            // Execute written directive
            if (IsKeyPressed(KEY_ENTER)) {
                console.Execute(Q, cwd);
            }

            // Delete
            else if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
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
            else if (keyboard.m.ctrl && IsKeyPressed(KEY_GRAVE)) {
                console.Toggle();
            }

            // Resize console
            else if (keyboard.m.shift && (IsKeyPressed(KEY_LEFT))) {
                console.Move(+50.0f);
                console.Cam().SetOrigin(GetScreenWidth() - console.Width(), gConsoleFont.size + 10); // Keep the camera anchored to the right side of the screen           
            }
            
            else if (keyboard.m.shift && (IsKeyPressed(KEY_RIGHT))) {
                console.Move(-50.0f);
                console.Cam().SetOrigin(GetScreenWidth() - console.Width(), gConsoleFont.size + 10); // Keep the camera anchored to the right side of the screen           
            }

            // Move camera offsets
            else if (keyboard.m.shift && (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN))) {
                console.Scroll(-gConsoleFont.size); // Scroll by line height
            }
            
            else if (keyboard.m.shift && (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP))) {
                console.Scroll(+gConsoleFont.size);
            }
            
            // Access console directive history
            else if (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)) {
                auto& history = console.History();
                auto& it = console.HistoryIt();
                
                if (!history.empty()) {
                    if (it == history.end()) {
                        // First time pressing UP goes to newest
                        it = history.begin();
                    } else if (std::next(it) != history.end()) {
                        // Move deeper into history
                        ++it;
                    }
                
                    console.ConsoleDirective().Becomes(*it);
                    console.ConsoleCursor().SetAt(
                        console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE).size(),
                        0,
                        console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE)
                    );
                }
            }

            else if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) {
                auto& history = console.History();
                auto& it = console.HistoryIt();
                
                if (!history.empty()) {
                    if (it != history.end()) {
                        if (it == history.begin()) {
                            // Back to "current input"
                            it = history.end();
                            console.ConsoleDirective().Becomes("");
                        } else {
                            --it;
                            console.ConsoleDirective().Becomes(*it);
                        }
                
                        console.ConsoleCursor().SetAt(
                            console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE).size(),
                            0,
                            console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE)
                        );
                    }
                }
            }
            
            // Remove the console message
            else if (IsKeyPressed(KEY_ESCAPE) && console.Message().messageType != ConsoleMessage::NONE) {
                console.Message().messageType = ConsoleMessage::NONE;
            } 

            // Console autocomplete
            else if (IsKeyPressed(KEY_TAB)) {
                Cursor& cc = console.ConsoleCursor();
                std::string autocmp = console.Autocomplete();
                console.ConsoleDirective().Becomes(autocmp);
                cc.SetAt(autocmp.size(), cc.Line(), console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE));
            }

            // Overide the file to handle movement at, since without any specifications it will try to write at the current open user file
            HandleMovement(console.ConsoleCursor(), &console.ConsoleDirective().DirectiveFile());

            return;
        }

        if (keyboard.m.ctrl && IsKeyPressed(KEY_GRAVE)) {
            console.Toggle();
            return;
        }

        if (Q.Size() == 0) return; // No controller update, except for the console

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

                    // As seen by the CTRL + Q shortcut
                    // since calling the Console::Execute will yield the
                    // exact invoked effects, the last active file is DQ'ed
                    // and later we try to call Q.Active() somewhere else,
                    // really bad... use DEQUEUED_LAST_FILE_VIA_SHORTCUT label
                    // to get the fuck outta there

                    // OR, check size again
                    // boohoo you fucking idiot
                    // should have used a client/server method

                    if (Q.Size() == 0) return; // Nothing is safe to call after the shortcuts, possibility the last NQ'ed file was DQ'ed, Q.Active() won't shut the fuck up 

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

    void Controller::HandleConsoleMouseWheel(void) {
        UT::i32 scroll = (UT::i32)GetMouseWheelMove();

        // Move camera offsets
        if (scroll < 0) {
            console.Scroll(-gConsoleFont.size); // Scroll by line height
        } else if (scroll > 0) {
            console.Scroll(+gConsoleFont.size);
        }
    }

    Rectangle Controller::HandleConsoleMouseClick(void) {
        Vector2 mouse = GetMousePosition();
    
        const auto& entries = console.CWDEntries();
        std::string directiveLine = console.ConsoleDirective().DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE);
    
        UT::ui32 contentCount = 0;
    
        for (UT::llui32 i = 0; i < entries.size(); i++) {
            const CWDContentToken& token = entries[i];
    
            // SAME FILTERING LOGIC AS DRAW
            UT::b toDraw = false;
    
            if (directiveLine.empty()) {
                toDraw = true;
            } else if (directiveLine[0] != ':') {
                if (token.n.rfind(directiveLine, 0) == 0) {
                    toDraw = true;
                }
            } else {
                std::string trimmed = UF::TrimLeadingColon(directiveLine);
    
                size_t idx = trimmed.find(' ');
    
                std::string dir;
                std::string param;
    
                if (idx == std::string::npos) {
                    dir = trimmed;
                    param = "";
                } else {
                    dir = trimmed.substr(0, idx);
                    param = trimmed.substr(idx + 1);
                }
    
                std::transform(dir.begin(), dir.end(), dir.begin(), ::tolower);
    
                if (dir == "cd" && token.isDir) {
                    if (param.empty() || token.n.rfind(param, 0) == 0) {
                        toDraw = true;
                    }
                }
            }
    
            if (!toDraw) continue;
    
            // MATCH DRAW POSITION EXACTLY 
            float x = GetScreenWidth() - console.Width() + DirectiveMargins::CWDContentMargin;
    
            float y =
                (float)gConsoleFont.size +
                (float)(CBLT::DirectiveMargins::directiveMarginFromConsoleY + 5) +
                (
                    contentCount *
                    (
                        (float)gConsoleFont.size +
                        (float)DirectiveMargins::directiveMarginFromConsoleY
                    )
                ) +
                console.CameraOffset().y +
                DirectiveMargins::directiveMarginFromConsoleY;
    
            Rectangle entryRect = {
                x,
                y,
                console.Width() - 20.0f,
                (float)gConsoleFont.size
            };
    
            // HIT TEST 
            if (CheckCollisionPointRec(mouse, entryRect)) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (token.isDir) {
                        console.ConsoleDirective().Becomes(":cd " + token.n);
                        console.Execute(Q, cwd);
                    } else {
                        console.ConsoleDirective().Becomes(token.n);
                        console.Execute(Q, cwd);
                    }
                } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                    console.ConsoleDirective().Becomes(":cd .."); // Go up, need to hover a file/dir for it to proc
                    console.Execute(Q, cwd);
                }
    
                return entryRect;
            }
    
            contentCount++;
        }
    
        return {0, 0, 0, 0};
    }
} // CBLT