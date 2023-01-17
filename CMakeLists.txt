cmake_minimum_required(VERSION 3.24)

project(servio C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

include(cmake/flags.cmake)

cmake_policy(SET CMP0077 NEW)
option(SERVIO_PLATFORM "Target platform")
option(SERVIO_EMLABCPP_SRC "Gives a location of emlabcpp source")

if(NOT TARGET emlabcpp)
if(SERVIO_EMLABCPP_SRC)
  if(NOT IS_ABSOLUTE ${SERVIO_EMLABCPP_SRC})
    set(SERVIO_EMLABCPP_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/${SERVIO_EMLABCPP_SRC}")
  endif()
  add_subdirectory(${SERVIO_EMLABCPP_SRC} emlabcpp)
else()
  include(FetchContent)
  FetchContent_Declare(
    emlabcpp
    GIT_REPOSITORY https://github.com/koniarik/emlabcpp
    GIT_TAG main)
  FetchContent_MakeAvailable(emlabcpp)
endif()
endif()
target_compile_options(emlabcpp PRIVATE -Os)

add_library(servio_lib STATIC src/metrics.cpp src/control.cpp src/protocol.cpp src/indication.cpp)
target_include_directories(servio_lib PUBLIC src/)
target_link_libraries(servio_lib PUBLIC emlabcpp)
servio_compile_options(servio_lib)
target_compile_options(servio_lib PRIVATE -Os)

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
    REQUIRED
    GLOBAL
)
  find_package(
    HAL
    COMPONENTS STM32G4
    REQUIRED
    GLOBAL
)

  function(servio_board_library)
    cmake_parse_arguments(A "" "TARGET" "CUSTOM_FILES" ${ARGN})

    add_library(
      ${A_TARGET} STATIC
      ${A_CUSTOM_FILES}
      src/fw/cfg_dispatcher.cpp
      src/fw/drivers/acquisition.cpp
      src/fw/drivers/comms.cpp
      src/fw/drivers/debug_comms.cpp
      src/fw/drivers/hbridge.cpp
      src/fw/drivers/leds.cpp
      src/fw/extra.cpp
      src/fw/globals.cpp
      src/fw/util.cpp)
    servio_compile_options(${A_TARGET})
    target_link_libraries(
      ${A_TARGET}
      PUBLIC servio_lib
             CMSIS::STM32::G431xx
             HAL::STM32::G4::ADC
             HAL::STM32::G4::ADCEx
             HAL::STM32::G4::CORTEX
             HAL::STM32::G4::DAC
             HAL::STM32::G4::DMA
             HAL::STM32::G4::GPIO
             HAL::STM32::G4::OPAMP
             HAL::STM32::G4::PWR
             HAL::STM32::G4::PWREx
             HAL::STM32::G4::RCC
             HAL::STM32::G4::RCCEx
             HAL::STM32::G4::TIM
             HAL::STM32::G4::TIMEx
             HAL::STM32::G4::UART
             HAL::STM32::G4::UARTEx
             STM32::NoSys)
  endfunction()

  servio_board_library(
    TARGET
    prototype2_board
    CUSTOM_FILES
    boards/prototype2/adc.cpp
    boards/prototype2/board.cpp
    boards/prototype2/gpio.cpp
    boards/prototype2/clock.cpp
    boards/prototype2/config.cpp
    boards/prototype2/duart.cpp
    boards/prototype2/extra.cpp
    boards/prototype2/iuart.cpp
    boards/prototype2/timers.cpp)
  target_compile_options(prototype2_board PRIVATE -Og)

  add_executable(prototype2_fw src/fw/main.cpp src/fw/it.cpp)
  servio_compile_options(prototype2_fw)
  target_link_libraries(prototype2_fw PUBLIC prototype2_board)
  stm32_add_linker_script(
    prototype2_fw PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/ld/STM32G431KBTx_FLASH.ld")
  stm32_print_size_of_target(prototype2_fw)

else()
  message(FATAL_ERROR "unsupported platform ${SERVIO_PLATFORM}")
endif()
