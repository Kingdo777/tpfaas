include(FindGit)
find_package(Git)
include (ExternalProject)
include (FetchContent)

# Protobuf/ grpc config
# See the example in the gRPC repo here:
# https://github.com/grpc/grpc/blob/master/examples/cpp/helloworld/CMakeLists.txt
if(BUILD_SHARED_LIBS)
    set(Protobuf_USE_STATIC_LIBS OFF)
else()
    set(Protobuf_USE_STATIC_LIBS ON)
endif()

include(FindProtobuf)
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf REQUIRED)

message(STATUS "Using protobuf  \
    ${PROTOBUF_LIBRARY} \
    ${PROTOBUF_PROTOC_LIBRARY} \
    ${PROTOBUF_PROTOC_EXECUTABLE} \
")

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

include_directories(${PROTOBUF_INCLUDE_DIR})

set(PROTOC_EXE /usr/local/bin/protoc)
set(GRPC_PLUGIN /usr/local/bin/grpc_cpp_plugin)

# Include FlatBuffers
# I couldn't get the proper find_package working, so we have this hack now, and
# assume that FB is installed where we think it should be.
set(FLATBUFFERS_FLATC_EXECUTABLE "/usr/local/bin/flatc")
set(FLATBUFFERS_INCLUDE_DIRS "/usr/local/include/flatbuffers")