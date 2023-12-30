servio_add_library(
  TARGET klmnlib
  INCLUDE ../
  SOURCES simulate.cpp
  LIBS emlabcpp)

if(${SERVIO_PLATFORM} STREQUAL "host")

  find_package(GTest REQUIRED)

  servio_add_test(
    TARGET
    kalman_utest
    SOURCES
    tests/kalman_utest.cpp
    LIBS
    klmnlib
    GTest::GTest
    gtest_main)

endif()
