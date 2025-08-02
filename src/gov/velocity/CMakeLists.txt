servio_gen_configuration(
  TARGET
  vel_cfg_def
  DEF
  cfg_def.json
  OUTPUT
  cfg.hpp
  DOC
  cfg.md)
servio_gen_iface(
  TARGET
  velgvnr_iface
  DEF
  iface_def.json
  HDR
  iface.hpp
  PARSER
  parse.cpp
  DOC
  iface.md)
servio_add_library(
  TARGET velgvnrlib
  SOURCES velocity.cpp parse.cpp
  LIBS gvnrlib vari
  OPTS -Os)
add_dependencies(velgvnrlib vel_cfg_def velgvnr_iface)
