#pragma once

#include <string>

namespace CBLT {
    // File extension types, for language support and tokenization
    enum class FileExtension {
        // C / C++
        C,
        CPP,
    
        // Systems / Compiled
        RS,
        GO,
        ZIG,
        NIM,
        SWIFT,
        KT,     // Kotlin
        CS,     // C#
        D,
    
        // JVM
        JAVA,
        GROOVY,
        SCALA,
    
        // Web
        HTML,
        CSS,
        SCSS,
        LESS,
        JS,
        JSX,
        TS,
        TSX,
        WASM,
    
        // Scripting
        PY,
        RB,
        PHP,
        PL,
        LUA,
        SH,
        PS1,
    
        // Data / Config
        JSON,
        YAML,
        YML,
        XML,
        TOML,
        INI,
        ENV,
        SQL,
        GRAPHQL,
    
        // Docs
        MD,
        RST,
        TXT,
    
        // DevOps / Build
        DOCKERFILE,
        MAKEFILE,
        CMAKE,
        BAZEL,
        HCL,
    
        // Functional / Academic
        HS,
        ML,
        FSHARP,
        LISP,
        SCM,
        RKT,
        PROLOG,
    
        // Low-level / Hardware
        ASM,
        S,
        VHDL,
        VERILOG,
    
        // Shaders / Game
        GLSL,
        HLSL,
        WGSL,
        GD,
        GDSHADER,
    
        // Other
        PROTO,
        THRIFT,
        ANTLR,
        REGEX,

        // CoBaLT scripting language
        // 1 language for creating macros
        // 1 language for editing options
        // 1 language for pallete editing
        //
        // Will most likely use an interpreter and NOT a compiler
        // so changes are visible in runtime

        CMSL,               // Cobalt Macro Scripting Language
        COSL,               // Cobalt Option Scripting Language
        CPSL                // Cobalt Pallete Scripting Language
    };
} // CBLT