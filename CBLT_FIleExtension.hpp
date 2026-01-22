#pragma once

#include <string>

namespace CBLT {
    // File extension types, for language support and tokenization
    enum class FileExtension {
        //     CoBaLT scripting language (CSL)
        //
        // Will most likely use an interpreter and NOT a compiler
        // so changes are visible in runtime
        //
        // It will be used for options for the theme
        // configurations, preferances and macros 

                ////////////////////////////////////////////   
        CSL,    //       Cobalt Scripting Language        //
                ////////////////////////////////////////////



        C,
        CPP,
        CS,
        ASM,
        JAVA,
        RS,
        D,
        GO,
        ZIG,
        NIM,
        SWIFT,
        KT,
        GROOVY,
        SCALA,
        HTML,
        CSS,
        SCSS,
        LESS,
        JS,
        JSX,
        TS,
        TSX,
        WASM,
        PY,
        RB,
        PHP,
        PL,
        LUA,
        SH,
        PS1,
        JSON,
        YAML,
        YML,
        XML,
        TOML,
        INI,
        ENV,
        SQL,
        GRAPHQL,
        MD,
        RST,
        TXT,

        // TODO
        DOCKERFILE,
        MAKEFILE,
        CMAKE,
        BAZEL,
        HCL,
        HS,
        ML,
        FSHARP,
        LISP,
        SCM,
        RKT,
        PROLOG,
        VHDL,
        VERILOG,
        GLSL,
        HLSL,
        WGSL,
        GD,
        GDSHADER,
        PROTO,
        THRIFT,
        ANTLR,
        REGEX,
    }; // File extension class
} // CBLT