if(${SERVIO_PLATFORM} STREQUAL "host")

  add_library(drvlib bits/rx_buffer.cpp)
  servio_compile_options(drvlib)
  target_include_directories(drvlib INTERFACE ../)
  target_link_libraries(drvlib PUBLIC servio_emlabcpp)

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()

else()

  servio_add_library(
    TARGET drvlib
    SOURCES bits/rx_buffer.cpp cobs_uart.cpp leds.cpp hbridge.cpp
            paged_i2c_eeprom.cpp
    LIBS servio_emlabcpp platform cmake_git_version_tracking vari zll
    OPTS -Os)
endif()
