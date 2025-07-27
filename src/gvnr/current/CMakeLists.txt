servio_add_configuration(
  TARGET
  curr_cfg_def
  DEF
  cfg_def.json
  OUTPUT
  cfg.hpp
  DOC
  cfg.md)

servio_add_library(
  TARGET currgvnrlib
  SOURCES current.cpp
  LIBS gvnrlib vari
  OPTS -Os)
add_dependencies(currgvnrlib curr_cfg_def)
