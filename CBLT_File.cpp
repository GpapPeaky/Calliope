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

    void File::Draw(Camera& cam) { // FIXME: file text is drawn too much the left and right
        float lineHeight = gFont.size;
    
        for(size_t i = 0; i < lines.size(); i++) {
            Vector2 pos = {
                CBLT::FileMargins::Text::LEFT_FROM_FILE_LINES_UI + CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y + CBLT::FileMargins::UI::LEFT_FROM_FILE_LINES
                + CBLT::gOffsets.x,
                CBLT::UI::TOP_BAR_HEIGHT + i * lineHeight + lineHeight + CBLT::gOffsets.y
            };

            // Draw Text/Line seperator
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
    
            // Check if inside the camera, if not do not draw
            if (!cam.Contains(
                (UT::i32)pos.x,
                (UT::i32)(pos.y - CBLT::UI::TOP_BAR_HEIGHT),
                GetScreenWidth(),
                (UT::i32)lineHeight
            )) continue;

            // Draw the actual line
            DrawTextEx(
                gFont.f,
                lines.at(i).c_str(),
                pos,
                gFont.size,
                0.0f,
                Color{0, 255, 0, 255}
            );

            pos.x = CBLT::FileMargins::Lines::LEFT_FROM_WINDOW_Y;
            pos.y = CBLT::UI::TOP_BAR_HEIGHT + i * lineHeight + lineHeight;

            // Draw line count
            DrawTextEx(
                gFont.f,
                std::to_string(i).c_str(),
                pos,
                gFont.size,
                0.0f,
                Color{0, 255, 0, 255}                
            );
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
}