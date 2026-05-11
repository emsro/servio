
.DEFAULT_GOAL := build
.PHONY: build build_host build_h5

FIND_FILTER = -not \( -path ./build -prune \) -not \( -path ./stm32-cmake -prune \) -not \( -path ./.doxygen -prune \)

BUILD_ARGS = -GNinja

configure:
	$(MAKE) configure_host
	$(MAKE) configure_h5

configure_host:
	cmake --preset "host_debug"
configure_h5:
	cmake --preset "stm32h5_debug"
configure_asan:
	cmake --preset "host_asan"

build:
	$(MAKE) build_host
	$(MAKE) build_h5

build_host:
	cmake --build --preset "host_debug"
build_h5:
	cmake --build --preset "stm32h5_debug"
build_asan:
	cmake --build --preset "host_asan"

test: build_host
	ctest --preset host_debug --output-on-failure

test_asan: build_asan
	ctest --preset host_asan --output-on-failure
