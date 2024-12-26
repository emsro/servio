servio_add_library(
  TARGET mtrlib
  SOURCES metrics.cpp
  LIBS emlabcpp platform
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET metrics_utest
    SOURCES tests/metrics_utest.cpp
    LIBS mtrlib gtest_main)

endif()
