add_subdirectory(cfg)
add_subdirectory(cnv)
add_subdirectory(core)
add_subdirectory(ctl)
add_subdirectory(sntr)
add_subdirectory(iface)
add_subdirectory(klmn)
add_subdirectory(mon)
add_subdirectory(mtr)
add_subdirectory(lib)
add_subdirectory(drv)

add_library(serviolib INTERFACE)

target_link_libraries(
  serviolib
  INTERFACE sntrlib
            monlib
            ifacelib
            corelib
            cfglib
            mtrlib
            ctllib
            drvlib)

if(${SERVIO_PLATFORM} STREQUAL "host")

  add_subdirectory(scmdio)
  target_link_libraries(serviolib INTERFACE scmdiolib)
  add_subdirectory(bb)
  add_subdirectory(ftester)

else()

  add_subdirectory(fw)
  target_link_libraries(serviolib INTERFACE fwlib)

  add_subdirectory(ftest)

endif()
