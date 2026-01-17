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

    void Console::GetCWDContents(void) {
        cwdContents.clear();

        const std::string cwd = directive.DirectiveFile().CWD();

        // // Platform specific directory reading
        // #if defined(_WIN32) || defined(_WIN64) // TODO
        //     std::string searchPath = cwd + "\\*.*";
        //     WIN32_FIND_DATA fd; 
        //     HANDLE hFind = ::FindFirstFile(searchPath.c_str(), &fd); 

        //     if(hFind != INVALID_HANDLE_VALUE) { 
        //         do { 
        //             // read all (real) files in current folder, delete '!' read other 2 default folder . and ..
        //             if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        //                 cwdContents.push_back(std::string(fd.cFileName));
        //             }
        //         } while(::FindNextFile(hFind, &fd)); 
        //         ::FindClose(hFind); 
        //     }
        // #else
        //     DIR* dir;
        //     struct dirent* ent;
        //     if ((dir = opendir (cwd.c_str())) != NULL) {
        //         // print all the files and directories within directory
        //         while ((ent = readdir (dir)) != NULL) {
        //             if (ent->d_type != DT_DIR) { // Not a directory
        //                 cwdContents.push_back(std::string(ent->d_name));
        //             }
        //         }
        //         closedir (dir);
        //     } 
        // #endif
    }

    void Console::Toggle(void) {
        toggled = !toggled;
    }

    UT::b Console::IsOpen(void) const {
        return toggled;
    }

    void Console::Execute(File& f) {
        DirectiveResult dr = { "", ConsoleMessage::NONE }; // Write here for any messages that need to be displayed, info, error, guide or none if all's well

        std::string directiveLine = directive.DirectiveFile().GetCurrentLine(DIRECTIVE_FILE_LINE);
        
        if (directiveLine.empty()){
            dirRes = dr;
         
            return; // Nothing to show
        }
        
        if (directiveLine[0] == ':') { // Directive mode
            std::string drctv = U::TrimLeadingColon(directiveLine); // Trim

            // Match the remainder after converting to lowercase
            std::transform(drctv.begin(), drctv.end(), drctv.begin(), ::tolower);

            // Exi
            if (drctv == "e") {
                exit(EXIT_SUCCESS);
            }

            // Save and exit
            else if (drctv == "q") {
                f.Save();

                exit(EXIT_SUCCESS);
            }

            // Write to file
            else if (drctv == "w") {
                f.Save();
            } 

            // Help guide
            else if (drctv == "h") {
                dr.message = 
                    "Co.Ba.L.T Console Help Guide:\n"
                    ":e      - Exit Co.Ba.L.T\n"
                    ":q      - Save and exit Co.Ba.L.T\n"
                    ":w      - Save current file\n"
                    ":i      - Display file info and metadata\n"
                    ":h      - Display this help guide\n";
                dr.messageType = ConsoleMessage::GUIDE;
            }

            // DIsplay file info and metadata
            else if (drctv == "i") {
                dr.message = f.Info();
                dr.messageType = ConsoleMessage::INFO;
            }
            
            else { // Invalid directive given fallback
                dr.message = "CBLT_ERR: unkown directive :" + drctv;
                dr.messageType = ConsoleMessage::DIRECTIVE_ERROR;
            }

        } else { // File switch mode
            
        }
        
        directive.Clear();

        cursor.Primary().SetAt(0, DIRECTIVE_FILE_LINE); // Reset the cursor

        dirRes = dr;
    }    

    void Console::Draw(std::string cwd) {
        const UT::ui32 directiveFontSize = 20;
        const UT::ui32 directiveBottomMargin = CBLT::DirectiveMargins::directiveMarginFromConsoleY + 5; // 5 + 5 see CBLT_Directive.hpp
        
        // Background rectangle
        DrawRectangle(
            GetScreenWidth() - width,
            0,
            width + 1,
            GetScreenHeight(),
            Color{0, 255, 0, 255}
        );

        // Foreground rectangle
        DrawRectangle(
            GetScreenWidth() - width + 1,
            0,
            width,
            GetScreenHeight(),
            Color{0, 0, 0, 255}
        );

        // Directive/CWD contents seperator
        DrawLine(
            GetScreenWidth() - width,
            directiveFontSize + directiveBottomMargin,
            GetScreenWidth(),
            directiveFontSize + directiveBottomMargin,
            Color{0, 255, 0, 255}
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
                    Color{255, 100, 0, 255}
                );

                break;
            case CursorSymbol::NON_ASCII_HOLLOW_BOX:
                DrawRectangleLines(
                    cursorX,
                    cursorY,
                    cc.charWidth,
                    gFont.size,
                    Color{255, 100, 0, 255}
                );

                DrawRectangleLines(
                    cursorX + 1,
                    cursorY + 1,
                    cc.charWidth - 2,
                    gFont.size - 2,
                    Color{255, 100, 0, 255}
                );

                break;
            case CursorSymbol::NON_ASCII_LINE:
                DrawRectangle(
                    cursorX,
                    cursorY,
                    1,
                    gFont.size,
                    Color{255, 100, 0, 255}
                );

                break;
            case CursorSymbol::NON_ASCII_UNDERSCORE:
                DrawRectangle(
                    cursorX,
                    cursorY + gFont.size,
                    cc.charWidth,
                    1,
                    Color{255, 100, 0, 255}
                );

                break;
        }

        // Draw directive contents
        directive.Draw(
            GetScreenWidth() - width,
            0
        );

        // TODO
        // Draw CWD contents
        // for (UT::llui32 i = 0 ; i < cwdContents.size() ; i++) {
        //     DrawTextEx(
        //         gFont.f,
        //         cwdContents[i].c_str(),
        //         {
        //             GetScreenWidth() - width + DirectiveMargins::directiveMarginFromConsoleX,
        //             (UT::f32)(directiveFontSize + directiveBottomMargin + (i * (directiveFontSize + 5)))
        //         },
        //         directiveFontSize,
        //         0.0f,
        //         Color{255, 255, 255, 255}
        //     );
        // }
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
                    Color{0, 255, 0, 255}
                );
    
                // Foreground
                DrawRectangle(
                    msgX + 1,
                    msgY + 1,
                    msgW - 2,
                    msgH - 2,
                    Color{0, 0, 0, 255}
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
                    Color{255, 0, 128, 255}
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
                    Color{0, 255, 0, 255}
                );
            
                // Foreground
                DrawRectangle(
                    guideX + 1,
                    guideY + 1,
                    guideW - 2,
                    guideH - 2,
                    Color{0, 0, 0, 255}
                );
            
                textX = guideX + (guideW - messageWidth) / 2.0f;
                textY = guideY + (guideH - directiveFontSize) / 2.0f;
            
                DrawTextEx(
                    gFont.f,
                    dirRes.message.c_str(),
                    { textX, textY },
                    directiveFontSize,
                    0.0f,
                    Color{0, 255, 255, 255}
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
                    Color{0, 255, 0, 255}
                );
            
                // Foreground
                DrawRectangle(
                    infoX + 1,
                    infoY + 1,
                    infoW - 2,
                    infoH - 2,
                    Color{0, 0, 0, 255}
                );
            
                textX = infoX + (infoW - messageWidth) / 2.0f;
                textY = infoY + (infoH - textBlockHeight) / 2.0f;
            
                DrawTextEx(
                    gFont.f,
                    dirRes.message.c_str(),
                    { textX, textY },
                    directiveFontSize,
                    0.0f,
                    Color{0, 255, 255, 255}
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