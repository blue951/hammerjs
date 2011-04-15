cmake_minimum_required(VERSION 2.6)

project(HammerJS)

add_subdirectory(v8)
set(LIBV8 ${PROJECT_SOURCE_DIR}/v8/libv8.a)

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

# FIXME: this should 64-bit vs 32-bit architecture instead
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(v8 PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
else()
    set_target_properties(v8 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()

link_directories(${PROJECT_SOURCE_DIR}/v8)
target_link_libraries(hammerjs v8)

if(UNIX)
    target_link_libraries(hammerjs pthread)
endif()

if(WIN32)
    target_link_libraries(hammerjs ws2_32 winmm)
    if(MINGW)
        set_target_properties(hammerjs PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")
    endif()
endif()


install(TARGETS hammerjs DESTINATION bin)
