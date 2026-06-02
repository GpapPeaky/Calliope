#include "CBLT_Font.hpp"

namespace CBLT {
    EditorFont::EditorFont(void) :
        size(20),
        f()
    {}

    EditorFont::~EditorFont(void) {}

    std::vector<UT::i32> EditorFont::BuildGlyphSet(void) {
        std::vector<UT::i32> glyphs;
    
        // Basic ASCII
        for (int i = 32; i < 127; i++) {
            glyphs.push_back(i);
        }
    
        // Box Drawing ─ │ ┌ ┐ └ ┘ etc.
        for (int i = 0x2500; i <= 0x257F; i++) {
            glyphs.push_back(i);
        }
    
        // Block Elements ░ ▒ ▓ █
        for (int i = 0x2580; i <= 0x259F; i++) {
            glyphs.push_back(i);
        }
    
        return glyphs;
    }

    void EditorFont::Load(std::string fontName) {
        auto glyphs = BuildGlyphSet();
    
        f = LoadFontEx(
            fontName.c_str(),
            size,
            glyphs.data(),
            glyphs.size()
        );
    
        SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);

        name = fontName; // Saved for resizing
    }

    void EditorFont::Config(void) {
        advanceMap.clear();
        for (UT::i32 g = 0; g < this->f.glyphCount; g++)
            this->advanceMap[this->f.glyphs[g].value] = this->f.glyphs[g].advanceX;
    }

    std::vector<UT::i32> EditorFont::Utf8ToCodepoints(const std::string& str) {
        std::vector<UT::i32> cps;
        for(size_t i = 0; i < str.size(); ) {
            unsigned char c = str[i];
            UT::i32 cp = 0;
            UT::i32 len = 0;
    
            if(c < 0x80) { cp = c; len = 1; }
            else if((c & 0xE0) == 0xC0) { cp = c & 0x1F; len = 2; }
            else if((c & 0xF0) == 0xE0) { cp = c & 0x0F; len = 3; }
            else if((c & 0xF8) == 0xF0) { cp = c & 0x07; len = 4; }
    
            for(UT::i32 j = 1 ; j < len ; j++) {
                cp = (cp << 6) | (str[i + j] & 0x3F);
            }
    
            cps.push_back(cp);
            i += len;
        }
        return cps;
    }

    std::vector<UT::i32> EditorFont::Utf8ToCodepoints(const std::string_view str) {
        std::vector<UT::i32> cps;
        cps.reserve(str.size()); // approximate reserve
    
        for(size_t i = 0; i < str.size(); ) {
            unsigned char c = str[i];
            UT::i32 cp = 0;
            UT::i32 len = 0;
    
            if(c < 0x80) { cp = c; len = 1; }
            else if((c & 0xE0) == 0xC0) { cp = c & 0x1F; len = 2; }
            else if((c & 0xF0) == 0xE0) { cp = c & 0x0F; len = 3; }
            else if((c & 0xF8) == 0xF0) { cp = c & 0x07; len = 4; }
    
            for(UT::i32 j = 1; j < len; j++) {
                cp = (cp << 6) | (str[i + j] & 0x3F);
            }
    
            cps.push_back(cp);
            i += len;
        }
    
        return cps;
    }

    EditorFont gFont;

    EditorFont gConsoleFont;

    EditorFont gTopBarFont;

    EditorFont gFileQueueFont;
} // CBLT
