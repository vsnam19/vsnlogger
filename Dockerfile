# Use Ubuntu 20.04 as the base image
FROM ubuntu:20.04

# Avoid prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Set the working directory in the container
WORKDIR /app

# Update package lists and install essential build tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config debhelper devscripts dh-make cmake lintian \
    && apt-get clean \ 
    && rm -rf /var/lib/apt/lists/*

# Build spdlog with Position Independent Code enabled
RUN git clone https://github.com/gabime/spdlog.git /tmp/spdlog && \
    cd /tmp/spdlog && git checkout v1.15.3 && mkdir build && cd build && \
    cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON .. && \
    cmake --build . && make install && \
    ldconfig

# Set the default command to run when the container starts
CMD ["bash"]