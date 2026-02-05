#include "CBLT_Console.hpp"

namespace CBLT {
    Console::Console(void) {
        width = 500;
        toggled = false;
        cursor.AddCursorAt(0, 0);

        directive = Directive();

        dirRes.message = ""; 
        dirRes.messageType = ConsoleMessage::NONE; 
    }
    
    Console::~Console(void) {}

    void Console::GetCWDContents(std::string cwd) {
        cwdContents.clear();

        namespace fs = std::filesystem;

        for (auto& entry : fs::directory_iterator(fs::path(cwd))) {
            if (entry.is_directory()) {
                cwdContents.push_back(
                    {gPalette.cwdDir, entry.path().filename().string() + "/"}
                );
            } else {
                cwdContents.push_back(
                    {gPalette.cwdFile, entry.path().filename().string()}
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

        File f = Q.Active();
        
        DirectiveResult dr = { "", ConsoleMessage::NONE }; // Write here for any messages that need to be displayed, info, error, guide or none if all's well

        std::string directiveLine = directive.DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE);
        
        if (directiveLine.empty()){
            dirRes = dr;
         
            return; // Nothing to show
        }
        
        if (directiveLine.at(0) == '@') { // Directive command context
            std::string trimmed = UF::TrimLeadingAt(directiveLine); // Trimmed of '@'

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

            // Exit
            if (dir == "e") {
                exit(EXIT_SUCCESS);
            }

            // Save and exit
            else if (dir == "we") {
                if (Q.Size() > 0) f.Save();

                exit(EXIT_SUCCESS);
            }

            // Write to file
            else if (dir == "w") {
                if (Q.Size() > 0) f.Save();
            }

            // Help guide
            else if (dir == "h") {
                dr.message = 
                    "Co.Ba.L.T Console Help Guide:\n"
                    "@e      - Exit Co.Ba.L.T\n"
                    "@we     - Write and exit Co.Ba.L.T\n"
                    "@w      - Write current file\n"
                    "@i      - Display file info and metadata\n"
                    "@h      - Display this help guide\n"
                    "@c      - Create a file\n"
                    "@r      - Delete a file\n"
                    "@m      - Create a directory\n"
                    "@d      - Delete a directory\n"
                    "@cd     - Change diretory\n"
                    "@o      - Open native folder picker\n";
                dr.messageType = ConsoleMessage::GUIDE;
            }

            // Create a directory, check if directory exists
            else if (dir == "m") {
                if (directiveParam.empty()) {
                    dr.message = "CBLT_ERR: NO DIRECTORY NAME GIVEN TO MAKE @" + dir;
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                } else {
                    fs::path dirpath = fs::path(cwd) / directiveParam;
                
                    if (fs::exists(dirpath)) {
                        dr.message = "CBLT_ERR: DIRECTORY " + directiveParam + " ALREADY EXISTS";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    } else if (fs::create_directory(dirpath)) {
                        dr.message = "CBLT_LOG: DIRECTORY " + directiveParam + "/ CREATED";
                        dr.messageType = ConsoleMessage::INFO;
                
                        GetCWDContents(cwd);
                    } else {
                        dr.message = "CBLT_ERR: COULD NOT CREATE DIRECTORY " + directiveParam;
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    }
                }
                
                dirRes = dr;

                directive.Clear();
                
                return;
            }

            // Delete a directory, check if directory exists 
            else if (dir == "d") {
                if (directiveParam.empty()) {
                    dr.message = "CBLT_ERR: NO DIRECTORY NAME GIVEN TO DELETE @" + dir;
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                } else {
                    fs::path dirpath = fs::path(cwd) / directiveParam;
                    std::error_code ec;
            
                    if (!fs::exists(dirpath)) {
                        dr.message = "CBLT_ERR: DIRECTORY " + directiveParam + " NOT FOUND";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    }
                    else if (!fs::is_directory(dirpath)) {
                        dr.message = "CBLT_ERR: " + directiveParam + " IS NOT A DIRECTORY";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                    }
                    else {
                        std::uintmax_t removed = fs::remove_all(dirpath, ec);
            
                        if (ec) {
                            dr.message = "CBLT_ERR: " + std::string(ec.message());
                            dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                        } 
                        else if (removed == 0) {
                            dr.message = "CBLT_ERR: FAILED TO DELETE DIRECTORY " + directiveParam;
                            dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                        }
                        else {
                            dr.message = "CBLT_LOG: DIRECTORY " + directiveParam +
                                        "/ DELETED (" + std::to_string(removed) + " items)";
                            dr.messageType = ConsoleMessage::INFO;
            
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
                    dr.message = "CBLT_ERR: NO FILE NAME GIVEN TO CREATE @" + dir;
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
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

                        return; // Early exit
                    }

                    // Load the new file
                    File F;
                    F.Load(directiveParam, cwd);
                    Q.LoadFileToQueue(F);

                    GetCWDContents(cwd); // Update

                    dr.message = "CBLT_LOG: FILE " + directiveParam + " CREATED";
                    dr.messageType = ConsoleMessage::INFO;
                }
            }

            // Remove a file, check if file exists
            else if (dir == "r") {
                if (directiveParam.empty()) { // No param
                    dr.message = "CBLT_ERR: NO FILE NAME GIVEN TO REMOVE @" + dir;
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                } else {
                    fs::path dirpath(cwd.c_str());
                    
                    if (!fs::exists(dirpath)) {
                        dr.message = "CBLT_ERR: INVALID PATH";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                        
                        dirRes = dr;

                        directive.Clear();

                        return;
                    }
                    
                    fs::path filepath = dirpath / directiveParam.c_str();
                    
                    if (!fs::exists(filepath)) {
                        dr.message = "CBLT_ERR: FILE " + directiveParam + " NOT FOUND";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                        dirRes = dr;

                        directive.Clear();

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

                        return;
                    } else { // Failed
                        dr.message = "CBLT_ERR: FILE " + directiveParam + " COULDN'T BE REMOVED";
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                        
                        dirRes = dr;

                        directive.Clear();

                        return;
                    }
                }
            }

            else if (dir == "cd") {
                if (directiveParam.empty()) { // No param, does nothing
                    directive.Clear();
                    cursor.Primary().SetAt(0, DIRECTIVE_FILE_LINE); // Reset the cursor
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
                        } else { // Not a directory
                            dr.message = "CBLT_ERR: NOT A DIRECTORY: " + directiveParam;
                            dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                        }
                    } else {
                        dr.message = "CBLT_ERR: DIRECTORY DOES NOT EXIST: " + directiveParam;
                        dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
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

                    return; // Early out
                }

                fs::path p = fs::path(selected);

                if (!fs::is_directory(p)) {
                    dr.message = "CBLT_ERR: NOT A DIRECTORY";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

                    dirRes = dr;
                    
                    return; // Early out
                }

                cwd = p.string();
                GetCWDContents(cwd);                                    // and get the contents

                dr.message = "CBLT_LOG: CHANGED TO DIR " + cwd + "/";
                dr.messageType = ConsoleMessage::INFO;
            }

            // Display file info and metadata
            else if (dir == "i") {
                if (Q.Size() > 0) {
                    dr.message = f.Info();
                    dr.messageType = ConsoleMessage::INFO;
                } else {
                    dr.message = "CBLT_ERR: NO CURRENT FILE";
                    dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
                }
            }
            
            else { // Invalid directive given fallback
                dr.message = "CBLT_ERR: UNKNOWN DIRECTIVE @" + dir;
                dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
            }

        } else { // Directive file-switch context
            for (auto& entry : cwdContents) {
                if (entry.n == directiveLine) {
                    File F;
                    F.Load(entry.n, cwd);

                    Q.LoadFileToQueue(F); // Add it to the queue

                    dr.message = "CBLT_LOG: SWITCHED TO " + directiveLine;
                    dr.messageType = ConsoleMessage::INFO;
        
                    dirRes = dr;

                    directive.Clear();

                    cursor.Primary().SetAt(0, DIRECTIVE_FILE_LINE); // Reset the cursor
                    
                    return; // Early exit
                }
            }

            // File not fount for file-switch fallback
            dr.message = "CBLT_ERR: UNKNOWN FILE TO SWITCH TO " + directiveLine;
            dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;

            dirRes = dr;
                    
            directive.Clear();
        }
        
        directive.Clear();

        cursor.Primary().SetAt(0, DIRECTIVE_FILE_LINE); // Reset the cursor

        dirRes = dr;
    }

    void Console::Draw(void) {
        const UT::ui32 directiveFontSize = 20;
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
        const std::string& lineText = directive.DirectiveFile().GetCurrentLine(cc.Line());
        const UT::i32 minorHorizotalFix = 2;

        // Compute cursor position inside console
        UT::f32 cursorX = cc.GetCursorX(lineText, directiveFontSize) + minorHorizotalFix;
        UT::f32 cursorY = DirectiveMargins::directiveMarginFromConsoleY;
        
        // Offset for the console's left edge + margins
        cursorX += GetScreenWidth() - width + DirectiveMargins::directiveMarginFromConsoleX;

        const UT::i32 horizontalFix = 2;

        // Hash on the symbol
        switch (cc.cursorSymbol) {
            case CursorSymbol::NON_ASCII_BOX:
                DrawRectangle(
                    cursorX - horizontalFix,
                    cursorY,
                    cc.charWidth,
                    gFont.size,
                    gPalette.consoleCursor
                );

                break;
            case CursorSymbol::NON_ASCII_HOLLOW_BOX:
                DrawRectangleLines(
                    cursorX,
                    cursorY,
                    cc.charWidth,
                    gFont.size,
                    gPalette.consoleCursor
                );

                break;
            case CursorSymbol::NON_ASCII_LINE:
                DrawRectangle(
                    cursorX,
                    cursorY,
                    1,
                    gFont.size,
                    gPalette.consoleCursor
                );

                break;
            case CursorSymbol::NON_ASCII_UNDERSCORE:
                DrawRectangle(
                    cursorX,
                    cursorY + gFont.size,
                    cc.charWidth,
                    1,
                    gPalette.consoleCursor
                );

                break;
        }

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

            if (directiveLine.empty() || directiveLine[0] == '@') {
                // Empty or directive mode -> draw all
                toDraw = true;
            } else {
                // Partial match -> draw only matching entries
                if (current.n.find(directiveLine) != std::string::npos) {
                    toDraw = true;
                }
            }

            if (toDraw) {
                DrawTextEx(
                    gFont.f,
                    current.n.c_str(),
                    {
                        GetScreenWidth() - width + DirectiveMargins::CWDContentMargin,
                        (UT::f32)(directiveFontSize + directiveBottomMargin + (contentCount * (directiveFontSize + DirectiveMargins::directiveMarginFromConsoleY))) + DirectiveMargins::directiveMarginFromConsoleY
                    },
                    directiveFontSize,
                    0.0f,
                    current.c
                );
        
                contentCount++;
            }
        }
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
                    gFont.f,
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

            // Message from console for guide
            case ConsoleMessage::GUIDE: {
                const UT::i32 guideMargin = 50;
            
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
            
                textX = guideX + (guideW - messageWidth) / 2.0f;
                textY = guideY + (guideH - directiveFontSize) / 2.0f;
            
                DrawTextEx(
                    gFont.f,
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
                    gFont.f,
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
} // CBLT