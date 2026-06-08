
# Servio

[![Build](https://github.com/emsro/servio/actions/workflows/build.yml/badge.svg)](https://github.com/emsro/servio/actions/workflows/build.yml)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/nlohmann/json/master/LICENSE.MIT)

Open firmware for DC servomotors written in C++20.
The FW uses closed control loops, serial communication, and has automated testing infrastructure.
More details in [Documentation](https://emsro.github.io/servio/index.html)

# Disclaimer

The project is in development state, beware that the code still might have problematic bugs.

# Dependencies

We maintain a `Dockerfile` with installed dependencies for CI.
Use that to install adequate packages in your system, or use the Docker image directly.

The `Dockerfile` is stored in this repository: https://github.com/emsro/build-env

# Build

CMake with presets is used as the build system. There are separate workflow presets for each target platform. The simplest way to build everything:

```
make build
```

This runs both host and STM32H5 builds. Individual targets:

```
make build_host   # host tools and unit tests (_build/host/)
make build_h5     # STM32H5 firmware (_build/stm32h5/)
```

Alternatively, use cmake workflow presets directly:

```
cmake --workflow --preset host_debug
cmake --workflow --preset stm32h5_debug
```

# Flashing

To flash firmware, first enter the ROM bootloader by holding the BOOT0 pin high (e.g. with tweezers on the board header) and power-cycling the device. Then use the `flash` command to upload and start the new firmware:

```
_install/bin/scmdio flash --comms /dev/cu.usbserial-0001 _build/stm32h5/src/brd/proto4/proto4_fw.bin
```

For manual control, upload directly with:

```
_install/bin/scmdio dfu upload --comms /dev/cu.usbserialXXXX _build/stm32h5/src/brd/proto4/proto4_fw.bin
```

# Configuration

After flashing, load a preset configuration into the servo's EEPROM over the UART config port (230400 baud). Available presets are in the `preset/` directory (`yellow.proto4`, `kavango6.proto4`, `LX15D.proto4`):

```
_build/host/src/scmdio/scmdio preset load --comms /dev/cu.usbserialXXXX preset/yellow.proto4
```

Individual fields can also be set directly:

```
_install/bin/scmdio cfg set <field> <value> --comms /dev/cu.usbserialXXXX
```

# Communication

The servo communicates over UART (230400 baud, newline-delimited text protocol) on the interface connector (UART1). Configuration is managed via `scmdio`:

```
_build/host/src/scmdio/scmdio cfg query --comms /dev/cu.usbserialXXXX
_build/host/src/scmdio/scmdio cfg get <field> --comms /dev/cu.usbserialXXXX
_build/host/src/scmdio/scmdio cfg set <field> <value> --comms /dev/cu.usbserialXXXX
```
