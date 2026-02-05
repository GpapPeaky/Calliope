#include "CBLT_Language.hpp"

std::unordered_set<std::string_view> CBLT::gKeywords;

void CBLT::AssignLanguageKeywords(CBLT::FileExtension fe) {
    CBLT::gKeywords.clear(); // Clear before loading the new keywords.
    
    using namespace CBLT;

    switch (fe) {
        case FileExtension::CRS:
            
            break;
        case FileExtension::C:
            CBLT::gKeywords = {
                "auto","break","case","char","const","continue","default","do",
                "double","else","enum","extern","float","for","goto","if","inline",
                "int","long","register","restrict","return","short","signed",
                "sizeof","static","struct","switch","typedef","union","unsigned",
                "void","volatile","while",
                "_Alignas","_Alignof","_Atomic","_Bool","_Complex","_Generic",
                "_Imaginary","_Noreturn","_Static_assert","_Thread_local"
            };

            break;
        case FileExtension::CPP:
            CBLT::gKeywords = {
                "alignas","alignof","and","and_eq","asm","bitand","bitor","bool",
                "catch","char8_t","char16_t","char32_t","class","co_await",
                "co_return","co_yield","compl","concept","consteval","constexpr",
                "constinit","decltype","delete","dynamic_cast","explicit","export",
                "false","friend","import","module","mutable","namespace","new",
                "noexcept","not","not_eq","nullptr","operator","or","or_eq",
                "private","protected","public","reinterpret_cast","requires",
                "static_assert","template","this","thread_local","throw","true",
                "try","typeid","typename","using","virtual","wchar_t","xor","xor_eq"
            };

            break;
        case FileExtension::CS:
            CBLT::gKeywords = {
                "abstract","as","base","bool","break","byte","case","catch","char",
                "checked","class","const","continue","decimal","default","delegate",
                "do","double","else","enum","event","explicit","extern","false",
                "finally","fixed","float","for","foreach","goto","if","implicit",
                "in","int","interface","internal","is","lock","long","namespace",
                "new","null","object","operator","out","override","params","private",
                "protected","public","readonly","ref","return","sbyte","sealed",
                "short","sizeof","stackalloc","static","string","struct","switch",
                "this","throw","true","try","typeof","uint","ulong","unchecked",
                "unsafe","ushort","using","virtual","void","volatile","while",
                "with","yield",
            
                // contextual
                "add","alias","ascending","async","await","by","descending",
                "dynamic","equals","from","get","global","group","init","into",
                "join","let","nameof","nint","notnull","nuint","on","orderby",
                "partial","record","remove","select","set","unmanaged","value",
                "var","when","where"
            };
            
            break;
        case FileExtension::ASM:
            CBLT::gKeywords = {
                // Data movement
                "mov","movsx","movzx","push","pop","lea","xchg",
            
                // Arithmetic
                "add","sub","mul","imul","div","idiv","inc","dec","neg",
            
                // Logic
                "and","or","xor","not","test","shl","shr","sal","sar","rol","ror",
            
                // Control flow
                "jmp","je","jne","jg","jge","jl","jle","ja","jae","jb","jbe",
                "jo","jno","js","jns","jc","jnc","jz","jnz","loop","loope",
                "loopne","call","ret","iret","syscall","sysret","int","into",
            
                // Comparison
                "cmp","cmpsb","cmpsw","cmpsd","cmpsq","scasb","scasw","scasd","scasq",
            
                // String ops
                "lodsb","lodsw","lodsd","lodsq",
                "stosb","stosw","stosd","stosq",
                "rep","repe","repne","repz","repnz",
            
                // Flags
                "clc","stc","cmc","cli","sti","cld","std","lahf","sahf","pushf","popf",
            
                // Floating point / SIMD (common)
                "fld","fst","fstp","fadd","fsub","fmul","fdiv",
                "movaps","movups","addps","subps","mulps","divps",
            
                // Misc
                "nop","hlt","pause","cpuid"
            };

            break;
        case FileExtension::JAVA:
            CBLT::gKeywords = {
                "abstract","assert","boolean","break","byte","case","catch","char",
                "class","const","continue","default","do","double","else","enum",
                "extends","final","finally","float","for","goto","if","implements",
                "import","instanceof","int","interface","long","native","new",
                "package","private","protected","public","return","short","static",
                "strictfp","super","switch","synchronized","this","throw","throws",
                "transient","try","void","volatile","while",
            
                // literals / newer
                "true","false","null","var","record","sealed","permits",
                "non-sealed","yield"
            };
            
            break;
        case FileExtension::RS: // TODO: Finish language keywords

            break;
        case FileExtension::D:

            break;
        case FileExtension::GO:

            break;
        case FileExtension::ZIG:

            break;
        case FileExtension::NIM:

            break;
        case FileExtension::SWIFT:

            break;
        case FileExtension::KT:

            break;
        case FileExtension::GROOVY:

            break;
        case FileExtension::SCALA:

            break;
        case FileExtension::HTML:

            break;
        case FileExtension::CSS:

            break;
        case FileExtension::SCSS:

            break;
        case FileExtension::LESS:

            break;
        case FileExtension::JS:

            break;
        case FileExtension::JSX:

            break;
        case FileExtension::TS:

            break;
        case FileExtension::TSX:

            break;
        case FileExtension::WASM:

            break;
        case FileExtension::PY:

            break;
        case FileExtension::RB:

            break;
        case FileExtension::PHP:

            break;
        case FileExtension::PL:

            break;
        case FileExtension::LUA:

            break;
        case FileExtension::SH:

            break;
        case FileExtension::PS1:

            break;
        case FileExtension::JSON:

            break;
        case FileExtension::YAML:

            break;
        case FileExtension::YML:

            break;
        case FileExtension::XML:

            break;
        case FileExtension::TOML:

            break;
        case FileExtension::INI:

            break;
        case FileExtension::ENV:

            break;
        case FileExtension::SQL:

            break;
        case FileExtension::GRAPHQL:

            break;
        case FileExtension::MD:

            break;
        case FileExtension::RST:

            break;
        case FileExtension::TXT:

            break;
        case FileExtension::HCL:

            break;
        case FileExtension::HS:

            break;
        case FileExtension::ML:

            break;
        case FileExtension::FSHARP:

            break;
        case FileExtension::LISP:

            break;
        case FileExtension::SCM:

            break;
        case FileExtension::RKT:

            break;
        case FileExtension::PROLOG:

            break;
        case FileExtension::VHDL:

            break;
        case FileExtension::VERILOG:

            break;
        case FileExtension::GLSL:

            break;
        case FileExtension::HLSL:

            break;
        case FileExtension::WGSL:

            break;
        case FileExtension::GD:

            break;
        case FileExtension::GDSHADER:

            break;
        case FileExtension::PROTO:

            break;
        case FileExtension::THRIFT:

            break;
        case FileExtension::ANTLR:

            break;
        case FileExtension::REGEX:

            break;

        default:
            // TXT
            break;
    }
}

bool CBLT::IsKeyword(std::string_view sv) {
    return CBLT::gKeywords.contains(sv);
}
