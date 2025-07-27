
#pragma once

#include "./base.hpp"

#include <emlabcpp/experimental/cfg/handler.h>
#include <emlabcpp/static_function.h>

namespace servio::cfg
{

template < typename T >
struct handler : iface
{
        T& _x;

        handler( T& x )
          : _x( x )
        {
        }

        status on_storage_load( uint32_t id, std::span< std::byte const > data ) override
        {
                auto ptr = _x.ref_by_id( id );
                if ( !ptr )
                        return ERROR;
                return ptr.vref().visit( [&]< typename U >( U& val ) -> status {
                        auto newval = em::cfg::get_val< U >( data );
                        if ( newval ) {
                                val = *newval;
                                return SUCCESS;
                        }
                        return ERROR;
                } );
        }

        opt< std::span< std::byte const > >
        on_storage_save( uint32_t id, std::span< std::byte > buffer ) override
        {
                opt< std::span< std::byte > > res;
                auto                          ptr = _x.ref_by_id( id );
                if ( !ptr )
                        return res;
                ptr.vref().visit( [&]< typename U >( U const& val ) {
                        auto ran = em::cfg::store_val( val, buffer );
                        if ( ran )
                                res = *ran;
                } );
                return res;
        }

        bool on_storage_change_check( uint32_t id, std::span< std::byte const > data ) override
        {
                auto ptr = _x.ref_by_id( id );
                if ( !ptr )  // XXX: maybe rethink?
                        return false;
                return ptr.vref().visit( [&]< typename U >( U const& val ) {
                        auto newval = em::cfg::get_val< U >( data );
                        if ( newval )
                                return val != *newval;
                        return false;
                } );
        }
};

template < typename T >
vari::vval< typename T::key_type, str_err >
on_cmd_set( T& map, std::string_view key, vari::vref< servio::cfg::base_t const > value )
{
        auto k_opt = T::str_to_key( key );
        if ( !k_opt )
                return "unknown key"_err;
        using R = opt_str_err;
        R tmp   = map.ref_by_key( *k_opt ).visit(
            [&]( vari::empty_t ) -> R {
                    return {};
            },
            [&]( auto& x ) -> R {
                    return servio::cfg::load_value( x, value );
            } );
        if ( tmp )
                return (str_err) tmp;
        return *k_opt;
}

template < typename T >
vari::vval< servio::cfg::base_t, str_err > on_cmd_get( T& map, std::string_view key )
{
        auto k_opt = T::str_to_key( key );
        if ( !k_opt )
                return "unknown key"_err;
        using R = vari::vval< servio::cfg::base_t, str_err >;
        return map.ref_by_key( *k_opt ).visit(
            [&]( vari::empty_t ) -> R {
                    return "internal error"_err;
            },
            [&]( auto& x ) -> R {
                    return servio::cfg::store_value( x );
            } );
}

}  // namespace servio::cfg
