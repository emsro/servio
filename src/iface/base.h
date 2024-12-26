#pragma once

#include <cstdint>
//
#include <avakar/atom.h>
#include <vari/bits/typelist.h>

namespace servio::iface
{

template < typename T >
using opt = std::optional< T >;

using avakar::atom;
using namespace avakar::literals;

template < auto Key, typename T >
struct kval
{
        static_assert( std::three_way_comparable< T > );
        static constexpr auto key     = Key;
        using value_type              = T;
        static constexpr bool is_void = false;

        T value;

        constexpr auto operator<=>( kval const& ) const noexcept = default;
};

template < auto Key >
struct kval< Key, void >
{

        static constexpr auto key     = Key;
        using value_type              = void;
        static constexpr bool is_void = true;

        constexpr auto operator<=>( kval const& ) const noexcept = default;
};

template < auto Key, typename T >
constexpr kval< Key, T > operator|( T val, kval< Key, void > ) noexcept
{
        return { std::move( val ) };
}

namespace literals
{
template < avakar::s x >
constexpr kval< avakar::a< x >{}, void > operator""_kv() noexcept
{
        return {};
}
}  // namespace literals

template < uint32_t IDX, auto Key, typename T >
struct field
{
        static_assert( std::three_way_comparable< T > );

        static constexpr auto id  = IDX;
        static constexpr auto key = Key;
        using value_type          = T;

        using kv_type = kval< Key, value_type >;

        constexpr auto operator<=>( field const& ) const noexcept = default;
};

template < uint32_t IDX, auto Key >
struct field< IDX, Key, void >
{
        static constexpr auto id  = IDX;
        static constexpr auto key = Key;
        using value_type          = void;

        constexpr auto operator<=>( field const& ) const noexcept = default;
};

template < typename T >
struct field_tuple;

template < typename... Ts >
struct field_tuple< vari::typelist< Ts... > >
{
        using type = std::tuple< Ts... >;
};

template < typename T >
using field_tuple_t = typename field_tuple< T >::type;

template < avakar::s Desc, typename... Ts >
struct group
{
        using value_type = vari::typelist< Ts... >;
};

template < auto K, typename T >
struct key_filter
{
        static_assert( sizeof( K ) == 0, "Could not match the key" );
};

template < auto K, typename F, typename... Fs >
requires( K.to_string() == F::key.to_string() )
struct key_filter< K, vari::typelist< F, Fs... > >
{
        using type = F;
};

template < auto K, typename F, typename... Fs >
requires( K.to_string() != F::key.to_string() )
struct key_filter< K, vari::typelist< F, Fs... > > : key_filter< K, vari::typelist< Fs... > >
{
};

template < typename Cfg >
struct field_traits;

template < typename... Field >
struct field_traits< vari::typelist< Field... > >
{

        template < auto K >
        using ktof = typename key_filter< K, vari::typelist< Field... > >::type;

        using keys  = atom< Field::key... >;
        using types = vari::unique_typelist_t< vari::typelist< typename Field::value_type... > >;
        using vals  = vari::typelist< kval< Field::key, typename Field::value_type >... >;
        static constexpr uint32_t ids[] = { Field::id... };
};

}  // namespace servio::iface
