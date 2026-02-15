#include "CBLT_Language.hpp"

std::unordered_set<std::string_view> CBLT::gKeywords;

void CBLT::AssignLanguageKeywords(CBLT::FileExtension fe) {
    using namespace CBLT;
    
    gKeywords.clear(); // Clear before loading the new keywords.

    switch (fe) {
        case FileExtension::CRS:
            
            break;
        case FileExtension::C:
            CBLT::gKeywords = {
                "auto","break","case","char","const","continue","default","do",
                "double","else","enum","extern","float","for","goto","if","inline",
                "int","long","register","restrict","return","short","signed",
                "sizeof","static","struct","switch","typedef","union","unsigned",
                "void","volatile","while","void",
                "_Alignas","_Alignof","_Atomic","_Bool","_Complex","_Generic",
                "_Imaginary","_Noreturn","_Static_assert","_Thread_local"
            };

            break;
        case FileExtension::CPP:
            CBLT::gKeywords = {
                "alignas","alignof","and","and_eq","asm","bitand","bitor","bool",
                "catch","char8_t","char16_t","char32_t","class","co_await","void",
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
        case FileExtension::RS:
            CBLT::gKeywords = {
                // Rust keywords
                "as","break","const","continue","crate","dyn","else","enum","extern",
                "false","fn","for","if","impl","in","let","loop","match","mod","move",
                "mut","pub","ref","return","Self","self","static","struct","super",
                "trait","true","type","unsafe","use","where","while",
            
                // reserved / contextual keywords
                "abstract","become","box","do","final","macro","override","priv","try",
                "typeof","unsized","virtual","yield",
            
                // primitive types
                "i8","i16","i32","i64","i128","isize",
                "u8","u16","u32","u64","u128","usize",
                "f32","f64",
                "bool","char","str"
            };

            break;
        case FileExtension::D:
            CBLT::gKeywords = {
                // D keywords
                "abstract","alias","align","asm","assert","auto","body","bool","break",
                "byte","case","cast","catch","cdouble","cent","cfloat","char","class",
                "const","continue","creal","dchar","debug","default","delegate","delete",
                "deprecated","do","double","else","enum","export","extern","false","final",
                "finally","float","for","foreach","foreach_reverse","function","goto","idouble",
                "if","ifloat","immutable","import","in","inout","int","interface","invariant",
                "ireal","is","lazy","long","macro","mixin","module","new","nothrow","null",
                "out","override","package","pragma","private","protected","public","pure",
                "ref","return","scope","shared","short","static","struct","super","switch",
                "synchronized","template","this","throw","true","try","typeid","typeof","ubyte",
                "ucent","uint","ulong","union","unittest","ushort","version","void","volatile",
                "wchar","while","with","__traits","__vector","__gshared",
            
                // D primitive types (for clarity)
                "bool","byte","ubyte","short","ushort","int","uint","long","ulong","cent","ucent",
                "float","double","real","ifloat","idouble","ireal","char","wchar","dchar",
            
                // literals
                "true","false","null"
            };

            break;
        case FileExtension::GO:
            CBLT::gKeywords = {
                // Go keywords
                "break","case","chan","const","continue","default","defer","else",
                "fallthrough","for","func","go","goto","if","import","interface",
                "map","package","range","return","select","struct","switch","type","var",
            
                // predeclared types
                "bool","byte","complex64","complex128","error","float32","float64",
                "int","int8","int16","int32","int64","rune","string","uint","uint8",
                "uint16","uint32","uint64","uintptr",
            
                // predeclared constants
                "true","false","iota","nil",
            
                // predeclared functions (optional, sometimes highlighted)
                "append","cap","close","complex","copy","delete","imag","len","make",
                "new","panic","print","println","real","recover"
            };
            
            break;
        case FileExtension::ZIG:
            CBLT::gKeywords = {
                // Zig keywords
                "alignof","allowzero","and","asm","async","await","break","call",
                "catch","comptime","const","continue","defer","else","enum","errdefer",
                "error","export","extern","false","fn","for","if","inline","linksection",
                "noalias","null","orelse","packed","promise","pub","return","struct",
                "suspend","switch","test","true","try","typeof","union","unreachable",
                "usingnamespace","var","volatile","while",
            
                // Zig primitive types
                "bool","u8","u16","u32","u64","u128","i8","i16","i32","i64","i128",
                "f16","f32","f64","comptime_int","comptime_float","void","anyerror",
                "noreturn","c_void","c_int","c_uint","c_long","c_ulong","c_short","c_ushort",
            
                // literals
                "true","false","null"
            };
            
            break;
        case FileExtension::NIM:
            CBLT::gKeywords = {
                // Nim keywords
                "addr","and","as","asm","atomic","bind","block","break","case","cast",
                "concept","const","continue","converter","discard","distinct","div","do",
                "elif","else","end","enum","except","export","finally","for","from","func",
                "generic","if","import","in","include","interface","is","isnot","iterator",
                "let","macro","method","mixin","mod","nil","not","notin","object","of",
                "or","out","proc","ptr","raise","ref","return","shl","shr","static",
                "template","try","tuple","type","using","var","when","while","with","without",
                "xor","yield",
            
                // primitive types
                "int","int8","int16","int32","int64","uint","uint8","uint16","uint32","uint64",
                "float32","float64","float","bool","char","string","cchar","cstring","pointer",
                "seq","array","openarray","range","set","tuple","object","variant","nil",
            
                // literals
                "true","false","nil"
            };

            break;
        case FileExtension::SWIFT:
            CBLT::gKeywords = {
                // Swift keywords
                "associatedtype","class","deinit","enum","extension","fileprivate","func",
                "import","init","inout","internal","let","open","operator","private","protocol",
                "public","rethrows","static","struct","subscript","typealias","var","break",
                "case","continue","default","defer","do","else","fallthrough","for","guard",
                "if","in","repeat","return","switch","where","while","as","Any","catch","false",
                "is","nil","super","self","Self","throw","throws","true","try","__COLUMN__",
                "__FILE__","__FUNCTION__","__LINE__",
            
                // primitive types
                "Bool","Int","Int8","Int16","Int32","Int64","UInt","UInt8","UInt16","UInt32",
                "UInt64","Float","Double","String","Character","Optional","Array","Dictionary",
                "Set","Any","AnyObject","Never","Void"
            };

            break;
        case FileExtension::KT:
            CBLT::gKeywords = {
                // Kotlin keywords
                "as","as?","break","class","continue","do","else","false","for","fun",
                "if","in","!in","interface","is","!is","null","object","package","return",
                "super","this","throw","true","try","typealias","val","var","when","while",
                
                // primitive types
                "Byte","Short","Int","Long","UByte","UShort","UInt","ULong","Float","Double",
                "Boolean","Char","String","Unit","Nothing","Any","Any?","Array","List","Set",
                "Map"
            };
            
            break;
        case FileExtension::GROOVY:
            CBLT::gKeywords = {
                // Groovy keywords
                "as","assert","break","case","catch","class","const","continue","def",
                "default","do","else","enum","extends","false","finally","for","goto","if",
                "implements","import","in","instanceof","interface","native","new","null",
                "package","private","protected","public","return","static","strictfp","super",
                "switch","synchronized","this","throw","throws","trait","transient","true",
                "try","void","volatile","while",
                
                // literals / common types
                "true","false","null","Integer","Long","Float","Double","Boolean","Character",
                "String","BigDecimal","BigInteger","List","Map","Set","def"
            };
                
            break;
        case FileExtension::SCALA:
            CBLT::gKeywords = {
                // Scala keywords
                "abstract","case","catch","class","def","do","else","extends","false","final",
                "finally","for","forSome","if","implicit","import","lazy","match","new","null",
                "object","override","package","private","protected","return","sealed","super",
                "this","throw","trait","try","true","type","val","var","while","with","yield"
            };

            break;
        case FileExtension::HTML:
            CBLT::gKeywords = {
                // HTML tags (common ones)
                "html","head","title","base","link","meta","style","script","body","section",
                "nav","article","aside","h1","h2","h3","h4","h5","h6","header","footer","address",
                "p","hr","pre","blockquote","ol","ul","li","dl","dt","dd","figure","figcaption",
                "div","a","em","strong","small","s","cite","q","dfn","abbr","data","time","code",
                "var","samp","kbd","sub","sup","i","b","u","mark","ruby","rt","rp","bdi","bdo",
                "span","br","wbr","ins","del","img","iframe","embed","object","param","video",
                "audio","source","track","canvas","map","area","svg","math","table","caption",
                "colgroup","col","tbody","thead","tfoot","tr","td","th","form","fieldset","legend",
                "label","input","button","select","datalist","optgroup","option","textarea","keygen",
                "output","progress","details","summary","menuitem","menu"
            };

            break;
        case FileExtension::CSS:
            CBLT::gKeywords = {
                // CSS properties (common ones)
                "align-content","align-items","align-self","all","animation","animation-delay",
                "animation-direction","animation-duration","animation-fill-mode","animation-iteration-count",
                "animation-name","animation-play-state","animation-timing-function","backface-visibility",
                "background","background-attachment","background-blend-mode","background-clip",
                "background-color","background-image","background-origin","background-position",
                "background-repeat","background-size","border","border-bottom","border-bottom-color",
                "border-bottom-left-radius","border-bottom-right-radius","border-bottom-style",
                "border-bottom-width","border-collapse","border-color","border-image","border-image-outset",
                "border-image-repeat","border-image-slice","border-image-source","border-image-width",
                "border-left","border-left-color","border-left-style","border-left-width","border-radius",
                "border-right","border-right-color","border-right-style","border-right-width","border-spacing",
                "border-style","border-top","border-top-color","border-top-left-radius","border-top-right-radius",
                "border-top-style","border-top-width","border-width","bottom","box-decoration-break","box-shadow",
                "box-sizing","break-after","break-before","break-inside","caption-side","caret-color","charset",
                "clear","clip","color","column-count","column-fill","column-gap","column-rule","column-rule-color",
                "column-rule-style","column-rule-width","column-span","column-width","columns","content","counter-increment",
                "counter-reset","cursor","direction","display","empty-cells","filter","flex","flex-basis","flex-direction",
                "flex-flow","flex-grow","flex-shrink","flex-wrap","float","font","font-family","font-feature-settings",
                "font-kerning","font-language-override","font-size","font-size-adjust","font-stretch","font-style",
                "font-variant","font-variant-alternates","font-variant-caps","font-variant-east-asian",
                "font-variant-ligatures","font-variant-numeric","font-variant-position","font-weight","gap",
                "grid","grid-area","grid-auto-columns","grid-auto-flow","grid-auto-rows","grid-column","grid-column-end",
                "grid-column-gap","grid-column-start","grid-gap","grid-row","grid-row-end","grid-row-gap","grid-row-start",
                "grid-template","grid-template-areas","grid-template-columns","grid-template-rows","hanging-punctuation",
                "height","hyphens","image-rendering","isolation","justify-content","left","letter-spacing","line-break",
                "line-height","list-style","list-style-image","list-style-position","list-style-type","margin","margin-bottom",
                "margin-left","margin-right","margin-top","mask","mask-type","max-height","max-width","min-height","min-width",
                "mix-blend-mode","object-fit","object-position","opacity","order","orphans","outline","outline-color",
                "outline-offset","outline-style","outline-width","overflow","overflow-wrap","overflow-x","overflow-y",
                "padding","padding-bottom","padding-left","padding-right","padding-top","page-break-after","page-break-before",
                "page-break-inside","perspective","perspective-origin","place-content","place-items","place-self","pointer-events",
                "position","quotes","resize","right","row-gap","scroll-behavior","tab-size","table-layout","text-align",
                "text-align-last","text-combine-upright","text-decoration","text-decoration-color","text-decoration-line",
                "text-decoration-style","text-indent","text-justify","text-orientation","text-overflow","text-shadow",
                "text-transform","top","transform","transform-origin","transform-style","transition","transition-delay",
                "transition-duration","transition-property","transition-timing-function","unicode-bidi","user-select",
                "vertical-align","visibility","white-space","widows","width","word-break","word-spacing","word-wrap","writing-mode",
                "z-index"
            };

            break;
        case FileExtension::SCSS:
            CBLT::gKeywords = {
                // SCSS specific (mixins, variables, functions)
                "$","@mixin","@include","@function","@return","@if","@else","@for","@each","@while",
                "@import","@use","@forward","@debug","@warn","@error","@media","@supports","@keyframes"
            };

            break;
        case FileExtension::LESS:
            CBLT::gKeywords = {
                // LESS keywords / features
                "@import","@mixin","@include","@media","@font-face","@keyframes","@extend",
                "@variable","@function","@return","all","and","as","when","not","or","default",
                "each","if","else","for","in","while"
            };

            break;
        case FileExtension::JS:
            CBLT::gKeywords = {
                // JavaScript keywords
                "await","break","case","catch","class","const","continue","debugger","default",
                "delete","do","else","enum","export","extends","false","finally","for","function",
                "if","implements","import","in","instanceof","interface","let","new","null","package",
                "private","protected","public","return","static","super","switch","this","throw",
                "true","try","typeof","var","void","while","with","yield",
            
                // built-in types / literals
                "Array","Boolean","Date","Error","Function","Map","Math","Number","Object","Promise",
                "RegExp","Set","String","Symbol","BigInt","undefined","NaN","Infinity"
            };
            
            break;
        case FileExtension::JSX:
            CBLT::gKeywords = {
                // JSX is JavaScript + XML tags (for syntax highlighting)
                // JSX tags are treated like HTML tags
                "div","span","p","a","img","ul","ol","li","section","header","footer",
                "nav","article","aside","h1","h2","h3","h4","h5","h6","form","input",
                "textarea","button","label","select","option","canvas","svg","path","g",
                "circle","rect","line","polygon","polyline","text","defs","use"
            };

            break;
        case FileExtension::TS:
            CBLT::gKeywords = {
                // TypeScript keywords (includes JS + TS types)
                "abstract","any","as","asserts","bigint","boolean","break","case","catch","class",
                "const","continue","debugger","declare","default","delete","do","else","enum","export",
                "extends","false","finally","for","from","function","get","if","implements","import",
                "in","infer","instanceof","interface","is","keyof","let","module","namespace","never",
                "new","null","number","object","package","private","protected","public","readonly",
                "require","return","set","static","string","super","switch","symbol","this","throw",
                "true","try","type","typeof","undefined","unique","unknown","var","void","while","with","yield"
            };
            
            break;
        case FileExtension::TSX:
            CBLT::gKeywords = {
                // TSX includes TypeScript + JSX tags
                "abstract","any","as","asserts","bigint","boolean","break","case","catch","class",
                "const","continue","debugger","declare","default","delete","do","else","enum","export",
                "extends","false","finally","for","from","function","get","if","implements","import",
                "in","infer","instanceof","interface","is","keyof","let","module","namespace","never",
                "new","null","number","object","package","private","protected","public","readonly",
                "require","return","set","static","string","super","switch","symbol","this","throw",
                "true","try","type","typeof","undefined","unique","unknown","var","void","while","with","yield",
            
                // JSX tags
                "div","span","p","a","img","ul","ol","li","section","header","footer",
                "nav","article","aside","h1","h2","h3","h4","h5","h6","form","input",
                "textarea","button","label","select","option","canvas","svg","path","g",
                "circle","rect","line","polygon","polyline","text","defs","use"
            };

            break;
        case FileExtension::WASM:
            CBLT::gKeywords = {
                // WebAssembly (WAT / text format) keywords
                "module","func","param","result","local","global","memory","table","elem",
                "data","start","import","export","type","offset","align","i32","i64","f32","f64",
                "select","block","loop","if","else","br","br_if","br_table","call","call_indirect",
                "drop","return","get_local","set_local","tee_local","get_global","set_global",
                "unreachable","nop","current_memory","grow_memory","load","store","const"
            };

            break;
        case FileExtension::PY:
            CBLT::gKeywords = {
                // Python keywords
                "False","None","True","and","as","assert","async","await","break","class","continue",
                "def","del","elif","else","except","finally","for","from","global","if","import",
                "in","is","lambda","nonlocal","not","or","pass","raise","return","try","while","with","yield",
            
                // built-in types / literals
                "int","float","complex","bool","str","list","tuple","set","frozenset","dict","bytes","bytearray","memoryview",
                "range","object","type"
            };

            break;
        case FileExtension::RB:
            CBLT::gKeywords = {
                // Ruby keywords
                "__ENCODING__","__LINE__","__FILE__","BEGIN","END","alias","and","begin","break",
                "case","class","def","defined?","do","else","elsif","end","ensure","false","for",
                "if","in","module","next","nil","not","or","redo","rescue","retry","return","self",
                "super","then","true","undef","unless","until","when","while","yield",
            
                // built-in types
                "Array","Integer","Float","String","Symbol","Hash","Range","Regexp","Time","IO","File","Enumerable","Enumerator","Proc"
            };

            break;
        case FileExtension::PHP:
            CBLT::gKeywords = {
                // PHP keywords
                "__halt_compiler","abstract","and","array","as","break","callable","case","catch",
                "class","clone","const","continue","declare","default","die","do","echo","else",
                "elseif","empty","enddeclare","endfor","endforeach","endif","endswitch","endwhile",
                "eval","exit","extends","final","finally","for","foreach","function","global","goto",
                "if","implements","include","include_once","instanceof","insteadof","interface","isset",
                "list","namespace","new","or","print","private","protected","public","require","require_once",
                "return","static","switch","throw","trait","try","unset","use","var","while","xor","yield",
            
                // literals
                "true","false","null"
            };

            break;
        case FileExtension::PL:
            CBLT::gKeywords = {
                // Perl keywords
                "BEGIN","END","abs","accept","alarm","and","atan2","AUTOLOAD","BEGIN","bind",
                "binmode","bless","break","caller","chdir","chmod","chomp","chop","chown",
                "chr","chroot","close","closedir","cmp","connect","continue","cos","crypt","dbmclose",
                "dbmopen","defined","delete","die","do","dump","else","elsif","endgrent","endhostent",
                "endnetent","endprotoent","endpwent","endservent","eof","eq","eval","exec","exists",
                "exit","exp","fcntl","fileno","flock","for","foreach","fork","format","formline",
                "ge","getc","getgrent","getgrgid","getgrnam","gethostbyaddr","gethostbyname",
                "gethostent","getlogin","getnetbyaddr","getnetbyname","getnetent","getpeername",
                "getpgrp","getppid","getpriority","getprotobyname","getprotobynumber","getprotoent",
                "getpwent","getpwnam","getpwuid","getservbyname","getservbyport","getservent",
                "getsockname","getsockopt","given","glob","goto","grep","gt","hex","if","index",
                "int","ioctl","join","keys","kill","last","lc","lcfirst","le","length","link","listen",
                "local","localtime","lock","log","lt","lstat","map","mkdir","msgctl","msgget","msgrcv",
                "msgsnd","my","ne","next","no","not","oct","open","opendir","or","ord","our","pack",
                "package","pipe","pop","pos","print","printf","prototype","push","q","qq","qr","qw",
                "qx","quotemeta","rand","read","readdir","readline","readlink","readpipe","recv","redo",
                "ref","rename","require","reset","return","reverse","rewinddir","rindex","rmdir","s",
                "scalar","seek","seekdir","select","semctl","semget","semop","send","setgrent","sethostent",
                "setnetent","setpgrp","setpriority","setprotoent","setpwent","setservent","setsockopt",
                "shift","shmctl","shmget","shmread","shmwrite","shutdown","sin","sleep","socket","socketpair",
                "sort","splice","split","sprintf","sqrt","srand","stat","state","study","sub","substr","symlink",
                "syscall","sysread","sysseek","system","syswrite","tell","telldir","tie","tied","time","times",
                "truncate","uc","ucfirst","umask","undef","unlink","unpack","unshift","untie","until","use",
                "utime","values","vec","wait","waitpid","wantarray","warn","while","write","xor"
            };

            break;
        case FileExtension::LUA:
            CBLT::gKeywords = {
                // Lua keywords
                "and","break","do","else","elseif","end","false","for","function","goto","if",
                "in","local","nil","not","or","repeat","return","then","true","until","while"
            };
            
            break;
        case FileExtension::SH:
            CBLT::gKeywords = {
                // Shell (sh) keywords
                "alias","bg","break","cd","command","continue","echo","eval","exec","exit","export",
                "fg","fi","for","function","if","in","jobs","kill","let","local","pwd","read",
                "readonly","return","set","shift","test","then","time","trap","true","type","ulimit",
                "umask","unset","until","wait","while"
            };
            
            break;
        case FileExtension::PS1:
            CBLT::gKeywords = {
                // PowerShell (PS1) keywords
                "Begin","Break","Catch","Continue","Data","Do","Else","ElseIf","End","Exit","Filter",
                "For","Foreach","From","Function","If","In","Param","Process","Return","Switch","Throw",
                "Trap","Try","Until","Using","While"
            };
            
            break;
        case FileExtension::JSON:
            CBLT::gKeywords = {
                // JSON structural elements / literals
                "{","}","[","]",":",",",
                "true","false","null"
            };

            break;
        case FileExtension::YAML:
            CBLT::gKeywords = {
                // YAML / YML keywords / literals
                "---","...","true","false","yes","no","on","off","null","~",":","-","?","|",">","%YAML","%TAG"
            };

            break;
        case FileExtension::YML:
            CBLT::gKeywords = {
                // YAML / YML keywords / literals
                "---","...","true","false","yes","no","on","off","null","~",":","-","?","|",">","%YAML","%TAG"
            };

            break;
        case FileExtension::XML:
            CBLT::gKeywords = {
                // XML tags (generic, for highlighting)
                "xml","!DOCTYPE","a","abbr","address","area","article","aside","audio","b","base","bdi",
                "bdo","blockquote","body","br","button","canvas","caption","cite","code","col","colgroup",
                "data","datalist","dd","del","details","dfn","dialog","div","dl","dt","em","embed","fieldset",
                "figcaption","figure","footer","form","h1","h2","h3","h4","h5","h6","head","header","hr",
                "html","i","iframe","img","input","ins","kbd","label","legend","li","link","main","map",
                "mark","meta","meter","nav","noscript","object","ol","optgroup","option","output","p","param",
                "picture","pre","progress","q","rp","rt","ruby","s","samp","script","section","select","slot",
                "small","source","span","strong","style","sub","summary","sup","table","tbody","td","template",
                "textarea","tfoot","th","thead","time","title","tr","track","u","ul","var","video","wbr"
            };

            break;
        case FileExtension::TOML:
            CBLT::gKeywords = {
                // TOML keywords / literals
                "true","false","inf","nan",
                "[","]","[[]]",",","=","#",
                "localDate","localTime","localDateTime","offsetDateTime","array","table"
            };

            break;
        case FileExtension::INI:
            CBLT::gKeywords = {
                // INI keywords / structure
                ";","#","[","]","=","true","false","on","off"
            };
            
            break;
        case FileExtension::ENV:
            CBLT::gKeywords = {
                // ENV file structure / literals
                "#","=","true","false","null"
            };
            
            break;
        case FileExtension::SQL:
            CBLT::gKeywords = {
                // SQL keywords (common subset)
                "ADD","ALL","ALTER","AND","ANY","AS","ASC","BETWEEN","BY","CASE","CHECK",
                "COLUMN","CREATE","DATABASE","DEFAULT","DELETE","DESC","DISTINCT","DROP",
                "ELSE","EXISTS","FALSE","FROM","FULL","GROUP","HAVING","IN","INDEX","INNER",
                "INSERT","INTO","IS","JOIN","LEFT","LIKE","LIMIT","NOT","NULL","ON","OR",
                "ORDER","OUTER","PRIMARY","RIGHT","SELECT","SET","TABLE","THEN","TRUE","UNION",
                "UNIQUE","UPDATE","VALUES","VIEW","WHEN","WHERE"
            };

            break;
        case FileExtension::GRAPHQL:
            CBLT::gKeywords = {
                // GraphQL keywords
                "query","mutation","subscription","fragment","on","true","false","null","schema",
                "type","interface","union","enum","input","implements","directive","extend"
            };
            
            break;
        case FileExtension::MD:
            CBLT::gKeywords = {
                // Markdown elements / pseudo-keywords
                "#","##","###","####","#####","######","*","-","_","**","__","```","```","[","]","(",")","!","`",">","+","~"
            };

            break;
        case FileExtension::RST:
            CBLT::gKeywords = {
                // reStructuredText elements
                "..",".. code::",".. warning::",".. note::",".. todo::","*","**","-","=","`","_","|","<","[","]"
            };

            break;
        case FileExtension::TXT:
            CBLT::gKeywords = {
                // Plain text (minimal highlighting, mostly literals/punctuation)
                ".","-","_","#","*"
            };
            
            break;
        case FileExtension::HCL:
            CBLT::gKeywords = {
                // HCL (HashiCorp Configuration Language) keywords
                "true","false","null","variable","output","provider","module","resource","data","locals","terraform"
            };
            
            break;
        case FileExtension::HS:
            CBLT::gKeywords = {
                // Haskell (HS) keywords
                "case","class","data","default","deriving","do","else","foreign","if","import",
                "in","infix","infixl","infixr","instance","let","module","newtype","of","then",
                "type","where","qualified","as","hiding","forall"
            };
            
            break;
        case FileExtension::ML:
            CBLT::gKeywords = {
                // OCaml / ML keywords
                "and","as","assert","asr","begin","class","constraint","do","done","downto",
                "else","end","exception","external","false","for","fun","function","if","in",
                "include","inherit","initializer","land","lazy","let","lor","lsl","lsr","lxor",
                "match","method","mod","module","mutable","new","nil","object","of","open","or",
                "private","rec","sig","struct","then","to","true","try","type","val","virtual",
                "when","while","with"
            };

            break;
        case FileExtension::FSHARP:
            CBLT::gKeywords = {
                // F# keywords
                "abstract","and","as","assert","base","begin","class","default","delegate",
                "do","done","downcast","downto","elif","else","end","exception","extern",
                "false","finally","for","fun","function","global","if","in","inherit",
                "inline","interface","internal","lazy","let","match","member","module",
                "mutable","namespace","new","null","of","open","or","override","private",
                "public","rec","return","sig","static","struct","then","to","true","try",
                "type","upcast","use","val","virtual","void","when","while","with","yield"
            };
            
            break;
        case FileExtension::LISP:
            CBLT::gKeywords = {
                // Lisp keywords / built-ins
                "defun","defvar","defparameter","defmacro","setq","quote","if","cond","loop",
                "progn","lambda","let","let*","catch","throw","unwind-protect","and","or","not",
                "car","cdr","cons","append","list","listp","atom","eq","eql","equal","funcall",
                "apply","eval","format","print","princ","read","read-line","load","require"
            };

            break;
        case FileExtension::SCM:
            CBLT::gKeywords = {
                // Scheme (SCM) keywords
                "define","define-syntax","let","let*","letrec","lambda","if","cond","case",
                "and","or","begin","do","delay","quote","quasiquote","unquote","unquote-splicing",
                "set!","call/cc","else","import","export","module","require","provide"
            };

            break;
        case FileExtension::RKT:
            CBLT::gKeywords = {
                // Racket (RKT) keywords
                "#lang","#%app","#%datum","#%module-begin","#%plain-module-begin",
                "define","define-syntax","lambda","let","let*","letrec","if","cond","case",
                "and","or","begin","do","quote","quasiquote","unquote","unquote-splicing",
                "set!","import","provide","require","module","class","interface"
            };
            
            break;
        case FileExtension::PROLOG:
            CBLT::gKeywords = {
                // Prolog keywords
                ":-",".","?-","assert","retract","true","fail","repeat","is","=..","call",
                "not","\\+",";","->","if","then","else","dynamic","static","predicate","functor",
                "arg","length","member","append","findall","bagof","setof","halt"
            };
            
            break;
        case FileExtension::VHDL:
            CBLT::gKeywords = {
                // VHDL keywords
                "abs","access","after","alias","all","and","architecture","array","assert","attribute",
                "begin","block","body","buffer","bus","case","component","configuration","constant",
                "disconnect","downto","else","elsif","end","entity","exit","file","for","function",
                "generate","generic","group","guarded","if","impure","in","inertial","inout","is",
                "label","library","linkage","literal","loop","map","mod","nand","new","next","nor",
                "not","null","of","on","open","or","others","out","package","port","postponed","procedure",
                "process","pure","range","record","register","reject","rem","report","return","rol","ror",
                "select","severity","signal","shared","sla","sll","sra","srl","subtype","then","to","transport",
                "type","unaffected","units","until","use","variable","wait","when","while","with","xnor","xor"
            };
            
            break;
        case FileExtension::VERILOG:
            CBLT::gKeywords = {
                // Verilog keywords
                "always","and","assign","begin","buf","bufif0","bufif1","case","casex","casez",
                "cell","cmos","deassign","default","defparam","disable","edge","else","end",
                "endcase","endconfig","endfunction","endgenerate","endmodule","endprimitive",
                "endspecify","endtable","endtask","event","for","force","forever","fork","function",
                "generate","genvar","highz0","highz1","if","ifnone","incdir","include","initial",
                "inout","input","instance","integer","join","large","liblist","library","localparam",
                "macromodule","medium","module","nand","negedge","nmos","nor","noshowcancelled",
                "not","notif0","notif1","or","output","parameter","pmos","posedge","primitive","pull0",
                "pull1","pulldown","pullup","pulsestyle_onevent","pulsestyle_ondetect","rcmos","real",
                "realtime","reg","release","repeat","rnmos","rpmos","rtran","rtranif0","rtranif1",
                "scalared","showcancelled","signed","small","specify","specparam","strong0","strong1",
                "supply0","supply1","table","task","time","tran","tranif0","tranif1","tri","tri0","tri1",
                "triand","trior","trireg","unsigned","use","vectored","wait","wand","while","wire",
                "wor","xnor","xor"
            };
            
            break;
        case FileExtension::GLSL:
            CBLT::gKeywords = {
                // GLSL keywords
                "attribute","const","uniform","varying","break","continue","do","for","while","if",
                "else","in","out","inout","float","int","void","bool","true","false","lowp","mediump",
                "highp","precision","invariant","discard","return","mat2","mat3","mat4","vec2","vec3",
                "vec4","ivec2","ivec3","ivec4","bvec2","bvec3","bvec4","sampler1D","sampler2D",
                "sampler3D","samplerCube","sampler1DShadow","sampler2DShadow","struct"
            };
            
            break;
        case FileExtension::HLSL:
            CBLT::gKeywords = {
                // HLSL keywords
                "AppendStructuredBuffer","asm","asm_fragment","BlendState","bool","break","Buffer","ByteAddressBuffer",
                "case","cbuffer","centroid","class","column_major","compile","compile_fragment","CompileShader",
                "const","continue","ComputeShader","ConsumeStructuredBuffer","default","discard","do","double",
                "dword","else","export","extern","false","float","for","fxgroup","if","in","inout","InputPatch",
                "int","interface","line","lineadj","linear","matrix","min16float","min10float","min16int","min12int",
                "min16uint","namespace","nointerpolation","noperspective","NULL","out","OutputPatch","packoffset",
                "pass","pixelfragment","PixelShader","point","precise","return","register","row_major","RWBuffer",
                "RWByteAddressBuffer","RWStructuredBuffer","RWTexture1D","RWTexture1DArray","RWTexture2D",
                "RWTexture2DArray","RWTexture3D","sample","sampler","SamplerState","SamplerComparisonState",
                "shared","snorm","stateblock","stateblock_state","static","string","struct","switch","tbuffer",
                "technique","technique10","technique11","texture1D","texture1DArray","texture2D","texture2DArray",
                "texture2DMS","texture2DMSArray","texture3D","textureCube","textureCubeArray","true","typedef",
                "uint","uniform","unorm","unsigned","vector","vertexfragment","VertexShader","void","volatile","while"
            };
            
            break;
        case FileExtension::WGSL:
            CBLT::gKeywords = {
                // WGSL keywords
                "let","var","const","override","fn","return","break","continue","if","else","loop","while",
                "for","switch","case","default","struct","array","bitcast","bool","f32","i32","u32","vec2",
                "vec3","vec4","mat2x2","mat3x3","mat4x4","texture_1d","texture_2d","texture_3d","texture_cube",
                "sampler","true","false"
            };
            
            break;
        case FileExtension::GD:
            CBLT::gKeywords = {
                // Godot (GDscript) keywords
                "and","as","assert","break","class","const","continue","elif","else","enum","extends","export",
                "false","for","func","if","in","is","master","match","not","null","onready","or","pass","preload",
                "puppet","return","self","setget","signal","static","switch","tool","true","var","while","yield"
            };

            break;
        case FileExtension::GDSHADER:
            CBLT::gKeywords = {
                // Godot shader (GDSHADER) keywords
                "shader_type","canvas_item","spatial","vertex","fragment","light","uniform","varying",
                "const","true","false","void","if","else","for","while","break","continue","return",
                "discard","texture","textureLod","sampler2D","samplerCube","mat2","mat3","mat4",
                "vec2","vec3","vec4","float","int","bool","sin","cos","tan","asin","acos","atan",
                "length","normalize","dot","cross","mix","clamp","min","max","pow","exp","log","abs","sign"
            };
            
            break;
        case FileExtension::PROTO:
            CBLT::gKeywords = {
                // Protocol Buffers (PROTO) keywords
                "syntax","import","package","option","message","enum","service","rpc","returns",
                "repeated","optional","required","map","bool","int32","int64","uint32","uint64",
                "sint32","sint64","fixed32","fixed64","sfixed32","sfixed64","float","double",
                "string","bytes"
            };
            
            break;
        case FileExtension::THRIFT:
            CBLT::gKeywords = {
                // Apache Thrift (THRIFT) keywords
                "namespace","include","const","typedef","enum","struct","union","exception","service",
                "void","bool","byte","i8","i16","i32","i64","double","string","binary","map","set","list"
            };

            break;
        case FileExtension::ANTLR:
            CBLT::gKeywords = {
                // ANTLR grammar keywords / pseudo-keywords
                "lexer","parser","grammar","tokens","options","import","fragment","rule","@header","@members",
                "@lexer::members","@parser::members","@lexer::header","@parser::header","returns","locals",
                "throws","catch","finally","fragment","skip","channel","mode","type","fragment"
            };
            
            break;
        case FileExtension::REGEX:
            CBLT::gKeywords = {
                // Regex common keywords / literals
                ".","^","$","*","+","?","{","}","[","]","|","(",")","\\","d","D","w","W","s","S","b","B"
            };

            break;

        default:
            // TXT
            break;
    }
}

bool CBLT::IsKeyword(std::string_view sv) {
    return CBLT::gKeywords.contains(sv);
}