FROM ubuntu:23.10

RUN apt-get update

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential autoconf libtool pkg-config cmake clang libc++-dev

RUN apt-get install -y git build-essential autoconf libtool pkg-config cmake clang libc++-dev

WORKDIR /builder

RUN git clone -b v1.58.0 https://github.com/grpc/grpc

WORKDIR /builder/grpc

RUN git submodule update --init --recursive && \
    echo "Install absl" && \
    mkdir -p "third_party/abseil-cpp/cmake/build" && \
    cd third_party/abseil-cpp/cmake/build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../.. && \
    make install && \
    cd /builder/grpc

RUN echo "Install c-ares"
# If the distribution provides a new-enough version of c-ares,
# this section can be replaced with:
# apt-get install -y libc-ares-dev
RUN mkdir -p "third_party/cares/cares/cmake/build"
RUN cd third_party/cares/cares/cmake/build && \
    cmake -DCMAKE_BUILD_TYPE=Release ../.. && \
    make install && \
    cd /builder/grpc

RUN echo "Install protobuf"
RUN mkdir -p "third_party/protobuf/cmake/build"  && \
    cd third_party/protobuf/cmake/build  && \
    cmake -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -Dprotobuf_ABSL_PROVIDER=package ../..  && \
    make install  && \
    cd /builder/grpc

RUN echo "Install re2"
RUN mkdir -p "third_party/re2/cmake/build" && \
    cd third_party/re2/cmake/build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../..  && \
    make install  && \
    cd /builder/grpc
#
RUN echo "Install zlib"
RUN mkdir -p "third_party/zlib/cmake/build"  && \
    cd third_party/zlib/cmake/build  && \
    cmake -DCMAKE_BUILD_TYPE=Release ../..  && \
    make install  && \
    cd /builder/grpc

RUN echo "Last step"
RUN apt install -y libssl-dev

RUN cmake -Bbuild \
      -DCMAKE_BUILD_TYPE=Release \
      -DgRPC_INSTALL=ON \
      -DgRPC_ABSL_PROVIDER=package \
      -DgRPC_CARES_PROVIDER=package \
      -DgRPC_PROTOBUF_PROVIDER=package \
      -DgRPC_RE2_PROVIDER=package \
      -DgRPC_SSL_PROVIDER=package \
      -DgRPC_ZLIB_PROVIDER=package && \
      cmake --build build --target install

RUN echo "Configure CMake"
RUN ls -l build/
RUN cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=./grpc/grpc_install

COPY . lugo
RUN cd lugo && \
    cmake -DCMAKE_BUILD_TYPE=Release .  && \
    make

RUN ls -l

RUN cd lugo/example/simple && \
    cmake -DCMAKE_BUILD_TYPE=Release .


ENTRYPOINT ["/builder/grpc/lugo/example/simple/lugo4cpp_example"]
CMD []
