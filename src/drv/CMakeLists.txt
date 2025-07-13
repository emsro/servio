if(${SERVIO_PLATFORM} STREQUAL "host")

  add_library(drvlib bits/rx_buffer.cpp)
  target_include_directories(drvlib INTERFACE ../)
  target_link_libraries(drvlib PUBLIC emlabcpp)

  servio_expand_test(TARGET servio_utest SOURCES
                     ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS drvlib)

else()

  servio_add_library(
    TARGET drvlib
    SOURCES bits/rx_buffer.cpp cobs_uart.cpp leds.cpp hbridge.cpp flash_cfg.cpp
            paged_i2c_eeprom.cpp
    LIBS emlabcpp platform cmake_git_version_tracking vari
    OPTS -O0)
endif()
