servio_gen_iface(
  TARGET pow_iface_def
  DEF iface_def.json
  HDR iface.hpp
  PARSER parse.cpp
  DOC iface.md)

servio_add_library(
  TARGET powgovlibb
  SOURCES parse.cpp
  LIBS govlib vari
  OPTS -Os)
add_dependencies(powgovlibb pow_iface_def)
add_library(powgovlib OBJECT power.cpp)
target_link_libraries(powgovlib PUBLIC powgovlibb)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(
    TARGET servio_utest SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp
    LIBS powgovlib)

endif()
