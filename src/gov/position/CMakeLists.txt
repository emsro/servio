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
  TARGET posgovlibb
  SOURCES parse.cpp
  LIBS govlib vari
  OPTS -Os)
add_dependencies(posgovlibb pos_cfg_def posgov_iface)
add_library(posgovlib OBJECT position.cpp)
servio_compile_options(posgovlib)
target_compile_options(posgovlib PRIVATE -Os)
target_link_libraries(posgovlib PUBLIC posgovlibb)

if(${SERVIO_PLATFORM} STREQUAL "host")

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()

endif()
