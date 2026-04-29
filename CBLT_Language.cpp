#include "CBLT_Language.hpp"

using namespace CBLT;

void Language::ReadLangFile(std::string fname) {
    std::string fpath;

    #if defined(__linux__)
        const char* resource_path = getenv("CBLT_RESOURCES");
        std::string resourceDir = resource_path ? std::string(resource_path) : ".";
    
        fpath = resourceDir + "/meta/lang/" + fname;
    #elif defined(_WIN32)     // WIN32
        fpath =  "meta/lang/" + fname;
    #elif  defined(__APPLE__) // APPLE
        fpath =  "meta/lang/" + fname;
    #endif

    std::ifstream file(fpath);
    if (!file.is_open()) {
        UE::Log("CBLT_ERR: FAILED TO PARSE " + fpath + " LANG CONF");
        
        return;
    }

    LanguageLoaderSection current = LanguageLoaderSection::NONE;

    std::string line;
    while (std::getline(file, line)) {
        line = UF::Trim(line);

        if (line.empty() || line[0] == '>') continue;

        if (line == "%keywords:")            { current = LanguageLoaderSection::KEYWORDS; continue; }
        if (line == "%controlFlow:")         { current = LanguageLoaderSection::CONTROL_FLOW; continue; }
        if (line == "%storageClass:")        { current = LanguageLoaderSection::STORAGE_CLASS; continue; }
        if (line == "%typeQualifiers:")      { current = LanguageLoaderSection::TYPE_QUALIFIERS; continue; }
        if (line == "%userDefinedClass:")    { current = LanguageLoaderSection::USER_DEFINED; continue; }
        if (line == "%utility:")             { current = LanguageLoaderSection::UTILITY; continue; }
        if (line == "%dataTypes:")           { current = LanguageLoaderSection::DATA_TYPES; continue; }

        if (line == "%operators:")           { current = LanguageLoaderSection::OPERATORS; continue; }
        if (line == "%punctuation:")         { current = LanguageLoaderSection::PUNCTUATION; continue; }
        if (line == "%commentBlock:")        { current = LanguageLoaderSection::COMMENT_BLOCK; continue; }
        if (line == "%commentLine:")         { current = LanguageLoaderSection::COMMENT_LINE; continue; }
        if (line == "%stringDelim:")         { current = LanguageLoaderSection::STRING_DELIM; continue; }
        if (line == "%annotations:")         { current = LanguageLoaderSection::ANNOTATIONS; continue; }
        if (line == "%macros:")              { current = LanguageLoaderSection::MACROS; continue; }
        if (line == "%settings:")            { current = LanguageLoaderSection::SETTINGS; continue; }

        if (line == "%end") {
            current = LanguageLoaderSection::NONE;
            continue;
        }

        switch (current) {
            case LanguageLoaderSection::PUNCTUATION: {
                std::stringstream ss(line);
                std::string pun;
                while (ss >> pun) {
                    punctuations.insert(pun);
                }
                break;
            }

            case LanguageLoaderSection::CONTROL_FLOW: {
                std::stringstream ss(line);
                std::string word;
                while (ss >> word) {
                    keywords.controlFlow.insert(word);
                }
                break;
            }

            case LanguageLoaderSection::STORAGE_CLASS: {
                std::stringstream ss(line);
                std::string word;
                while (ss >> word) {
                    keywords.storageClass.insert(word);
                }
                break;
            }
            
            case LanguageLoaderSection::TYPE_QUALIFIERS: {
                std::stringstream ss(line);
                std::string word;
                while (ss >> word) {
                    keywords.typeQualifiers.insert(word);
                }
                break;
            }
            
            case LanguageLoaderSection::USER_DEFINED: {
                std::stringstream ss(line);
                std::string word;
                while (ss >> word) {
                    keywords.userDefinedClass.insert(word);
                }
                break;
            }
            
            case LanguageLoaderSection::UTILITY: {
                std::stringstream ss(line);
                std::string word;
                while (ss >> word) {
                    keywords.utility.insert(word);
                }
                break;
            }
            
            case LanguageLoaderSection::DATA_TYPES: {
                std::stringstream ss(line);
                std::string word;
                while (ss >> word) {
                    keywords.dataTypes.insert(word);
                }
                break;
            }

            case LanguageLoaderSection::OPERATORS: {
                std::stringstream ss(line);
                std::string op;
                while (ss >> op) {
                    operators.insert(op);
                }
                break;
            }

            case LanguageLoaderSection::COMMENT_BLOCK: {
                std::stringstream ss(line);
                std::string start, end;
                ss >> start >> end;
                commentBlock.emplace_back(start, end);
                break;
            }

            case LanguageLoaderSection::COMMENT_LINE: {
                std::stringstream ss(line);
                std::string cl;
                while (ss >> cl) {
                    commentLine.emplace_back(cl);
                }
                break;
            }

            case LanguageLoaderSection::STRING_DELIM: {
                std::stringstream ss(line);
                std::string start, end;
                ss >> start >> end;
                stringDelim.emplace_back(start, end);
                break;
            }

            // Per line definition
            case LanguageLoaderSection::ANNOTATIONS:
                annotations.push_back(line);
                break;

            // Per line definition
            case LanguageLoaderSection::MACROS:
                macros.push_back(line);
                break;

            case LanguageLoaderSection::SETTINGS: {
                auto pos = line.find(':');
                if (pos == std::string::npos) break;

                std::string key = UF::Trim(line.substr(0, pos));
                std::string val = UF::Trim(line.substr(pos + 1));

                bool b = (val == "true");

                if (key == "mls") settings.multilineStrings = b;
                else if (key == "hex") settings.hexNumerals = b;
                else if (key == "bin") settings.binNumerals = b;
                else if (key == "esc") settings.escapeSequences = b;

                break;
            }

            default:
                break;
        }
    }
}

