servio_add_library(
  TARGET hostlib
  INCLUDE ../
  SOURCES serial.cpp config_cmds.cpp cli.cpp
  LIBS serviolib proto CLI11::CLI11)

servio_add_executable(TARGET servio SOURCES main.cpp LIBS hostlib)
