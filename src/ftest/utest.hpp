#pragma once

#include "../base.hpp"
#include "../gov/governor_manager.hpp"
#include "../status.hpp"

#include <asrtrpp/collect.hpp>
#include <asrtrpp/diag.hpp>
#include <asrtrpp/reac_assm.hpp>
#include <asrtrpp/stream.hpp>
#include <asrtrpp/task_unit.hpp>
#include <bitset>
#include <emlabcpp/defer.h>
#include <source_location>
#include <string_view>
#include <type_traits>

namespace em = emlabcpp;

namespace servio::ftest
{

struct utest : asrt::task_test
{
        asrt_reac_assm& assm;
        asrt::flat_id   met_id;

        utest( asrt::task_ctx& ctx, asrt_reac_assm& assm )
          : asrt::task_test( ctx )
          , assm( assm )
        {
        }
};

inline asrt::task< void > init_utest( utest& ctx )
{
        ctx.met_id = co_await asrt::set< asrt::arr >( ctx.assm.collect, 0, "metrics" );
}

template < typename T >
struct _metric_conv
{
        static_assert( sizeof( T ) > 0 && false, "unsupported metric value type" );
};

template < typename T >
requires( std::is_integral_v< T > && std::is_unsigned_v< T > && sizeof( T ) <= 4 )
struct _metric_conv< T >
{
        static uint32_t convert( T v ) noexcept
        {
                return static_cast< uint32_t >( v );
        }
};

template < typename T >
requires( std::is_integral_v< T > && std::is_signed_v< T > && sizeof( T ) <= 4 )
struct _metric_conv< T >
{
        static int32_t convert( T v ) noexcept
        {
                return static_cast< int32_t >( v );
        }
};

template <>
struct _metric_conv< float >
{
        static float convert( float v ) noexcept
        {
                return v;
        }
};

template <>
struct _metric_conv< bool >
{
        static bool convert( bool v ) noexcept
        {
                return v;
        }
};

template <>
struct _metric_conv< char const* >
{
        static char const* convert( char const* v ) noexcept
        {
                return v;
        }
};

template <>
struct _metric_conv< uint64_t >
{
        static asrt::u32d2 convert( uint64_t v ) noexcept
        {
                return {
                    .hi = static_cast< uint32_t >( v >> 32U ),
                    .lo = static_cast< uint32_t >( v & 0xFFFF'FFFFU ) };
        }
};

template <>
struct _metric_conv< int64_t >
{
        static asrt::u32d2 convert( int64_t v ) noexcept
        {
                return _metric_conv< uint64_t >::convert( static_cast< uint64_t >( v ) );
        }
};

template <>
struct _metric_conv< std::string_view >
{
        static char const* convert( std::string_view v ) noexcept
        {
                return v.data();
        }
};

template <>
struct _metric_conv< std::string >
{
        static char const* convert( std::string const& v ) noexcept
        {
                return v.c_str();
        }
};

template < typename Rep, typename Period >
struct _metric_conv< std::chrono::duration< Rep, Period > >
{
        static auto convert( std::chrono::duration< Rep, Period > v ) noexcept
        {
                return _metric_conv< Rep >::convert( v.count() );
        }
};

template < typename T >
requires std::is_enum_v< T >
struct _metric_conv< T >
{
        static auto convert( T v ) noexcept( noexcept( to_str( v ) ) )
        {
                return _metric_conv< std::remove_cvref_t< decltype( to_str( v ) ) > >::convert(
                    to_str( v ) );
        }
};

template < std::size_t N >
struct _metric_conv< std::bitset< N > >
{
        static uint32_t convert( std::bitset< N > v ) noexcept
        {
                static_assert( N <= 32, "std::bitset too wide for metric (max 32 bits)" );
                return static_cast< uint32_t >( v.to_ulong() );
        }
};

template < typename T >
asrt::task< void > store_metric( utest& ctx, char const* name, T&& value, char const* unit = "" )
{
        auto converted = _metric_conv< std::remove_cvref_t< T > >::convert( value );
        auto id        = co_await asrt::append< asrt::obj >( ctx.assm.collect, ctx.met_id );
        co_await asrt::set( ctx.assm.collect, id, "name", name );
        co_await asrt::set( ctx.assm.collect, id, "unit", unit );
        co_await asrt::set( ctx.assm.collect, id, "value", converted );
}

inline asrt::task< void >
expect( utest& ctx, bool condition, std::source_location loc = std::source_location::current() )
{
        if ( !condition ) {
                co_await asrt::rec_diag( ctx.assm.diag, loc.file_name(), loc.line(), nullptr );
                co_yield asrt::with_error{ asrt::test_fail };
        }
}

inline auto setup_poweroff( gov::governor_manager& gv )
{
        return em::defer{ [&] {
                // XXX: maybe improve error handling?
                std::ignore = gv.deactivate();
        } };
}

template < typename T >
void setup_utest( asrt::task_ctx& ctx, asrt_reac_assm& assm, status& res, T def )
{
        if ( res != status::success )
                return;

        using U = asrt::task_unit< T >;
        auto& a = ctx.query( ecor::get_memory_resource );
        void* p = a.allocate( sizeof( U ), alignof( U ) );
        if ( !p ) {
                res = status::error;
                return;
        }

        auto& test = *new ( p ) U{ T{ std::move( def ) } };
        if ( asrt::add_test( assm.reactor, test ) != ASRT_SUCCESS ) {
                res = status::error;
                return;
        }
}

}  // namespace servio::ftest
