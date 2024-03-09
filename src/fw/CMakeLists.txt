servio_add_library(
  TARGET fwlib
  SOURCES dispatcher.cpp extra.cpp store_persistent_config.cpp servio_pb.cpp
          util.cpp setup.cpp
  LIBS emlabcpp cfglib mtrlib proto platform cmake_git_version_tracking
  OPTS -Os)
