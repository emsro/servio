if(${SERVIO_PLATFORM} STREQUAL "host")

  add_library(drvlib INTERFACE)
  target_include_directories(drvlib INTERFACE ../)

  servio_add_test(
    TARGET cobs_utest
    SOURCES tests/cobs_utest.cpp
    LIBS drvlib emlabcpp gtest_main)

else()

  servio_add_library(
    TARGET drvlib
    SOURCES cobs_uart.cpp leds.cpp hbridge.cpp flash_cfg.cpp
    LIBS emlabcpp platform cmake_git_version_tracking
    OPTS -Os)
endif()
