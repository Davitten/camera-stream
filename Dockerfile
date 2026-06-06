# syntax=docker/dockerfile:1.4
ARG BASE_IMAGE=ubuntu:24.04

#################################
#   Librealsense Stage          #
#################################

FROM ${BASE_IMAGE} AS librealsense-stage
# Define here to include inside stage
ARG BUILDPLATFORM
ARG TARGETPLATFORM
ARG TARGETARCH
RUN echo "I'm running on BUILDPLATFORM=${BUILDPLATFORM} building for TARGETPLATFORM=${TARGETPLATFORM}, TARGETARCH=${TARGETARCH}."
ENV DEBIAN_FRONTEND=noninteractive

# Install all build dependencies
RUN apt-get update \
    && apt-get install -qq -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    libssl-dev \
    libusb-1.0-0-dev \
    pkg-config \
    libgtk-3-dev \
    libglfw3-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    curl \
    python3 \
    python3-dev \
    ca-certificates \
    ninja-build \
    wget \
    unzip \
    openssh-client \
    && rm -rf /var/lib/apt/lists/*

# Build librealsense
ARG LIBRS_VERSION=2.56.5
WORKDIR /usr/src
RUN curl https://codeload.github.com/realsenseai/librealsense/tar.gz/refs/tags/v$LIBRS_VERSION -o librealsense.tar.gz \
    && tar -zxf librealsense.tar.gz \
    && rm librealsense.tar.gz \
    && ln -s /usr/src/librealsense-$LIBRS_VERSION /usr/src/librealsense

RUN cd /usr/src/librealsense \
    && mkdir build && cd build \
    && cmake \
    -G Ninja \
    -DCMAKE_C_FLAGS_RELEASE="${CMAKE_C_FLAGS_RELEASE} -s" \
    -DCMAKE_CXX_FLAGS_RELEASE="${CMAKE_CXX_FLAGS_RELEASE} -s" \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DBUILD_GRAPHICAL_EXAMPLES=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_PYTHON_BINDINGS:bool=true \
    -DCMAKE_BUILD_TYPE=Release ../ \
    && cmake --build . -j$(($(nproc)-1)) -t all \
    && cmake --build . -t install

#################################
#   OpenCV Stage                #
#################################
FROM librealsense-stage AS opencv-stage

ARG OPENCV_VERSION=4.12.0
WORKDIR /usr/src
RUN wget -q -O opencv.zip https://github.com/opencv/opencv/archive/refs/tags/${OPENCV_VERSION}.zip \
    && unzip -qq opencv.zip \
    && rm -rf opencv.zip \
    && cd opencv-${OPENCV_VERSION}  \
    && mkdir -p build  \
    && cd build  \
    && cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_SHARED_LIBS=ON -D WITH_JAVA=OFF -D BUILD_JAVA=OFF -G Ninja ..  \
    && cmake --build . -j$(($(nproc)-1)) \
    && cmake --build . -t install

FROM opencv-stage AS dev
RUN apt-get update \
    && apt-get install -qq -y --no-install-recommends \
    gdb \
    gcc-12 \
    g++-12 \
    && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 10 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 10


#
##################################
##   Application Builder Stage   #
##################################
#FROM opencv-stage AS app-builder
#
#WORKDIR /app
#COPY . .
#
#RUN --mount=type=ssh,required=true \
#    && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -b build \
#    && cmake --build build -j$(($(nproc)-1))
#
#WORKDIR /
#CMD ["./app/build/camera-stream"]
