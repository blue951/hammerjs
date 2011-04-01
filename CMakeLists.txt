cmake_minimum_required(VERSION 2.6)

project(HammerJS)

if(${EBUG} EQUAL 1)
    set(CMAKE_BUILD_TYPE "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DHAMMERJS_DEBUGGER")
    set(LIBV8 ${PROJECT_SOURCE_DIR}/v8/libv8_g.a)
else()
    set(CMAKE_BUILD_TYPE "Release")
    set(LIBV8 ${PROJECT_SOURCE_DIR}/v8/libv8.a)
endif()

if(CMAKE_COMPILER_IS_GNUCXX )
   add_definitions(-Wall)
endif(CMAKE_COMPILER_IS_GNUCXX )

set(HammerJS_HEADERS
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

set(HammerJS_SOURCES
    hammerjs.cpp
    parser/JSParser.cpp
    parser/Lexer.cpp
    parser/ParserArena.cpp
    parser/Parser.cpp
    parser/TreeDumper.cpp
    runtime/JSGlobalObjectFunctions.cpp
    wtf/dtoa.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/parser
    ${PROJECT_SOURCE_DIR}/runtime
    ${PROJECT_SOURCE_DIR}/wtf
    ${PROJECT_SOURCE_DIR}/v8/include
)

add_executable(hammerjs ${HammerJS_SOURCES})

link_directories(
    ${PROJECT_SOURCE_DIR}/v8
)

add_library(v8 STATIC IMPORTED)
set_property(TARGET v8 PROPERTY IMPORTED_LOCATION ${LIBV8})

target_link_libraries(hammerjs v8 pthread)

if(WIN32)
    target_link_libraries(hammerjs ws2_32 winmm)
endif()

install(TARGETS hammerjs DESTINATION bin)
