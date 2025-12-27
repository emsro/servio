servion_gen_cfg(
  TARGET curr_cfg_def
  DEF cfg_def.json
  OUTPUT cfg.hpp
  DOC cfg.md)
servio_gen_iface(
  TARGET currgov_iface
  DEF iface_def.json
  HDR iface.hpp
  PARSER parse.cpp
  DOC iface.md)

servio_add_library(
  TARGET currgovlibb
  SOURCES parse.cpp
  LIBS govlib vari
  OPTS -Os)
add_dependencies(currgovlibb curr_cfg_def currgov_iface)
add_library(currgovlib OBJECT current.cpp)
target_link_libraries(currgovlib PUBLIC currgovlibb)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(
    TARGET servio_utest SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp
    LIBS currgovlib)

endif()
