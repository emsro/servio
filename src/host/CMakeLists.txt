servio_add_library(
  TARGET hostlib
  INCLUDE ../
  SOURCES serial.cpp config_cmds.cpp cli.cpp
  LIBS serviolib proto CLI11::CLI11)

add_executable(servio main.cpp)
target_link_libraries(servio PRIVATE hostlib)
servio_compile_options(servio)
target_compile_options(servio PRIVATE -Og -g)
