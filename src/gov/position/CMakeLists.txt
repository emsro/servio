servion_gen_cfg(
  TARGET pos_cfg_def
  DEF cfg_def.json
  OUTPUT cfg.hpp
  DOC cfg.md)
servio_gen_iface(
  TARGET posgov_iface
  DEF iface_def.json
  HDR iface.hpp
  PARSER parse.cpp
  DOC iface.md)
servio_add_library(
  TARGET posgovlib
  SOURCES position.cpp parse.cpp
  LIBS gvnrlib vari
  OPTS -Os)
add_dependencies(posgovlib pos_cfg_def posgov_iface)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(
    TARGET servio_utest SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp
    LIBS posgovlib)

endif()
