
#pragma once
#include "./base.hpp"

#include <emlabcpp/view.h>

namespace em = emlabcpp;

namespace servio::base
{

constexpr std::string_view fmt_arithm( em::view< char* > buff, unsigned long x )
{
        int res = snprintf( buff.begin(), buff.size(), "%lu", x );
        // XXX: if(res <0) wat now?

        std::size_t size = std::min( (std::size_t) std::max( res, 0 ), buff.size() );
        return { buff.begin(), size };
}

constexpr std::string_view fmt_arithm( em::view< char* > buff, float const& x )
{
        int res = snprintf( buff.begin(), buff.size(), "%f", x );
        // XXX: if(res <0) wat now?

        std::size_t size = std::min( (std::size_t) std::max( res, 0 ), buff.size() );
        return { buff.begin(), size };
}

}  // namespace servio::base
