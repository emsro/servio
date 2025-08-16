
#pragma once

#include "../iface/base.hpp"
#include "./base.hpp"

#include <emlabcpp/experimental/cfg/handler.h>
#include <emlabcpp/experimental/function_view.h>
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

struct cmd_iface
{

        template < typename T >
        static opt< typename T::key_type > on_cmd_set(
            T&                                      map,
            std::string_view                        key,
            vari::vref< servio::cfg::base_t const > value,
            servio::iface::root_ser                 out )
        {
                auto k_opt = T::str_to_key( key );
                if ( !k_opt ) {
                        unknown_key_branch( std::move( out ) );
                        return {};
                }
                map.ref_by_key( *k_opt ).visit(
                    [&]( vari::empty_t ) {
                            internal_error_branch( std::move( out ) );
                    },
                    [&]( auto& x ) {
                            cmd_set_load_val( x, value, std::move( out ) );
                    } );
                return k_opt;
        }

        template < typename T >
        static void on_cmd_list( int32_t offset, servio::iface::root_ser out )
        {
                auto f = []( uint32_t k ) -> std::string_view {
                        return to_str( T::keys[k] );
                };
                return on_cmd_list( offset, T::keys.size(), f, std::move( out ) );
        }

        template < typename T >
        static void on_cmd_get( T& map, std::string_view key, servio::iface::root_ser out )
        {
                auto k_opt = T::str_to_key( key );
                if ( !k_opt )
                        return unknown_key_branch( std::move( out ) );

                map.ref_by_key( *k_opt ).visit(
                    [&]( vari::empty_t ) {
                            return internal_error_branch( std::move( out ) );
                    },
                    [&]( auto& x ) {
                            cmd_get_store_val( x, std::move( out ) );
                    } );
        }

private:
        static void unknown_key_branch( servio::iface::root_ser out )
        {
                std::move( out ).nok()( "unknown key" );
        }

        static void internal_error_branch( servio::iface::root_ser out )
        {
                std::move( out ).nok()( "internal error" );
        }

        static void cmd_set_load_val(
            auto&                                   x,
            vari::vref< servio::cfg::base_t const > value,
            servio::iface::root_ser                 out )
        {
                auto opt_e = servio::cfg::load_value( x, value );
                if ( opt_e )
                        std::move( out ).nok()( opt_e.error );
                else
                        std::move( out ).ok();
        }

        static void cmd_get_store_val( auto const& x, servio::iface::root_ser out )
        {
                auto as = std::move( out ).ok();
                servio::cfg::store_value( x ).visit( as );
        }

        static void on_cmd_list(
            int32_t                                           offset,
            std::size_t                                       size,
            em::function_view< std::string_view( uint32_t ) > key_to_str,
            servio::iface::root_ser                           out )
        {
                if ( offset < 0 ) {
                        std::move( out ).nok()( "negative offset" );
                        return;
                }
                auto as  = std::move( out ).ok();
                auto sub = as.sub();

                if ( offset >= (int32_t) size )
                        return;
                sub( key_to_str( (uint32_t) offset ) );
        }
};

}  // namespace servio::cfg
