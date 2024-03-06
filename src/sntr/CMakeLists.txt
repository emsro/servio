servio_add_library(
  TARGET sntrlib
  INCLUDE ../
  SOURCES central_sentry.cpp sentry.cpp
  LIBS emlabcpp platform
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET central_sentry_utest
    SOURCES tests/central_sentry_utest.cpp
    LIBS sntrlib GTest::GTest gtest_main)

endif()
