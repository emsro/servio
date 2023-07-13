
# Servio

[![Tests](https://github.com/emsro/servio/actions/workflows/tests.yml/badge.svg)](https://github.com/emsro/servio/actions/workflows/tests.yml)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/nlohmann/json/master/LICENSE.MIT)

Open firmware for servomotors written in C++20.
It is designed for small servos with DC motors and potentioemeters as encoders, the intent is that you can buy traditional RC servo and convert it to smart servo with servio hardware and firmware.

The servo can communicate and send/receive messages, has full feedback loops, and supports following control modes:
 - power - set % of power that should go to the DC motor
 - current - maintain desired current to flow throu the DC motor
 - velocity - maintain desired angular velocity
 - position - maintain desired position

# Install

We maintain `Dockerfile` with a setup that can install dependencies of the project and compile the code.
Use that to install adequate packages in your system, or you can just use that docker file.

# Build

`cmake` is used as build system, however the setup is not trivial.
We can't compile code for multiple platforms at the same time, hence the design is to use cmake multiple times, once for each platform.
If you don't care about the details, you can just use `make build` in the root folder of the repository, which will correctly build for all platforms.

We use cmake variable called `SERVIO_PLATFORM` to tell cmake for which platform it shall compile in that run.
So, for example:

```
cd build/host/
cmake -DSERVIO_PLATFORM=host ../../
make
```

Commands cmake to compile stuff for host system (utilities and unit tests), but the following:

```
cd build/host/
cmake -DSERVIO_PLATFORM=stm32g4 -DCMAKE_TOOLCHAIN_FAIL=../../stm32-cmake/cmake/stm32_gcc.cmake ../../
make
```

Commands cmake to compile stuff for STM32G4 (the firmware itself), note that this time we had to specify toolchain.
