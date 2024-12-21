add_library(baselib INTERFACE)
target_include_directories(baselib INTERFACE ../)
target_link_libraries(baselib INTERFACE emlabcpp)
