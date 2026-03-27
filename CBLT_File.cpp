#include "CBLT_File.hpp"

namespace CBLT {
    File gNAF;
    
    void File::InsertDirtyLine(UT::ui32 line) {
        if (line >= lines.size()) return;
    
        dirtyLines.insert(line);
    }

    void File::RetokenizeDirtyLines(void) {
        if (dirtyLines.empty()) return;
    
        UT::ui32 minDirty = *std::min_element(dirtyLines.begin(), dirtyLines.end());
    
        // Seed block comment state from the line just before the first dirty line
        UT::b inBlock = (minDirty > 0) ? lineStartsInBlockComment[minDirty - 1] : false;
    
        // Run from minDirty to end of file, since a block comment change
        // at line N can affect every line after it
        for (UT::ui32 i = minDirty; i < lines.size(); ++i) {
            UT::b isDirty   = dirtyLines.count(i) > 0;
    
            if (isDirty || lineStartsInBlockComment[i] != inBlock) {
                lineStartsInBlockComment[i] = inBlock;
                inBlock = LexLine(lines[i], i, inBlock);
            } else {
                // Line is clean and block state matches — no further propagation needed
                inBlock = lineStartsInBlockComment[i]; // keep inBlock consistent
                // Early exit only if we're past all dirty lines and state is stable
                if (i > *std::max_element(dirtyLines.begin(), dirtyLines.end())) break;
            }
        }
    
        // Update dirty autocomplete lines
        for (auto line : dirtyLines) {
            autocomplete.UpdateLine(line, tokens[line], lines[line]);
        }

        dirtyLines.clear();
    }

