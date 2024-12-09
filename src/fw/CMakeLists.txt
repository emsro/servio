servio_add_library(
  TARGET fwlib
  SOURCES dispatcher.cpp extra.cpp store_persistent_config.cpp util.cpp
          context.cpp
  LIBS avakar::atom
       vari
       emlabcpp
       cfglib
       mtrlib
       platform
       cmake_git_version_tracking
  OPTS -Os)
