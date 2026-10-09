servio_add_library(
  TARGET govlib
  SOURCES governor.cpp
  LIBS servio_emlabcpp platform zll vari
  OPTS -Os)

add_subdirectory(power)
add_subdirectory(current)
add_subdirectory(position)
add_subdirectory(velocity)

if(${SERVIO_PLATFORM} STREQUAL "host")

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()

endif()
