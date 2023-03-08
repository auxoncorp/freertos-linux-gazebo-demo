FROM ubuntu:jammy

ENV NVIDIA_VISIBLE_DEVICES \
   ${NVIDIA_VISIBLE_DEVICES:-all}
ENV NVIDIA_DRIVER_CAPABILITIES \
   ${NVIDIA_DRIVER_CAPABILITIES:+$NVIDIA_DRIVER_CAPABILITIES,}graphics

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
  git \
  sudo \
  tzdata \
  locales \
  wget \
  gnupg \
  lsb-release \
  build-essential \
  ca-certificates \
  iproute2 \
  netcat \
  vim \
  pkg-config

RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && locale-gen
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8
RUN update-ca-certificates

RUN apt-get install -y mono-complete
RUN wget https://github.com/renode/renode/releases/download/v1.13.3/renode_1.13.3_amd64.deb -O /renode.deb
RUN apt-get install -y /renode.deb

RUN apt-get update && apt-get install -y cmake gcc-arm-none-eabi gettext bridge-utils

RUN mkdir -p /app
WORKDIR /app
COPY cmake/ cmake/
COPY config/ config/
COPY include/ include/
COPY linker/ linker/
COPY sensor_models/ sensor_models/
COPY src/ src/
COPY emulate.resc .
COPY CMakeLists.txt .
RUN rm -rf build && mkdir build
RUN cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/gcc-toolchain.cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && make

RUN mkdir -p /root/.config/renode
COPY docker/renode.config /root/.config/renode/config

CMD ["renode", "/app/emulate.resc"]
