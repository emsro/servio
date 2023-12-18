servio_add_library(
  TARGET platform
  INCLUDE ../inc/ ./
  SOURCES adc.cpp platform.cpp
  LIBS HAL::STM32::H5 CMSIS::STM32::H503xx serviolib
  OPTS -Os)
