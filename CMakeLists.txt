cmake_minimum_required(VERSION 3.22)
project(lugo4cpp)

set(CMAKE_CXX_STANDARD 17)
include(CMakeFindDependencyMacro)

# Downloading github subproject with proto files
include(FetchContent)
FetchContent_Declare(pb
        GIT_REPOSITORY https://github.com/lugobots/protos
        GIT_TAG origin/master
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/pb")
FetchContent_MakeAvailable(pb)

# Generating protobuf and wrap it as library proto
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(PROTO_FILES
        pb/src/broadcast.proto
        pb/src/health.proto
        pb/src/physics.proto
        pb/src/remote.proto
        pb/src/server.proto)

add_library(lugo_proto
        ${PROTO_FILES})

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(
        TARGET lugo_proto
        LANGUAGE cpp
        IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/pb/src")
protobuf_generate(
        TARGET lugo_proto
        LANGUAGE grpc
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
        IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/pb/src"
        PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")

target_include_directories(lugo_proto
        PUBLIC
        ${PROTOBUF_INCLUDE_DIR})

# Generating main project
add_library(lugo4cpp
        src/mapper.cpp)

target_include_directories(lugo4cpp
        PUBLIC
        include
        ${CMAKE_CURRENT_BINARY_DIR}
        ${Protobuf_INCLUDE_DIRS})

target_link_libraries(lugo4cpp
        PRIVATE
        lugo_proto
        ${Protobuf_LIBRARIES}
        gRPC::grpc++)

add_subdirectory(example/simple)
