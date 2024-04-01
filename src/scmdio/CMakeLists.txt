servio_add_library(
  TARGET scmdiolib
  INCLUDE ../
  SOURCES serial.cpp config_cmds.cpp cli.cpp async_cobs.cpp
  LIBS emlabcpp proto_iface CLI11::CLI11)

servio_add_executable(
  TARGET scmdio
  SOURCES main.cpp
  LIBS scmdiolib)
