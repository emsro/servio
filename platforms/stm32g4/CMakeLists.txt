servio_add_library(
  TARGET platform
  INCLUDE ../inc/ ./ ../../src/
  SOURCES platform.cpp
  LIBS HAL::STM32::G4 CMSIS::STM32::G431xx
  OPTS -Os)
