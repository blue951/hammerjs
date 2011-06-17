cmake_minimum_required(VERSION 2.6)

project(HammerJS)

# FIXME: this should detect 64-bit vs 32-bit architecture instead
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(HAMMERJS_ARCH "x64")
    add_definitions(-mmacosx-version-min=10.5)
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -mmacosx-version-min=10.5")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -mmacosx-version-min=10.5")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mmacosx-version-min=10.5")
else()
    set(HAMMERJS_ARCH "ia32")
endif()

add_subdirectory(v8)
add_subdirectory(modules)

if(CMAKE_COMPILER_IS_GNUCXX )
   add_definitions(-Wall)
endif(CMAKE_COMPILER_IS_GNUCXX )

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif(MSVC)

include_directories(${PROJECT_SOURCE_DIR}/v8/include)
add_executable(hammerjs hammerjs.cpp)

if(${HAMMERJS_ARCH} MATCHES "x64")
    set_target_properties(hammerjs PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
    set_target_properties(hammerjs_reflect PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
    set_target_properties(hammerjs_system PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
    set_target_properties(hammerjs_fs PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
    set_target_properties(v8 PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
else()
    set_target_properties(hammerjs PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    set_target_properties(hammerjs_reflect PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    set_target_properties(hammerjs_system PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    set_target_properties(hammerjs_fs PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    set_target_properties(v8 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()

link_directories(${PROJECT_SOURCE_DIR}/v8)
target_link_libraries(hammerjs v8)

target_link_libraries(hammerjs hammerjs_system)
target_link_libraries(hammerjs hammerjs_fs)
target_link_libraries(hammerjs hammerjs_reflect)

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
