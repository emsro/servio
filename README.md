
# Servio

[![Tests](https://github.com/emsro/servio/actions/workflows/tests.yml/badge.svg)](https://github.com/emsro/servio/actions/workflows/tests.yml)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/nlohmann/json/master/LICENSE.MIT)

Open firmware for DC servomotors written in C++20.
The FW uses closed control loops, serial communication, and has automatized testing infrastructure.
More details in [Documentation](https://emsro.github.io/servio/index.html)

# Disclaimer

The project is in development state, beware that the code still might have problematic bugs.

# Dependencies

We maintain `Dockerfile` with installed dependencies for CI.
Use that to install adequate packages in your system, or you can just use that docker file.

`Dockerfile` is stored in this repository: https://github.com/emsro/build-env

# Build

`cmake` is used as build system, however the setup is not trivial.
We can't compile code for multiple platforms at the same time, hence the design is to use cmake multiple times, once for each platform.
If you don't care about the details, you can just use `make configure` followed by `make build` in the root folder of the repository, which will correctly build for all platforms.

We use cmake presets with definitions of the builds for each platform. The presets contain the name of the platform in their name, so `stm32h5_debug_build` represents build for STM32H5, and it is used as so:
```
cmake --preset "stm32h5_debug_cfg"
cmake --build --preset "stm32h5_debug_build"
```
`host_debug_build` represents build of stuff used on the host, and is used as so:
```
cmake --preset "host_debug_cfg"
cmake --build --preset "host_debug_build"
```

Under the hood, the presets use `SERVIO_PLATFORM` to setup which platform should be currently build and the cmake files in the project react accordingly.

# Flashing

To flash the firmware, find a openocd file for your platform (`src/plt/stm32g4/openocd.cfg` for STM32G4) and use it with openocd to flash  a firmware file into the device. See `make flash` target for inspiration.

The pattern for naming the firmware is `<board_name>_fw.elf` and can be found in `build/<platform_name>`.

# Usage

After the firmware was fleshed, the servo should communicate over UART via the interface connector. The communication protocol is protobuf wrapped in COBS (Consistent Overhead Byte Stuffing
) for framing.

All our protobuf messages are in single file: [src/iface/io.proto](src/iface/io.protoz) We recommend that you use that file to generate a bindings for your favourite language. All that is necessary is to use the proper protobuf message and wrap it in COBS.
