servio_add_library(
  TARGET platform
  INCLUDE ../inc/ ./
  SOURCES adc.cpp platform.cpp
  LIBS serviolib
  OPTS -Os)

target_link_libraries(platform PUBLIC HAL::STM32::H5 CMSIS::STM32::H503xx)
