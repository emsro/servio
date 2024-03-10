servio_add_library(
  TARGET ftesterlib
  INCLUDE ../
  SOURCES config.cpp handle_eptr.cpp cli.cpp flash.cpp
  LIBS scmdiolib GTest::GTest
  OPTS -Og)

servio_add_executable(
  TARGET ftester
  INCLUDE ../
  SOURCES main.cpp
  LIBS ftesterlib
  OPTS -Og)
