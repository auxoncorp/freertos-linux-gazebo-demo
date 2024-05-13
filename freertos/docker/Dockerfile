FROM auxondemoimages/demo-freertos-base:latest

ARG DEBIAN_FRONTEND=noninteractive

RUN mkdir -p /app
WORKDIR /app
COPY cmake/ cmake/
COPY config/ config/
COPY include/ include/
COPY linker/ linker/
COPY sensor_models/ sensor_models/
COPY src/ src/
COPY CMakeLists.txt .
RUN rm -rf build && mkdir build
RUN cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/gcc-toolchain.cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && make -j

COPY emulate.resc .
COPY renode/ renode/

RUN mkdir -p /root/.config/renode
COPY docker/renode.config /root/.config/renode/config

COPY docker/entrypoint.sh /
CMD ["/entrypoint.sh"]
