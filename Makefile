
.DEFAULT_GOAL := build
.PHONY: build build_host build_g4

FIND_FILTER = -not \( -path ./build -prune \) -not \( -path ./stm32-cmake -prune \) -not \( -path ./.doxygen -prune \)

BUILD_ARGS = -GNinja

build:
	make build_host
	make build_g4

build_host:
	mkdir -p build/host
	cmake -Bbuild/host -DSERVIO_PLATFORM=host ${BUILD_ARGS}
	cmake --build build/host -j 4

build_g4:
	mkdir -p build/g4
	cmake -Bbuild/g4 -DSERVIO_PLATFORM=stm32g4 -DCMAKE_TOOLCHAIN_FILE=stm32-cmake/cmake/stm32_gcc.cmake ${BUILD_ARGS}
	cmake --build build/g4 -j 4

test: build_host
	cd build/host && ctest -T Test --output-on-failure

clang-format:
	find ./ ${FIND_FILTER} \( -iname "*.h" -o -iname "*.cpp" -o -iname "*.hpp" \)  | xargs clang-format -i

cmake-format:
	find ./ ${FIND_FILTER} \( -iname "*CMakeLists.txt" -o -iname "*.cmake" \) | xargs cmake-format -i
