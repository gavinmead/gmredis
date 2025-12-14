FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive

# Install essential build tools and dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    curl \
    wget \
    pkg-config \
    python3 \
    python3-pip \
    python3-venv \
    gdb \
    valgrind \
    clang-format \
    clang-tidy \
    cppcheck \
    lcov \
    doxygen \
    graphviz \
    && rm -rf /var/lib/apt/lists/*

# Install GCC 14 for C++23 support
RUN apt-get update && apt-get install -y \
    gcc-14 \
    g++-14 \
    && rm -rf /var/lib/apt/lists/* \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100 \
    && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 100 \
    && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 100

# Install Conan via pip in a virtual environment
RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"
RUN pip install --upgrade pip && pip install conan

# Create non-root user
ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN apt-get update && apt-get install -y sudo \
    && if getent passwd $USER_UID > /dev/null; then \
         EXISTING_USER=$(getent passwd $USER_UID | cut -d: -f1); \
         usermod -l $USERNAME -d /home/$USERNAME -m $EXISTING_USER; \
         groupmod -n $USERNAME $(getent group $USER_GID | cut -d: -f1); \
       else \
         groupadd --gid $USER_GID $USERNAME; \
         useradd --uid $USER_UID --gid $USER_GID -m $USERNAME; \
       fi \
    && echo "$USERNAME ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME \
    && rm -rf /var/lib/apt/lists/*

# Set up Conan for the user
USER $USERNAME
RUN conan profile detect --force

WORKDIR /workspace
