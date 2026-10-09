cmake_minimum_required(VERSION 3.28)

project(servio C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 99)

cmake_policy(SET CMP0077 NEW)
option(SERVIO_PLATFORM "Target platform")

if(${SERVIO_PLATFORM} STREQUAL "stm32h5")
  set(SERVIO_CPU_FLAGS
      "-mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mthumb")
  set(SERVIO_SHARED_FLAGS
      "-fsingle-precision-constant -gdwarf-2 -lm ${SERVIO_CPU_FLAGS}")
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} ${SERVIO_SHARED_FLAGS} -gdwarf-2 -DVARI_DISABLE_ASSERTS=ON"
  )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SERVIO_SHARED_FLAGS}")
endif()

include(FetchContent)
include(cmake/flags.cmake)
include(cmake/utils.cmake)

FetchContent_Declare(
  emlabcpp SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dep/emlabcpp EXCLUDE_FROM_ALL)
FetchContent_Declare(
  cmake_git_version_tracking
  GIT_REPOSITORY https://github.com/andrew-hardin/cmake-git-version-tracking
  GIT_TAG 6c0cb87edd029ddfb403a8e24577c144a03605a6
  SYSTEM EXCLUDE_FROM_ALL)
FetchContent_Declare(
  zll
  GIT_REPOSITORY https://github.com/koniarik/zll
  GIT_TAG v2.0.1
  GIT_SHALLOW TRUE
  SYSTEM EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(emlabcpp cmake_git_version_tracking zll)

if(${SERVIO_PLATFORM} STREQUAL "host")
  # asrt adds ecor, CLI11 and json through CPM, which reuses content already
  # populated under the same name, so these have to be available before asrt.
  FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG v2.7.2
    GIT_SHALLOW TRUE
    SYSTEM EXCLUDE_FROM_ALL)
  FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.12.0
    GIT_SHALLOW TRUE
    SYSTEM EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(CLI11 json)
  set(_servio_asrt_libs "asrtl;asrtc;asrtr;asrtlpp;asrtcpp;asrtrpp;asrtio")
else()
  set(_servio_asrt_libs "asrtl;asrtr;asrtlpp;asrtrpp")
endif()
set(ASRT_LIBRARIES
    "${_servio_asrt_libs}"
    CACHE STRING "" FORCE)
FetchContent_Declare(ecor SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dep/ecor
                          EXCLUDE_FROM_ALL)
FetchContent_Declare(asrt SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dep/asrt
                          SYSTEM EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(ecor asrt)

FetchContent_Declare(
  magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum
  GIT_TAG v0.9.8
  GIT_SHALLOW TRUE
  SYSTEM EXCLUDE_FROM_ALL)
FetchContent_Declare(
  atom
  GIT_REPOSITORY https://github.com/avakar/atom
  GIT_TAG f5d5c6c0f5fdc96a6275d55f7c4ee98396e407a6
  SYSTEM EXCLUDE_FROM_ALL)
FetchContent_Declare(
  vari
  GIT_REPOSITORY https://github.com/koniarik/vari
  GIT_TAG v1.0.3
  GIT_SHALLOW TRUE
  SYSTEM EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(magic_enum atom vari)

# Servio's emlabcpp configuration lives on its own target, so emlabcpp's target
# and its export stay as emlabcpp defines them.
add_library(servio_emlabcpp INTERFACE)
target_link_libraries(servio_emlabcpp INTERFACE emlabcpp::emlabcpp magic_enum)

if(${SERVIO_PLATFORM} STREQUAL "host")
  target_compile_options(
    servio_emlabcpp
    INTERFACE -DEMLABCPP_USE_LOGGING -DEMLABCPP_USE_NLOHMANN_JSON
              -DEMLABCPP_USE_TYPEID -DEMLABCPP_USE_OSTREAM -DEMLABCPP_USE_GTEST
              -DEMLABCPP_ASSERT_NATIVE)

elseif(${SERVIO_PLATFORM} STREQUAL "stm32h5")
  target_compile_options(vari INTERFACE -DVARI_DISABLE_ASSERTS=ON)
  target_compile_options(servio_emlabcpp INTERFACE -Wno-psabi)
endif()

if(${SERVIO_PLATFORM} STREQUAL "host")

  FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.17.0
    GIT_SHALLOW TRUE
    SYSTEM EXCLUDE_FROM_ALL)
  set(BOOST_ENABLE_CMAKE
      ON
      CACHE BOOL "")
  set(BOOST_INCLUDE_LIBRARIES
      container asio
      CACHE STRING "")
  FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.92.0/boost-1.92.0-cmake.tar.xz
    URL_HASH
      SHA256=9bed76128d4e46755dbe818487788c6fceb6f72b378f4daa49b7e1e600d9088d
    SYSTEM EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(spdlog Boost)
  target_compile_definitions(spdlog PUBLIC -DSPDLOG_USE_STD_FORMAT=ON)

  target_link_libraries(servio_emlabcpp INTERFACE nlohmann_json::nlohmann_json)

  include(CTest)
  if(BUILD_TESTING)
    set(INSTALL_GTEST
        OFF
        CACHE BOOL "")
    set(gtest_force_shared_crt
        ON
        CACHE BOOL "")
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest
      GIT_TAG v1.18.0
      GIT_SHALLOW TRUE
      SYSTEM EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(googletest)

    servio_add_executable(TARGET servio_utest LIBS servio_emlabcpp gtest_main
                                                   serviolib)
    include(GoogleTest)
    gtest_discover_tests(servio_utest)
  endif()
  add_subdirectory(src/plt/host)
  add_subdirectory(src)

elseif(${SERVIO_PLATFORM} STREQUAL "stm32h5")

  add_subdirectory(src/plt/stm32h5)

  target_link_libraries(servio_emlabcpp INTERFACE platform)
  target_link_libraries(cmake_git_version_tracking PRIVATE platform)

  foreach(_asrt_target asrtl asrtr asrtrpp)
    target_compile_options(${_asrt_target} PRIVATE -Os)
  endforeach()

  add_subdirectory(src)
  add_subdirectory(src/brd/proto4)

else()
  message(WARNING "unsupported platform ${SERVIO_PLATFORM}")
endif()
