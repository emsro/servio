servio_add_library(
  TARGET scmdiolib
  INCLUDE ../
  SOURCES serial.cpp config_cmds.cpp cli.cpp
  LIBS emlabcpp proto CLI11::CLI11)

servio_add_executable(
  TARGET scmdio
  SOURCES main.cpp
  LIBS scmdiolib)
