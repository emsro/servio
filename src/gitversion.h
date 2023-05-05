#include <string_view>

#pragma once

extern const char _binary_gitrevision_start;
extern const int  _binary_gitrevision_size;

static std::string_view GITREVISION{
    &_binary_gitrevision_start,
    static_cast< std::size_t >( _binary_gitrevision_size ) };
