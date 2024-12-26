servio_add_library(
  TARGET cfglib
  SOURCES storage.cpp dispatcher.cpp
  LIBS emlabcpp platform ctllib ifacelib avakar::atom
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET cfg_utest
    SOURCES tests/cfg_utest.cpp
    LIBS cfglib ctllib mtrlib magic_enum gtest_main)

  servio_add_test(
    TARGET cfg_storage_utest
    SOURCES tests/cfg_storage_utest.cpp
    LIBS cfglib ctllib mtrlib gtest_main)

endif()
