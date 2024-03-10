servio_add_library(
  TARGET platform
  INCLUDE ../inc/ ./ ../../
  SOURCES adc.cpp
          platform.cpp
          clock.cpp
          uart.cpp
          config.cpp
          gpio.cpp
          timers.cpp
  LIBS emlabcpp HAL::STM32::H5 CMSIS::STM32::H503xx STM32::Nano STM32::NoSys
  OPTS -Os)

target_link_libraries(
  platform
  PRIVATE HAL::STM32::H5::FLASH
          HAL::STM32::H5::FLASHEx
          HAL::STM32::H5::ADC
          HAL::STM32::H5::ADCEx
          HAL::STM32::H5::CORTEX
          HAL::STM32::H5::DMA
          HAL::STM32::H5::DMAEx
          HAL::STM32::H5::GPIO
          HAL::STM32::H5::OPAMP
          HAL::STM32::H5::PWR
          HAL::STM32::H5::PWREx
          HAL::STM32::H5::RCC
          HAL::STM32::H5::ICACHE
          HAL::STM32::H5::RCCEx
          HAL::STM32::H5::TIM
          HAL::STM32::H5::TIMEx
          HAL::STM32::H5::UART
          HAL::STM32::H5::UARTEx)
