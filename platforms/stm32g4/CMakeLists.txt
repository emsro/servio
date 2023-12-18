servio_add_library(
  TARGET platform
  INCLUDE ../inc/ ./
  SOURCES platform.cpp
  LIBS HAL::STM32::G4 CMSIS::STM32::G431xx serviolib
  OPTS -Os)
