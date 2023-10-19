FROM ubuntu:23.10 as builder

RUN apt-get update
RUN echo "Prepare dependencies"
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y git build-essential autoconf libtool pkg-config cmake clang libc++-dev

WORKDIR /base

RUN echo "********************* START script*********************"


RUN echo "build grpc SCRIPT"
RUN git clone -b v1.58.0 https://github.com/grpc/grpc && \
    cd grpc && \
    git submodule update --init --recursive

WORKDIR /base/grpc

RUN echo "Install absl"
RUN mkdir -p "third_party/abseil-cpp/cmake/build" && \
    cd third_party/abseil-cpp/cmake/build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../.. && \
    make install && \
    cd /base/grpc

RUN mkdir -p "third_party/cares/cares/cmake/build"  && \
    cd third_party/cares/cares/cmake/build  && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../..  && \
    make install  && \
    cd /base/grpc

RUN echo "Install protobuf"
RUN mkdir -p "third_party/protobuf/cmake/build" && \
    cd third_party/protobuf/cmake/build && \
    cmake -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -Dprotobuf_ABSL_PROVIDER=package -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../..  && \
    make install && \
    cd /base/grpc

RUN echo "Install re2"
RUN mkdir -p "third_party/re2/cmake/build" && \
    cd third_party/re2/cmake/build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../.. && \
    make install && \
    cd /base/grpc

RUN echo "Install zlib"
RUN mkdir -p "third_party/zlib/cmake/build" && \
    cd third_party/zlib/cmake/build && \
    cmake -DCMAKE_BUILD_TYPE=Release ../.. && \
    make install && \
    cd /base/grpc

RUN apt install -y libssl-dev
RUN cmake -Bbuild \
      -DCMAKE_BUILD_TYPE=Release \
      -DgRPC_INSTALL=ON \
      -DgRPC_ABSL_PROVIDER=package \
      -DgRPC_CARES_PROVIDER=package \
      -DgRPC_PROTOBUF_PROVIDER=package \
      -DgRPC_RE2_PROVIDER=package \
      -DgRPC_SSL_PROVIDER=package \
      -DgRPC_ZLIB_PROVIDER=package  && \
      #-DBUILD_SHARED_LIBS=ON \
      #-DCMAKE_INSTALL_PREFIX=${{github.workspace}}/grpc/grpc_install
    cmake --build build --target install

RUN echo "********************* end of the script*********************"

COPY . /base/
WORKDIR /base
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
#RUN cp ./build/liblugo4cpp.so ./build/install/bin/liblugo4cpp.so
#RUN cp ./build/liblugo_proto.a ./build/install/bin/liblugo_proto.a




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
