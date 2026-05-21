#include "CBLT_File.hpp"

namespace CBLT {
    File::File(void) {
        lines.emplace_back("");

        tokens.resize(1);
        lineStates.resize(1, LexerState::NONE);

        lang = Language(FileExtension::TXT);

        markIdFactory = marks.size();
    }

    File::~File(void) {}

    void File::SetName(std::string name) {
        this->name = name;
    }
        
    UT::b File::Load(const std::string& fpath, const std::string& cwd) {
        namespace fs = std::filesystem;
    
        std::string pathString = (fs::path(cwd) / fpath).string();

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
        FileExtension ext = AssignExtension(path);

        // Seek cong name in the gLangFiles
        auto it = gLangFiles.find(ext);
        if (it != gLangFiles.end()) {
            lang.ReadLangFile(it->second); // Load language
            langConf = it->second;
        } else {
            lang.ReadLangFile("txt.cbltconf"); // TXT as base Fallback, extension unrecognized
            langConf = "txt.cbltconf";
        }

        dirty = false;
    
        Tokenize(); // Suck ass

        autocomplete.LoadTokens(tokens, lines, lang);

        LoadMarks();

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

        SaveMarks();

        return true;
    }

    UT::b File::Clear(void) {
        lines.clear();

        tokens.clear();
        lineStates.clear();
    
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
                CBLT::FileMargins::UI::TOP_BAR_HEIGHT + gConsoleFont.size
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
                    Color col = t.TokenColor();

                    // skips whitespaces
                    if (col.a == 0) continue;
                
                    const std::string& lineStr = lines[i];
                
                    if (t.col >= lineStr.size())
                        continue;
                
                    UT::ui32 maxLen = lineStr.size() - t.col;
                    UT::ui32 len = std::min(t.len, maxLen);
                
                    if (len == 0)
                        continue;
                
                    std::string_view tokenText(lineStr.data() + t.col, len);
                
                    UT::f32 tokX = pos.x + t.GetCursorX(std::string_view(lines[i]), gFont.size, t.col);
                
                    DrawTextEx(
                        gFont.f,
                        std::string(tokenText).c_str(),
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
                    GetScreenWidth() - 40.0f,
                    textBaseY + im.Line() * lineHeight + lineHeight + gOffsets.y
                };

                if (consoleOpen) {
                    pos.x -= consoleWidth;
                }

                if (!cam.Contains(pos.x, pos.y, (UT::f32)gFont.size, lineHeight))
                continue;

                im.Draw((UT::ui32)pos.x, (UT::ui32)pos.y, 40); // Kind of shit, might be better for files to OWN their cursors?
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
        LexerState inheritedState = (line > 0 && line - 1 < lineStates.size())
            ? lineStates[line - 1]
            : LexerState::NONE;
    
        lineStates.emplace(lineStates.begin() + line, inheritedState);
    
        InsertDirtyLine(line);

        assert(lines.size() == tokens.size());
        assert(lines.size() == lineStates.size());
    }

