#include "CBLT_Console.hpp"

namespace CBLT {
    Console::Console(void) {
        width = 400;
        toggled = false;
        
        directive = Directive();
        
        dirRes.message = ""; 
        dirRes.messageType = ConsoleMessage::NONE;
        
        camera = Camera();
        camera.SetHeight(GetScreenHeight() - gConsoleFont.size - 10);
        camera.SetWidth(width);
        camera.SetOrigin(GetScreenWidth() - width, gConsoleFont.size + 10);
        
        // New width
        cursor.activeCursors[0].charWidth = MeasureText("A", CBLT::gConsoleFont.size);

        // Bind console cursor animator as well!
        cursor.activeCursors[0].animator.SetProfile(&gSettings.OPTION_ANIM_ConsoleCursor);
        
        cameraOffset = {0.0f, 0.0f};
    }
    
    Console::~Console(void) {}

    void Console::GetCWDContents(std::string cwd) {
        cwdContents.clear();

        namespace fs = std::filesystem;

        for (auto& entry : fs::directory_iterator(fs::path(cwd))) {
            if (entry.is_directory()) {
                cwdContents.push_back(
                    {gPalette.cwdDir, entry.path().filename().string() + "/", true}
                );
            } else {
                cwdContents.push_back(
                    {gPalette.cwdFile, entry.path().filename().string(), false}
                );
            }
        }
    }

    void Console::Toggle(void) {
        toggled = !toggled;
    }

    UT::b Console::IsOpen(void) const {
        return toggled;
    }

    void Console::Execute(FileQueue& Q, std::string& cwd) {
        namespace fs = std::filesystem;

        DirectiveResult dr = { "", ConsoleMessage::NONE }; // Write here for any messages that need to be displayed, info, error, guide or none if all's well

        std::string directiveLine = directive.DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE);
        
        if (directiveLine.empty()){
            dirRes = dr;
         
            return; // Nothing to show
        }

        // Head of the list
        directiveHistory.push_front(directiveLine);

        // Max size reached, pop from the back, first added will be removed 
        if (directiveHistory.size() > ConsoleParameters::MAX_HISTORY_ENTRIES) {
            directiveHistory.pop_back();
        }

        // Reset iterator
        historyIt = directiveHistory.end();

