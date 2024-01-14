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

ARG BUILD_SHARED=ON

RUN cmake -S . -B build -DBUILD_SHARED=${BUILD_SHARED} -DBUILD_LOADER=${BUILD_SHARED}
RUN cmake --build build
RUN ctest --test-dir build --output-on-failure
