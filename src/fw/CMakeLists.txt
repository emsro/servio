servio_add_library(
  TARGET fwlib
  SOURCES extra.cpp util.cpp context.cpp
  LIBS avakar::atom
       vari
       emlabcpp
       govlib
       cfglib
       mtrlib
       platform
       zll
  OPTS -Os)
target_sources(fwlib INTERFACE it.cpp)
