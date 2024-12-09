#include "./map.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/experimental/string_buffer.h>

#pragma once

namespace em = emlabcpp;

namespace servio::cfg
{

using page = em::view< std::byte* >;

page const* find_unused_page( em::view< page const* > pages );

page const* find_oldest_page( em::view< page const* > pages );

page const* find_next_page( em::view< page const* > pages );

page const* find_latest_page( em::view< page const* > pages );

std::tuple< bool, page > store( payload const& pl, map const* m, page buffer );

bool load( page p, em::function_view< bool( payload const& ) > pl_cb, map& m );

}  // namespace servio::cfg
