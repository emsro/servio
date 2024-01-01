servio_add_library(
  TARGET cfglib
  INCLUDE ../
  SOURCES storage.cpp dispatcher.cpp
  LIBS emlabcpp platform
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET cfg_utest
    SOURCES tests/cfg_utest.cpp
    LIBS cfglib
         ctllib
         mtrlib
         proto
         magic_enum
         GTest::GTest
         gtest_main)

  servio_add_test(
    TARGET cfg_storage_utest
    SOURCES tests/cfg_storage_utest.cpp
    LIBS cfglib ctllib mtrlib proto GTest::GTest gtest_main)

endif()
