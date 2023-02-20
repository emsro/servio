cmake_minimum_required(VERSION 3.24)

project(servio C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

cmake_policy(SET CMP0077 NEW)
option(SERVIO_PLATFORM "Target platform")
option(SERVIO_EMLABCPP_SRC "Gives a location of emlabcpp source")

include(cmake/emlabcpp.cmake)
include(cmake/flags.cmake)
include(cmake/utils.cmake)

servio_add_library(
  TARGET serviolib
  INCLUDE src/
  SOURCES src/metrics.cpp src/control.cpp src/protocol.cpp src/indication.cpp
  LIBS emlabcpp
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")
  include(CTest)
  enable_testing()

  add_subdirectory(tests)

elseif(${SERVIO_PLATFORM} STREQUAL "stm32g4")

  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS}  -Fno-rtti -fno-exceptions -gdwarf-2 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard"
  )
  set(CMAKE_C_FLAGS
      "${CMAKE_C_FLAGS} -fsingle-precision-constant -gdwarf-2 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16  -mfloat-abi=hard"
  )

  stm32_fetch_hal(G4)
  stm32_fetch_cmsis(G4)
  find_package(
    CMSIS
    COMPONENTS STM32G4
    REQUIRED GLOBAL)
  find_package(
    HAL
    COMPONENTS STM32G4
    REQUIRED GLOBAL)

  servio_add_library(
    TARGET cfglib
    INCLUDE src/
    SOURCES src/fw/cfg_dispatcher.cpp
    LIBS serviolib
         CMSIS::STM32::G431xx
         HAL::STM32::G4::ADC
         STM32::NoSys
    OPTS -O3)

  servio_add_library(
    TARGET prototype2_board
    SOURCES boards/prototype2/adc.cpp
            boards/prototype2/board.cpp
            boards/prototype2/gpio.cpp
            boards/prototype2/clock.cpp
            boards/prototype2/config.cpp
            boards/prototype2/duart.cpp
            boards/prototype2/extra.cpp
            boards/prototype2/iuart.cpp
            boards/prototype2/timers.cpp
            src/fw/drivers/acquisition.cpp
            src/fw/drivers/comms.cpp
            src/fw/drivers/debug_comms.cpp
            src/fw/drivers/hbridge.cpp
            src/fw/drivers/leds.cpp
            src/fw/extra.cpp
            src/fw/globals.cpp
            src/fw/util.cpp
    LIBS  serviolib cfglib
         CMSIS::STM32::G431xx
         HAL::STM32::G4::CORTEX
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
         HAL::STM32::G4::UARTEx
         STM32::NoSys
    OPTS -Og)

  servio_add_board_executable(
    TARGET prototype2_fw
    SOURCES src/fw/main.cpp src/fw/it.cpp
    LIBS prototype2_board
    LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/ld/STM32G431KBTx_FLASH.ld")

else()
  message(FATAL_ERROR "unsupported platform ${SERVIO_PLATFORM}")
endif()
