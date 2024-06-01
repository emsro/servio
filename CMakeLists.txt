cmake_minimum_required(VERSION 3.24)

project(servio C CXX ASM)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

cmake_policy(SET CMP0077 NEW)
option(SERVIO_PLATFORM "Target platform")
option(SERVIO_EMLABCPP_SRC "Gives a location of emlabcpp source")

add_subdirectory(dep/emlabcpp)
add_subdirectory(dep/cmake-git-version-tracking)
add_subdirectory(dep/magic_enum)

include(cmake/flags.cmake)
include(cmake/utils.cmake)
include(cmake/nanopb.cmake)

if(${SERVIO_PLATFORM} STREQUAL "host")
  target_compile_options(
    emlabcpp
    PUBLIC -DEMLABCPP_USE_LOGGING -DEMLABCPP_USE_NLOHMANN_JSON
           -DEMLABCPP_USE_TYPEID -DEMLABCPP_USE_OSTREAM -DEMLABCPP_USE_GTEST
           -DEMLABCPP_ASSERT_NATIVE)

elseif(${SERVIO_PLATFORM} STREQUAL "stm32h5")
  set(SERVIO_SHARED_FLAGS "-fsingle-precision-constant -gdwarf-2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SERVIO_SHARED_FLAGS} -gdwarf-2")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SERVIO_SHARED_FLAGS}")

endif()

target_link_libraries(emlabcpp PUBLIC magic_enum)

if(${SERVIO_PLATFORM} STREQUAL "host")

  find_package(GTest REQUIRED)

  add_subdirectory(dep/CLI11)
  add_subdirectory(dep/joque)

  include(CTest)
  enable_testing()

  add_subdirectory(src/plt/host)
  add_subdirectory(src)

elseif(${SERVIO_PLATFORM} STREQUAL "stm32h5")

  stm32_fetch_hal(H5)
  stm32_fetch_cmsis(H5)
  find_package(CMSIS COMPONENTS STM32H5 REQUIRED GLOBAL)
  find_package(HAL COMPONENTS STM32H5 REQUIRED GLOBAL)

  target_compile_options(emlabcpp PUBLIC -Os)
  target_link_libraries(emlabcpp PRIVATE STM32::H5)

  add_subdirectory(src/plt/stm32h5)

  target_link_libraries(cmake_git_version_tracking PRIVATE platform)

  add_subdirectory(src)
  add_subdirectory(src/brd/proto3)

else()
  message(WARNING "unsupported platform ${SERVIO_PLATFORM}")
endif()
