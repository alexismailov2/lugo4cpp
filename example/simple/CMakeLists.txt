set(CMAKE_CXX_STANDARD 17)

# Generating main project
add_executable(lugo4cpp_example
        main.cpp)

#target_include_directories(lugo4cpp_example
#        PRIVATE
#        include
#        ${CMAKE_CURRENT_BINARY_DIR}
#        ${Protobuf_INCLUDE_DIRS})

target_link_libraries(lugo4cpp_example
        PRIVATE
        lugo4cpp
        lugo_proto
        ${Protobuf_LIBRARIES}
        gRPC::grpc++)