add_subdirectory(iface)
add_subdirectory(klmn)
add_subdirectory(base)
add_subdirectory(ctl)
add_subdirectory(mon)
add_subdirectory(mtr)
add_subdirectory(core)
add_subdirectory(cfg)

add_library(serviolib INTERFACE)

target_link_libraries(serviolib INTERFACE monlib corelib cfglib mtrlib ctllib)

if(${SERVIO_PLATFORM} STREQUAL "host")

  add_subdirectory(scmdio)
  add_subdirectory(bb)

else()

  add_subdirectory(fw)
  target_link_libraries(serviolib INTERFACE fwlib)

endif()
