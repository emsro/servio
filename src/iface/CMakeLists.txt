servio_add_library(
  TARGET ifacelib
  SOURCES parse.cpp
  LIBS avakar::atom vari emlabcpp platform
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET iface_utest
    SOURCES tests/util_utest.cpp tests/parse_utest.cpp
    LIBS ifacelib gtest gtest_main)

endif()
