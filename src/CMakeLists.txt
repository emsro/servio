servio_add_library(
  TARGET serviolib
  INCLUDE ./
  SOURCES metrics.cpp control.cpp indication.cpp cfg/storage.cpp
  LIBS emlabcpp
  OPTS -Os)

add_subdirectory(iface)
