FROM rubens21/cpp_compiler:v1

COPY . lugo
RUN cd lugo && \
    cmake -Bbuild -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./build/install && \
    cmake --build build --config Release --target install

# cmake -B<temporary folder for project generation, could be not only Makefile>
#       -DCMAKE_BUILD_TYPE=<Release|Debug|etc>
#       -DCMAKE_INSTALL_PREFIX=<path for install command, install configured over CMakeLists.txt script>
# cmake --build <temporary folder for project generation(created on -B previous stage), could be not only Makefile>
#       --config <Release|Debug|etc(some build systems need it here so it is universal approach)>
#       --target <it can be some of the target which defined bu CMakeLists.txt, in our case i suggest use "install" due to install include all dependencies>
# TODO: Please fix entry point, all installation apeared in build folder appeared in the root folder of the project after install
ENTRYPOINT ["/builder/grpc/lugo/build/install/bin/lugo4cpp_example"]
CMD []
