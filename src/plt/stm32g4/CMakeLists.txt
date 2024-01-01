servio_add_library(
  TARGET platform
  INCLUDE ../inc/ ./ ../../
  SOURCES platform.cpp
  LIBS HAL::STM32::G4 CMSIS::STM32::G431xx STM32::Nano
  OPTS -Os)

target_link_libraries(
  platform
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
