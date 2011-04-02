include_directories(src)

set(V8_SOURCES
    src/dtoa-config.c
    src/accessors.cc
    src/allocation.cc
    src/api.cc
    src/assembler.cc
    src/ast.cc
    src/bootstrapper.cc
    src/builtins.cc
    src/cached-powers.cc
    src/checks.cc
    src/circular-queue.cc
    src/code-stubs.cc
    src/codegen.cc
    src/compilation-cache.cc
    src/compiler.cc
    src/contexts.cc
    src/conversions.cc
    src/counters.cc
    src/cpu-profiler.cc
    src/data-flow.cc
    src/dateparser.cc
    src/debug-agent.cc
    src/debug.cc
    src/disassembler.cc
    src/diy-fp.cc
    src/dtoa.cc
    src/execution.cc
    src/factory.cc
    src/flags.cc
    src/frame-element.cc
    src/frames.cc
    src/full-codegen.cc
    src/func-name-inferrer.cc
    src/global-handles.cc
    src/fast-dtoa.cc
    src/fixed-dtoa.cc
    src/handles.cc
    src/hashmap.cc
    src/heap-profiler.cc
    src/heap.cc
    src/ic.cc
    src/interpreter-irregexp.cc
    src/jsregexp.cc
    src/jump-target.cc
    src/liveedit.cc
    src/log-utils.cc
    src/log.cc
    src/mark-compact.cc
    src/messages.cc
    src/objects.cc
    src/objects-visiting.cc
    src/oprofile-agent.cc
    src/parser.cc
    src/profile-generator.cc
    src/property.cc
    src/regexp-macro-assembler-irregexp.cc
    src/regexp-macro-assembler.cc
    src/regexp-stack.cc
    src/register-allocator.cc
    src/rewriter.cc
    src/runtime.cc
    src/scanner.cc
    src/scopeinfo.cc
    src/scopes.cc
    src/serialize.cc
    src/snapshot-common.cc
    src/snapshot-empty.cc
    src/spaces.cc
    src/string-search.cc
    src/string-stream.cc
    src/strtod.cc
    src/stub-cache.cc
    src/token.cc
    src/top.cc
    src/type-info.cc
    src/unicode.cc
    src/utils.cc
    src/v8-counters.cc
    src/v8.cc
    src/v8threads.cc
    src/variables.cc
    src/version.cc
    src/virtual-frame.cc
    src/vm-state.cc
    src/zone.cc
)

# generated
set(V8_SOURCES ${V8_SOURCES} generated/libraries.cc)

# FIXME: this should 64-bit vs 32-bit architecture instead
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DV8_TARGET_ARCH_X64)
    include_directories(src/x64)
    set(V8_SOURCES ${V8_SOURCES}
        src/jump-target-heavy.cc
        src/virtual-frame-heavy.cc
        src/x64/assembler-x64.cc
        src/x64/builtins-x64.cc
        src/x64/code-stubs-x64.cc
        src/x64/codegen-x64.cc
        src/x64/cpu-x64.cc
        src/x64/debug-x64.cc
        src/x64/disasm-x64.cc
        src/x64/frames-x64.cc
        src/x64/full-codegen-x64.cc
        src/x64/ic-x64.cc
        src/x64/jump-target-x64.cc
        src/x64/macro-assembler-x64.cc
        src/x64/regexp-macro-assembler-x64.cc
        src/x64/register-allocator-x64.cc
        src/x64/stub-cache-x64.cc
        src/x64/virtual-frame-x64.cc
    )
else()
    add_definitions(-DV8_TARGET_ARCH_IA32)
    include_directories(src/x32)
    set(V8_SOURCES ${V8_SOURCES}
        src/jump-target-heavy.cc
        src/virtual-frame-heavy.cc
        src/ia32/assembler-ia32.cc
        src/ia32/builtins-ia32.cc
        src/ia32/code-stubs-ia32.cc
        src/ia32/codegen-ia32.cc
        src/ia32/cpu-ia32.cc
        src/ia32/debug-ia32.cc
        src/ia32/disasm-ia32.cc
        src/ia32/frames-ia32.cc
        src/ia32/full-codegen-ia32.cc
        src/ia32/ic-ia32.cc
        src/ia32/jump-target-ia32.cc
        src/ia32/macro-assembler-ia32.cc
        src/ia32/regexp-macro-assembler-ia32.cc
        src/ia32/register-allocator-ia32.cc
        src/ia32/stub-cache-ia32.cc
        src/ia32/virtual-frame-ia32.cc
    )
endif()

add_definitions(-DENABLE_VMSTATE_TRACKING)
add_definitions(-DENABLE_LOGGING_AND_PROFILING)
add_definitions(-DENABLE_DEBUGGER_SUPPORT)


# platform-specific file
if(UNIX)
    set(V8_SOURCES ${V8_SOURCES} src/platform-posix.cc)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(V8_SOURCES ${V8_SOURCES} src/platform-linux.cc)
    add_definitions(-ansi)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(V8_SOURCES ${V8_SOURCES} src/platform-macos.cc)
    add_definitions(-mmacosx-version-min=10.4)
    add_definitions(-ansi)
    add_definitions(-O3)
endif()

if(WIN32)
    add_definitions(-DWIN32)
    set(V8_SOURCES ${V8_SOURCES}
        src/platform-win32.cc
    )
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-Wall)
    add_definitions(-fno-rtti -fno-exceptions)
    if(NOT MINGW)
        add_definitions(-fvisibility=hidden)
    endif()
    add_definitions(-Wno-uninitialized -fomit-frame-pointer -fdata-sections -ffunction-sections)
endif(CMAKE_COMPILER_IS_GNUCXX)

add_library(v8 STATIC ${V8_SOURCES})

# FIXME: this should 64-bit vs 32-bit architecture instead
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(v8 PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
else()
    set_target_properties(v8 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()