    // TODO: Add a token type for function calls
    UT::b File::LexLine(const std::string& s, UT::ui32 line, UT::b startInBlockComment) { // FIXME: Multiline comments and strings are a bit iffy
        if (line >= lines.size()) return false;

        tokens[line].clear();

        UT::b inBlock = startInBlockComment;
        UT::ui32 i = 0;

        while (i < s.size()) {
            char c = s[i];

            if (inBlock) {
                UT::ui32 start = i;
            
                while (i < s.size()) {
                    if (s[i] == '*' && i + 1 < s.size() && s[i + 1] == '/') {
                        i += 2;
                        inBlock = false;
                        break;
                    }
                    ++i;
                }
            
                tokens[line].push_back({ TokenClass::COMMENT, line, start, i - start });
                continue;
            }
            
            // Comment block entry
            if (!inBlock && c == '/' && i + 1 < s.size() && s[i + 1] == '*') {
                inBlock = true;
                UT::ui32 start = i;
                i += 2;
                
                // Inline check
                while (i < s.size()) {
                    if (s[i] == '*' && i + 1 < s.size() && s[i + 1] == '/') {
                        i += 2;
                        inBlock = false;
                        break;
                    }
                    ++i;
                }
            
                tokens[line].push_back({
                    TokenClass::COMMENT,
                    line,
                    start,
                    i - start
                });
                continue;
            }

            // String literal
            if (c == '"' || c == '\'' || c == '`') {
                char quote = c;
                UT::ui32 start = i++;
            
                UT::b escaped = false;
                while (i < s.size()) {
                    char ch = s[i++];
            
                    if (escaped) {
                        escaped = false;
                        continue;
                    }
            
                    if (ch == '\\') {
                        escaped = true;
                        continue;
                    }
            
                    if (ch == quote) {
                        break; // closing quote
                    }
                }
            
                tokens[line].push_back({
                    TokenClass::STRING,
                    line,
                    start,
                    i - start
                });
                continue;
            }

            // Identifier / keyword
            if (std::isalpha(c) || c == '_') {
                UT::ui32 start = i++;
                while (i < s.size() && (std::isalnum(s[i]) || s[i] == '_')) i++;
    
                TokenClass type = IsKeyword(s.substr(start, i - start))
                                ? TokenClass::KEYWORD
                                : TokenClass::ID;
    
                tokens[line].push_back({ type, line, start, i - start });
                continue;
            }
    
            // Number
            if (std::isdigit(c)) {
                UT::ui32 start = i++;
                while (i < s.size() && std::isdigit(s[i])) i++;
    
                tokens[line].push_back({ TokenClass::NUM, line, start, i - start });
                continue;
            }
    
            // Whitespace
            if (std::isspace(c)) {
                UT::ui32 start = i++;
                while (i < s.size() && std::isspace(s[i])) i++;
    
                tokens[line].push_back({ TokenClass::WHITESPACE, line, start, i - start });
                continue;
            }

            // -------------------------------------------------------------------------------------------------------------------------------------------------
            // Lang specific lexing (comments, misc)
            // -------------------------------------------------------------------------------------------------------------------------------------------------

            // FIXME: Move this to a function

            switch (ext) {
                case EXT(C):
                    if (c == '/' && i + 1 < s.size() && s[i + 1] == '/') {
                        tokens[line].push_back({
                            TokenClass::COMMENT,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return inBlock;
                    }

                    if (c == '#' && i + 1 < s.size()) {
                        tokens[line].push_back({
                            TokenClass::MISC,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return inBlock;
                    }
                    break;
                case EXT(CPP):
                    if (c == '/' && i + 1 < s.size() && s[i + 1] == '/') {
                        tokens[line].push_back({
                            TokenClass::COMMENT,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return inBlock;
                    }

                    if (c == '#' && i + 1 < s.size()) {
                        tokens[line].push_back({
                            TokenClass::MISC,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return inBlock;
                    }
                    break;
                case EXT(CS):
                    if (c == '/' && i + 1 < s.size() && s[i + 1] == '/') {
                        tokens[line].push_back({
                            TokenClass::COMMENT,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return inBlock;
                    }
                    break;
                case EXT(JAVA):
                    if (c == '/' && i + 1 < s.size() && s[i + 1] == '/') {
                        tokens[line].push_back({
                            TokenClass::COMMENT,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return inBlock;
                    }
                    break;

                case EXT(ASM): 
                    if (c == ';' || c == '#') {
                        tokens[line].push_back({
                            TokenClass::COMMENT,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return inBlock;
                    }
                    break;

                case EXT(PY):
                    if (c == '#') {
                        tokens[line].push_back({
                            TokenClass::COMMENT,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return inBlock;
                    }
                default:
                    break;
            }

            // -------------------------------------------------------------------------------------------------------------------------------------------------

            // Operators
            if (
                c == '+' ||
                c == '-' ||
                c == '/' ||
                c == '%' ||
                c == '*' ||
                c == '=' ||
                c == '>' ||
                c == '<' ||
                c == '^' ||
                c == '&' ||
                c == '|' ||
                c == '!' ||
                c == '~'
            ) {
                tokens[line].push_back({
                    TokenClass::OPERATOR,
                    line,
                    i,
                    1
                });
                ++i;
                continue;
            }

            // Default on
            //
            // Punctuation
            tokens[line].push_back({ TokenClass::PUNCTUATION, line, i, 1 });
            ++i;
        }

        return inBlock;
    }

    void InitNAF(void) {
        const UT::ui32 nameSize = rand() % 256;
        std::string NAFname;
        
        for (unsigned int i = 0; i < nameSize; i++) {
            char c = 32 + (rand() % (127 - 32)); // random printable ASCII
            NAFname.push_back(c);
        }
        
        gNAF.SetName(NAFname);

        CBLT::Utils::Err::Log("\n###############################################################################\n");
        CBLT::Utils::Err::Log("NAFname: " + NAFname + "\n");
    }

    UT::b IsNAF(std::string filename) {
        if (filename == gNAF.Name()) {
            return true;
        }

        return false;
    }

    File::File(void) {
        lines.emplace_back("");

        tokens.resize(1);
        lineStartsInBlockComment.resize(1, false);
    }

    File::~File(void) {}

    void File::Tokenize(void) {
        tokens.clear();
        lineStartsInBlockComment.clear();
    
        tokens.resize(lines.size());
        lineStartsInBlockComment.resize(lines.size());
    
        UT::b inBlock = false;
    
        for (UT::ui32 line = 0; line < lines.size(); ++line) {
            lineStartsInBlockComment[line] = inBlock;
            inBlock = LexLine(lines[line], line, inBlock);
        }
    }

    void File::SetName(std::string name) {
        this->name = name;
    }
        
    UT::b File::Load(const std::string& fpath, const std::string& cwd) {
        namespace fs = std::filesystem;
    
        std::string pathString = cwd + '\\' + fpath;

        std::ifstream file(pathString);
        
        if (!file.is_open()) {
            return false;
        }
        
        Clear();
    
        path = pathString;
        name = fpath; // File name
    
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
    
        if (lines.empty())
            lines.emplace_back("");

        // Seek file extension, and assign the correct language keywords for language support.
        ext = AssignExtension(path);
        AssignLanguageKeywords(ext);

        dirty = false;
    
        Tokenize(); // Suck ass

        if (!IsNAF(name)) {
            autocomplete.LoadTokens(tokens, lines);
        }

        return true;
    }
            
    UT::b File::Save(void) {
        if(path.empty())
            return false;
    
        std::ofstream file(path);
        if(!file.is_open())
            return false;
    
        for (UT::llui32 i = 0 ; i < lines.size() ; i++) {
            file << lines.at(i);
            if (i + 1 < lines.size()) file << '\n';
        }
    
        dirty = false;

        return true;
    }

    UT::b File::Clear(void) {
        lines.clear();

        tokens.clear();
        lineStartsInBlockComment.clear();
    
        dirtyLines.clear();

        return true;
    }

    void File::InsertChar(UT::ui32 col, UT::ui32 line, UT::i32 c) {
        if (line >= lines.size()) return;
    
        auto& ln = lines.at(line);
    
        if (col > ln.size()) col = ln.size();
    
        ln.insert(ln.begin() + col, static_cast<char>(c));

        InsertDirtyLine(line);
    }
    
    const std::string& File::GetPath(void) const {
        return path;
    }

    void File::Draw(CBLT::Camera& cam, UT::ui32 cursorX, UT::ui32 cursorY, UT::b consoleOpen, UT::ui32 consoleWidth) {
        UT::f32 lineHeight = gFont.size;
        
        const UT::f32 textBaseX = CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI + 
                                CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + 
                                CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES;
        const UT::f32 textBaseY = 0.0f;

        // Seperator
        DrawLineV(
            { 
                CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
                CBLT::FileMargins::UI::TOP_BAR_HEIGHT + gFont.size - 6
            },{ 
                CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
                static_cast<UT::f32>(GetScreenHeight())
            },
            gPalette.textSeperators
        );
    
        for(UT::llui32 i = 0 ; i < lines.size() ; i++) {
            Vector2 pos = {
                textBaseX + CBLT::gOffsets.x,
                textBaseY + i * lineHeight + lineHeight + CBLT::gOffsets.y
            };

            if (!cam.Contains(
                pos.x,
                pos.y,
                (UT::f32)cam.Width(),
                lineHeight + FileMargins::UI::TOP_BAR_HEIGHT
            )) continue; // Skip non visible lines

            // THIS JUST FUCKING CLIPS, DOESN'T REDUCE THE DRAW CALL, LEARNT IT THE HARD WAY, FUCK.
            BeginScissorMode(
                cam.Origin().x + CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI,
                cam.Origin().y,
                cam.Width(),
                cam.Height() + FileMargins::UI::TOP_BAR_HEIGHT
            );
                // File text
                // DrawTextEx(
                //     gFont.f,
                //     lines.at(i).c_str(),
                //     pos,
                //     gFont.size,
                //     0.0f,
                //     gPalette.textBase
                // );
                
                // Draw colored tokens
                for (Token& t : tokens.at(i)) {
                    // if (t.line != i) continue;
                
                    Color col = t.TokenColor();
                    if (col.a == 0) continue; // skips whitespaces
                
                    std::string_view lineView(lines[i]);
                    std::string_view tokenText = lineView.substr(t.col, t.len);
                    
                    // By counting glyphs
                    UT::f32 tokX = pos.x + t.GetCursorX(lineView, gFont.size, t.col);
                    
                    DrawTextEx(
                        gFont.f,
                        std::string(tokenText).c_str(), // convert to null-terminated C-string
                        { tokX, pos.y },
                        gFont.size,
                        0.0f,
                        col
                    );
                }
            EndScissorMode();   
            
            pos.x = CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y;
            pos.y = textBaseY + i * lineHeight + lineHeight + gOffsets.y;
            
            BeginScissorMode(
                cam.Origin().x,
                cam.Origin().y,
                cam.Width(),
                cam.Height() + FileMargins::UI::TOP_BAR_HEIGHT
            );
                // line num
                DrawTextEx(
                    gFont.f,
                    std::to_string(i).c_str(),
                    pos,
                    gFont.size,
                    0.0f,
                    gPalette.textLines         
                );
            EndScissorMode();
        }

        // Limit
        BeginScissorMode(
            cam.Origin().x,
            cam.Origin().y,
            cam.Width(),
            cam.Height() + FileMargins::UI::TOP_BAR_HEIGHT
        );
            // Draw infile marks
            for (auto& im : marks) {
                Vector2 pos = {
                    GetScreenWidth() - (UT::f32)gCharWidth,
                    textBaseY + im.Line() * lineHeight + lineHeight + gOffsets.y
                };

                if (consoleOpen) {
                    pos.x -= consoleWidth;
                }

                if (!cam.Contains(pos.x, pos.y, (UT::f32)gFont.size, lineHeight))
                continue;

                im.Draw((UT::ui32)pos.x, (UT::ui32)pos.y, gCharWidth); // Kind of shit, might be better for files to OWN their cursors?
            }
        
        EndScissorMode();
        
        autocomplete.DrawSuggestions(cursorX, cursorY);
    }

    UT::ui32 File::GetLineCount() const {
        return lines.size();
    }

    UT::ui32 File::GetLineLength(UT::ui32 line) const {
        if (line >= lines.size()) return 0;
        return lines.at(line).size();
    }

    void File::CreateLine(UT::ui32 line) {
        lines.emplace(lines.begin() + line, "");
        tokens.emplace(tokens.begin() + line);
    
        // Inherit block comment state from previous line, not false
        UT::b inheritedState = (line > 0 && line - 1 < lineStartsInBlockComment.size())
            ? lineStartsInBlockComment[line - 1]
            : false;
    
        lineStartsInBlockComment.emplace(lineStartsInBlockComment.begin() + line, inheritedState);
    
        InsertDirtyLine(line);
    }

    void File::CreateLine(UT::ui32 line, std::string content) {
        lines.emplace(lines.begin() + line, content);
        tokens.emplace(tokens.begin() + line);
    
        // Inherit block comment state from previous line, not false
        UT::b inheritedState = (line > 0 && line - 1 < lineStartsInBlockComment.size())
            ? lineStartsInBlockComment[line - 1]
            : false;
    
        lineStartsInBlockComment.emplace(lineStartsInBlockComment.begin() + line, inheritedState);
    
        InsertDirtyLine(line);
    }

    std::string File::SplitLine(UT::ui32 line, UT::ui32 col) {
        std::string& lineToSplit = lines.at(line);
    
        if (col > lineToSplit.size())
            col = lineToSplit.size();
    
        // Right side of the split
        std::string fragment = lineToSplit.substr(col);
    
        // Left side remains
        lineToSplit.erase(col);
        InsertDirtyLine(line);
    
        return fragment;
    }

    void File::DeleteLine(UT::ui32 line) {
        if (lines.size() > 1) {
            lines.erase(lines.begin() + line);
            tokens.erase(tokens.begin() + line);
            lineStartsInBlockComment.erase(lineStartsInBlockComment.begin() + line);
    
            // Mark the line at this position now (previously line+1) as dirty
            // Also mark line-1 so the seed is recalculated correctly
            if (line > 0) InsertDirtyLine(line - 1);
            InsertDirtyLine(std::min(line, (UT::ui32)lines.size() - 1));
        } else {
            lines[0].clear();
            InsertDirtyLine(0);
        }
    }

    void File::PushBackLineFragment(UT::ui32 sourceLine, UT::ui32 destinationLine) {
        if (sourceLine == destinationLine) return; // At start of file, do nothing

        lines.at(destinationLine).append(lines.at(sourceLine)); // Concat the fragment line to the end of the destination line
        InsertDirtyLine(destinationLine);

        // The line should probably be deleted afterwards
    }

    const std::string& File::GetCurrentLine(UT::ui32 line) const {
        return lines.at(line);
    }

    std::string& File::GetCurrentLine(UT::ui32 line) {
        return lines.at(line);
    }

    UT::b File::Dirt(void) const {
        return dirty;
    }

    void File::SetDirt(UT::b stat) {
        dirty = stat;
    }

    const std::string& File::Name(void) const {
        return name;
    }

    const std::string File::Info(void) const {
        std::string info = 
            "File Path:  " + path + "\n" +
            "File Name:  " + name + "\n" +
            "File Status: \n" +
            "Line Count: " + std::to_string(lines.size()) + "\n" +
            "Dirty:      " + (dirty ? "Yes" : "No");
        
        return info;
    }

    FileExtension File::Extension(void) const {
        return ext;
    }

    std::vector<std::string>& File::GetLines(void) {
        return lines;
    }

    InfileAutocomplete& File::Auto(void) {
        return autocomplete;
    }

    std::vector<std::vector<Token>>& File::Tokens(void) {
        return tokens;
    }
    
    UT::b File::AddMark(UT::ui32 l) {
        for (auto it = marks.begin() ; it != marks.end() ; ++it) {
            if (it->Line() == l) {
                return false; // exists
            }
        }

        InfileMark im = InfileMark(l);

        marks.push_back(im);

        return true; // created
    }
    
    UT::b File::RemoveMark(UT::ui32 l) {
        for (auto it = marks.begin(); it != marks.end(); ++it) {
            if (it->Line() == l) {
                marks.erase(it);
    
                // Reindex all marks
                UT::ui32 index = 0;
                for (auto& mark : marks) {
                    mark.ReIndex(index++);
                }
    
                gMarkIDFactory = marks.size();
    
                return true;
            }
        }

        return false;
    }
} // CBLT