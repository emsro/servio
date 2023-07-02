add_library(hostlib STATIC serial.cpp)
target_link_libraries(hostlib PUBLIC serviolib proto)
servio_compile_options(hostlib)

add_executable(config exe/config.cpp)
target_link_libraries(config PRIVATE hostlib dimcli)
servio_compile_options(config)
target_compile_options(config PRIVATE -Og -g)
