servio_add_library(
  TARGET proto3_board
  INCLUDE ../
  SOURCES board.cpp
  LIBS platform
  OPTS -Os)
