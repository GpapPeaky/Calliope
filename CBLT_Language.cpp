#include "CBLT_Language.hpp"

using namespace CBLT;

void Language::ReadLangFile(std::string fname) {
    std::string fpath;

    #if defined(__linux__)
        std::string resourceDir = Sys::ResourcePath();
    
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
        ReadLangFile("txt.cbltconf"); // TXT as base Fallback, extension unrecognized
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
        {FileExtension::CCF,      "ccf.cbltconf"},
        {FileExtension::C,        "c.cbltconf"},
        {FileExtension::CPP,      "cpp.cbltconf"},
        {FileExtension::CS,       "cs.cbltconf"},
        {FileExtension::ASM,      "asm.cbltconf"},
        {FileExtension::JAVA,     "java.cbltconf"},
        {FileExtension::RS,       "rs.cbltconf"},
        {FileExtension::D,        "d.cbltconf"},
        {FileExtension::GO,       "go.cbltconf"},
        {FileExtension::ZIG,      "zig.cbltconf"},
        {FileExtension::NIM,      "nim.cbltconf"},
        {FileExtension::SWIFT,    "swift.cbltconf"},
        {FileExtension::KT,       "kt.cbltconf"},
        {FileExtension::GROOVY,   "groovy.cbltconf"},
        {FileExtension::SCALA,    "scala.cbltconf"},
        {FileExtension::HTML,     "html.cbltconf"},
        {FileExtension::CSS,      "css.cbltconf"},
        {FileExtension::SCSS,     "scss.cbltconf"},
        {FileExtension::LESS,     "less.cbltconf"},
        {FileExtension::JS,       "js.cbltconf"},
        {FileExtension::JSX,      "jsx.cbltconf"},
        {FileExtension::TS,       "ts.cbltconf"},
        {FileExtension::TSX,      "tsx.cbltconf"},
        {FileExtension::WASM,     "wasm.cbltconf"},
        {FileExtension::PY,       "py.cbltconf"},
        {FileExtension::RB,       "rb.cbltconf"},
        {FileExtension::PHP,      "php.cbltconf"},
        {FileExtension::PL,       "pl.cbltconf"},
        {FileExtension::LUA,      "lua.cbltconf"},
        {FileExtension::SH,       "sh.cbltconf"},
        {FileExtension::PS1,      "ps1.cbltconf"},
        {FileExtension::JSON,     "json.cbltconf"},
        {FileExtension::YAML,     "yaml.cbltconf"},
        {FileExtension::YML,      "yml.cbltconf"},
        {FileExtension::XML,      "xml.cbltconf"},
        {FileExtension::TOML,     "toml.cbltconf"},
        {FileExtension::INI,      "ini.cbltconf"},
        {FileExtension::ENV,      "env.cbltconf"},
        {FileExtension::SQL,      "sql.cbltconf"},
        {FileExtension::GRAPHQL,  "graphql.cbltconf"},
        {FileExtension::MD,       "md.cbltconf"},
        {FileExtension::RST,      "rst.cbltconf"},
        {FileExtension::TXT,      "txt.cbltconf"},
        {FileExtension::HCL,      "hcl.cbltconf"},
        {FileExtension::HS,       "hs.cbltconf"},
        {FileExtension::ML,       "ml.cbltconf"},
        {FileExtension::FSHARP,   "fsharp.cbltconf"},
        {FileExtension::LISP,     "lisp.cbltconf"},
        {FileExtension::SCM,      "scm.cbltconf"},
        {FileExtension::RKT,      "rkt.cbltconf"},
        {FileExtension::PROLOG,   "prolog.cbltconf"},
        {FileExtension::VHDL,     "vhdl.cbltconf"},
        {FileExtension::VERILOG,  "verilog.cbltconf"},
        {FileExtension::GLSL,     "glsl.cbltconf"},
        {FileExtension::HLSL,     "hlsl.cbltconf"},
        {FileExtension::WGSL,     "wgsl.cbltconf"},
        {FileExtension::GD,       "gd.cbltconf"},
        {FileExtension::GDSHADER, "gdshader.cbltconf"},
        {FileExtension::PROTO,    "proto.cbltconf"},
        {FileExtension::THRIFT,   "thrift.cbltconf"},
        {FileExtension::ANTLR,    "antlr.cbltconf"},
        {FileExtension::REGEX,    "regex.cbltconf"}
    };
} // CBLT, needs to be wrapped ??