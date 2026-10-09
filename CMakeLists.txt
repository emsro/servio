cmake_minimum_required(VERSION 3.25)

project(servio C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 99)

cmake_policy(SET CMP0077 NEW)
option(SERVIO_PLATFORM "Target platform")
option(SERVIO_EMLABCPP_SRC "Gives a location of emlabcpp source")

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

add_subdirectory(dep/emlabcpp)

include(cmake/CPM.cmake)
include(cmake/flags.cmake)
include(cmake/utils.cmake)

cpmaddpackage(
  "gh:andrew-hardin/cmake-git-version-tracking#6c0cb87edd029ddfb403a8e24577c144a03605a6"
)
cpmaddpackage("gh:koniarik/zll@2.0.1")
if(${SERVIO_PLATFORM} STREQUAL "host")
  # asrtio requests CLI11 and json as well, and CPM keeps the first request, so
  # servio's versions have to come before asrt.
  cpmaddpackage("gh:CLIUtils/CLI11@2.7.2")
  cpmaddpackage("gh:nlohmann/json@3.12.0")
  set(_servio_asrt_libs "asrtl;asrtc;asrtr;asrtlpp;asrtcpp;asrtrpp;asrtio")
  set(_servio_asrt_targets
      asrtl
      asrtc
      asrtr
      asrtlpp
      asrtcpp
      asrtrpp
      asrtio_lib)
else()
  set(_servio_asrt_libs "asrtl;asrtr;asrtlpp;asrtrpp")
  set(_servio_asrt_targets asrtl asrtr asrtlpp asrtrpp)
endif()
set(ASRT_LIBRARIES
    "${_servio_asrt_libs}"
    CACHE STRING "" FORCE)
cpmaddpackage(NAME ecor VERSION 0.4.0 SOURCE_DIR
              ${CMAKE_CURRENT_SOURCE_DIR}/dep/ecor)
cpmaddpackage(NAME asrt SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dep/asrt)
foreach(_asrt_target IN LISTS _servio_asrt_targets)
  set_target_properties(${_asrt_target} PROPERTIES SYSTEM TRUE)
endforeach()
cpmaddpackage("gh:Neargye/magic_enum@0.9.8")
cpmaddpackage("gh:avakar/atom#f5d5c6c0f5fdc96a6275d55f7c4ee98396e407a6")
cpmaddpackage("gh:koniarik/vari@1.0.3")

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

  cpmaddpackage("gh:gabime/spdlog@1.17.0")
  target_compile_definitions(spdlog PUBLIC -DSPDLOG_USE_STD_FORMAT=ON)
  cpmaddpackage(
    NAME
    Boost
    VERSION
    1.92.0
    URL
    https://github.com/boostorg/boost/releases/download/boost-1.92.0/boost-1.92.0-cmake.tar.xz
    URL_HASH
    SHA256=9bed76128d4e46755dbe818487788c6fceb6f72b378f4daa49b7e1e600d9088d
    OPTIONS
    "BOOST_ENABLE_CMAKE ON"
    "BOOST_INCLUDE_LIBRARIES container\\\;asio" # Note the escapes!
    SYSTEM
    ON)
  cpmaddpackage(
    NAME
    googletest
    GITHUB_REPOSITORY
    google/googletest
    VERSION
    1.18.0
    OPTIONS
    "INSTALL_GTEST OFF"
    "gtest_force_shared_crt"
    SYSTEM
    ON)

  target_link_libraries(servio_emlabcpp INTERFACE nlohmann_json::nlohmann_json)

  include(CTest)
  enable_testing()

  servio_add_executable(TARGET servio_utest LIBS servio_emlabcpp gtest_main
                                                 serviolib)
  include(GoogleTest)
  gtest_discover_tests(servio_utest)
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
