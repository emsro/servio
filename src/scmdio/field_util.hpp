
#include <avakar/atom.h>
#include <format>
#include <nlohmann/json.hpp>
#include <vari/vopt.h>

#pragma once

template < avakar::atom_literal A >
struct std::formatter< A, char >
{
        template < typename ParseContext >
        constexpr ParseContext::iterator parse( ParseContext& ctx )
        {
                return ctx.begin();
        }

        template < class FmtContext >
        FmtContext::iterator format( A const&, FmtContext& ctx ) const
        {
                return std::formatter< std::string_view, char >{}.format( A::to_string(), ctx );
        }
};

template < auto... As >
struct std::formatter< avakar::atom< As... >, char >
{
        template < typename ParseContext >
        constexpr ParseContext::iterator parse( ParseContext& ctx )
        {
                return ctx.begin();
        }

        template < class FmtContext >
        FmtContext::iterator format( avakar::atom< As... > const& s, FmtContext& ctx ) const
        {
                return std::formatter< std::string_view, char >{}.format( s.to_string(), ctx );
        }
};

template < auto... Ts >
struct nlohmann::adl_serializer< avakar::atom< Ts... > >
{
        static avakar::atom< Ts... > from_json( nlohmann::json const& j )
        {
                std::string s = j;
                // XXX: better error handling
                return avakar::atom< Ts... >::from_string( s ).value();
        }
};

template < typename... Ts >
struct nlohmann::adl_serializer< vari::_vval< Ts... > >
{
        static vari::_vval< Ts... > from_json( nlohmann::json const& )
        {
                throw "XXX";
        }
};

namespace servio::scmdio
{

template < typename T >
struct kval_ser;

template <>
struct kval_ser< vari::typelist<> >
{
        static vari::vopt< vari::typelist<> > ser( std::string_view, std::string_view )
        {
                return {};
        }

        static vari::vopt< vari::typelist<> > from_json( auto&, nlohmann::json const& )
        {
                return {};
        }
};

template < typename KV, typename... KVs >
struct kval_ser< vari::typelist< KV, KVs... > >
{

        // XXX: hate this, can't pass anything - does not have to match
        template < typename... Ts >
        static vari::vopt< KV, KVs... > from_val( std::string_view name, vari::_vptr< Ts... > x )
        {
                auto k = KV::key_type::from_string( name );
                if ( !k )
                        return kval_ser< vari::typelist< KVs... > >::from_val( name, x );
                return x.visit(
                    [&]( vari::empty_t ) -> vari::vopt< KV, KVs... > {
                            if ( std::same_as< typename KV::value_type, void > )
                                    return KV{ *k };
                            return {};
                    },
                    [&]< typename U >( U& thing ) -> vari::vopt< KV, KVs... > {
                            if ( std::same_as< U, typename KV::value_type > )
                                    return KV{ *k, thing };
                            return {};
                    } );
        }

        static vari::vopt< KV, KVs... > ser( std::string_view name, std::string_view value )
        {
                if ( KV::key.to_string() == name )
                        return kval_ser< vari::typelist< KVs... > >::ser( name, value );
                return {};  // XXX: finish
        }

        static vari::vopt< KV, KVs... > from_json( std::string_view name, nlohmann::json const& j )
        {
                if ( KV::key.to_string() == name )
                        return KV{ j.get< typename KV::value_type >() };
                else
                        return kval_ser< vari::typelist< KVs... > >::from_json( name, j );
        }
};

}  // namespace servio::scmdio
