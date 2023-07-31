servio_add_library(
  TARGET serviolib
  INCLUDE ./
  SOURCES metrics.cpp control.cpp indication.cpp cfg/storage.cpp
  LIBS emlabcpp cmake_git_version_tracking
  OPTS -Os)

add_subdirectory(iface)
