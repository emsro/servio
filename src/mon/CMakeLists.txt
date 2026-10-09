servio_add_library(
  TARGET monlib
  SOURCES indication.cpp effects.cpp
  LIBS servio_emlabcpp platform
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()

endif()