        if (directiveLine.at(0) == ':') { // Directive command context
            std::string trimmed = UF::TrimLeadingColon(directiveLine); // Trimmed of ':'

            // Directive / parameter seperator
            UT::llui32 idx = trimmed.find_first_of(' ');
            
            // Parameter (limited to 1)
            std::string directiveParam; 

            // Only the directive command
            std::string dir;

            if (idx == std::string::npos) {
                dir = trimmed;
                directiveParam = "";
            } else {
                dir = trimmed.substr(0, idx);
                directiveParam = trimmed.substr(idx + 1); // skip the space
            }

            // Match the remainder after converting to lowercase
            std::transform(dir.begin(), dir.end(), dir.begin(), ::tolower);

            // Search token, this is token by token search, very expensive but ehh 
            if (dir == "f") {
                if (directiveParam.empty()) {
                    dr.message = "CBLT_ERR: NO TOKEN GIVEN TO FIND";        
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;        

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: FIND <NOTOKEN>");

                    dirRes = dr;

                    return;
                } else {
                    if (Q.Size() > 0) {
                        CBLT::Utils::Err::Log("DIRECTIVE: FIND " + directiveParam);

                        File& f = Q.Active();
                        std::vector<std::string> lines = f.GetLines();
                        std::vector<std::vector<Token>> tokens = f.Tokens();

                        for (UT::llui32 i = 0 ; i < lines.size() ; i++) {
                            for (Token t : tokens.at(i)) {
                                // Extract the tokens
                                std::string_view lineView(lines[i]);
                                std::string_view tokenText = lineView.substr(t.col, t.len);
                            
                                std::string tokenScouted = std::string(tokenText);

                                if (tokenScouted == directiveParam) {
                                    f.Cursors().Primary().SetAt(t.col, i, f.GetCurrentLine(i)); // Set cursor there immediately

                                    Toggle();
                                    return;
                                }
                            }
                        }

                        dr.message = "CBLT_ERR: TOKEN GIVEN NOT FOUND INSIDE " + f.Name();
                        dr.messageType = ConsoleMessage::INFO;

                        dirRes = dr;
    
                        return;
                    } else {
                        dr.message = "CBLT_ERR: NO OPEN FILE TO SEARCH";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;        
    
                        CBLT::Utils::Err::Log("DIRECTIVE FAIL: FIND <NAF>");   
                    
                        dirRes = dr;

                        directive.Clear();
                        
                        return;
                    }
                }
            }
            
            // Palette switch
            else if (dir == "pal") {
                if (directiveParam.empty()) {
                    dr.message = "CBLT_ERR: NO PALETTE NAME GIVEN TO SWITCH :" + dir;
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: PAL <NONAME>");
                } else {
                    CBLT::gPalette.ReadPaletteFile(directiveParam);
                    CBLT::Utils::Err::Log("DIRECTIVE: PAL " + directiveParam);
                }

                if (!CBLT::gPalette.ReadPaletteFile(directiveParam)) {
                    dr.message = "CBLT_ERR: FAILED TO LOAD PALETTE " + directiveParam;
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: PAL <NOTFOUND>");
                }
            }

            // Mark at
            else if (dir == "mat") {
                UT::ui32 lineNum = (UT::ui32)std::atoi(directiveParam.c_str());

                if (Q.Size() == 0) {
                    dr.message = "CBLT_ERR: NO FILE TO MARK";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: MARK " + directiveParam + " <NAF>");

                    dirRes = dr;

                    directive.Clear();

                    return;
                }
            
                File& f = Q.Active();
            
                if (lineNum >= f.GetLineCount()) {
                    dr.message = "CBLT_ERR: INVALID LINE TO MARK";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: MARK " + directiveParam + " <NOLINE>");
                
                    dirRes = dr;
                    directive.Clear();

                    return;
                }
            
                UT::b added = f.AddMark(lineNum);
            
                if (!added) {
                    dr.message = "CBLT_ERR: MARK ALREADY EXISTS";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: MARK " + directiveParam + " <MARKEXISTS>");
                    
                    dirRes = dr;
                } else {
                    f.SaveMarks();
                    CBLT::Utils::Err::Log("DIRECTIVE: MARK " + directiveParam);
                }
            
                directive.Clear();
                
                return;
            }

            // Unmark line
            else if (dir == "umat") {
                UT::ui32 lineNum = (UT::ui32)std::atoi(directiveParam.c_str());

                if (Q.Size() == 0) {
                    dr.message = "CBLT_ERR: NO FILE TO UNMARK";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: UNMARK " + directiveParam + " <NAF>");

                    dirRes = dr;

                    directive.Clear();

                    return;
                }
            
                File& f = Q.Active();

                if (lineNum >= f.GetLineCount()) {
                    dr.message = "CBLT_ERR: INVALID LINE TO MARK";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: MARK " + directiveParam + " <NOLINE>");
                    
                    dirRes = dr;
                    directive.Clear();
                    
                    return;
                }

                UT::b removed = f.RemoveMark(lineNum);

                if (!removed) {
                    dr.message = "CBLT_ERR: MARK DOESN'T EXIST";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: UNMARK " + directiveParam + " <MARKNOTEXISTS>");
                    
                    dirRes = dr;
                } else {
                    f.SaveMarks();
                    CBLT::Utils::Err::Log("DIRECTIVE: UNMARK " + directiveParam);
                }

                directive.Clear();

                return;
            }

            // Go to mark
            else if (dir == "gm") {
                UT::ui32 markId = (UT::ui32)std::atoi(directiveParam.c_str());

                if (Q.Size() == 0) {
                    dr.message = "CBLT_ERR: NO FILE TO GO TO";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: GOTO " + directiveParam + " <NAF>");

                    dirRes = dr;

                    directive.Clear();

                    return;
                }
            
                File& f = Q.Active();

                std::vector<InfileMark> marks = f.Marks();
                
                if (markId >= marks.size()) {
                    dr.message = "CBLT_ERR: INVALID MARK TO GO TO";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: GOTO <NOTFOUND>");

                    dirRes = dr;
                } else {
                    UT::ui32 lineNum = marks.at(markId).Line();

                    // Pass the valid goto directive
                    directive.Becomes(":g " + std::to_string(lineNum));

                    Execute(Q, cwd);
                }

                directive.Clear();

                return;
            }

            // Go to last mark
            else if (dir == "gml") {
                if (Q.Size() == 0) {
                    dr.message = "CBLT_ERR: NO FILE TO GO TO";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: GOTO <NAF>");

                    dirRes = dr;

                    directive.Clear();

                    return;
                }

                File& f = Q.Active();

                std::vector<InfileMark> marks = f.Marks();

                UT::ui32 markId = marks.size() - 1;

                // Give it to goto mark to figure it out
                directive.Becomes(":gm " + std::to_string(markId));

                Execute(Q, cwd);
            }

            // Remove last mark
            else if (dir == "uml") {
                if (Q.Size() == 0) {
                    dr.message = "CBLT_ERR: NO FILE TO UMARK";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: UMARK <NAF>");

                    dirRes = dr;

                    directive.Clear();

                    return;
                }
            
                File& f = Q.Active();

                if (f.Marks().empty()) {
                    dr.message = "CBLT_ERR: NO MARKS TO REMOVE";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: UNMARK <MARKNOTEXISTS>");
                    
                    dirRes = dr;
                    directive.Clear();
                    
                    return;
                }
            
                UT::ui32 lastLine = f.Marks().back().Line();
                f.RemoveMark(lastLine); // operates on the real vector
            
                f.SaveMarks();
                CBLT::Utils::Err::Log("DIRECTIVE: UNMARK LAST " + std::to_string(lastLine));
                
                directive.Clear();

                return;
            }

            // Unmark all
            else if (dir == "uma") {
                if (Q.Size() == 0) {
                    dr.message = "CBLT_ERR: NO FILE TO UNMARK";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: UNMARK <NAF>");

                    dirRes = dr;

                    directive.Clear();

                    return;
                }
            
                File& f = Q.Active();

                if (f.Marks().empty()) {
                    dr.message = "CBLT_ERR: NO MARKS TO REMOVE";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: UNMARK <MARKNOTEXISTS>");
                    
                    dirRes = dr;
                    directive.Clear();
                    
                    return;
                }

                // Logging requirement
                while (!f.Marks().empty()) {
                    directive.Becomes(":uml");

                    Execute(Q, cwd); // Recursively remove the last elements so we do not run to indexing problems
                }

                dr.message = "CBLT_LOG: REMOVED ALL INFILE MARKS";
                dr.messageType = ConsoleMessage::INFO;
                
                dirRes = dr;

                // Hardline removal

                f.Marks().clear();
                f.MarkIdFactory() = 0;
                f.SaveMarks();

                directive.Clear();

                return;
            }

            // Display Infile Marks with nearby data
            else if (dir == "im") {
                if (Q.Size() == 0) {
                    dr.message = "CBLT_ERR: NO FILE TO DISPLAY MARKS";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: INFILEMARKS <NAF>");

                    dirRes = dr;

                    directive.Clear();

                    return;
                }
            
                File& f = Q.Active();

                dr.message = f.GetMarksAndNearbyLinesMessageString();
                dr.messageType = ConsoleMessage::FILE_MARKS;

                CBLT::Utils::Err::Log("DIRECTIVE: INFILE MARKINGS");
                
                dirRes = dr;
                
                directive.Clear();

                return;
            }

            // Exit
            else if (dir == "e") {
                CBLT::Utils::Err::Log("DIRECTIVE: EXIT");
                exit(EXIT_SUCCESS);
            }

            // Shell bridge with native terminal
            else if (dir == "sh") {
                CBLT::Utils::Err::Log("DIRECTIVE: SHELL " + directiveParam);
                
                dr.message = gShellBridge.Execute(directiveParam, cwd);
                dr.messageType = ConsoleMessage::INFO;
                
                dirRes = dr;
                
                directive.Clear();
                
                return;
            }

            // Save and exit
            else if (dir == "we") {
                if (Q.Size() > 0) {
                    File& f = Q.Active();

                    CBLT::Utils::Err::Log("DIRECTIVE: WRITE " + f.Name());
                    f.Save();
                } else {
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: WRITE <NAF>" );
                }
                
                CBLT::Utils::Err::Log("DIRECTIVE: EXIT");
                exit(EXIT_SUCCESS);
            }

            // Write to file
            else if (dir == "w") {
                if (Q.Size() > 0) {
                    File& f = Q.Active();

                    CBLT::Utils::Err::Log("DIRECTIVE: WRITE " + f.Name());
                    f.Save();
                } else {
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: WRITE <NAF>");
                }
            }

            // Help guide
            else if (dir == "h") {
                CBLT::Utils::Err::Log("DIRECTIVE: HELP");

                dr.message = CBLT::gGuideMessage;
                dr.messageType = ConsoleMessage::GUIDE;
            }

            // Move one directory up
            else if (dir == "up") {
                // cd .. 
                directive.DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE) = ":cd ..";
                Execute(Q, cwd); // Retry, absolute shit code, but ehh..
            }

