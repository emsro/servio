servio_add_library(
  TARGET ctllib
  SOURCES control.cpp
  LIBS emlabcpp platform zll
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(TARGET servio_utest SOURCES
                     ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS ctllib)

endif()
