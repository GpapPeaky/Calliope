#include "CBLT_File.hpp"

namespace CBLT {
    File::File(void) {
        lines.emplace_back("");
    }

    File::~File(void) {}
        
    UT::b File::Load(const std::string& fpath, const std::string& cwd) {
        namespace fs = std::filesystem;
    
        fs::path fullPath = fs::path(cwd) / fpath;
        fullPath = fs::absolute(fullPath);
    
        std::ifstream file(fullPath);
        if (!file.is_open())
            return false;
    
        Clear();
    
        path = fullPath.string();
        name = fpath; // File name
    
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
    
        if (lines.empty())
            lines.emplace_back("");
    
        ext = AssignExtension(path);
        dirty = false;
    
        return true;
    }
            
    UT::b File::Save(void) {  // FIXME: Cannot write to file where path is not utf8/utf16 encoded, if seen as ASCII it simply ignores it
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
                DrawTextEx(
                    gFont.f,
                    lines.at(i).c_str(),
                    pos,
                    gFont.size,
                    0.0f,
                    gPalette.textBase
                );
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