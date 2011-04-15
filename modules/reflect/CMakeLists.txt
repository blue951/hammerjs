include_directories(
    ${PROJECT_SOURCE_DIR}/v8/include
    ${PROJECT_SOURCE_DIR}/modules/reflect
    ${PROJECT_SOURCE_DIR}/modules/reflect/parser
    ${PROJECT_SOURCE_DIR}/modules/reflect/runtime
    ${PROJECT_SOURCE_DIR}/modules/reflect/wtf
)

set(Reflect_HEADERS
    parser/JSParser.h
    parser/Lexer.h
    parser/Lookup.h
    parser/ParserArena.h
    parser/Parser.h
    parser/SyntaxTree.h
    parser/SourceCode.h
    parser/SourceProvider.h
    parser/TreeDumper.h
    runtime/Identifier.h
    runtime/JSGlobalData.h
    runtime/JSGlobalObjectFunctions.h
    runtime/UString.h
    wtf/ASCIICType.h
    wtf/AlwaysInline.h
    wtf/Assertions.h
    wtf/Noncopyable.h
    wtf/Platform.h
    wtf/SegmentedVector.h
    wtf/TypeTraits.h
    wtf/Vector.h
    wtf/VectorTraits.h
    wtf/dtoa.h
    config.h
)

set(Reflect_SOURCES
    reflect.cpp
    parser/JSParser.cpp
    parser/Lexer.cpp
    parser/ParserArena.cpp
    parser/Parser.cpp
    parser/TreeDumper.cpp
    runtime/JSGlobalObjectFunctions.cpp
    wtf/dtoa.cpp
)

add_library(hammerjs_reflect ${Reflect_SOURCES})
