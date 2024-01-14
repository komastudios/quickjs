FROM ubuntu:22.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    locales \
    locales-all \
    build-essential \
    vim \
    libarchive-tools \
    cmake \
 && rm -rf /var/lib/apt/lists/*

ADD . /build
WORKDIR /build

RUN make
