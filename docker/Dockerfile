ARG DOCKER_NVIDIA_CUDA_VERSION=12.4.1
ARG DOCKER_NVIDIA_CUDNN_VERSION=""
ARG DOCKER_UBUNTU_VERSION=22.04
FROM nvidia/cuda:${DOCKER_NVIDIA_CUDA_VERSION}-cudnn${DOCKER_NVIDIA_CUDNN_VERSION}-devel-ubuntu${DOCKER_UBUNTU_VERSION}

ARG GCC_VERSION=12
ARG CLANG_VERSION=17
ARG CMAKE_VERSION=3.29.8
ARG NINJA_VERSION=1.11.1
ARG LIBTORCH_VERSION=2.5.1
ARG CUDA_ARCH=8.6
ARG TORCH_CUDA_VERSION=124
ARG CXX_STANDARD=17

RUN apt-get update && apt-get install -y \
    gdb \
    git \
    python3 \
    python3-dev \
    tar \
    tree \
    unzip \
    wget \
    && apt-get clean

# Install CMake
RUN CMAKE_VERSION_MOC=$(echo ${CMAKE_VERSION} | rev | cut -c3- | rev) && wget "https://cmake.org/files/v${CMAKE_VERSION_MOC}/cmake-${CMAKE_VERSION}-linux-x86_64.sh" -q -O /tmp/cmake-install.sh
RUN chmod u+x /tmp/cmake-install.sh
RUN mkdir /opt/cmake-${CMAKE_VERSION}
RUN /tmp/cmake-install.sh --skip-license --prefix=/opt/cmake-${CMAKE_VERSION}
RUN rm /tmp/cmake-install.sh
RUN ln -s /opt/cmake-${CMAKE_VERSION}/bin/cmake /usr/bin/cmake
RUN /usr/bin/cmake --version

#Install GCC
RUN apt-get update && apt-get install -y software-properties-common && rm -rf /var/lib/apt/lists/*
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get update && apt-get install -y \
    gcc-${GCC_VERSION} \
    g++-${GCC_VERSION}
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_VERSION} ${GCC_VERSION} --slave /usr/bin/g++ g++ /usr/bin/g++-${GCC_VERSION}

# Install Clang
RUN apt-get update && apt-get install -y lsb-release software-properties-common gnupg
RUN wget https://apt.llvm.org/llvm.sh && chmod u+x llvm.sh
RUN ./llvm.sh ${CLANG_VERSION} all
RUN ln -s /usr/bin/clang-${CLANG_VERSION} /usr/bin/clang && ln -s /usr/bin/clang++-${CLANG_VERSION} /usr/bin/clang++

# Install Ninja
RUN wget "https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip"
RUN unzip ninja-linux.zip
RUN mv ninja /usr/bin

# Install LibTorch
RUN wget "https://download.pytorch.org/libtorch/cu${TORCH_CUDA_VERSION}/libtorch-cxx11-abi-shared-with-deps-${LIBTORCH_VERSION}%2Bcu${TORCH_CUDA_VERSION}.zip"
RUN unzip libtorch-cxx11-abi-shared-with-deps-${LIBTORCH_VERSION}+cu${TORCH_CUDA_VERSION}.zip -d /usr/local/lib/
RUN rm libtorch-cxx11-abi-shared-with-deps-${LIBTORCH_VERSION}+cu${TORCH_CUDA_VERSION}.zip

# Install OpenCV
RUN apt-get update && apt-get install -y \
    build-essential \
    libgtk-3-dev \
    python3-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libavcodec-dev \ 
    libavformat-dev \ 
    libswscale-dev \
    libswresample-dev \
    libgstreamer1.0-dev \ 
    libgstreamer-plugins-base1.0-dev \
    libxvidcore-dev \
    x264 \
    libx264-dev \
    libfaac-dev \
    libmp3lame-dev \
    libtheora-dev \
    libfaac-dev \
    libmp3lame-dev \
    libvorbis-dev
RUN mkdir -p opencv-build/build
RUN cd opencv-build && git clone https://github.com/opencv/opencv.git && git clone https://github.com/opencv/opencv_contrib.git
RUN cd opencv-build/build && cmake \
    -D CMAKE_CXX_STANDARD=${CXX_STANDARD} \
    -D WITH_TBB=ON \
    -D ENABLE_FAST_MATH=1 \
    -D CUDA_FAST_MATH=1 \
    -D WITH_CUBLAS=1 \
    -D BUILD_opencv_cudacodec=OFF \
    -D WITH_CUDA=ON \
    -D WITH_CUDNN=ON \
    -D CUDA_ARCH_BIN=${CUDA_ARCH}} \
    -D CUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda  \
    -D CUDNN_LIBRARY=/usr/lib/x86_64-linux-gnu/libcudnn.so \
    -D WITH_GTK=ON \
    -D WITH_V4L=ON \
    -D WITH_QT=OFF \
    -D WITH_OPENGL=ON \
    -D WITH_GSTREAMER=ON \
    -D INSTALL_PYTHON_EXAMPLES=OFF \
    -D INSTALL_C_EXAMPLES=OFF \
    -D BUILD_EXAMPLES=OFF \
    -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules \
    ../opencv
RUN cd opencv-build/build && make -j$(nproc) && make install
RUN rm -rf opencv-build 

# Add user
ARG UID=1000
RUN useradd -m -u ${UID} -s /bin/bash builder
RUN usermod -aG video builder
USER builder
