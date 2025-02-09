if(${SERVIO_PLATFORM} STREQUAL "host")

  add_library(drvlib bits/rx_buffer.cpp)
  target_include_directories(drvlib INTERFACE ../)
  target_link_libraries(drvlib PUBLIC emlabcpp)

  servio_add_test(
    TARGET cobs_utest
    SOURCES tests/cobs_utest.cpp
    LIBS drvlib emlabcpp gtest_main)

else()

  servio_add_library(
    TARGET drvlib
    SOURCES bits/rx_buffer.cpp
            cobs_uart.cpp
            leds.cpp
            hbridge.cpp
            flash_cfg.cpp
            paged_i2c_eeprom.cpp
            nl_uart.cpp
    LIBS emlabcpp platform cmake_git_version_tracking
    OPTS -Os)
endif()
