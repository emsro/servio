servio_add_library(
  TARGET scmdiolib
  SOURCES dfu_flash.cpp
          serial.cpp
          config_cmds.cpp
          cli.cpp
          port.cpp
          pid_autotune_cmd.cpp
          preset.cpp
  LIBS emlabcpp avakar::atom vari CLI11::CLI11 Boost::asio spdlog::spdlog)

servio_add_executable(
  TARGET scmdio
  SOURCES main.cpp
  LIBS scmdiolib)

install(TARGETS scmdio RUNTIME DESTINATION bin)

servio_expand_test(TARGET servio_utest SOURCES
                   ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS scmdiolib)
