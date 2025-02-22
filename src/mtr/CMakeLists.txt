servio_add_library(
  TARGET mtrlib
  SOURCES metrics.cpp
  LIBS emlabcpp platform
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(TARGET servio_utest SOURCES
                     ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS mtrlib)

endif()
