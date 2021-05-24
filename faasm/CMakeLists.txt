cmake_minimum_required(VERSION 3.13.0)
project(faasm)

# Optional functionality
option(FAASM_STATIC_LIBS "Statically link Faasm libs" ON)

# Performance functionality
option(FAASM_SELF_TRACING "Turn on system tracing using the logger" ON)
option(FAASM_PERF_PROFILING "Turn on profiling features as described in debugging.md" OFF)

# Top-level CMake config
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Faasm directories
set(FAASM_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(FAASM_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(FAASM_NATIVE_INSTALL "/usr/local/faasm/native")


# CPP REST SDK
set(cpprestsdk_DIR /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}/cmake/)
find_package(cpprestsdk REQUIRED NAMES cpprestsdk cpprest)

# ----------------------------------------
# Faasm configuration
# ----------------------------------------

# Library type (for shared libraries)
if (FAASM_STATIC_LIBS)
    set(FAABRIC_STATIC_LIBS ON CACHE BOOL "Faabric static")
    function(faasm_public_lib lib_name lib_deps)
        add_library(${lib_name} STATIC ${lib_deps})
    endfunction()
    function(faasm_private_lib lib_name lib_deps)
        add_library(${lib_name} STATIC ${lib_deps})
    endfunction()
else ()
    set(FAABRIC_STATIC_LIBS OFF CACHE BOOL "Faabric static")
    function(faasm_public_lib lib_name lib_deps)
        add_library(${lib_name} SHARED ${lib_deps})
    endfunction()
    function(faasm_private_lib lib_name lib_deps)
        add_library(${lib_name} STATIC ${lib_deps})
        target_compile_options(${lib_name} PRIVATE "-fPIC")
    endfunction()
endif ()

# Faasm profiling
if (${FAASM_SELF_TRACING})
    message("-- Activated FAASM tracing")
    add_definitions(-DTRACE_ALL=1)
endif ()

# LLVM config
if (${FAASM_PERF_PROFILING})
    # In accordance with bin/build_llvm_perf.sh and LLVM version for WAVM
    set(LLVM_DIR ${CMAKE_CURRENT_LIST_DIR}/dev/llvm-perf/build/lib/cmake/llvm)
    message(STATUS "Using custom LLVM at ${LLVM_DIR} for profiling")
    find_package(LLVM REQUIRED CONFIG NO_DEFAULT_PATH)

    set(WAVM_PERF_LIBS 1)
    add_definitions(-DWAVM_PERF_EVENTS=1)

    # Don't omit frame pointers (lets perf get better stack traces)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer")
else ()
    find_package(LLVM 10 REQUIRED CONFIG)
endif ()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Third party deps
include(cmake/ExternalProjects.cmake)

# Faabric
add_subdirectory(faabric)
include_directories(${CMAKE_CURRENT_LIST_DIR}/faabric/include)

# More include directories
include_directories(
        ${FAASM_INCLUDE_DIR}
        ${FAASM_PYTHON_LIB_DIR}
)

# Faasm runtime
add_subdirectory(src/conf)
add_subdirectory(src/faaslet)
add_subdirectory(src/ir_cache)
add_subdirectory(src/module_cache)
add_subdirectory(src/runner)
add_subdirectory(src/storage)
add_subdirectory(src/system)
add_subdirectory(src/threads)
add_subdirectory(src/upload)
add_subdirectory(src/wasm)
add_subdirectory(src/wavm)
