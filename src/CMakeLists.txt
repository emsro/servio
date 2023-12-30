add_subdirectory(iface)
add_subdirectory(klmn)
add_subdirectory(ctl)
add_subdirectory(mon)
add_subdirectory(mtr)

servio_add_library(
  TARGET serviolib
  INCLUDE ./
  SOURCES cfg/storage.cpp globals.cpp cfg/dispatcher.cpp
  LIBS emlabcpp cmake_git_version_tracking
  OPTS -Os)
