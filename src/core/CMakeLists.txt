servio_add_library(
  TARGET corelib
  SOURCES globals.cpp dispatcher.cpp
  LIBS emlabcpp platform avakar::atom vari cmake_git_version_tracking
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET disp_utest
    SOURCES tests/disp_utest.cpp
    LIBS corelib
         emlabcpp
         monlib
         ifacelib
         cfglib
         ctllib
         mtrlib
         gtest_main)

endif()
