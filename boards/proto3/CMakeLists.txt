servio_add_library(
  TARGET proto3_board
  INCLUDE ../
  SOURCES board.cpp
  LIBS serviolib platform
  OPTS -Os)

target_link_libraries(
  proto3_board
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
