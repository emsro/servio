servio_add_library(
  TARGET sntrlib
  INCLUDE ../
  SOURCES central_sentry.cpp sentry.cpp
  LIBS emlabcpp platform
  OPTS -Os)
