
#include <cstdint>
//
#include "./base.hpp"

#include <charconv>
#include <nlohmann/json.hpp>
#include <vari/vopt.h>

#pragma once

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
struct val_ser
{
        static T ser( std::string_view val )
        {
                if constexpr ( std::same_as< T, std::string_view > )
                        return val;
                else if constexpr ( std::same_as< T, float > )
                        return std::stof( std::string{ val } );
                else if constexpr ( std::same_as< T, bool > ) {
                        if ( val == "true" )
                                return true;
                        if ( val == "false" )
                                return false;
                        log_error( "Invalid bool value: ", val );
                } else if constexpr ( std::is_integral_v< T > ) {
                        T ret;
                        std::from_chars( val.data(), val.data() + val.size(), ret );
                        return ret;
                } else
                        static_assert( std::is_same_v< T, void >, "unsupported type" );
        }
};

template < auto... Ts >
struct val_ser< avakar::atom< Ts... > >
{
        static avakar::atom< Ts... > ser( std::string_view val )
        {
                auto x = avakar::atom< Ts... >::from_string( val );
                if ( x )
                        return *x;
                log_error( "Failed to parse atom: ", val );
        }
};

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
                        return KV{ val_ser< typename KV::value_type >::ser( value ) };
                else
                        return kval_ser< vari::typelist< KVs... > >::ser( name, value );
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
