servio_add_library(
  TARGET monlib
  SOURCES indication.cpp effects.cpp
  LIBS emlabcpp platform
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(TARGET servio_utest SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS monlib)

endif()
