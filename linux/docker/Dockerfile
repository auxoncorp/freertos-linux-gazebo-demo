FROM ubuntu:focal

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

RUN wget https://packages.osrfoundation.org/gazebo.gpg -O /usr/share/keyrings/pkgs-osrf-archive-keyring.gpg
RUN echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/pkgs-osrf-archive-keyring.gpg] http://packages.osrfoundation.org/gazebo/ubuntu-stable $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/gazebo-stable.list > /dev/null
RUN apt-get update && apt-get install -y libgz-transport12-dev gz-transport12-cli libgz-msgs9-dev

RUN apt-get install -y lttng-tools liblttng-ust-dev cmake gcc

RUN echo "demo-linux" > /etc/hostname

RUN mkdir /lttng && echo 'LTTNG_HOME=/lttng' >> /etc/environment
ENV LTTNG_HOME /lttng

RUN mkdir -p /actuator-lib/src /actuator-lib/include /actuator-lib/build
COPY actuator/CMakeLists.txt /actuator-lib
COPY actuator/actuator.cpp /actuator-lib
COPY actuator/include /actuator-lib/include
RUN cd /actuator-lib/build && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
RUN cd /actuator-lib/build && make && make install

RUN mkdir -p /actuator-app/src /actuator-app/include /actuator-app/build
COPY CMakeLists.txt /actuator-app
COPY src /actuator-app/src
COPY include /actuator-app/include
RUN cd /actuator-app/build && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
RUN cd /actuator-app/build && make && make install

COPY docker/entrypoint.sh /
CMD ["/entrypoint.sh"]
