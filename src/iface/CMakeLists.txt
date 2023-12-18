if(${SERVIO_PLATFORM} STREQUAL "host")

  find_package(Protobuf REQUIRED)

  protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS io.proto)
  get_filename_component(PROTO_HDRS_DIR ${PROTO_HDRS} DIRECTORY)

  add_library(proto STATIC ${PROTO_SRCS})
  target_link_libraries(proto PUBLIC ${Protobuf_LIBRARIES})
  target_include_directories(proto SYSTEM PUBLIC ${PROTO_HDRS_DIR})
  set_target_properties(proto PROPERTIES CXX_CLANG_TIDY "")

elseif(${SERVIO_PLATFORM} STREQUAL "stm32g4" OR ${SERVIO_PLATFORM} STREQUAL
                                                "stm32h5")

  nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS io.proto)

  add_library(proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})
  target_link_libraries(proto PUBLIC nanopb_lib platform)
  target_include_directories(proto SYSTEM PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

endif()
