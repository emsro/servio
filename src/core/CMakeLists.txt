servio_add_library(
  TARGET corelib
  SOURCES globals.cpp dispatcher.cpp
  LIBS emlabcpp platform avakar::atom vari cmake_git_version_tracking)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(TARGET servio_utest SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS corelib)

endif()
