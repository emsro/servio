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
include(cmake/magic_enum.cmake)
include(cmake/nanopb.cmake)

if(${SERVIO_PLATFORM} STREQUAL "host")
  target_compile_options(
    emlabcpp PUBLIC -DEMLABCPP_USE_LOGGING -DEMLABCPP_USE_NLOHMANN_JSON
                    -DEMLABCPP_USE_GTEST)

elseif(${SERVIO_PLATFORM} STREQUAL "stm32g4")
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fsingle-precision-constant  -Fno-rtti -fno-exceptions -gdwarf-2 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard"
  )
  set(CMAKE_C_FLAGS
      "${CMAKE_C_FLAGS} -fsingle-precision-constant -gdwarf-2 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16  -mfloat-abi=hard"
  )
elseif(${SERVIO_PLATFORM} STREQUAL "stm32g5")
  set(SERVIO_SHARED_FLAGS
      "-fsingle-precision-constant -gdwarf-2 -mcpu=cortex-m33 -mfpu=fpv5-d16  -mfloat-abi=hard"
  )
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} ${SERVIO_SHARED_FLAGS} -Fno-rtti -fno-exceptions -gdwarf-2"
  )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SERVIO_SHARED_FLAGS}")
endif()

include(cmake/cmake-git-version-tracking.cmake)

add_subdirectory(src)

if(${SERVIO_PLATFORM} STREQUAL "host")

  include(cmake/cli11.cmake)

  include(CTest)
  enable_testing()
  add_subdirectory(src/host)
  add_subdirectory(tests)

elseif(${SERVIO_PLATFORM} STREQUAL "stm32g4")

  stm32_fetch_hal(G4)
  stm32_fetch_cmsis(G4)
  find_package(
    CMSIS
    COMPONENTS STM32G4
    REQUIRED GLOBAL)
  find_package(HAL COMPONENTS STM32G4 REQUIRED GLOBAL)

  add_subdirectory(boards/prototype2)
  target_link_libraries(prototype2_board PUBLIC CMSIS::STM32::G431xx
                                                HAL::STM32::G4)

  add_subdirectory(src/fw)

  add_library(prototype2 STATIC)
  target_link_libraries(prototype2 PUBLIC prototype2_board serviolib fwlib
                                          STM32::Nano)
  target_link_libraries(
    prototype2
    PRIVATE HAL::STM32::G4::FLASH
            HAL::STM32::G4::FLASHEx
            HAL::STM32::G4::ADC
            HAL::STM32::G4::ADCEx
            HAL::STM32::G4::CORTEX
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
            HAL::STM32::G4::UARTEx)
  target_compile_options(prototype2 PRIVATE -Os)

  servio_add_board_executable(
    TARGET prototype2_fw
    SOURCES src/fw/main.cpp src/fw/it.cpp
    LIBS prototype2
    LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/ld/STM32G431KBTx_FLASH.ld")

elseif(${SERVIO_PLATFORM} STREQUAL "stm32h5")

  stm32_fetch_hal(H5)
  stm32_fetch_cmsis(H5)
  find_package(CMSIS COMPONENTS STM32H5 REQUIRED GLOBAL)
  find_package(HAL COMPONENTS STM32H5 REQUIRED GLOBAL)

  add_subdirectory(boards/prototype2)
  target_link_libraries(prototype2_board PUBLIC CMSIS::STM32::H503xx
                                                HAL::STM32::H5)

  add_subdirectory(src/fw)

  add_library(prototype2 STATIC)
  target_link_libraries(prototype2 PUBLIC prototype2_board serviolib fwlib
                                          STM32::Nano)
  target_link_libraries(
    prototype2
    PRIVATE HAL::STM32::H5::FLASH
            HAL::STM32::H5::FLASHEx
            HAL::STM32::H5::ADC
            HAL::STM32::H5::ADCEx
            HAL::STM32::H5::CORTEX
            HAL::STM32::H5::DMA
            HAL::STM32::H5::GPIO
            HAL::STM32::H5::OPAMP
            HAL::STM32::H5::PWR
            HAL::STM32::H5::PWREx
            HAL::STM32::H5::RCC
            HAL::STM32::H5::RCCEx
            HAL::STM32::H5::TIM
            HAL::STM32::H5::TIMEx
            HAL::STM32::H5::UART
            HAL::STM32::H5::UARTEx)
  target_compile_options(prototype2 PRIVATE -Os)

  servio_add_board_executable(
    TARGET prototype2_fw
    SOURCES src/fw/main.cpp src/fw/it.cpp
    LIBS prototype2
    LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/ld/STM32H503xx_FLASH.ld")

else()
  message(WARNING "unsupported platform ${SERVIO_PLATFORM}")
endif()
