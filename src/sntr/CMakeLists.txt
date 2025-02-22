servio_add_library(
  TARGET sntrlib
  SOURCES central_sentry.cpp sentry.cpp
  LIBS emlabcpp platform avakar::atom
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(TARGET servio_utest SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS sntrlib)

endif()
