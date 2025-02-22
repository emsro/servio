servio_add_library(
  TARGET klmnlib
  SOURCES simulate.cpp
  LIBS emlabcpp
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(TARGET servio_utest SOURCES
                     ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS klmnlib)

endif()
