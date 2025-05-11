servio_add_library(
  TARGET scmdiolib
  SOURCES bflash.cpp
          serial.cpp
          config_cmds.cpp
          cli.cpp
          port.cpp
          pid_autotune_cmd.cpp
          setuplib.cpp
  LIBS emlabcpp avakar::atom vari CLI11::CLI11 Boost::asio spdlog::spdlog)

servio_add_executable(
  TARGET scmdio
  SOURCES main.cpp
  LIBS scmdiolib)

servio_expand_test(TARGET servio_utest SOURCES
                   ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS scmdiolib)
