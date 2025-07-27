#pragma once

#include "../base.hpp"
#include "../lib/parser.hpp"
#include "../status.hpp"

#include <emlabcpp/experimental/string_buffer.h>
#include <vari/vref.h>

namespace servio::cfg
{

namespace em = emlabcpp;
using string = em::string_buffer< 32 >;

// Set of basic configure types used for exchanging configuration data
using base_t = vari::typelist< string, float, int32_t, bool >;

template < typename T >
struct cfg_type_trait
{
        static_assert( std::same_as< T, T >, "cfg_type_trait<T> is not specialized for this type" );
};

template <>
struct cfg_type_trait< uint32_t >
{
        using base_type = int32_t;

        static opt_str_err load( uint32_t& tg, base_type& inpt )
        {
                if ( inpt < 0 )
                        return "value must be non-negative"_err;
                tg = static_cast< uint32_t >( inpt );
                return {};
        }

        static base_type store( uint32_t const& val )
        {
                return static_cast< base_type >( val );
        }
};

template < typename T >
opt_str_err load_value( T& tg, vari::vref< base_t const > expr )
{
        if constexpr ( vari::contains_type_v< T, base_t > ) {
                if ( !parser::load_value( tg, expr ) )
                        return "type mismatch"_err;
                return {};
        } else {
                using trait     = cfg_type_trait< T >;
                using base_type = typename trait::base_type;
                base_type val;
                if ( auto e = load_value( val, expr ) )
                        return e;
                return trait::load( tg, val );
        }
}

template < typename T >
vari::vval< base_t > store_value( T val )
{
        if constexpr ( vari::contains_type_v< T, base_t > ) {
                return std::move( val );
        } else {
                using trait = cfg_type_trait< T >;
                return trait::store( val );
        }
}

struct iface
{
        virtual status on_storage_load( uint32_t id, std::span< std::byte const > data ) = 0;
        virtual opt< std::span< std::byte const > >
                     on_storage_save( uint32_t id, std::span< std::byte > buffer )             = 0;
        virtual bool on_storage_change_check( uint32_t id, std::span< std::byte const > data ) = 0;
};

}  // namespace servio::cfg
