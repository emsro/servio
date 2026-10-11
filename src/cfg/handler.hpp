
#pragma once

#include "../base.hpp"
#include "../iface/base.hpp"
#include "./base.hpp"

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <emlabcpp/experimental/cfg/handler.hpp>
#include <emlabcpp/experimental/function_view.hpp>
#include <emlabcpp/static_function.hpp>
#include <span>

namespace servio::cfg
{

template < typename T >
struct handler : iface
{
        using key_type = typename T::key_type;

        T&                                    _x;
        em::function_view< void( key_type ) > _on_cmd_set;
        std::bitset< T::ids.size() >          _seen;

        handler( T& x, em::function_view< void( key_type ) > on_cmd_set )
          : _x( x )
          , _on_cmd_set( on_cmd_set )
        {
        }

        opt_str_err
        on_cmd_set( std::string_view key, vari::vref< servio::cfg::base_t const > value ) override
        {
                auto k_opt = T::str_to_key( key );
                if ( !k_opt )
                        return "unknown key"_err;
                auto opt_e = _x.ref_by_key( *k_opt ).visit(
                    [&]( vari::empty_t ) -> opt_str_err {
                            return "internal error"_err;
                    },
                    [&]( auto& x ) -> opt_str_err {
                            auto opt_e = servio::cfg::load_value( x, value );
                            if ( opt_e )
                                    return opt_e;
                            return {};
                    } );
                if ( !opt_e )
                        _on_cmd_set( *k_opt );
                return opt_e;
        }

        std::string_view on_cmd_list( int32_t offset ) override
        {
                if ( offset < 0 )
                        return {};
                if ( offset >= (int32_t) T::keys.size() )
                        return {};
                return to_str( T::keys[(uint32_t) offset] );
        }

        vari::vval< base_t, str_err > on_cmd_get( std::string_view key ) override
        {
                auto k_opt = T::str_to_key( key );
                if ( !k_opt )
                        return "unknown key"_err;

                using R = vari::vval< base_t, str_err >;
                return _x.ref_by_key( *k_opt ).visit(
                    [&]( vari::empty_t ) -> R {
                            return "internal_error"_err;
                    },
                    [&]( auto& x ) -> R {
                            return servio::cfg::store_value( x );
                    } );
        }

        status on_storage_load( uint32_t id, std::span< std::byte const > data ) override
        {
                auto const ptr = _x.ref_by_id( id );
                if ( !ptr )
                        return status::error;
                return ptr.vref().visit( [&]< typename U >( U& val ) -> status {
                        auto newval = em::cfg::get_val< U >( data );
                        if ( newval ) {
                                val = *newval;
                                return status::success;
                        }
                        return status::error;
                } );
        }

        opt< std::span< std::byte const > >
        on_storage_save( uint32_t id, std::span< std::byte > buffer ) override
        {
                opt< std::span< std::byte > > res;
                auto const                    ptr = _x.ref_by_id( id );
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
                auto const ptr = _x.ref_by_id( id );
                if ( !ptr )  // XXX: maybe rethink?
                        return false;
                return ptr.vref().visit( [&]< typename U >( U const& val ) {
                        auto newval = em::cfg::get_val< U >( data );
                        if ( newval )
                                return val != *newval;
                        return false;
                } );
        }

        void set_apply( em::function_view< void( key_type ) > f )
        {
                _on_cmd_set = f;
        }

        void storage_reset_seen() override
        {
                _seen.reset();
        }

        bool storage_mark_seen( uint32_t id ) override
        {
                auto const it = std::ranges::find( T::ids, id );
                if ( it == T::ids.end() )
                        return false;
                auto const i = static_cast< std::size_t >( it - T::ids.begin() );
                if ( _seen[i] )
                        return false;
                _seen[i] = true;
                return true;
        }

        opt< uint32_t > storage_take_unseen() override
        {
                for ( std::size_t i = 0; i < T::ids.size(); ++i ) {
                        if ( _seen[i] )
                                continue;
                        _seen[i] = true;
                        return T::ids[i];
                }
                return {};
        }

        void apply_all() override
        {
                for ( key_type const k : T::keys )
                        _on_cmd_set( k );
        }
};

}  // namespace servio::cfg
