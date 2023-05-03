FROM auxondemoimages/demo-linux-base:latest

ARG DEBIAN_FRONTEND=noninteractive

RUN echo "demo-linux" > /etc/hostname

RUN mkdir /lttng && echo 'LTTNG_HOME=/lttng' >> /etc/environment
ENV LTTNG_HOME /lttng

RUN mkdir -p /actuator-lib/src /actuator-lib/include /actuator-lib/build
COPY actuator/CMakeLists.txt /actuator-lib
COPY actuator/actuator.cpp /actuator-lib
COPY actuator/include /actuator-lib/include
RUN cd /actuator-lib/build && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
RUN cd /actuator-lib/build && make -j && make install

RUN mkdir -p /actuator-app/src /actuator-app/include /actuator-app/build
COPY CMakeLists.txt /actuator-app
COPY src /actuator-app/src
COPY include /actuator-app/include
RUN cd /actuator-app/build && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
RUN cd /actuator-app/build && make -j && make install

COPY docker/entrypoint.sh /
CMD ["/entrypoint.sh"]
