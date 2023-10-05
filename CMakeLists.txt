cmake_minimum_required(VERSION 3.22)
project(lugo4cpp)

set(CMAKE_CXX_STANDARD 17)
include(CMakeFindDependencyMacro)

option(BUILD_SHARED_LIBRARY "Select shared library build or static" ON)
# Use option to control which type of library you will prefer
if (BUILD_SHARED_LIBRARY)
    set(LIBRARY_TYPE SHARED)
else()
    set(LIBRARY_TYPE STATIC)
endif()

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
find_package(absl REQUIRED)
find_package(utf8_range REQUIRED)

set(PROTO_FILES
        pb/src/broadcast.proto
        pb/src/health.proto
        pb/src/physics.proto
        pb/src/remote.proto
        pb/src/server.proto)

add_library(lugo_proto
        ${PROTO_FILES})

make_directory(${CMAKE_CURRENT_BINARY_DIR}/pb)

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(
        TARGET lugo_proto
        LANGUAGE cpp
        IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/pb/src"
        PROTOC_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/pb")
protobuf_generate(
        TARGET lugo_proto
        LANGUAGE grpc
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
        IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/pb/src"
        PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}"
        PROTOC_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/pb")

target_include_directories(lugo_proto
        PUBLIC
        ${PROTOBUF_INCLUDE_DIR})

# Generating main project
add_library(lugo4cpp
        ${LIBRARY_TYPE}
        src/client.cpp
        src/configurator.cpp
        src/geo.cpp
        src/goal.cpp
        src/orientation.cpp
        src/index.cpp
        src/mapper.cpp
        src/stub.cpp)

target_include_directories(lugo4cpp
        PUBLIC
        include
        ${CMAKE_CURRENT_BINARY_DIR}
        ${Protobuf_INCLUDE_DIRS})

target_link_libraries(lugo4cpp
        PUBLIC
        lugo_proto
        ${Protobuf_LIBRARIES}
        gRPC::grpc++
        utf8_range::utf8_validity
        absl::log_internal_check_op)

add_subdirectory(example/simple)

# install the binaries and include files
install(TARGETS lugo4cpp lugo_proto
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY include/lugo DESTINATION include FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pb DESTINATION include FILES_MATCHING PATTERN "*.h")