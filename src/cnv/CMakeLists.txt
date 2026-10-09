add_library(cnvlib INTERFACE)
target_link_libraries(cnvlib INTERFACE servio_emlabcpp)

if(${SERVIO_PLATFORM} STREQUAL "host")

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()

endif()
