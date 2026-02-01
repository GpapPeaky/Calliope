#include "CBLT_FileExtension.hpp"

namespace CBLT {
     FileExtension AssignExtension(std::string path) {
        UT::llui32 extPos = path.find_last_of('.');
        FileExtension ext;

        if (extPos == std::string::npos) {
            ext = EXT(TXT); // No extension, handle the file as a normal .txt

            return ext;
        }

        // We now hold the string '.<ext>'
        std::string extStr = path.substr(extPos);

        // Cobalt scripting language
        if (extStr == ".crs") {
            ext = EXT(CRS);
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

        if (extStr == ".gdshader") {
            ext = EXT(GDSHADER);
        }

        if (extStr == ".proto") {
            ext = EXT(PROTO);
        }

        if (extStr == ".thrift") {
            ext = EXT(THRIFT);
        }

        if (extStr == ".g4") {
            ext = EXT(ANTLR);
        }

        if (extStr == ".regex" || extStr == ".re") {
            ext = EXT(REGEX);
        }

        return ext;
    }
} // CBLT