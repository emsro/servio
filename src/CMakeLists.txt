add_subdirectory(base)
add_subdirectory(cfg)
add_subdirectory(cnv)
add_subdirectory(core)
add_subdirectory(ctl)
add_subdirectory(iface)
add_subdirectory(klmn)
add_subdirectory(mon)
add_subdirectory(mtr)

add_library(serviolib INTERFACE)

target_link_libraries(serviolib INTERFACE monlib corelib cfglib mtrlib ctllib)

if(${SERVIO_PLATFORM} STREQUAL "host")

  add_subdirectory(scmdio)
  target_link_libraries(serviolib INTERFACE scmdiolib)
  add_subdirectory(bb)

else()

  add_subdirectory(fw)
  add_subdirectory(drv)
  target_link_libraries(serviolib INTERFACE fwlib drvlib)

endif()
