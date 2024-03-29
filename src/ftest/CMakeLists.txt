servio_add_library(
  TARGET ftestlib
  SOURCES main.cpp rewind.cpp
  LIBS fwlib platform
  OPTS -Os)
