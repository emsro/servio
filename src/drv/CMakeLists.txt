if(${SERVIO_PLATFORM} STREQUAL "host")

  add_library(drvlib INTERFACE)
  target_include_directories(drvlib INTERFACE ../)

  servio_add_test(
    TARGET cobs_utest
    SOURCES tests/cobs_utest.cpp
    LIBS drvlib emlabcpp GTest::GTest gtest_main)

else()

  servio_add_library(
    TARGET drvlib
    INCLUDE ../
    SOURCES cobs_uart.cpp leds.cpp hbridge.cpp
    LIBS emlabcpp proto platform cmake_git_version_tracking
    OPTS -Os)

  servio_add_library(
    TARGET drv_testlib
    INCLUDE ../
    SOURCES tests/setup_drv.cpp
    LIBS drvlib proto platform
    OPTS -Os)

endif()
