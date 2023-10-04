FROM rubens21/cpp_compiler:v1

COPY . lugo
RUN cd lugo && \
    cmake -DCMAKE_BUILD_TYPE=Release .  && \
    make

RUN cd lugo/example/simple && \
    cmake -DCMAKE_BUILD_TYPE=Release .


ENTRYPOINT ["/builder/grpc/lugo/example/simple/lugo4cpp_example"]
CMD []
