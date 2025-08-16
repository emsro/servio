servio_add_library(
  TARGET govlib
  SOURCES governor.cpp
  LIBS emlabcpp platform zll vari
  OPTS -Os)

add_subdirectory(power)
add_subdirectory(current)
add_subdirectory(position)
add_subdirectory(velocity)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(TARGET servio_utest SOURCES
                     ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS govlib)

endif()