            // Create a directory, check if directory exists
            else if (dir == "m") {
                if (directiveParam.empty()) {
                    dr.message = "CBLT_ERR: NO DIRECTORY NAME GIVEN TO MAKE :" + dir;
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: MAKEDIR <NONAME>");
                } else {
                    fs::path dirpath = fs::path(cwd) / directiveParam;
                
                    if (fs::exists(dirpath)) {
                        dr.message = "CBLT_ERR: DIRECTORY " + directiveParam + " ALREADY EXISTS";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                        CBLT::Utils::Err::Log("DIRECTIVE FAIL: MAKEDIR <EXISTS>");
                    } else if (fs::create_directory(dirpath)) {
                        dr.message = "CBLT_LOG: DIRECTORY " + directiveParam + "/ CREATED";
                        dr.messageType = ConsoleMessage::INFO;
                
                        CBLT::Utils::Err::Log("DIRECTIVE: MAKEDIR " + directiveParam);
                        GetCWDContents(cwd);
                    } else {
                        dr.message = "CBLT_ERR: COULD NOT CREATE DIRECTORY " + directiveParam;
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                        CBLT::Utils::Err::Log("DIRECTIVE FAIL: MAKEDIR <FAILED>");
                    }
                }
                
                dirRes = dr;

                directive.Clear();
                
                return;
            }

            // Delete a directory, check if directory exists 
            else if (dir == "d") {
                if (directiveParam.empty()) {
                    dr.message = "CBLT_ERR: NO DIRECTORY NAME GIVEN TO DELETE :" + dir;
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: DELDIR <NONAME>");
                } else {
                    fs::path dirpath = fs::path(cwd) / directiveParam;
                    std::error_code ec;
            
                    if (!fs::exists(dirpath)) {
                        dr.message = "CBLT_ERR: DIRECTORY " + directiveParam + " NOT FOUND";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                        CBLT::Utils::Err::Log("DIRECTIVE FAIL: DELDIR <NOTFOUND>");
                    }
                    else if (!fs::is_directory(dirpath)) {
                        dr.message = "CBLT_ERR: " + directiveParam + " IS NOT A DIRECTORY";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                        CBLT::Utils::Err::Log("DIRECTIVE FAIL: DELDIR <NOTADIR>");
                    }
                    else {
                        std::uintmax_t removed = fs::remove_all(dirpath, ec);
            
                        if (ec) {
                            dr.message = "CBLT_ERR: " + std::string(ec.message());
                            dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                            CBLT::Utils::Err::Log("DIRECTIVE FAIL: DELDIR <FAILED>");
                        } 
                        else if (removed == 0) {
                            dr.message = "CBLT_ERR: FAILED TO DELETE DIRECTORY " + directiveParam;
                            dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                            CBLT::Utils::Err::Log("DIRECTIVE FAIL: DELDIR <FAILED>");
                        }
                        else {
                            dr.message = "CBLT_LOG: DIRECTORY " + directiveParam +
                                        "/ DELETED (" + std::to_string(removed) + " items)";
                            dr.messageType = ConsoleMessage::INFO;

                            CBLT::Utils::Err::Log("DIRECTIVE: DELDIR " + directiveParam);
            
                            GetCWDContents(cwd);
                        }
                    }
                }
            
                dirRes = dr;

                directive.Clear();
                
                return;
            }

