FROM archlinux:latest

# Instal build tools
RUN pacman -Sy make cmake ninja gcc arm-none-eabi-gcc --noconfirm

# Instal dependencies
RUN pacman -Sy gtest protobuf boost nlohmann-json --noconfirm

# Instal other tools
RUN pacman -Sy git clang --noconfirm

WORKDIR /servio

COPY . .

RUN make
