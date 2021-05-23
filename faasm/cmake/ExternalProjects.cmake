include(FindGit)
find_package(Git)
include (ExternalProject)
include (FetchContent)

include_directories(${CMAKE_INSTALL_PREFIX}/include)

# Tightly-coupled dependencies
set(FETCHCONTENT_QUIET OFF)
#FetchContent_Declare(wavm_ext
#    GIT_REPOSITORY "https://github.com/faasm/WAVM.git"
#    GIT_TAG "faasm"
#    CMAKE_ARGS "-DDLL_EXPORT= \
#        -DDLL_IMPORT="
#)

FetchContent_Declare(wavm_ext
        GIT_REPOSITORY "https://github.com/Kingdo777/WAVM.git"
        CMAKE_ARGS "-DDLL_EXPORT= \
        -DDLL_IMPORT="
        )

FetchContent_MakeAvailable(wavm_ext)

# Allow access to headers nested in other projects
FetchContent_GetProperties(wavm_ext SOURCE_DIR FAASM_WAVM_SOURCE_DIR)
message(STATUS FAASM_WAVM_SOURCE_DIR ${FAASM_WAVM_SOURCE_DIR})

# General 3rd party dependencies
ExternalProject_Add(eigen_ext
    GIT_REPOSITORY "https://gitlab.com/shillaker/eigen.git"
    GIT_TAG "faasm"
    CMAKE_CACHE_ARGS "-DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}"
)

ExternalProject_Add(catch2_ext
     GIT_REPOSITORY "https://github.com/catchorg/Catch2"
     GIT_TAG "v2.13.2"
     CMAKE_ARGS "-DCATCH_INSTALL_DOCS=OFF \
         -DCATCH_INSTALL_EXTRAS=OFF"
     CMAKE_CACHE_ARGS "-DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}"
)
 