Language::Language(FileExtension fe) {
    auto it = gLangFiles.find(fe);
    if (it != gLangFiles.end()) {
        ReadLangFile(it->second);
    } else {
        ReadLangFile("txt.conf"); // TXT as base Fallback, extension unrecognized
    }
}

Language::~Language(void) {}

UT::b Language::IsKeyword(const std::string s) {
    return
        keywords.controlFlow.find(s) != keywords.controlFlow.end() ||
        keywords.storageClass.find(s) != keywords.storageClass.end() ||
        keywords.typeQualifiers.find(s) != keywords.typeQualifiers.end() ||
        keywords.userDefinedClass.find(s) != keywords.userDefinedClass.end() ||
        keywords.utility.find(s) != keywords.utility.end() ||
        keywords.dataTypes.find(s) != keywords.dataTypes.end();
}

UT::b Language::IsOp(const std::string s) {
    return operators.find(s) != operators.end();
}

namespace CBLT {

    std::unordered_map<FileExtension, std::string> gLangFiles = {
        {FileExtension::C,        "c.conf"},
        {FileExtension::CPP,      "cpp.conf"},
        {FileExtension::CS,       "cs.conf"},
        {FileExtension::ASM,      "asm.conf"},
        {FileExtension::JAVA,     "java.conf"},
        {FileExtension::RS,       "rs.conf"},
        {FileExtension::D,        "d.conf"},
        {FileExtension::GO,       "go.conf"},
        {FileExtension::ZIG,      "zig.conf"},
        {FileExtension::NIM,      "nim.conf"},
        {FileExtension::SWIFT,    "swift.conf"},
        {FileExtension::KT,       "kt.conf"},
        {FileExtension::GROOVY,   "groovy.conf"},
        {FileExtension::SCALA,    "scala.conf"},
        {FileExtension::HTML,     "html.conf"},
        {FileExtension::CSS,      "css.conf"},
        {FileExtension::SCSS,     "scss.conf"},
        {FileExtension::LESS,     "less.conf"},
        {FileExtension::JS,       "js.conf"},
        {FileExtension::JSX,      "jsx.conf"},
        {FileExtension::TS,       "ts.conf"},
        {FileExtension::TSX,      "tsx.conf"},
        {FileExtension::WASM,     "wasm.conf"},
        {FileExtension::PY,       "py.conf"},
        {FileExtension::RB,       "rb.conf"},
        {FileExtension::PHP,      "php.conf"},
        {FileExtension::PL,       "pl.conf"},
        {FileExtension::LUA,      "lua.conf"},
        {FileExtension::SH,       "sh.conf"},
        {FileExtension::PS1,      "ps1.conf"},
        {FileExtension::JSON,     "json.conf"},
        {FileExtension::YAML,     "yaml.conf"},
        {FileExtension::YML,      "yml.conf"},
        {FileExtension::XML,      "xml.conf"},
        {FileExtension::TOML,     "toml.conf"},
        {FileExtension::INI,      "ini.conf"},
        {FileExtension::ENV,      "env.conf"},
        {FileExtension::SQL,      "sql.conf"},
        {FileExtension::GRAPHQL,  "graphql.conf"},
        {FileExtension::MD,       "md.conf"},
        {FileExtension::RST,      "rst.conf"},
        {FileExtension::TXT,      "txt.conf"},
        {FileExtension::HCL,      "hcl.conf"},
        {FileExtension::HS,       "hs.conf"},
        {FileExtension::ML,       "ml.conf"},
        {FileExtension::FSHARP,   "fsharp.conf"},
        {FileExtension::LISP,     "lisp.conf"},
        {FileExtension::SCM,      "scm.conf"},
        {FileExtension::RKT,      "rkt.conf"},
        {FileExtension::PROLOG,   "prolog.conf"},
        {FileExtension::VHDL,     "vhdl.conf"},
        {FileExtension::VERILOG,  "verilog.conf"},
        {FileExtension::GLSL,     "glsl.conf"},
        {FileExtension::HLSL,     "hlsl.conf"},
        {FileExtension::WGSL,     "wgsl.conf"},
        {FileExtension::GD,       "gd.conf"},
        {FileExtension::GDSHADER, "gdshader.conf"},
        {FileExtension::PROTO,    "proto.conf"},
        {FileExtension::THRIFT,   "thrift.conf"},
        {FileExtension::ANTLR,    "antlr.conf"},
        {FileExtension::REGEX,    "regex.conf"}
    };
} // CBLT, needs to be wrapped ??