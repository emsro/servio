#pragma once

#include "./def.hpp"

namespace servio::iface
{

template < typename T >
struct kvals_parse;

template < typename... Ts >
struct kvals_parse< vari::typelist< Ts... > >
{
        using kvals = vari::typelist< Ts... >;

        static vari::vopt< kvals > parse( parser::parser& p ) noexcept
        {
                auto id = p.exp( "id"_a );

                vari::vopt< kvals > res;
                auto                f = [&]< typename KV >() {
                        if ( *id != KV::key.to_string() )
                                return false;
                        auto val = parser::parse_trait< typename KV::value_type >::parse( p );
                        if ( !val )
                                return true;

                        res = KV{ .value = std::move( *val ) };
                        return true;
                };
                ( f.template operator()< Ts >() || ... );
                return res;
        }
};
}  // namespace servio::iface
