servio_add_library(
  TARGET sntrlib
  SOURCES central_sentry.cpp sentry.cpp
  LIBS servio_emlabcpp platform avakar::atom vari zll
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()

endif()
