
.DEFAULT_GOAL := build
.PHONY: build build_host build_g4

FIND_FILTER = -not \( -path ./build -prune \) -not \( -path ./stm32-cmake -prune \) -not \( -path ./.doxygen -prune \)

BUILD_ARGS = -GNinja

configure:
	$(MAKE) configure_host
	$(MAKE) configure_g4
	$(MAKE) configure_h5

configure_host:
	cmake --preset "host_debug_cfg"
configure_g4:
	cmake --preset "stm32g4_debug_cfg"
configure_h5:
	cmake --preset "stm32h5_debug_cfg"
configure_asan:
	cmake --preset "host_asan_cfg"

build:
	$(MAKE) build_host
	$(MAKE) build_g4
	$(MAKE) build_h5

build_host:
	cmake --build --preset "host_debug_build"
build_g4:
	cmake --build --preset "stm32g4_debug_build"
build_h5:
	cmake --build --preset "stm32h5_debug_build"
build_asan:
	cmake --build --preset "host_asan_build"

flash:
	/usr/local/bin/openocd -f src/plt/stm32h5/openocd.cfg -c "program build/h5/proto3_fw.elf verify reset exit"

test: build_host
	cd build/host && ctest -T Test --output-on-failure

test_asan: build_asan
	cd build/asan && ctest -T Test --output-on-failure
