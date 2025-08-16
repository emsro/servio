servion_gen_cfg(
  TARGET vel_cfg_def
  DEF cfg_def.json
  OUTPUT cfg.hpp
  DOC cfg.md)
servio_gen_iface(
  TARGET vel_iface_def
  DEF iface_def.json
  HDR iface.hpp
  PARSER parse.cpp
  DOC iface.md)
servio_add_library(
  TARGET velgovlib
  SOURCES velocity.cpp parse.cpp
  LIBS govlib vari
  OPTS -Os)
add_dependencies(velgovlib vel_cfg_def vel_iface_def)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(
    TARGET servio_utest SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp
    LIBS velgovlib)

endif()
