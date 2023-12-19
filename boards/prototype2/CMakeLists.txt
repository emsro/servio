servio_add_library(
  TARGET prototype2_board
  INCLUDE ../
  SOURCES adc.cpp
          board.cpp
          gpio.cpp
          clock.cpp
          config.cpp
          uart.cpp
          timers.cpp
  LIBS serviolib platform
  OPTS -Os)

target_link_libraries(
  prototype2_board
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
