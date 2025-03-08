servio_add_library(
  TARGET ifacelib
  SOURCES parse.cpp
  LIBS avakar::atom vari emlabcpp platform
  OPTS -Og)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(TARGET servio_utest SOURCES
                     ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS ifacelib)

endif()
