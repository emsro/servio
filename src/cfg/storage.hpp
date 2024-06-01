#include "cfg/map.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/experimental/string_buffer.h>

#pragma once

namespace em = emlabcpp;

namespace servio::cfg
{

using page = em::view< std::byte* >;

const page* find_unused_page( em::view< const page* > pages );

const page* find_oldest_page( em::view< const page* > pages );

const page* find_next_page( em::view< const page* > pages );

const page* find_latest_page( em::view< const page* > pages );

std::tuple< bool, page > store( const payload& pl, const map* m, page buffer );

bool load( page p, em::function_view< bool( const payload& ) > pl_cb, map& m );

}  // namespace servio::cfg
