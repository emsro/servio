add_library(baselib INTERFACE)
target_include_directories(baselib INTERFACE ../)
target_link_libraries(baselib INTERFACE emlabcpp)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET fmt_utest
    SOURCES tests/fmt_utest.cpp
    LIBS baselib gtest_main)

endif()
