cmake_minimum_required(VERSION 3.25)
if(POLICY CMP0184)
    cmake_policy(SET CMP0184 NEW)
endif()
project(Moulang VERSION 0.1.1 LANGUAGES C CXX)

set(MOULANG_MSVC_INCLUDE_PREFIX "" CACHE STRING "Override the localized MSVC /showIncludes prefix for Ninja")
if(CMAKE_GENERATOR MATCHES "^Ninja" AND MOULANG_MSVC_INCLUDE_PREFIX)
    set(CMAKE_CL_SHOWINCLUDES_PREFIX "${MOULANG_MSVC_INCLUDE_PREFIX}")
endif()

if(NOT WIN32 OR NOT MSVC OR NOT CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "x64")
    message(FATAL_ERROR "Moulang Phase 1 requires an x64 Windows MSVC toolchain.")
endif()

find_package(LLVM REQUIRED CONFIG)
if(LLVM_PACKAGE_VERSION VERSION_LESS 22)
    message(FATAL_ERROR "Moulang requires LLVM 22 or newer.")
endif()
if(NOT "X86" IN_LIST LLVM_TARGETS_TO_BUILD)
    message(FATAL_ERROR "The LLVM package must include the X86 target.")
endif()
message(STATUS "Using LLVM ${LLVM_PACKAGE_VERSION} from ${LLVM_DIR}")
llvm_map_components_to_libnames(moulang_llvm core target passes X86)

# The official Windows archive can reference the release builder's DIA SDK.
if(TARGET LLVMDebugInfoPDB)
    get_target_property(moulang_pdb_deps LLVMDebugInfoPDB INTERFACE_LINK_LIBRARIES)
    foreach(dependency IN LISTS moulang_pdb_deps)
        if(IS_ABSOLUTE "${dependency}" AND dependency MATCHES "[/\\\\]diaguids\\.lib$"
                AND NOT EXISTS "${dependency}")
            find_library(MOULANG_DIAGUIDS NAMES diaguids REQUIRED
                HINTS "$ENV{VSINSTALLDIR}/DIA SDK/lib/amd64")
            list(TRANSFORM moulang_pdb_deps REPLACE "^.*[/\\\\]diaguids\\.lib$" "${MOULANG_DIAGUIDS}")
            set_target_properties(LLVMDebugInfoPDB PROPERTIES INTERFACE_LINK_LIBRARIES "${moulang_pdb_deps}")
            break()
        endif()
    endforeach()
endif()

find_library(MOULANG_KERNEL32 NAMES kernel32 REQUIRED HINTS ENV LIB)
find_library(MOULANG_LIBCMT NAMES libcmt REQUIRED HINTS ENV LIB)
find_library(MOULANG_LIBCPMT NAMES libcpmt REQUIRED HINTS ENV LIB)
find_library(MOULANG_LIBVCRUNTIME NAMES libvcruntime REQUIRED HINTS ENV LIB)
set(MOULANG_LINKER "${CMAKE_LINKER}")
configure_file(cmake/toolchain.hpp.in generated/toolchain.hpp @ONLY)

add_library(moulang_runtime STATIC runtime/src/runtime.cpp runtime/src/real.cpp)
target_compile_features(moulang_runtime PRIVATE cxx_std_20)
target_compile_options(moulang_runtime PRIVATE /GS- /Zl /W4 /utf-8)
# Keep the runtime compatible with the standalone executable's minimal CRT.
target_compile_options(moulang_runtime PRIVATE $<$<CONFIG:Debug>:/O2>)
set_property(TARGET moulang_runtime PROPERTY MSVC_RUNTIME_CHECKS "")
set_target_properties(moulang_runtime PROPERTIES
    MSVC_RUNTIME_LIBRARY MultiThreaded
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_executable(moulang
    src/cli/main.cpp
    src/source/source.cpp
    src/source/program.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/semantic/semantic.cpp
    src/codegen/llvm/object.cpp
    src/driver/windows.cpp
)
target_compile_features(moulang PRIVATE cxx_std_20)
target_compile_options(moulang PRIVATE /W4 /utf-8)
target_include_directories(moulang PRIVATE include "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_include_directories(moulang SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(moulang PRIVATE ${moulang_llvm})
set_target_properties(moulang PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
add_dependencies(moulang moulang_runtime)

install(TARGETS moulang moulang_runtime RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)

include(CTest)
if(BUILD_TESTING)
    find_program(MOULANG_POWERSHELL NAMES pwsh powershell REQUIRED)
    add_test(NAME moulang.integration COMMAND "${MOULANG_POWERSHELL}"
        -NoProfile -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/tests/integration/compiler.ps1"
        -Compiler "$<TARGET_FILE:moulang>" -Hello "${CMAKE_SOURCE_DIR}/examples/hello.m"
        -TestRoot "${CMAKE_BINARY_DIR}/integration")
    set_tests_properties(moulang.integration PROPERTIES TIMEOUT 45)
endif()
