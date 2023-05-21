#include "config.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/experimental/string_buffer.h>

#pragma once

namespace em = emlabcpp;

namespace cfg
{

struct payload
{
        em::string_buffer< 16 > git_ver;
        uint32_t                id;

        friend constexpr auto operator<=>( const payload&, const payload& ) = default;
};

using page = em::view< std::byte* >;

std::optional< page > find_unused_page( em::view< const page* > pages );

std::optional< page > find_oldest_page( em::view< const page* > pages );

std::optional< page > find_next_page( em::view< const page* > pages );

std::optional< page > find_latest_page( em::view< const page* > pages );

bool store(
    const payload&                                     pl,
    const cfg_map&                                     m,
    em::function_view< bool( std::size_t, uint64_t ) > writer );

bool load( page p, em::function_view< bool( const payload& ) > pl_cb, cfg_map& m );

}  // namespace cfg
