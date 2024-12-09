servio_add_library(
  TARGET ftesterlib
  SOURCES config.cpp handle_eptr.cpp cli.cpp flash.cpp
  LIBS scmdiolib joque
  OPTS -Og)

servio_add_executable(
  TARGET ftester
  SOURCES main.cpp
  LIBS ftesterlib
  OPTS -Og)
