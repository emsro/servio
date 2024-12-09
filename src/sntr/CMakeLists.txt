servio_add_library(
  TARGET sntrlib
  SOURCES central_sentry.cpp sentry.cpp
  LIBS emlabcpp platform
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET central_sentry_utest
    SOURCES tests/central_sentry_utest.cpp
    LIBS sntrlib gtest gtest_main)

  servio_add_test(
    TARGET sentry_utest
    SOURCES tests/sentry_utest.cpp
    LIBS sntrlib gtest gtest_main)

endif()
