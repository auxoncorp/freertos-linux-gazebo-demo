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
  vim

RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && locale-gen
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

RUN update-ca-certificates
RUN wget https://packages.osrfoundation.org/gazebo.gpg -O /usr/share/keyrings/pkgs-osrf-archive-keyring.gpg
RUN echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/pkgs-osrf-archive-keyring.gpg] http://packages.osrfoundation.org/gazebo/ubuntu-stable $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/gazebo-stable.list > /dev/null
RUN apt-get update && apt-get install -y gz-garden

RUN wget --no-verbose --quiet https://get.keygen.sh/auxon-io/auxon.deb -O auxon.deb
RUN apt-get install -y ./auxon.deb
RUN apt-get update && apt-get install -y --no-install-recommends modality-sdk

RUN adduser --disabled-password --gecos '' docker
RUN adduser docker sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

WORKDIR /home/docker/

RUN mkdir -p /home/docker/gazebo_plugins /home/docker/gazebo_worlds
RUN git clone --depth 1 https://github.com/auxoncorp/modality-gazebo.git
RUN cd modality-gazebo && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && make -j && cp libModalityTracingPlugin.so /home/docker/gazebo_plugins/

COPY imu_relay /home/docker/imu_relay
RUN cd imu_relay && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && make -j && cp libImuRelay.so /home/docker/gazebo_plugins/
COPY demo_world.sdf /home/docker/gazebo_worlds/demo_world.sdf
COPY gui.config /home/docker/.gz/sim/7/gui.config

RUN chown -R docker:docker /home/docker
USER docker
ARG DEBIAN_FRONTEND=noninteractive

ENV GZ_SIM_SYSTEM_PLUGIN_PATH=/home/docker/gazebo_plugins:${GZ_SIM_SYSTEM_PLUGIN_PATH}
ENV GZ_SIM_RESOURCE_PATH=/home/docker/gazebo_worlds:${GZ_SIM_RESOURCE_PATH}

CMD ["gz", "sim", "-r", "demo_world.sdf"]
