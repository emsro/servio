add_library(cnvlib INTERFACE)
target_include_directories(cnvlib INTERFACE ../)
target_link_libraries(cnvlib INTERFACE emlabcpp)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_add_test(
    TARGET conv_utest
    SOURCES tests/conv_utest.cpp
    LIBS cnvlib GTest::GTest gtest_main)

endif()
