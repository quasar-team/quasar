FROM ubuntu:18.04
RUN apt-get update && apt-get install -y \
  g++ \
  git \
  python3 \
  python3-lxml \
  python3-jinja2 \
  python3-colorama \
  default-jre-headless \
  cmake \
  libboost-program-options-dev \
  libboost-regex-dev \
  libboost-chrono-dev \
  libboost-thread-dev \
  libboost-system-dev \
  libxml2-utils \
  xsdcxx \
  astyle \
  libxerces-c-dev \
  libssl-dev \
  libxml2-dev \
  && update-alternatives --install /usr/bin/python python /usr/bin/python3 10
COPY sdk/include /opt/uasdk/include
COPY sdk/lib /opt/uasdk/lib
