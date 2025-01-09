servio_add_library(
  TARGET scmdiolib
  SOURCES bflash.cpp serial.cpp config_cmds.cpp cli.cpp port.cpp
          pid_autotune_cmd.cpp
  LIBS emlabcpp avakar::atom vari CLI11::CLI11 Boost::asio spdlog::spdlog)

servio_add_executable(
  TARGET scmdio
  SOURCES main.cpp
  LIBS scmdiolib)

servio_add_test(
  TARGET autotune_utest
  SOURCES tests/autotune_utest.cpp
  LIBS scmdiolib gtest_main)

servio_add_test(
  TARGET scmdio_iface_utest
  SOURCES tests/iface_utest.cpp
  LIBS scmdiolib corelib monlib mtrlib cfglib gtest_main)

servio_add_test(
  TARGET scmdio_bflash_utest
  SOURCES tests/bflash_utest.cpp
  LIBS scmdiolib corelib monlib mtrlib cfglib gtest_main)
