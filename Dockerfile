FROM archlinux:latest

RUN pacman -Sy make cmake ninja gcc arm-none-eabi-gcc --noconfirm
RUN pacman -Sy gtest protobuf boost nlohmann-json --noconfirm
RUN pacman -Sy git clang --noconfirm

WORKDIR /servio

COPY . .

RUN make
