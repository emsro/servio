servio_add_library(
  TARGET cfglib
  SOURCES storage.cpp dispatcher.cpp
  LIBS emlabcpp platform ctllib ifacelib avakar::atom
  OPTS -Os)

if(${SERVIO_PLATFORM} STREQUAL "host")
 
  servio_expand_test(TARGET servio_utest SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS cfglib)

endif()
