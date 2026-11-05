cmake_minimum_required(VERSION 3.25)
project(Melang VERSION 0.1.0 LANGUAGES C CXX)

set(MELANG_MSVC_INCLUDE_PREFIX "" CACHE STRING "Override the localized MSVC /showIncludes prefix for Ninja")
if(CMAKE_GENERATOR MATCHES "^Ninja" AND MELANG_MSVC_INCLUDE_PREFIX)
    set(CMAKE_CL_SHOWINCLUDES_PREFIX "${MELANG_MSVC_INCLUDE_PREFIX}")
endif()

if(NOT WIN32 OR NOT MSVC OR NOT CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "x64")
    message(FATAL_ERROR "Melang Phase 1 requires an x64 Windows MSVC toolchain.")
endif()

find_package(LLVM REQUIRED CONFIG)
if(LLVM_PACKAGE_VERSION VERSION_LESS 22)
    message(FATAL_ERROR "Melang requires LLVM 22 or newer.")
endif()
if(NOT "X86" IN_LIST LLVM_TARGETS_TO_BUILD)
    message(FATAL_ERROR "The LLVM package must include the X86 target.")
endif()
message(STATUS "Using LLVM ${LLVM_PACKAGE_VERSION} from ${LLVM_DIR}")
llvm_map_components_to_libnames(melang_llvm core target passes X86)

# The official Windows archive can reference the release builder's DIA SDK.
if(TARGET LLVMDebugInfoPDB)
    get_target_property(melang_pdb_deps LLVMDebugInfoPDB INTERFACE_LINK_LIBRARIES)
    foreach(dependency IN LISTS melang_pdb_deps)
        if(IS_ABSOLUTE "${dependency}" AND dependency MATCHES "[/\\\\]diaguids\\.lib$"
                AND NOT EXISTS "${dependency}")
            find_library(MELANG_DIAGUIDS NAMES diaguids REQUIRED
                HINTS "$ENV{VSINSTALLDIR}/DIA SDK/lib/amd64")
            list(TRANSFORM melang_pdb_deps REPLACE "^.*[/\\\\]diaguids\\.lib$" "${MELANG_DIAGUIDS}")
            set_target_properties(LLVMDebugInfoPDB PROPERTIES INTERFACE_LINK_LIBRARIES "${melang_pdb_deps}")
            break()
        endif()
    endforeach()
endif()

find_library(MELANG_KERNEL32 NAMES kernel32 REQUIRED HINTS ENV LIB)
find_library(MELANG_LIBCMT NAMES libcmt REQUIRED HINTS ENV LIB)
find_library(MELANG_LIBCPMT NAMES libcpmt REQUIRED HINTS ENV LIB)
find_library(MELANG_LIBVCRUNTIME NAMES libvcruntime REQUIRED HINTS ENV LIB)
set(MELANG_LINKER "${CMAKE_LINKER}")
configure_file(cmake/toolchain.hpp.in generated/toolchain.hpp @ONLY)

add_library(melang_runtime STATIC runtime/src/runtime.cpp runtime/src/real.cpp)
target_compile_features(melang_runtime PRIVATE cxx_std_20)
target_compile_options(melang_runtime PRIVATE /GS- /Zl /W4 /utf-8)
set_target_properties(melang_runtime PROPERTIES
    MSVC_RUNTIME_LIBRARY MultiThreaded
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_executable(melang
    src/cli/main.cpp
    src/source/source.cpp
    src/source/program.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/semantic/semantic.cpp
    src/codegen/llvm/object.cpp
    src/driver/windows.cpp
)
target_compile_features(melang PRIVATE cxx_std_20)
target_compile_options(melang PRIVATE /W4 /utf-8)
target_include_directories(melang PRIVATE include "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_include_directories(melang SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(melang PRIVATE ${melang_llvm})
set_target_properties(melang PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
add_dependencies(melang melang_runtime)

install(TARGETS melang melang_runtime RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)

include(CTest)
if(BUILD_TESTING)
    find_program(MELANG_POWERSHELL NAMES pwsh powershell REQUIRED)
    add_test(NAME melang.integration COMMAND "${MELANG_POWERSHELL}"
        -NoProfile -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/tests/integration/compiler.ps1"
        -Compiler "$<TARGET_FILE:melang>" -Hello "${CMAKE_SOURCE_DIR}/examples/hello.m"
        -TestRoot "${CMAKE_BINARY_DIR}/integration")
    set_tests_properties(melang.integration PROPERTIES TIMEOUT 45)
endif()
