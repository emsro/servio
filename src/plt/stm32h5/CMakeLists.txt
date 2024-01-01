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
  LIBS emlabcpp
  OPTS -Os)

target_link_libraries(platform PUBLIC HAL::STM32::H5 CMSIS::STM32::H503xx)
