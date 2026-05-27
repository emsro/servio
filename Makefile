
.DEFAULT_GOAL := build
.PHONY: build build_host build_h5 build_asan test test_asan flash_fw flash ftest load_preset

FIND_FILTER = -not \( -path ./build -prune \) -not \( -path ./stm32-cmake -prune \) -not \( -path ./.doxygen -prune \)

BMP_PORTS := $(sort $(wildcard /dev/cu.usbmodem*))
BMP_GDB   ?= $(firstword $(BMP_PORTS))
BMP_UART  ?= $(lastword $(BMP_PORTS))

FW_ELF    ?= _build/stm32h5/src/brd/proto4/proto4_fw.elf
FTEST_ELF ?= _build/stm32h5/src/brd/proto4/proto4_fw_ftest.elf
FTEST_OUT  = _test/yellow.proto4/fw/proto4_fw_ftest
PRESET    ?= preset/yellow.proto4

SCMDIO     = _build/host/src/scmdio/scmdio
ASRTIO     = _build/host/_deps/asrt-build/asrtio/asrtio
ASRT_TIMEOUT ?= 30000
FTEST_START_DELAY ?= 2
FTEST_RETRY_DELAY ?= 1

GDB_FLASH  = arm-none-eabi-gdb -nx --batch \
	  -ex "target extended-remote $(BMP_GDB)" \
	  -ex "monitor swdp_scan" \
	  -ex "attach 1" \
	  -ex "load" \
	  -ex "tbreak main" \
	  -ex "continue" \
	  -ex "detach"

build:
	$(MAKE) build_host
	$(MAKE) build_h5

build_host:
	cmake --workflow --preset "host_debug"
build_h5:
	cmake --workflow --preset "stm32h5_debug"
build_asan:
	cmake --workflow --preset "host_asan"

test: build_host

test_asan: build_asan

# Flash the main firmware (needed before load_preset on a fresh board)
flash_fw:
	$(GDB_FLASH) $(FW_ELF)

# Load a preset config into EEPROM via the main firmware (run flash_fw first)
load_preset: flash_fw
	$(SCMDIO) preset load --comms $(BMP_UART) $(PRESET)

# Flash the ftest firmware
flash:
	$(GDB_FLASH) $(FTEST_ELF)

# Flash ftest firmware and run the test suite
ftest: flash
	sleep $(FTEST_START_DELAY)
	$(ASRTIO) serial \
	  --timeout $(ASRT_TIMEOUT) \
	  --port $(BMP_UART) \
	  --baud 230400 \
	  --output $(FTEST_OUT)
