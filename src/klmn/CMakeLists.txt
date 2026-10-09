servio_add_library(
  TARGET klmnlib
  SOURCES simulate.cpp
  LIBS servio_emlabcpp
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()

endif()
