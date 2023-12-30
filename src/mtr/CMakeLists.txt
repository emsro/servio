servio_add_library(
  TARGET mtrlib
  INCLUDE ../
  SOURCES metrics.cpp
  LIBS emlabcpp)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET
    metrics_utest
    SOURCES
    tests/metrics_utest.cpp
    LIBS
    mtrlib
    GTest::GTest
    gtest_main)

endif()
