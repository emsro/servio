add_library(cnvlib INTERFACE)
target_link_libraries(cnvlib INTERFACE emlabcpp)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET conv_utest
    SOURCES tests/conv_utest.cpp
    LIBS cnvlib gtest_main)

endif()
