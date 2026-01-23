#include "CBLT_File.hpp"

namespace CBLT {
    File::File() {
        lines.emplace_back("");
    }
        
    UT::b File::Load(const std::string& fpath) {
        namespace fs = std::filesystem;

        std::ifstream file(fpath);
        if(!file.is_open())
            return false;

        fs::path absPath = fs::absolute(fpath);
    
        Clear(); // Clear the previous open file
        
        path = fpath;
        cwd = absPath.parent_path().string(); 
    
        std::string line;

        while(std::getline(file, line)) {
            lines.push_back(line);
        }
    
        if(lines.empty())
            lines.emplace_back("");
    
        AssignExtension();

        dirty = false;

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
    
        for(UT::llui32 i = 0; i < lines.size(); i++) {
            Vector2 pos = {
                textBaseX + CBLT::gOffsets.x,
                textBaseY + i * lineHeight + lineHeight + CBLT::gOffsets.y
            };
            
            DrawLineV(
                { 
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
                    CBLT::UI::TOP_BAR_HEIGHT + gFont.size - 6
                },{ 
                    CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES,
                    static_cast<UT::f32>(GetScreenHeight())
                },
                Color{0, 255, 0, 255}
            );
            
            BeginScissorMode(
                cam.Origin().x + CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES + CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI,
                cam.Origin().y,
                cam.Width(),
                cam.Height()
            );
    
            DrawTextEx(
                gFont.f,
                lines.at(i).c_str(),
                pos,
                gFont.size,
                0.0f,
                Color{0, 255, 0, 255}
            );
            EndScissorMode();
            
            pos.x = CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y;
            pos.y = textBaseY + i * lineHeight + lineHeight + gOffsets.y;
            
            BeginScissorMode(
                cam.Origin().x,
                cam.Origin().y,
                cam.Width(),
                cam.Height()
            );
            
            DrawTextEx(
                gFont.f,
                std::to_string(i).c_str(),
                pos,
                gFont.size,
                0.0f,
                Color{0, 255, 0, 255}                
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

    const UT::b File::Dirt(void) const {
        return dirty;
    }

    void File::SetDirt(UT::b stat) {
        dirty = stat;
    }

    const std::string& File::Name(void) const {
        return path;        
    }

    const std::string& File::CWD(void) const {
        return cwd;
    }

    const std::string File::Info(void) const {
        std::string info = "File Path: " + path + "\n" +
               "CWD: " + cwd + "\n" +
               "Line Count: " + std::to_string(lines.size()) + "\n" +
               "Dirty: " + (dirty ? "Yes" : "No");
        
        return info;
    }

    void File::AssignExtension(void) {
        UT::llui32 extPos = path.find_last_of('.');

        // We now hold the string '.<ext>'
        std::string extStr = path.substr(extPos);

        // Cobalt scripting language
        if (extStr == ".csl") {
            ext = EXT(CSL);
        }

        // Other

        if (extStr == ".c" || extStr == ".h" || extStr == ".i" || extStr == ".aux") {
            ext = EXT(C);
        }

        if (extStr == ".cpp" || extStr == ".cc" || extStr == ".cxx" || extStr == ".C" ||
            extStr == ".CPP" || extStr == ".cp" || extStr == ".c++" ||
            extStr == ".hpp" || extStr == ".hh" || extStr == ".hxx" || extStr == ".H" ||
            extStr == ".HPP" || extStr == ".hp" || extStr == ".h++" ||
            extStr == ".ii" || extStr == ".ixx" || extStr == ".aux") 
        {
            ext = EXT(CPP);
        }
        
        if (extStr == ".s" || extStr == ".S" || extStr == ".asm" || extStr == ".inc" ||
            extStr == ".a51" || extStr == ".a80" || extStr == ".a86") 
        {
            ext = EXT(ASM);
        }
        
        if (extStr == ".java" || extStr == ".jav") {
            ext = EXT(JAVA);
        }
        
        if (extStr == ".rs") {
            ext = EXT(RS);
        }
        
        if (extStr == ".d" || extStr == ".di") {
            ext = EXT(D);
        }

        if (extStr == ".go") {
            ext = EXT(GO);
        }

        if (extStr == ".zig") {
            ext = EXT(ZIG);
        }

        if (extStr == ".nim") {
            ext = EXT(NIM);
        }

        if (extStr == ".swift") {
            ext = EXT(SWIFT);
        }

        if (extStr == ".kt" || extStr == ".kts") {
            ext = EXT(KT);
        }
        
        if (extStr == ".groovy") {
            ext = EXT(GROOVY);
        }

        if (extStr == ".scala") {
            ext = EXT(SCALA);
        }

        if (extStr == ".html" || extStr == ".htm") {
            ext = EXT(HTML);
        }

        if (extStr == ".css") {
            ext = EXT(CSS);
        }

        if (extStr == ".scss") {
            ext = EXT(SCSS);
        }

        if (extStr == ".less") {
            ext = EXT(LESS);
        }

        if (extStr == ".js") {
            ext = EXT(JS);
        }

        if (extStr == ".jsx") {
            ext = EXT(JSX);
        }

        if (extStr == ".ts") {
            ext = EXT(TS);
        }

        if (extStr == ".tsx") {
            ext = EXT(TSX);
        }

        if (extStr == ".wat") {
            ext = EXT(WASM);
        }

        if (extStr == ".py") {
            ext = EXT(PY);
        }

        if (extStr == ".rb") {
            ext = EXT(RB);
        }

        if (extStr == ".php") {
            ext = EXT(PHP);
        }

        if (extStr == ".pl") {
            ext = EXT(PL);
        }

        if (extStr == ".lua") {
            ext = EXT(LUA);
        }

        if (extStr == ".sh") {
            ext = EXT(SH);
        }

        if (extStr == ".ps1") {
            ext = EXT(PS1);
        }

        if (extStr == ".json") {
            ext = EXT(JSON);
        }

        if (extStr == ".yaml") {
            ext = EXT(YAML);
        }

        if (extStr == ".yml") {
            ext = EXT(YML);
        }

        if (extStr == ".xml") {
            ext = EXT(XML);
        }

        if (extStr == ".toml") {
            ext = EXT(TOML);
        }

        if (extStr == ".ini") {
            ext = EXT(INI);
        }

        if (extStr == ".env") {
            ext = EXT(ENV);
        }

        if (extStr == ".sql") {
            ext = EXT(SQL);
        }

        if (extStr == ".graphql" || extStr == "gql") {
            ext = EXT(GRAPHQL);
        }

        if (extStr == ".md") {
            ext = EXT(MD);
        }

        if (extStr == ".rst") {
            ext = EXT(RST);
        }

        if (extStr == ".txt") {
            ext = EXT(TXT);
        }

        if (extStr == ".hcl" || extStr == ".tf") {
            ext = EXT(HCL);
        }

        if (extStr == ".hs") {
            ext = EXT(HS);
        }

        if (extStr == ".ml" || extStr == ".mli") {
            ext = EXT(ML);
        }

        if (extStr == ".fs" || extStr == ".fsi" || extStr == "fsx") {
            ext = EXT(FSHARP);
        }

        if (extStr == ".lisp" || extStr == ".lsp") {
            ext = EXT(LISP);
        }

        if (extStr == ".scm") {
            ext = EXT(SCM);
        }

        if (extStr == ".rkt") {
            ext = EXT(RKT);
        }

        if (extStr == ".pl" || extStr == ".pro") {
            ext = EXT(PROLOG);
        }

        if (extStr == ".vhdl" || extStr == ".vhd") {
            ext = EXT(VHDL);
        }

        if (extStr == ".v" || extStr == ".vh") {
            ext = EXT(VERILOG);
        }

        if (extStr == ".glsl" || extStr == ".vert" || extStr == ".frag" || extStr == ".geom" || extStr == ".comp") {
            ext = EXT(GLSL);
        }

        if (extStr == ".hlsl" || extStr == ".fx" || extStr == ".fsh" || extStr == ".vsh" || extStr == ".csh") {
            ext = EXT(HLSL);
        }

        if (extStr == ".wgsl") {
            ext = EXT(WGSL);
        }

        if (extStr == ".gd") {
            ext = EXT(GD);
        }
    }

    const FileExtension File::Extension(void) const {
        return ext;
    }
}