            // Create a file
            else if (dir == "c") {
                if (directiveParam.empty()) { // No param
                    dr.message = "CBLT_ERR: NO FILE NAME GIVEN TO CREATE :" + dir;
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: CREATEFILE <NONAME>");
                } else {
                    fs::path dirpath{cwd.c_str()};
                    
                    if (!fs::exists(dirpath)) {
                        fs::create_directory(dirpath);
                    }

                    fs::path filepath = dirpath / directiveParam.c_str();
                    std::ofstream newfile(filepath); // Specify file

                    if(!newfile.is_open()) {
                        dr.message = "CBLT_ERR: FILE " + directiveParam + " ALREADY EXISTS";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                        dirRes = dr;

                        directive.Clear();

                        CBLT::Utils::Err::Log("DIRECTIVE FAIL: CREATEFILE <EXISTS>");

                        return; // Early exit
                    }

                    // Load the new file
                    Q.LoadFileToQueue(directiveParam, cwd);

                    File& f = Q.Active();

                    f.Cursors().Primary().SetAt(0, 0, f.GetCurrentLine(0)); // Moven the new file main cursor to the start of the file

                    GetCWDContents(cwd); // Update

                    dr.message = "CBLT_LOG: FILE " + directiveParam + " CREATED";
                    dr.messageType = ConsoleMessage::INFO;

                    CBLT::Utils::Err::Log("DIRECTIVE: CREATEFILE " + directiveParam);
                }
            }

            // Remove a file, check if file exists
            else if (dir == "r") {
                if (directiveParam.empty()) { // No param
                    dr.message = "CBLT_ERR: NO FILE NAME GIVEN TO REMOVE :" + dir;
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: REMFILE <NONAME>");
                } else {
                    fs::path dirpath(cwd.c_str());
                    
                    if (!fs::exists(dirpath)) {
                        dr.message = "CBLT_ERR: INVALID PATH";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                        
                        dirRes = dr;

                        directive.Clear();

                        CBLT::Utils::Err::Log("DIRECTIVE FAIL: REMFILE <NOTFOUND>");

                        return;
                    }
                    
                    fs::path filepath = dirpath / directiveParam.c_str();
                    
                    if (!fs::exists(filepath)) {
                        dr.message = "CBLT_ERR: FILE " + directiveParam + " NOT FOUND";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                        dirRes = dr;

                        directive.Clear();

                        CBLT::Utils::Err::Log("DIRECTIVE FAIL: REMFILE <NOTFOUND>");

                        return;
                    }

                    // Proceed to delete
                    if (fs::remove(filepath.string().c_str())) { // Success
                        dr.message = "CBLT_LOG: FILE " + directiveParam + " REMOVED";
                        dr.messageType = ConsoleMessage::INFO;
                        
                        // Note: If the file is in the queue and it is saved
                        // It will reappear in the next GetCWD(...) call!
                        // else it can be closed by removing it from the queue

                        GetCWDContents(cwd);

                        dirRes = dr;

                        directive.Clear();

                        CBLT::Utils::Err::Log("DIRECTIVE: REMFILE " + directiveParam);

                        return;
                    } else { // Failed
                        dr.message = "CBLT_ERR: FILE " + directiveParam + " COULDN'T BE REMOVED";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                        
                        dirRes = dr;

                        directive.Clear();

                        CBLT::Utils::Err::Log("DIRECTIVE FAIL: REMFILE <FAILED>");

                        return;
                    }
                }
            }

