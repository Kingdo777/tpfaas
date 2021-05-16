cmake_minimum_required(VERSION 3.12)
project(tpfaas C CXX ASM)


option(TPFAAS_STATIC_LIBS "Statically link Tpfaas libs" ON)
option(TPFAAS_FCGI "If Open the FCGI Module" ON)
option(OPEN_TRACE "If Open Time Trace" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(TPFAAS_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(TPFAAS_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(FAASM_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/faasm)
set(FAASM_INCLUDE_DIR ${FAASM_SOURCE_DIR}/include)
set(FAABRIC_SOURCE_DIR ${FAASM_SOURCE_DIR}/faabric)
set(FAABRIC_INCLUDE_DIR ${FAABRIC_SOURCE_DIR}/include)

# Library type (for shared libraries)
if (TPFAAS_STATIC_LIBS)
    function(tpfaas_public_lib lib_name lib_deps)
        add_library(${lib_name} STATIC ${lib_deps})
    endfunction()
    function(tpfaas_private_lib lib_name lib_deps)
        add_library(${lib_name} STATIC ${lib_deps})
    endfunction()
else ()
    function(tpfaas_public_lib lib_name lib_deps)
        add_library(${lib_name} SHARED ${lib_deps})
    endfunction()
    function(tpfaas_private_lib lib_name lib_deps)
        add_library(${lib_name} STATIC ${lib_deps})
        target_compile_options(${lib_name} PRIVATE "-fPIC")
    endfunction()
endif ()


if (TPFAAS_FCGI)
    add_definitions(-DFCGI_ENABLE)
endif ()
if (OPEN_TRACE)
    add_definitions(-DOPEN_TRACE)
endif ()
add_definitions(-DWAVM_API=)

include(cmake/ExternalProjects.cmake)

include_directories(
        ${FAABRIC_INCLUDE_DIR}
        ${TPFAAS_INCLUDE_DIR}
        ${WAVM_SOURCE_DIR}/Include
        ${RapidJSON_SOURCE_DIR}/include
)

add_subdirectory(faasm/faabric)
add_subdirectory(src/RFIT)
add_subdirectory(src/utils)
add_subdirectory(src/test)

add_executable(tpfaas main.cpp include/test/test.h)
target_link_libraries(tpfaas fcgi RFIT utils)
