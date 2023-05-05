#include "config.hpp"

#include <emlabcpp/experimental/string_buffer.h>

#pragma once

namespace em = emlabcpp;

namespace cfg
{

struct payload
{
        em::string_buffer< 16 > git_ver;
        uint32_t                id;
};

using page = em::view< std::byte* >;

std::optional< page > find_unused_page( em::view< const page* > pages );

std::optional< page > find_oldest_page( em::view< const page* > pages );

std::optional< page > find_next_page( em::view< const page* > pages );

std::optional< page > find_lastest_page( em::view< const page* > pages );

bool store( page p, const payload& pl, const cfg_map& m );

}  // namespace cfg
