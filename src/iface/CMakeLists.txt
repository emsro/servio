if(${SERVIO_PLATFORM} STREQUAL "host")

  find_package(Protobuf REQUIRED)

  protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS io.proto)
  get_filename_component(PROTO_HDRS_DIR ${PROTO_HDRS} DIRECTORY)

  add_library(proto STATIC ${PROTO_SRCS})
  target_link_libraries(proto PUBLIC ${Protobuf_LIBRARIES})
  target_include_directories(proto PUBLIC ${PROTO_HDRS_DIR})

elseif(${SERVIO_PLATFORM} STREQUAL "stm32g4")

  nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS io.proto)

  add_library(proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})
  target_link_libraries(proto PUBLIC nanopb_lib)
  target_include_directories(proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

endif()
