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
cpmaddpackage("gh:koniarik/zll@1.0.0")
if(${SERVIO_PLATFORM} STREQUAL "host")
  set(_servio_asrt_libs "asrtl;asrtc;asrtr;asrtlpp;asrtcpp;asrtrpp;asrtio")
  set(_servio_asrt_targets asrtl asrtc asrtr asrtlpp asrtcpp asrtrpp)
else()
  set(_servio_asrt_libs "asrtl;asrtr;asrtlpp;asrtrpp")
  set(_servio_asrt_targets asrtl asrtr asrtlpp asrtrpp)
endif()
set(ASRT_LIBRARIES
    "${_servio_asrt_libs}"
    CACHE STRING "" FORCE)
cpmaddpackage(NAME asrt GITHUB_REPOSITORY emsro/asrt GIT_TAG v0.0.3)
foreach(_asrt_target IN LISTS _servio_asrt_targets)
  set_target_properties(${_asrt_target} PROPERTIES SYSTEM TRUE)
endforeach()
cpmaddpackage("gh:Neargye/magic_enum@0.9.7")
cpmaddpackage("gh:avakar/atom#f5d5c6c0f5fdc96a6275d55f7c4ee98396e407a6")
cpmaddpackage("gh:koniarik/vari#8846bd024215997f5009c4fb920ecfb64d4c7efb")

if(${SERVIO_PLATFORM} STREQUAL "host")
  target_compile_options(
    emlabcpp
    INTERFACE -DEMLABCPP_USE_LOGGING -DEMLABCPP_USE_NLOHMANN_JSON
              -DEMLABCPP_USE_TYPEID -DEMLABCPP_USE_OSTREAM -DEMLABCPP_USE_GTEST
              -DEMLABCPP_ASSERT_NATIVE)

elseif(${SERVIO_PLATFORM} STREQUAL "stm32h5")
  target_compile_options(vari INTERFACE -DVARI_DISABLE_ASSERTS=ON)
  target_compile_options(emlabcpp INTERFACE -Wno-psabi)
endif()

target_link_libraries(emlabcpp INTERFACE magic_enum)

if(${SERVIO_PLATFORM} STREQUAL "host")

  cpmaddpackage("gh:CLIUtils/CLI11@2.4.2")
  cpmaddpackage("gh:nlohmann/json@3.11.3")
  cpmaddpackage("gh:koniarik/spdlog#20ad67481efcd5e539857297af5c20959ff993d7")
  target_compile_definitions(spdlog PUBLIC -DSPDLOG_USE_STD_FORMAT=ON)
  cpmaddpackage(
    NAME
    Boost
    VERSION
    1.88.0
    URL
    https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-cmake.tar.xz
    URL_HASH
    SHA256=f48b48390380cfb94a629872346e3a81370dc498896f16019ade727ab72eb1ec
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
    1.15.2
    OPTIONS
    "INSTALL_GTEST OFF"
    "gtest_force_shared_crt"
    SYSTEM
    ON)

  target_link_libraries(emlabcpp INTERFACE nlohmann_json::nlohmann_json)

  set(JOQUE_REPROC_ENABLED ON)
  set(JOQUE_NLOHMANN_JSON_ENABLED ON)
  add_subdirectory(dep/joque)

  include(CTest)
  enable_testing()

  servio_add_executable(TARGET servio_utest LIBS emlabcpp gtest_main serviolib)
  include(GoogleTest)
  gtest_discover_tests(servio_utest)
  add_subdirectory(src/plt/host)
  add_subdirectory(src)

elseif(${SERVIO_PLATFORM} STREQUAL "stm32h5")

  add_subdirectory(src/plt/stm32h5)

  target_link_libraries(emlabcpp INTERFACE platform)
  target_link_libraries(cmake_git_version_tracking PRIVATE platform)

  foreach(_asrt_target asrtl asrtr asrtrpp)
    target_compile_options(${_asrt_target} PRIVATE -Os)
  endforeach()

  add_subdirectory(src)
  add_subdirectory(src/brd/proto4)

else()
  message(WARNING "unsupported platform ${SERVIO_PLATFORM}")
endif()
