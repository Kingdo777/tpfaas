include(FindGit)
find_package(Git)
include(ExternalProject)
include(FetchContent)

include_directories(${CMAKE_INSTALL_PREFIX}/include)

# Tightly-coupled dependencies
set(FETCHCONTENT_QUIET OFF)

# RapidJSON
set(RAPIDJSON_BUILD_DOC OFF CACHE INTERNAL "")
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(RAPIDJSON_BUILD_TESTS OFF CACHE INTERNAL "")
FetchContent_Declare(rapidjson_ext
        GIT_REPOSITORY "https://github.com/Tencent/rapidjson"
        GIT_TAG "2ce91b823c8b4504b9c40f99abf00917641cef6c"
        CMAKE_CACHE_ARGS "-DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}"
        )

# WAVM
FetchContent_Declare(wavm_ext
        GIT_REPOSITORY "https://github.com/Kingdo777/WAVM.git"
        CMAKE_ARGS "-DDLL_EXPORT= \
        -DDLL_IMPORT="
        )

FetchContent_MakeAvailable(wavm_ext rapidjson_ext)

# Allow access to headers nested in other projects
FetchContent_GetProperties(wavm_ext SOURCE_DIR WAVM_SOURCE_DIR)
message(STATUS WAVM_SOURCE_DIR ${WAVM_SOURCE_DIR})

FetchContent_GetProperties(rapidjson_ext SOURCE_DIR RapidJSON_SOURCE_DIR)
message(STATUS RapidJSON_SOURCE_DIR ${RapidJSON_SOURCE_DIR})