servio_add_library(
  TARGET corelib
  SOURCES globals.cpp dispatcher.cpp
  OPTS -Os
  LIBS servio_emlabcpp
       platform
       cfglib
       avakar::atom
       vari
       cmake_git_version_tracking
       govlib
       zll::zll)

if(${SERVIO_PLATFORM} STREQUAL "host")

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()

endif()
