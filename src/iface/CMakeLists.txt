if(${SERVIO_PLATFORM} STREQUAL "host")

  find_package(Protobuf CONFIG REQUIRED)

  add_library(proto_iface io.proto)
  target_link_libraries(proto_iface PUBLIC protobuf::libprotobuf)
  set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
  target_include_directories(proto_iface
                             PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")

  protobuf_generate(TARGET proto_iface PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")

endif()

nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS io.proto)

add_library(nano_iface STATIC ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(nano_iface PUBLIC nanopb_lib platform)
target_include_directories(nano_iface SYSTEM PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options(nano_iface PRIVATE -Os)