    void File::CreateLine(UT::ui32 line, std::string content) {
        lines.emplace(lines.begin() + line, content);
        tokens.emplace(tokens.begin() + line);
    
        // Inherit block comment state from previous line, not false
        LexerState inheritedState = (line > 0 && line - 1 < lineStates.size())
            ? lineStates[line - 1]
            : LexerState::NONE;
    
        lineStates.emplace(lineStates.begin() + line, inheritedState);
    
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
            lineStates.erase(lineStates.begin() + line);
    
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

        im.ReIndex(markIdFactory++);

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
    
                markIdFactory = marks.size();
    
                return true;
            }
        }

        return false;
    }

    std::vector<InfileMark>& File::Marks(void) {
        return marks;
    }

    UT::ui32& File::MarkIdFactory(void) {
        return markIdFactory;
    }

    CursorManager& File::Cursors(void) {
        return cursors;
    }

    void File::ClampCursor(Cursor& c) {
        UT::ui32 newLine = std::min(c.Line(), static_cast<UT::ui32>(GetLineCount()) - 1);

        c.SetAt(
            std::min(c.Col(), static_cast<UT::ui32>(GetLineLength(c.Line()))),
            newLine,
            GetCurrentLine(newLine)
        );
    }

    void File::SaveMarks(void) const {
        if (path.empty() || name.empty()) return;
    
        namespace fs = std::filesystem;

        std::string installationPath;

        #if defined(__linux__)
            const char* resource_path = getenv("CBLT_RESOURCES");
            installationPath = resource_path ? std::string(resource_path) : ".";
        #endif
    
        fs::path dir = fs::path(installationPath) / "meta" / "marks";
    
        // Always safe
        if (!fs::create_directories(dir) && !fs::exists(dir)) {
            CBLT::Utils::Err::Log("FAILED TO CREATE DIRECTORY: " + dir.string());
            return;
        }
    
        std::string markFname = CBLT::Utils::Func::PathToMarkFileName(path);
    
        std::string encodedFname = CBLT::Utils::Func::MarkFileEncode(markFname) + ".marks";
        fs::path sidecarPath = dir / encodedFname;
    
        std::ofstream out(sidecarPath, std::ios::out | std::ios::trunc);
    
        if (!out.is_open()) {
            CBLT::Utils::Err::Log("FAILED TO OPEN MARKS FILE: " + sidecarPath.string());
            return;
        }
    
        out << "%marks\n";
    
        for (const auto& mark : marks) {
            out << mark.Line() << "\n";
        }
    
        out << "%marks\n";
    
        CBLT::Utils::Err::Log("MARKINGS SAVED FOR: " + name);
    }

    void File::LoadMarks(void) {
        if (path.empty() || name.empty()) return;
    
        namespace fs = std::filesystem;
    
        std::string installationPath;

        #if defined(__linux__)
            const char* resource_path = getenv("CBLT_RESOURCES");
            installationPath = resource_path ? std::string(resource_path) : ".";
        #endif

        std::string markFname = CBLT::Utils::Func::PathToMarkFileName(path);
    
        std::string encodedFname = CBLT::Utils::Func::MarkFileEncode(markFname) + ".marks";
        fs::path sidecarPath = fs::path(installationPath) / "meta" / "marks" / encodedFname;
    
        if (!fs::exists(sidecarPath)) {
            // Optional log
            CBLT::Utils::Err::Log("NO MARKS FILE FOUND FOR: " + name);
            return;
        }
    
        std::ifstream in(sidecarPath);
    
        if (!in.is_open()) {
            CBLT::Utils::Err::Log("FAILED TO OPEN MARKS FILE: " + sidecarPath.string());
            return;
        }
    
        marks.clear();
        markIdFactory = 0;
    
        std::string line;
        bool inBlock = false;
    
        while (std::getline(in, line)) {
            line = UF::Trim(line);
    
            if (line.empty()) continue;
    
            if (line == "%marks") {
                if (!inBlock) {
                    inBlock = true;
                } else {
                    break; // stop after closing block (safer)
                }
                continue;
            }
    
            if (!inBlock) continue;
    
            try {
                UT::ui32 lineNum = static_cast<UT::ui32>(std::stoul(line));
    
                if (lineNum < lines.size()) {
                    InfileMark im(lineNum);
                    im.ReIndex(markIdFactory++);
                    marks.push_back(im);
                } else {
                    CBLT::Utils::Err::Log("MARK OUT OF BOUNDS SKIPPED: " + line);
                }
            } catch (...) {
                CBLT::Utils::Err::Log("MARK PARSE ERROR SKIPPED: " + line);
            }
        }
    
        CBLT::Utils::Err::Log("LOADED MARKS: " + name);
    }

    std::string File::GetMarksAndNearbyLinesMessageString(void) const {
        if (marks.empty()) return "NO MARKS";

        std::string result;

        for (const auto& mark : marks) {
            UT::ui32 lineNum = mark.Line();
            result += "MARK " + std::to_string(mark.Id()) + " AT LINE:" + std::to_string(lineNum) + "\n"
            + "     | " + (lineNum > 0 ? lines[lineNum - 1] : "<START OF FILE>") + "\n"
            + "->   | " + lines[lineNum] + "\n"
            + "     | " + (lineNum + 1 < lines.size() ? lines[lineNum + 1] : "<END OF FILE>") + "\n\n";
        }

        return result;
    }

    // File lang support and tokenization

    void File::Tokenize(void) {
        tokens.clear();
        lineStates.clear();
    
        tokens.resize(lines.size());
        lineStates.resize(lines.size());
    
        LexerState state = LexerState::NONE;
    
        for (UT::ui32 line = 0; line < lines.size(); ++line) {
            lineStates[line] = state;
            state = LexLine(lines[line], line, state, lang, tokens[line]);
        }
    }

    void File::RetokenizeDirtyLines() {
        if (dirtyLines.empty()) return;
    
        UT::ui32 minDirty = *std::min_element(dirtyLines.begin(), dirtyLines.end());
        UT::ui32 maxDirty = *std::max_element(dirtyLines.begin(), dirtyLines.end());
    
        LexerState state = (minDirty > 0)
            ? lineStates[minDirty - 1]
            : LexerState::NONE;
    
        for (UT::ui32 i = minDirty; i < lines.size(); ++i) {
            LexerState prev = lineStates[i];
            tokens[i].clear();
            state = LexLine(lines[i], i, state, lang, tokens[i]);
            lineStates[i] = state;
    
            if (i > maxDirty && state == prev) break; // Stabilized
        }
    
        for (auto line : dirtyLines)
            autocomplete.UpdateLine(line, tokens[line], lines[line]);

        dirtyLines.clear();
    }

    std::string File::LangConf(void) {
        return langConf;
    }

    void File::InsertDirtyLine(UT::ui32 line) {
        if (line >= lines.size()) return;
    
        dirtyLines.insert(line);
    }

    Language& File::FileLanguage(void) {
        return lang;
    }
} // CBLT