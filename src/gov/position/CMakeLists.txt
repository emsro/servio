servio_gen_configuration(
  TARGET
  pos_cfg_def
  DEF
  cfg_def.json
  OUTPUT
  cfg.hpp
  DOC
  cfg.md)
servio_gen_iface(
  TARGET
  posgvnr_iface
  DEF
  iface_def.json
  HDR
  iface.hpp
  PARSER
  parse.cpp
  DOC
  iface.md)
servio_add_library(
  TARGET posgvnrlib
  SOURCES position.cpp parse.cpp
  LIBS gvnrlib vari
  OPTS -Os)
add_dependencies(posgvnrlib pos_cfg_def posgvnr_iface)
