add_executable(ftester main.cpp harness.cpp)
target_link_libraries(ftester PRIVATE asrtio_lib CLI11::CLI11)
