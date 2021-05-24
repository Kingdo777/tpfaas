include(FindGit)
find_package(Git)
include(ExternalProject)
include(FetchContent)

include_directories(${CMAKE_INSTALL_PREFIX}/include)

# Tightly-coupled dependencies
set(FETCHCONTENT_QUIET OFF)

# Pistache
ExternalProject_Add(pistache_ext
        GIT_REPOSITORY "https://github.com/pistacheio/pistache.git"
        GIT_TAG "2ef937c434810858e05d446e97acbdd6cc1a5a36"
        CMAKE_CACHE_ARGS "-DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}"
        BUILD_BYPRODUCTS ${CMAKE_INSTALL_PREFIX}/lib/libpistache.so
        )
ExternalProject_Get_Property(pistache_ext SOURCE_DIR)
set(PISTACHE_INCLUDE_DIR ${SOURCE_DIR}/include)
add_library(pistache SHARED IMPORTED)
add_dependencies(pistache pistache_ext)
set_target_properties(pistache
        PROPERTIES IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/lib/libpistache.so
        )

# WAVM
FetchContent_Declare(wavm_ext
        GIT_REPOSITORY "https://github.com/Kingdo777/WAVM.git"
        CMAKE_ARGS "-DDLL_EXPORT= \
        -DDLL_IMPORT="
        )

FetchContent_MakeAvailable(wavm_ext)
FetchContent_GetProperties(wavm_ext SOURCE_DIR WAVM_SOURCE_DIR)

# spdlog
ExternalProject_Add(spdlog_ext
        GIT_REPOSITORY "https://github.com/gabime/spdlog"
        GIT_TAG "v1.8.0"
        CMAKE_CACHE_ARGS "-DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}"
        )
ExternalProject_Get_Property(spdlog_ext SOURCE_DIR)
set(RapidJSON_SOURCE_DIR ${SOURCE_DIR}/include)

# RapidJSON
set(RAPIDJSON_BUILD_DOC OFF CACHE INTERNAL "")
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(RAPIDJSON_BUILD_TESTS OFF CACHE INTERNAL "")
ExternalProject_Add(rapidjson_ext
        GIT_REPOSITORY "https://github.com/Tencent/rapidjson"
        GIT_TAG "2ce91b823c8b4504b9c40f99abf00917641cef6c"
        CMAKE_CACHE_ARGS "-DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}"
        )
ExternalProject_Get_Property(rapidjson_ext SOURCE_DIR)
set(SPDLOG_INCLUDE_DIR ${SOURCE_DIR}/include)

# cppcodec (for base64)
ExternalProject_Add(cppcodec_ext
        GIT_REPOSITORY "https://github.com/tplgy/cppcodec"
        GIT_TAG "v0.2"
        CMAKE_ARGS "-DBUILD_TESTING=OFF"
        CMAKE_CACHE_ARGS "-DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}"
        )
ExternalProject_Get_Property(cppcodec_ext SOURCE_DIR)
set(CPPCODEC_INCLUDE_DIR ${SOURCE_DIR})

#zstd
set(ZSTD_BUILD_CONTRIB OFF CACHE INTERNAL "")
set(ZSTD_BUILD_CONTRIB OFF CACHE INTERNAL "")
set(ZSTD_BUILD_PROGRAMS OFF CACHE INTERNAL "")
set(ZSTD_BUILD_SHARED OFF CACHE INTERNAL "")
set(ZSTD_BUILD_STATIC ON CACHE INTERNAL "")
set(ZSTD_BUILD_TESTS OFF CACHE INTERNAL "")
# This means zstd doesn't use threading internally,
# not that it can't be used in a multithreaded context
set(ZSTD_MULTITHREAD_SUPPORT OFF CACHE INTERNAL "")
set(ZSTD_LEGACY_SUPPORT OFF CACHE INTERNAL "")
set(ZSTD_ZLIB_SUPPORT OFF CACHE INTERNAL "")
set(ZSTD_LZMA_SUPPORT OFF CACHE INTERNAL "")
set(ZSTD_LZ4_SUPPORT OFF CACHE INTERNAL "")

#find_package(zstd CONFIG REQUIRED)
#message(STATUS "Using zstd ${gRPC_VERSION}")

FetchContent_Declare(zstd_ext
        GIT_REPOSITORY "https://github.com/facebook/zstd"
        GIT_TAG "v1.4.9"
        SOURCE_SUBDIR "build/cmake"
        )

FetchContent_MakeAvailable(zstd_ext)
# Work around zstd not declaring its targets properly
target_include_directories(libzstd_static INTERFACE $<BUILD_INTERFACE:${zstd_ext_SOURCE_DIR}/lib>)
add_library(zstd::libzstd_static ALIAS libzstd_static)
