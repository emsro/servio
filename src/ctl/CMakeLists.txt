servio_add_library(
  TARGET ctllib
  SOURCES control.cpp
  LIBS emlabcpp platform
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET control_utest
    SOURCES tests/control_utest.cpp
    LIBS ctllib gtest_main)

endif()
