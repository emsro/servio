servio_add_library(
  TARGET corelib
  INCLUDE ../
  SOURCES globals.cpp
  LIBS emlabcpp platform
  OPTS -Os)
