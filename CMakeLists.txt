cmake_minimum_required(VERSION 3.24)

project(servio C CXX ASM)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

cmake_policy(SET CMP0077 NEW)
option(SERVIO_PLATFORM "Target platform")
option(SERVIO_EMLABCPP_SRC "Gives a location of emlabcpp source")

include(cmake/emlabcpp.cmake)
include(cmake/flags.cmake)
include(cmake/utils.cmake)
# include(cmake/gitversion.cmake) # is not fine tuned yet
include(cmake/nanopb.cmake)

servio_add_library(
  TARGET serviolib
  INCLUDE src/
  SOURCES src/metrics.cpp src/control.cpp src/protocol.cpp src/indication.cpp
          src/cfg/storage.cpp
  LIBS emlabcpp
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")
  include(cmake/dimcli.cmake)

  find_package(Protobuf REQUIRED)

  include(CTest)
  enable_testing()

  protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS src/iface/io.proto)
  get_filename_component(PROTO_HDRS_DIR ${PROTO_HDRS} DIRECTORY)

  target_compile_options(
    emlabcpp PUBLIC -DEMLABCPP_USE_LOGGING -DEMLABCPP_USE_NLOHMANN_JSON
                    -DEMLABCPP_USE_GTEST)

  add_library(proto STATIC ${PROTO_SRCS})
  target_link_libraries(proto PUBLIC ${Protobuf_LIBRARIES})
  target_include_directories(proto PUBLIC ${PROTO_HDRS_DIR})

  add_subdirectory(src/host)
  add_subdirectory(tests)

elseif(${SERVIO_PLATFORM} STREQUAL "stm32g4")

  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fsingle-precision-constant  -Fno-rtti -fno-exceptions -gdwarf-2 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard"
  )
  set(CMAKE_C_FLAGS
      "${CMAKE_C_FLAGS} -fsingle-precision-constant -gdwarf-2 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16  -mfloat-abi=hard"
  )

  stm32_fetch_hal(G4)
  stm32_fetch_cmsis(G4)
  find_package(CMSIS COMPONENTS STM32G4 REQUIRED GLOBAL)
  find_package(HAL COMPONENTS STM32G4 REQUIRED GLOBAL)

  nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS src/iface/io.proto)

  add_library(proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})
  target_link_libraries(proto PUBLIC nanopb_lib)
  target_include_directories(proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

  servio_add_library(
    TARGET dispatchlib
    INCLUDE src/
    SOURCES src/fw/dispatcher.cpp src/fw/cfg_dispatcher.cpp
    LIBS serviolib proto CMSIS::STM32::G431xx HAL::STM32::G4
    OPTS -O3)

  servio_add_library(
    TARGET fwlib
    SOURCES src/fw/drivers/acquisition.cpp
            src/fw/drivers/comms.cpp
            src/fw/drivers/debug_comms.cpp
            src/fw/drivers/hbridge.cpp
            src/fw/drivers/leds.cpp
            src/fw/extra.cpp
            src/fw/globals.cpp
            src/fw/util.cpp
    LIBS serviolib dispatchlib proto CMSIS::STM32::G431xx HAL::STM32::G4
    OPTS -Og)

  servio_add_library(
    TARGET prototype2_board
    SOURCES boards/prototype2/adc.cpp
            boards/prototype2/board.cpp
            boards/prototype2/gpio.cpp
            boards/prototype2/clock.cpp
            boards/prototype2/config.cpp
            boards/prototype2/uart.cpp
            boards/prototype2/timers.cpp
    LIBS serviolib dispatchlib CMSIS::STM32::G431xx HAL::STM32::G4
    OPTS -Og)

  add_library(prototype2 STATIC)
  target_link_libraries(prototype2 PUBLIC prototype2_board serviolib
                                          dispatchlib fwlib STM32::Nano)
  target_link_libraries(
    prototype2
    PRIVATE HAL::STM32::G4::CORTEX
            HAL::STM32::G4::OPAMP
            HAL::STM32::G4::PWR
            HAL::STM32::G4::PWREx
            HAL::STM32::G4::RCC
            HAL::STM32::G4::RCCEx
            HAL::STM32::G4::ADC
            HAL::STM32::G4::ADCEx
            HAL::STM32::G4::DMA
            HAL::STM32::G4::GPIO
            HAL::STM32::G4::TIM
            HAL::STM32::G4::TIMEx
            HAL::STM32::G4::UART
            HAL::STM32::G4::UARTEx)
  target_compile_options(prototype2 PRIVATE -Os)

  servio_add_board_executable(
    TARGET prototype2_fw
    SOURCES src/fw/main.cpp src/fw/it.cpp
    LIBS prototype2
    LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/ld/STM32G431KBTx_FLASH.ld")

else()
  message(WARNING "unsupported platform ${SERVIO_PLATFORM}")
endif()