            else if (dir == "cd") {
                if (directiveParam.empty()) { // No param, does nothing
                    directive.Clear();
                    cursor.Primary().SetAt(0, DIRECTIVE_FILE_LINE, this->directive.DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE)); // Reset the cursor
                    dirRes = dr;
                } else {
                    fs::path newpath = fs::path(cwd) / directiveParam;
                    newpath = fs::weakly_canonical(newpath); // Normalize path

                    if (fs::exists(newpath)) {
                        if (fs::is_directory(newpath)) {
                            cwd = newpath.string(); // Update the value by reference
                            GetCWDContents(cwd);    // and get the contents
                            dr.message = "CBLT_LOG: CHANGED TO DIR " + cwd + "/";
                            dr.messageType = ConsoleMessage::INFO;

                            CBLT::Utils::Err::Log("DIRECTIVE: CHANGEDIR " + directiveParam);
                        } else { // Not a directory
                            dr.message = "CBLT_ERR: NOT A DIRECTORY: " + directiveParam;
                            dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                            CBLT::Utils::Err::Log("DIRECTIVE FAIL: CHANGEDIR <NOTADIR>");
                        }
                    } else {
                        dr.message = "CBLT_ERR: DIRECTORY DOES NOT EXIST: " + directiveParam;
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                        CBLT::Utils::Err::Log("DIRECTIVE FAIL: CHANGEDIR <NOTFOUND>");
                    }
                }
            }

            // Open native file explorer to pick a file/folder more easily
            else if (dir == "o") {
                std::string selected = gDialog.OpenFolderPicker();

                if (selected.empty()) { // Invalid directory
                    dr.message = "CBLT_ERR: INVALID DIR SELECTED";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    dirRes = dr;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: OPEN <INVALID>");

                    return; // Early out
                }

                fs::path p = fs::path(selected);

                if (!fs::is_directory(p)) {
                    dr.message = "CBLT_ERR: NOT A DIRECTORY";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    dirRes = dr;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: OPEN <NOTADIR>");
                    
                    return; // Early out
                }

                cwd = p.string();
                GetCWDContents(cwd);                                    // and get the contents

                dr.message = "CBLT_LOG: CHANGED TO DIR " + cwd + "/";
                dr.messageType = ConsoleMessage::INFO;

                CBLT::Utils::Err::Log("DIRECTIVE: OPEN " + selected);
            }

            // Display file info and metadata
            else if (dir == "i") {
                if (Q.Size() > 0) {
                    File& f = Q.Active();

                    dr.message = f.Info();
                    dr.messageType = ConsoleMessage::INFO;

                    CBLT::Utils::Err::Log("DIRECTIVE: INFO " + f.Name());
                } else {
                    dr.message = "CBLT_ERR: NO CURRENT FILE";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: INFO <NAF>");
                }
            }

            // Go to line
            else if (dir == "g") {
                if (directiveParam.empty()) { // No param, does nothing
                    directive.Clear();

                    dr.message = "CBLT_ERR: PLEASE PROVIDE A LINE NUMBER TO GO TO " + directiveLine;
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    dirRes = dr;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: GOTO <NOLINE>");
                } else {
                    if (Q.Size() == 0) {
                        dr.message = "CBLT_ERR: NO CURRENT FILE";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                        dirRes = dr;

                        directive.Clear();

                        CBLT::Utils::Err::Log("DIRECTIVE FAIL: GOTO <NAF>");

                        return; // Early out
                    }

                    File& f = Q.Active();
                    Cursor& c = f.Cursors().Primary();

                    UT::ui32 line = atoi(directiveParam.c_str());

                    if (line < Q.Active().GetLineCount()) {
                        c.SetAt(0, line, Q.Active().GetCurrentLine(line));

                        Toggle();

                        CBLT::Utils::Err::Log("DIRECTIVE: GOTO " + directiveParam);
                    } else {
                        dr.message = "CBLT_ERR: INVALID LINE NUMBER " + directiveLine;
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
    
                        dirRes = dr;

                        CBLT::Utils::Err::Log("DIRECTIVE FAIL: GOTO <LINEOUTOFBOUNDS>");
                    }
                }
            } 
            
            // Dequeue file
            else if (dir == "q") {
                if (Q.Size() > 0) {
                    GetCWDContents(cwd); // Update CWD contents
                    
                    dr.message = "CBLT_LOG: FILE CLOSED";
                    dr.messageType = ConsoleMessage::INFO;
                    
                    CBLT::Utils::Err::Log("DIRECTIVE: DQFILE " + Q.Active().Name());
                    
                    Q.CloseFile(Q.Index());
                } else {
                    dr.message = "CBLT_ERR: NO CURRENT FILE TO CLOSE";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: DQFILE <NAF>");
                }
            }
            
            // Dequeue all files
            else if (dir == "qa") {
                if (Q.Size() > 0) {
                    while (Q.Size() > 0) {
                        CBLT::Utils::Err::Log("DIRECTIVE: DQFILE " + Q.Active().Name());
                        Q.CloseFile(Q.Index());
                    }

                    GetCWDContents(cwd); // Update CWD contents

                    dr.message = "CBLT_LOG: ALL FILES CLOSED";
                    dr.messageType = ConsoleMessage::INFO;

                } else {
                    dr.message = "CBLT_ERR: NO CURRENT FILES TO CLOSE";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: DQFILE <NAF>");
                }
            }

            // Dequeue all clean files, safe version
            else if (dir == "qas") {
                if (Q.Size() > 0) {
                    for (UT::llui32 i = Q.Size() ; i-- > 0 ; ) {
                        const File& f = Q.GetLoadedFiles().at(i);
            
                        if (!f.Dirt()) {
                            std::string name = f.Name(); // store BEFORE erase
                            CBLT::Utils::Err::Log("DIRECTIVE: DQFILE " + name);
                            
                            Q.CloseFile(i);
                        }
                    }
            
                    GetCWDContents(cwd);
            
                    dr.message = "CBLT_LOG: ALL CLEAN FILES CLOSED";
                    dr.messageType = ConsoleMessage::INFO;
            
                } else {
                    dr.message = "CBLT_ERR: NO CURRENT FILES TO CLOSE";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
            
                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: DQFILE <NAF>");
                }
            }

            // Write and dequeue all
            else if (dir == "wqa") {
                if (Q.Size() > 0) {
                    while (Q.Size() > 0) {
                        Q.Active().Save();

                        CBLT::Utils::Err::Log("DIRECTIVE: WRITE " + Q.Active().Name());

                        CBLT::Utils::Err::Log("DIRECTIVE: DQFILE " + Q.Active().Name());
                        
                        Q.CloseFile(Q.Index());
                    }

                    GetCWDContents(cwd); // Update CWD contents

                    dr.message = "CBLT_LOG: ALL FILES WRITTEN AND CLOSED";
                    dr.messageType = ConsoleMessage::INFO;

                } else {
                    dr.message = "CBLT_ERR: NO CURRENT FILES TO CLOSE";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: DQFILE <NAF>");
                }
            } 
            
            // Write and dequeue
            else if (dir == "wq") {
                if (Q.Size() > 0) {
                    Q.Active().Save();

                    CBLT::Utils::Err::Log("DIRECTIVE: WRITE " + Q.Active().Name());

                    CBLT::Utils::Err::Log("DIRECTIVE: DQFILE " + Q.Active().Name());
                    
                    GetCWDContents(cwd); // Update CWD contents
                    
                    dr.message = "CBLT_LOG: FILE CLOSED";
                    dr.messageType = ConsoleMessage::INFO;
                    
                    Q.CloseFile(Q.Index());
                } else {
                    dr.message = "CBLT_ERR: NO CURRENT FILE TO CLOSE";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: DQFILE <NAF>");
                }
            }
        
            // Goto start
            else if (dir == "gs") {
                if (Q.Size() > 0) {
                    File& f = Q.Active();
                    Cursor& c = f.Cursors().Primary();

                    c.SetAt(0, 0, Q.Active().GetCurrentLine(0)); // Go to start of file

                    if (IsOpen()) Toggle();

                    CBLT::Utils::Err::Log("DIRECTIVE: GOTO 0");
                } else {
                    dr.message = "CBLT_ERR: NO CURRENT FILE";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    dirRes = dr;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: GOTO <NAF>");
                }

                directive.Clear();

                return; // Early out
            }

            // Goto end
            else if (dir == "ge") {
                if (Q.Size() > 0) {
                    File& f = Q.Active();
                    Cursor& c = f.Cursors().Primary();

                    UT::ui32 fileLength = f.GetLineCount() - 1;
                    c.SetAt(0, fileLength, f.GetCurrentLine(fileLength)); // Go to end of file

                    if (IsOpen()) Toggle();

                    CBLT::Utils::Err::Log("DIRECTIVE: GOTO " + std::to_string(fileLength));
                } else {
                    dr.message = "CBLT_ERR: NO CURRENT FILE";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    dirRes = dr;

                    CBLT::Utils::Err::Log("DIRECTIVE FAIL: GOTO <NAF>");
                }

                directive.Clear();

                return; // Early out
            }

            // Open CBLT Resource file for settings
            else if (dir == "set") {
                namespace fs = std::filesystem;

                std::string installationPath;

                #if defined(__linux__)
                    const char* resource_path = getenv("CBLT_RESOURCES");
                    installationPath = resource_path ? std::string(resource_path) : ".";
                #endif

                fs::path optionsDir = fs::path(installationPath) / "options";

                Q.LoadFileToQueue("settings.conf", optionsDir.string());

                directive.Clear();

                CBLT::Utils::Err::Log("DIRECTIVE: NQ" + optionsDir.string() + "setting.conf");

                return;
            }

            // Reload settings to globals
            else if (dir == "rst") {
                CBLT::gSettings.ReadSettings();

                // if (Q.Size() > 0) {

                //     // Update file cursor animations
                //     for (auto& f : Q.GetLoadedFiles()) {
                //         for (auto& c : f.Cursors().activeCursors) {
                //             c.animator.SetProfile(&gSettings.OPTION_ANIM_FileCursor);
                //         }
                //     }
                // } 
                
                // Reload the palette option
                std::string palOp = gSettings.OPTION_Palette;

                directive.Becomes(":pal " + palOp);

                Execute(Q, cwd);

                dr.message = "CBLT_LOG: RELOADED SETTINGS";
                dr.messageType = ConsoleMessage::INFO;

                dirRes = dr;

                return;
            }

            // Display an info message
            else if (dir == "msg") {
                dr.message = directiveParam;
                dr.messageType = ConsoleMessage::INFO;

                dirRes = dr;

                CBLT::Utils::Err::Log("DIRECTIVE: MSG " + directiveParam);

                return;
            }
        } else { // Directive file-switch context
            for (auto& entry : cwdContents) {
                if (entry.n.compare(0, directiveLine.length(), directiveLine) == 0) {
                    Q.LoadFileToQueue(entry.n, cwd); // Add it to the queue, constructs it inside it

                    // Complete queue mutations, bind a reference
                    File& f = Q.Active();
                    
                    // We also need to reset the secondaries before switching
                    f.Cursors().Primary().SetAt(0, 0, f.GetCurrentLine(0)); // Move the new file main cursor to the start of the file

                    dr.message = "CBLT_LOG: SWITCHED TO " + directiveLine;
                    dr.messageType = ConsoleMessage::INFO;
        
                    dirRes = dr;

                    directive.Clear();

                    // Console cursor manager
                    cursor.Primary().SetAt(0, DIRECTIVE_FILE_LINE, directive.DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE)); // Reset the cursor
                    

                    CBLT::Utils::Err::Log("DIRECTIVE: NQ " + directiveLine);

                    return; // Early exit
                }
            }

            // File not fount for file-switch fallback
            dr.message = "CBLT_ERR: UNKNOWN FILE TO SWITCH TO " + directiveLine;
            dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

            dirRes = dr;
            
            directive.Clear();

            CBLT::Utils::Err::Log("DIRECTIVE FAIL: NQ <NOTFOUND>");
        }
        
        directive.Clear();

        cursor.Primary().SetAt(0, DIRECTIVE_FILE_LINE, directive.DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE)); // Reset the cursor

        dirRes = dr;
    }

    void Console::Draw(FileQueue& Q, Rectangle rect) {
        const UT::ui32 directiveFontSize = gConsoleFont.size;
        const UT::ui32 directiveBottomMargin = CBLT::DirectiveMargins::directiveMarginFromConsoleY + 5; // 5 + 5 see CBLT_Directive.hpp
        
        // Background rectangle
        DrawRectangle(
            GetScreenWidth() - width,
            0,
            width + 1,
            GetScreenHeight(),
            gPalette.console
        );

        // Foreground rectangle
        DrawRectangle(
            GetScreenWidth() - width + 1,
            0,
            width,
            GetScreenHeight(),
            gPalette.consoleBackground
        );

        // Directive/CWD contents seperator
        DrawLine(
            GetScreenWidth() - width,
            directiveFontSize + directiveBottomMargin,
            GetScreenWidth(),
            directiveFontSize + directiveBottomMargin,
            gPalette.console
        );

        // Draw console cursor
        Cursor& cc = cursor.Primary();
        const std::string& lineText = directive.DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE);
        const UT::i32 minorHorizotalFix = 2;

        // Compute cursor position inside console
        UT::f32 cursorX = cc.GetCursorX(lineText, directiveFontSize) + minorHorizotalFix;
        UT::f32 cursorY = DirectiveMargins::directiveMarginFromConsoleY;
        
        // Offset for the console's left edge + margins
        cursorX += GetScreenWidth() - width + DirectiveMargins::directiveMarginFromConsoleX;

        const UT::i32 horizontalFix = 2;

        // Hash on the symbol
        switch (*(cc.cursorSymbol)) {
            case CursorSymbol::NON_ASCII_BOX:
                DrawRectangle(
                    cursorX - horizontalFix,
                    cursorY,
                    cc.charWidth,
                    gConsoleFont.size,
                    gPalette.consoleCursor
                );

                break;
            case CursorSymbol::NON_ASCII_HOLLOW_BOX:
                DrawRectangleLines(
                    cursorX,
                    cursorY,
                    cc.charWidth,
                    gConsoleFont.size,
                    gPalette.consoleCursor
                );

                break;
            case CursorSymbol::NON_ASCII_LINE:
                DrawRectangle(
                    cursorX,
                    cursorY,
                    1,
                    gConsoleFont.size,
                    gPalette.consoleCursor
                );

                break;
            case CursorSymbol::NON_ASCII_UNDERSCORE:
                DrawRectangle(
                    cursorX,
                    cursorY + gConsoleFont.size,
                    cc.charWidth,
                    1,
                    gPalette.consoleCursor
                );

                break;
        }

        // camera.Draw();

        // Draw directive contents
        directive.Draw(
            GetScreenWidth() - width,
            0
        );

        std::string directiveLine = directive.DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE);

        UT::ui32 contentCount = 0;

        // Draw CWD contents
        for (UT::llui32 i = 0 ; i < cwdContents.size() ; i++) {
            CWDContentToken& current =  cwdContents[i]; // Pre calculated token colour

            UT::b toDraw = false;
            
            // Complete filtering for both files and directories when needed
            if (directiveLine.empty()) {
                toDraw = true;
            } else if (directiveLine[0] != ':') {
                // File filtering mode
                if (current.n.rfind(directiveLine, 0) == 0) {
                    toDraw = true;
                }
            } else {
                // Directive mode
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
            
                if (dir == "cd" && current.isDir) {
                    if (param.empty() || current.n.rfind(param, 0) == 0) {
                        toDraw = true;
                    }
                }
            }

            if (toDraw) {
                Color c = current.c;

                // Size check for queue
                if (Q.Size() > 0 && current.n == Q.Active().Name()) {
                    c = gPalette.openFileColor;
                }

                // Only show shit inside the camera, clip other content!
                BeginScissorMode(
                    (UT::i32)camera.Origin().x,
                    (UT::i32)camera.Origin().y,
                    (UT::i32)camera.Width(),
                    (UT::i32)camera.Height()
                );

                    DrawTextEx(
                        gConsoleFont.f,
                        current.n.c_str(),
                        {
                            GetScreenWidth() - width + DirectiveMargins::CWDContentMargin,
    
                            (UT::f32)(directiveFontSize +
                                directiveBottomMargin +
                                (
                                    contentCount *
                                    (
                                        directiveFontSize +
                                        DirectiveMargins::directiveMarginFromConsoleY
                                    )
                                )
                                + cameraOffset.y
                            ) +
                            DirectiveMargins::directiveMarginFromConsoleY
                        },
                        directiveFontSize,
                        0.0f,
                        c
                    );

                EndScissorMode();
        
                contentCount++;
            }
        }

        // Entry hover colour
        Color c = UF::C(40, 40, 40);
        c.a = 60;

        DrawRectangleRec(rect, c);
    };

    void Console::DrawMessage(void) {
        // Draw any console messages if any
        if (dirRes.messageType == ConsoleMessage::NONE) return;
        
        const UT::ui32 directiveFontSize = 20;

        const UT::i32 messageTextHorizontalMargins = 10;
        const UT::i32 messageTextVerticalMargins = 10;
        const UT::i32 messageWidth = MeasureText(dirRes.message.c_str(), directiveFontSize);

        const UT::i32 msgX = (GetScreenWidth() / 2) - (messageWidth / 2);
        const UT::i32 msgY = (GetScreenHeight() / 2) - (directiveFontSize / 2);
        const UT::i32 msgW = messageWidth + messageTextHorizontalMargins;
        const UT::i32 msgH = directiveFontSize + messageTextVerticalMargins;

        UT::f32 textX;
        UT::f32 textY;

        switch (dirRes.messageType) {
            // Message from console for error
            case ConsoleMessage::DIRECTIVE_ERROR: {
                // Background
                DrawRectangle(
                    msgX,
                    msgY,
                    msgW,
                    msgH,
                    gPalette.msgForeground
                );
    
                // Foreground
                DrawRectangle(
                    msgX + 1,
                    msgY + 1,
                    msgW - 2,
                    msgH - 2,
                    gPalette.msgBackground
                );
    
                textX = msgX + (msgW - messageWidth) / 2.0f;
                textY = msgY + (msgH - directiveFontSize) / 2.0f;
    
                // Message
                DrawTextEx(
                    gConsoleFont.f,
                    dirRes.message.c_str(),
                    {
                        textX,
                        textY
                    },
                    directiveFontSize,
                    0.0f,
                    gPalette.msgErr
                );

                break;
            }

            // Message from console for guide or shell-bridge output
            case ConsoleMessage::FILE_MARKS:
            case ConsoleMessage::SHELL_BRIDGE:
            case ConsoleMessage::GUIDE: {
                const UT::i32 guideMargin = 57;
            
                const UT::i32 guideW = GetScreenWidth()  - guideMargin * 2;
                const UT::i32 guideH = GetScreenHeight() - guideMargin * 2;
            
                const UT::i32 guideX = guideMargin;
                const UT::i32 guideY = guideMargin;
            
                // Background
                DrawRectangle(
                    guideX,
                    guideY,
                    guideW,
                    guideH,
                    gPalette.msgForeground
                );
            
                // Foreground
                DrawRectangle(
                    guideX + 1,
                    guideY + 1,
                    guideW - 2,
                    guideH - 2,
                    gPalette.msgBackground
                );
            
                const UT::ui32 textMargin = 10;

                textX = guideX + textMargin;
                textY = guideY + textMargin;
            
                DrawTextEx(
                    gConsoleFont.f,
                    dirRes.message.c_str(),
                    { textX, textY },
                    directiveFontSize,
                    0.0f,
                    gPalette.msgGuide
                );
            
                break;
            }

            // Message from console for info
            case ConsoleMessage::INFO: {
                const UT::i32 infoLines =  dirRes.message.size() / (messageWidth / directiveFontSize) + 1;

                const UT::i32 infoPadding = 45;
            
                const UT::i32 textBlockHeight = infoLines * directiveFontSize;
            
                const UT::i32 infoW = messageWidth + infoPadding * 2;
                const UT::i32 infoH = textBlockHeight + infoPadding * 2;
            
                const UT::i32 infoX = (GetScreenWidth()  - infoW) / 2;
                const UT::i32 infoY = (GetScreenHeight() - infoH) / 2;
            
                // Background
                DrawRectangle(
                    infoX,
                    infoY,
                    infoW,
                    infoH,
                    gPalette.msgForeground
                );
            
                // Foreground
                DrawRectangle(
                    infoX + 1,
                    infoY + 1,
                    infoW - 2,
                    infoH - 2,
                    gPalette.msgBackground
                );
            
                textX = infoX + (infoW - messageWidth) / 2.0f;
                textY = infoY + (infoH - textBlockHeight) / 2.0f;
            
                // Message
                DrawTextEx(
                    gConsoleFont.f,
                    dirRes.message.c_str(),
                    { textX, textY },
                    directiveFontSize,
                    0.0f,
                    gPalette.msgInfo
                );
            
                break;
            }

            default:
                break;
        }
    }

    std::vector<CWDContentToken>& Console::CWDEntries(void) {
        return cwdContents;
    }

    void Console::DrawGuide(void) {
        const UT::ui32 directiveFontSize = 20;

        UT::f32 textX;
        UT::f32 textY;

        const UT::i32 guideMargin = 57;
    
        const UT::i32 guideX = 5; // Small padding 
        const UT::i32 guideY = guideMargin + 5; // Small padding
    
        const UT::ui32 textMargin = 10;

        textX = guideX + textMargin;
        textY = guideY + textMargin;
    
        DrawTextEx(
            gConsoleFont.f,
            gGuideMessage.c_str(),
            { textX, textY },
            directiveFontSize,
            0.0f,
            gPalette.msgGuide
        );
    }

    Directive& Console::ConsoleDirective(void) {
        return directive;
    }

    void Console::Update() {
        
    }

    void Console::Move(UT::f32 offset) {
        UT::f32 maxWidth = GetScreenWidth() * ConsoleWidth::WIDTH_MAX_RATIO;
        width = std::clamp(width + offset, ConsoleWidth::WIDTH_MIN, maxWidth);
    }

    DirectiveResult& Console::Message(void) {
        return dirRes;
    }

    Cursor& Console::ConsoleCursor(void) {
        return cursor.Primary();
    }

    UT::f32 Console::Width(void) const {
        return width;
    }

    std::string Console::Autocomplete(void) const {
        const std::string& directiveLine = directive.DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE);

        if (directiveLine.empty()) return "";

        // Normal file autocomplete
        if (directiveLine[0] != ':') {
            for (const auto& entry : cwdContents) {
                if (entry.n.rfind(directiveLine, 0) == 0) { // prefix match
                    return entry.n;
                }
            }
            return "";
        }

        // Trim for change dir
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

        // CD autocomplete
        if (dir == "cd") {
            for (const auto& entry : cwdContents) {
                if (!entry.isDir)
                    continue;
    
                if (param.empty() || entry.n.rfind(param, 0) == 0) {
                    return ":cd " + entry.n; // Combine and return, since we replace the entire line
                }
            }
        }
    
        return "";
    }   

    void Console::Scroll(UT::i32 dy) {
        const float lineHeight =
            gConsoleFont.size + DirectiveMargins::directiveMarginFromConsoleY;
    
        const float contentHeight = lineHeight * cwdContents.size();
        const float cameraHeight  = camera.Height();
    
        // If content fits entirely don't scroll
        if (contentHeight <= cameraHeight) {
            cameraOffset.y = 0;
            return;
        }
    
        const float minOffset = -(contentHeight - cameraHeight);
    
        cameraOffset.y += dy;
    
        // Clamp between bounds
        cameraOffset.y = std::clamp(cameraOffset.y, minOffset, 0.0f);
    }

    Camera& Console::Cam(void) {
        return camera;
    }

    Offset Console::CameraOffset(void) const {
        return cameraOffset;
    }

    std::list<std::string>& Console::History(void) {
        return directiveHistory;
    }

    std::list<std::string>::iterator& Console::HistoryIt(void) {
        return historyIt;
    }
} // CBLT