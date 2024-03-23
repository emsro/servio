servio_add_library(
  TARGET monlib
  INCLUDE ../
  SOURCES indication.cpp effects.cpp
  LIBS emlabcpp platform
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET effects_utest
    SOURCES tests/effects_utest.cpp
    LIBS monlib GTest::GTest gtest_main)

endif()
