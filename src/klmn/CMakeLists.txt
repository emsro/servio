servio_add_library(
  TARGET klmnlib
  SOURCES simulate.cpp
  LIBS emlabcpp
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET kalman_utest
    SOURCES tests/kalman_utest.cpp
    LIBS klmnlib gtest_main)

endif()
