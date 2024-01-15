servio_add_library(
  TARGET drvlib
  INCLUDE ../
  SOURCES cobs_uart.cpp leds.cpp hbridge.cpp
  LIBS emlabcpp proto platform cmake_git_version_tracking
  OPTS -Os)
