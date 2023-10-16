FROM rubens21/cpp_compiler:debuging

COPY ./example/simple /base/example/simple

WORKDIR /base/
RUN echo "Configure CMake"
RUN cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=./grpc/grpc_install -DCMAKE_INSTALL_PREFIX=./build/install
RUN echo "Build"
RUN cmake --build ./build --config Release --target install
RUN ls -las && pwd



#RUN ls -las ./build && pwd && ls -las ./build/install/bin
#FROM ubuntu:23.10
#COPY --from=builder /base/build/liblugo4cpp.so /base/build/install/bin/liblugo4cpp.so
#COPY --from=builder /base/build/liblugo_proto.a /base/build/install/bin/liblugo_proto.a
#RUN ls -las ./build && pwd && ls -las ./build/install/bin
RUN cp ./build/liblugo4cpp.so ./build/install/bin/liblugo4cpp.so
RUN cp ./build/liblugo_proto.a ./build/install/bin/liblugo_proto.a

# cmake -B<temporary folder for project generation, could be not only Makefile>
#       -DCMAKE_BUILD_TYPE=<Release|Debug|etc>
#       -DCMAKE_INSTALL_PREFIX=<path for install command, install configured over CMakeLists.txt script>
# cmake --build <temporary folder for project generation(created on -B previous stage), could be not only Makefile>
#       --config <Release|Debug|etc(some build systems need it here so it is universal approach)>
#       --target <it can be some of the target which defined bu CMakeLists.txt, in our case i suggest use "install" due to install include all dependencies>
# TODO: Please fix entry point, all installation apeared in build folder appeared in the root folder of the project after install
ENTRYPOINT ["/base/build/install/bin/lugo4cpp_example"]
CMD []
