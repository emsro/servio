add_subdirectory(cfg)
add_subdirectory(iface)
add_subdirectory(cnv)
add_subdirectory(core)
add_subdirectory(ctl)
add_subdirectory(gov)
add_subdirectory(sntr)
add_subdirectory(klmn)
add_subdirectory(mon)
add_subdirectory(mtr)
add_subdirectory(lib)
add_subdirectory(drv)

servio_add_library(TARGET serviolib SOURCES status.cpp)

target_link_libraries(
  serviolib
  PUBLIC sntrlib
         monlib
         ifacelib
         corelib
         cfglib
         mtrlib
         ctllib
         drvlib)

if(${SERVIO_PLATFORM} STREQUAL "host")

  add_subdirectory(scmdio)
  target_link_libraries(serviolib PUBLIC scmdiolib)
  add_subdirectory(bb)
  add_subdirectory(ftester)

else()

  add_subdirectory(fw)
  target_link_libraries(serviolib PUBLIC fwlib)

  add_subdirectory(ftest)

endif()
