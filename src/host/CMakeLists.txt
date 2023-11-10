add_library(hostlib STATIC serial.cpp config_cmds.cpp cli.cpp)
target_link_libraries(hostlib PUBLIC serviolib proto CLI11::CLI11)
servio_compile_options(hostlib)

add_executable(servio main.cpp)
target_link_libraries(servio PRIVATE hostlib)
servio_compile_options(servio)
target_compile_options(servio PRIVATE -Og -g)
