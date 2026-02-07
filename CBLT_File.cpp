#include "CBLT_File.hpp"

namespace CBLT {
    File gNAF;
    UT::b gInBlockComment = false;

    void File::LexLine(const std::string& s, UT::ui32 line) {
        UT::ui32 i = 0;

        while (i < s.size()) {
            char c = s[i];
    
            // Comment block entry
            if (c == '/' && i + 1 < s.size() && s[i + 1] == '*') {
                gInBlockComment = true;
            }

            if (gInBlockComment) {
                UT::ui32 start = i++;
            
                while (i < s.size()) {
                    char ch = s[i++];
            
                    if (ch == '*' && s[i] == '/') {
                        i++; // Include the '/'
                        gInBlockComment = false;
                        break; // Closer
                    }
                }
            
                tokens.push_back({
                    TokenClass::COMMENT,
                    line,
                    start,
                    i - start
                });

                continue;
            }

            // String literal
            if (c == '"' || c == '\'') {
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
            
                tokens.push_back({
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
    
                tokens.push_back({ type, line, start, i - start });
                continue;
            }
    
            // Number
            if (std::isdigit(c)) {
                UT::ui32 start = i++;
                while (i < s.size() && std::isdigit(s[i])) i++;
    
                tokens.push_back({ TokenClass::NUM, line, start, i - start });
                continue;
            }
    
            // Whitespace
            if (std::isspace(c)) {
                UT::ui32 start = i++;
                while (i < s.size() && std::isspace(s[i])) i++;
    
                tokens.push_back({ TokenClass::WHITESPACE, line, start, i - start });
                continue;
            }

            // -------------------------------------------------------------------------------------------------------------------------------------------------
            // Lang specific lexing (comments, misc)
            // -------------------------------------------------------------------------------------------------------------------------------------------------

            switch (ext) { // FIXME: Weird
                case EXT(C):
                    if (c == '/' && i + 1 < s.size() && s[i + 1] == '/') {
                        tokens.push_back({
                            TokenClass::COMMENT,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return;
                    }

                    if (c == '#' && i + 1 < s.size()) {
                        tokens.push_back({
                            TokenClass::MISC,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return;
                    }
                    break;
                case EXT(CPP):
                    if (c == '/' && i + 1 < s.size() && s[i + 1] == '/') {
                        tokens.push_back({
                            TokenClass::COMMENT,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return;
                    }

                    if (c == '#' && i + 1 < s.size()) {
                        tokens.push_back({
                            TokenClass::MISC,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return;
                    }
                    break;
                case EXT(CS):
                    if (c == '/' && i + 1 < s.size() && s[i + 1] == '/') {
                        tokens.push_back({
                            TokenClass::COMMENT,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return;
                    }
                    break;
                case EXT(JAVA):
                    if (c == '/' && i + 1 < s.size() && s[i + 1] == '/') {
                        tokens.push_back({
                            TokenClass::COMMENT,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return;
                    }
                    break;

                case EXT(ASM): 
                    if (c == ';' || c == '#') {
                        tokens.push_back({
                            TokenClass::COMMENT,
                            line,
                            i,
                            static_cast<UT::ui32>(s.size() - i)
                        });

                        return;
                    }
                    break;

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
                tokens.push_back({
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
            tokens.push_back({ TokenClass::PUNCTUATION, line, i, 1 });
            ++i;
        }
    }

    void InitNAF(void) {
        const UT::ui32 nameSize = rand() % 256;
        std::string NAFname;
        
        for (unsigned int i = 0; i < nameSize; i++) {
            char c = 32 + (rand() % (127 - 32)); // random printable ASCII
            NAFname.push_back(c);
        }
        
        gNAF.SetName(NAFname);

        std::cout << "CBLT_LOG: NAFname: " <<  NAFname << "\n";
    }

    UT::b IsNAF(std::string filename) {
        if (filename == gNAF.Name()) {
            return true;
        }

        return false;
    }

    File::File(void) {
        lines.emplace_back("");
    }

    File::~File(void) {}

    void File::Tokenize(void) {
        tokens.clear();

        for (UT::ui32 line = 0; line < lines.size(); ++line) {
            LexLine(lines[line], line);
        }
    }

    void File::SetName(std::string name) {
        this->name = name;
    }
        
    UT::b File::Load(const std::string& fpath, const std::string& cwd) { // FIXME: Crashes sometimes
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
    
        // Language support stuff
        ext = AssignExtension(path);
        AssignLanguageKeywords(ext);

        dirty = false;
    
        Tokenize(); // Suck ass

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

        // Might need to emplace a new empty line here?

        return true;
    }

    void File::InsertChar(UT::ui32 col, UT::ui32 line, UT::i32 c) {
        if (line >= lines.size()) return;
    
        auto& ln = lines.at(line);
    
        if (col > ln.size()) col = ln.size();
    
        ln.insert(ln.begin() + col, static_cast<char>(c));
    }
    
    const std::string& File::GetPath(void) const {
        return path;
    }

    void File::Draw(Camera& cam) {
        UT::f32 lineHeight = gFont.size;
        
        const UT::f32 textBaseX = CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI + 
                                CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + 
                                CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES;
        const UT::f32 textBaseY = 0.0f;

        // Seperator
        DrawLineV(
            { 
                CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
                CBLT::UI::TOP_BAR_HEIGHT + gFont.size - 6
            },{ 
                CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
                static_cast<UT::f32>(GetScreenHeight())
            },
            gPalette.textSeperators
        );
    
        for(UT::llui32 i = 0; i < lines.size(); i++) {
            Vector2 pos = {
                textBaseX + CBLT::gOffsets.x,
                textBaseY + i * lineHeight + lineHeight + CBLT::gOffsets.y
            };

            if (!cam.Contains(
                pos.x,
                pos.y,
                (UT::f32)cam.Width(),
                lineHeight + + UI::TOP_BAR_HEIGHT
            )) continue; // Skip non visible lines

            // THIS JUST FUCKING CLIPS, DOESN'T REDUCE THE DRAW CALL, LEARNT IT THE HARD WAY, FUCK.
            BeginScissorMode(
                cam.Origin().x + CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI,
                cam.Origin().y,
                cam.Width(),
                cam.Height() + UI::TOP_BAR_HEIGHT
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
                for (Token& t : tokens) {
                    if (t.line != i) continue;
                
                    Color col = t.TokenColor();
                    if (col.a == 0) continue; // skips whitespaces
                
                    // By counting glyphs
                    float tokX = pos.x + t.GetCursorX(lines[i].substr(0, t.col), gFont.size, t.col);
                
                    // Wooow this echoes the characters, at
                    //
                    // col 0 -> once
                    // col 1 -> twice
                    // col 2 -> three times
                    // col 4 -> ++
                    //
                    //
                    //
                    // std::string_view sv(
                        // lines[i].data() + t.col,
                        // t.len
                    // );

                    // Drawing is correct but it 
                    std::string tokenText = lines[i].substr(t.col, t.len);
                
                    DrawTextEx(
                        gFont.f,
                        tokenText.c_str(),
                        {
                            tokX, pos.y
                        },
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
                cam.Height() + UI::TOP_BAR_HEIGHT
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
    }

    UT::ui32 File::GetLineCount() const {
        return lines.size();
    }

    UT::ui32 File::GetLineLength(UT::ui32 line) const {
        if (line >= lines.size()) return 0;
        return lines.at(line).size();
    }

    void File::CreateLine(UT::ui32 line) {
        lines.emplace(lines.begin() + line, std::string("")); // Place an empty line
    }

    void File::CreateLine(UT::ui32 line, std::string content) {
        lines.emplace(lines.begin() + line, content); // Place the provided string
    }

    std::string File::SplitLine(UT::ui32 line, UT::ui32 col) {
        std::string& lineToSplit = lines.at(line);
    
        if (col > lineToSplit.size())
            col = lineToSplit.size();
    
        // Right side of the split
        std::string fragment = lineToSplit.substr(col);
    
        // Left side remains
        lineToSplit.erase(col);
    
        return fragment;
    }

    void File::DeleteLine(UT::ui32 line) {
        if (lines.size() > 1) {
            lines.erase(lines.begin() + line);
        } else {
            lines.at(0).clear();
        }
    }

    void File::PushBackLineFragment(UT::ui32 sourceLine, UT::ui32 destinationLine) {
        if (sourceLine == destinationLine) return; // At start of file, do nothing

        lines.at(destinationLine).append(lines.at(sourceLine)); // Concat the fragment line to the end of the destination line

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
}