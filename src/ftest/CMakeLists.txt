servio_add_library(
  TARGET ftestlib
  SOURCES main.cpp
  LIBS fwlib platform asrt::asrtrpp
  OPTS -Os)
