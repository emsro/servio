servio_add_library(
  TARGET fwlib
  SOURCES dispatcher.cpp extra.cpp store_persistent_config.cpp servio_pb.cpp
          util.cpp context.cpp
  LIBS emlabcpp cfglib mtrlib nano_iface platform cmake_git_version_tracking
  OPTS -Os)
