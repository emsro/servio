servio_gen_configuration(
  TARGET
  curr_cfg_def
  DEF
  cfg_def.json
  OUTPUT
  cfg.hpp
  DOC
  cfg.md)
servio_gen_iface(
  TARGET
  currgvnr_iface
  DEF
  iface_def.json
  HDR
  iface.hpp
  PARSER
  parse.cpp
  DOC
  iface.md)
servio_add_library(
  TARGET currgvnrlib
  SOURCES current.cpp parse.cpp
  LIBS gvnrlib vari
  OPTS -Os)
add_dependencies(currgvnrlib curr_cfg_def currgvnr_iface)
