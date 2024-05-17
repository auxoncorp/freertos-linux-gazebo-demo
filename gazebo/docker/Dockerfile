FROM auxondemoimages/demo-gz-sim-base:latest

ARG DEBIAN_FRONTEND=noninteractive

RUN wget --no-verbose --quiet https://get.keygen.sh/auxon-io/auxon.deb -O auxon.deb
RUN apt-get install -y ./auxon.deb
RUN apt-get update && apt-get install -y --no-install-recommends modality-sdk

RUN adduser --disabled-password --gecos '' docker
RUN adduser docker sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

WORKDIR /home/docker/

RUN mkdir -p /home/docker/gazebo_plugins /home/docker/gazebo_worlds
RUN git clone --depth 1 https://github.com/auxoncorp/modality-gazebo.git --branch topic-based-interactions-prototype
#RUN git clone --depth 1 https://github.com/auxoncorp/modality-gazebo.git --branch topic-based-interactions
#RUN git clone --depth 1 https://github.com/auxoncorp/modality-gazebo.git
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

COPY docker/shutdown_on_contact.sh /shutdown_on_contact.sh

COPY docker/entrypoint.sh /
CMD ["/entrypoint.sh"]
