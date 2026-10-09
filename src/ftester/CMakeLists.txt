# ftester talks to the board through POSIX serial and termios
if(NOT WIN32)
  add_executable(ftester main.cpp harness.cpp)
  servio_compile_options(ftester)
  target_link_libraries(ftester PRIVATE asrtio_lib CLI11::CLI11)
endif()
