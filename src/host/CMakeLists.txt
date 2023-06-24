add_executable(config exe/config.cpp)
target_link_libraries(config PRIVATE serviolib proto dimcli)
servio_compile_options(config)
target_compile_options(config PRIVATE -Og -g)
