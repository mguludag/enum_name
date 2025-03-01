FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.22.2"
ARG GCC_VERSION=14
ARG LLVM_VERSION=19

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh


RUN apt-get update && apt-get install -y --no-install-recommends \
    gnupg2 ca-certificates software-properties-common

# +-----------------------------+
# | GCC                         |
# +-----------------------------+

RUN add-apt-repository universe && \
apt-get update && apt-get install -y --no-install-recommends \
software-properties-common \
libmpfr-dev libgmp3-dev libmpc-dev \
gcc-${GCC_VERSION} g++-${GCC_VERSION}

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_VERSION} 100 && \
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-${GCC_VERSION} 100

RUN apt-get autoremove -y libmpfr-dev libgmp3-dev libmpc-dev

# +-----------------------------+
# | LLVM                        |
# +-----------------------------+

RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh ${LLVM_VERSION} all && \
    rm ./llvm.sh

# add llvm to path
ENV PATH="/usr/lib/llvm-${LLVM_VERSION}/bin:/usr/lib/llvm-${LLVM_VERSION}/include:${PATH}"
ENV LD_LIBRARY_PATH="/usr/lib/llvm-${LLVM_VERSION}/lib:${LD_LIBRARY_PATH}"

# # unversionize the binaries
# RUN ln -s /usr/bin/clang-${LLVM_VERSION} /usr/bin/clang && \
#     ln -s /usr/bin/clang++-${LLVM_VERSION} /usr/bin/clang++ && \
#     ln -s /usr/bin/clang-format-${LLVM_VERSION} /usr/bin/clang-format

# update compiler environment vars
ENV CC=/usr/bin/clang \
    CXX=/usr/bin/clang++

# update alternatives
RUN update-alternatives --install /usr/bin/cc cc /usr/bin/clang 100 && \
    update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 100

# Install the latest Python, pip, and pytest
RUN apt-get update && \
    apt-get install -y python3 python3-pip python3-venv pipx && \
    pipx ensurepath --force && \
    ln -s /usr/bin/python3 /usr/bin/python

# Install vcpkg (C++ package manager)
RUN git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg && \
    /opt/vcpkg/bootstrap-vcpkg.sh

ENV VCPKG_ROOT=/opt/vcpkg
ENV PATH="${VCPKG_ROOT}:${PATH}"


# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>
