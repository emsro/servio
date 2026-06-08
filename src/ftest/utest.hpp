#pragma once

#include "../base.hpp"
#include "../drv/interfaces.hpp"
#include "../gov/governor_manager.hpp"
#include "../status.hpp"

#include <asrtrpp/collect.hpp>
#include <asrtrpp/diag.hpp>
#include <asrtrpp/param.hpp>
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
template < typename T >
using task = asrt::task< T >;

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

struct _init_utest_ctx
{
        using completion_signatures =
            ecor::completion_signatures< ecor::set_value_t(), ecor::set_error_t( asrt::status ) >;

        utest* pctx;

        template < typename OP >
        void start( OP& op )
        {
                asrt_flat_value const array_value = {
                    .type = asrt::collect_append_traits< asrt::arr >::flat_type,
                };

                auto s = asrt_collect_client_insert(
                    &pctx->assm.collect,
                    0,
                    "metrics",
                    &array_value,
                    &pctx->met_id,
                    +[]( void* ptr, asrt::status st ) {
                            auto& o = *static_cast< OP* >( ptr );
                            if ( st != ASRT_SUCCESS )
                                    o.receiver.set_error( st );
                            else
                                    o.receiver.set_value();
                    },
                    &op );
                if ( s != ASRT_SUCCESS )
                        op.receiver.set_error( static_cast< asrt::status >( s ) );
        }
};

inline ecor::sender_from< _init_utest_ctx > init_utest( utest& ctx )
{
        return { { &ctx } };
}

struct _wait_for_sender
{
        using sender_concept = ecor::sender_t;

        using completion_signatures = ecor::completion_signatures< ecor::set_value_t() >;

        utest*          pctx;
        drv::clk_iface* pclk;
        microseconds    duration;

        template < ecor::receiver R >
        struct _op : ecor::schedulable
        {
                using operation_state_concept = ecor::operation_state_t;

                R                receiver;
                ecor::task_core* core;
                drv::clk_iface*  clk;
                microseconds     end;

                _op( R recv, utest& ctx, drv::clk_iface& clock, microseconds dur )
                  : receiver( std::move( recv ) )
                  , core( &ctx.query( ecor::get_task_core ) )
                  , clk( &clock )
                  , end( clock.get_us() + dur )
                {
                }

                void start()
                {
                        core->reschedule( *this );
                }

                void resume() override
                {
                        if ( clk->get_us() < end )
                                core->reschedule( *this );
                        else
                                receiver.set_value();
                }
        };

        template < ecor::receiver R >
        auto connect( R receiver ) &&
        {
                static_assert(
                    ecor::receiver_for< R, _wait_for_sender >,
                    "Receiver does not satisfy the requirements for wait_for's completion signatures" );
                return _op< R >{ std::move( receiver ), *pctx, *pclk, duration };
        }
};

inline _wait_for_sender wait_for( utest& ctx, drv::clk_iface& clk, microseconds duration )
{
        return { &ctx, &clk, duration };
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

/// store metric in collect
///
/// Equivalent of following coroutine:
/// template < typename T >
/// task< void > _store_metric( utest& ctx, char const* name, T&& value, char const* unit )
/// {
///         auto id = co_await asrt::append< asrt::obj >( ctx.assm.collect, ctx.met_id );
///         co_await asrt::set( ctx.assm.collect, id, "name", name );
///         co_await asrt::set( ctx.assm.collect, id, "unit", unit );
///         co_await asrt::set( ctx.assm.collect, id, "value", value );
/// }
struct _store_metric_ctx
{
        using completion_signatures =
            ecor::completion_signatures< ecor::set_value_t(), ecor::set_error_t( asrt::status ) >;

        asrt_collect_client* client;
        asrt::flat_id        parent;
        char const*          name;
        char const*          unit;
        asrt_flat_value      metric_value;

        template < typename OP >
        void start( OP& op )
        {
                asrt_flat_value const object_value = {
                    .type = asrt::collect_append_traits< asrt::obj >::flat_type,
                };
                auto s = asrt_collect_client_insert(
                    client,
                    parent,
                    nullptr,
                    &object_value,
                    &parent,
                    +[]( void* ptr, asrt::status st ) {
                            auto& o = *static_cast< OP* >( ptr );
                            if ( st != ASRT_SUCCESS )
                                    o.receiver.set_error( st );
                            else
                                    o.ctx._start_name( o );
                    },
                    &op );
                if ( s != ASRT_SUCCESS )
                        op.receiver.set_error( static_cast< asrt::status >( s ) );
        }

        template < typename OP >
        void _start_name( OP& op )
        {
                asrt_flat_value const name_value = {
                    .type = asrt::collect_append_traits< char const* >::flat_type,
                    .data = { .s = { .str_val = name } },
                };
                auto s = asrt_collect_client_insert(
                    client,
                    parent,
                    "name",
                    &name_value,
                    nullptr,
                    +[]( void* ptr, asrt::status st ) {
                            auto& o = *static_cast< OP* >( ptr );
                            if ( st != ASRT_SUCCESS )
                                    o.receiver.set_error( st );
                            else
                                    o.ctx._start_unit( o );
                    },
                    &op );
                if ( s != ASRT_SUCCESS )
                        op.receiver.set_error( static_cast< asrt::status >( s ) );
        }

        template < typename OP >
        void _start_unit( OP& op )
        {
                asrt_flat_value const unit_value = {
                    .type = asrt::collect_append_traits< char const* >::flat_type,
                    .data = { .s = { .str_val = unit } },
                };
                auto s = asrt_collect_client_insert(
                    client,
                    parent,
                    "unit",
                    &unit_value,
                    nullptr,
                    +[]( void* ptr, asrt::status st ) {
                            auto& o = *static_cast< OP* >( ptr );
                            if ( st != ASRT_SUCCESS )
                                    o.receiver.set_error( st );
                            else
                                    o.ctx._start_value( o );
                    },
                    &op );
                if ( s != ASRT_SUCCESS )
                        op.receiver.set_error( static_cast< asrt::status >( s ) );
        }

        template < typename OP >
        void _start_value( OP& op )
        {
                auto s = asrt_collect_client_insert(
                    client,
                    parent,
                    "value",
                    &metric_value,
                    nullptr,
                    +[]( void* ptr, asrt::status st ) {
                            auto& o = *static_cast< OP* >( ptr );
                            if ( st != ASRT_SUCCESS )
                                    o.receiver.set_error( st );
                            else
                                    o.receiver.set_value();
                    },
                    &op );
                if ( s != ASRT_SUCCESS )
                        op.receiver.set_error( static_cast< asrt::status >( s ) );
        }
};

using _store_metric_sender = ecor::sender_from< _store_metric_ctx >;

template < typename T >
asrt_flat_value _make_metric_value( T val ) noexcept
{
        using traits      = asrt::collect_append_traits< T >;
        using member_type = decltype( asrt_flat_scalar{}.*traits::member );

        asrt_flat_value v        = { .type = traits::flat_type };
        v.data.s.*traits::member = static_cast< member_type >( val );
        return v;
}

template < typename T >
ecor::sender auto store_metric( utest& ctx, char const* name, T&& value, char const* unit = "" )
{
        auto converted = _metric_conv< std::remove_cvref_t< T > >::convert( value );
        return _store_metric_sender{ {
            .client       = &ctx.assm.collect,
            .parent       = ctx.met_id,
            .name         = name,
            .unit         = unit,
            .metric_value = _make_metric_value( converted ),
        } };
}

struct _expect_ctx
{
        using completion_signatures =
            ecor::completion_signatures< ecor::set_value_t(), ecor::set_error_t( asrt::status ) >;

        utest*               pctx;
        bool                 condition;
        std::source_location loc;

        template < typename OP >
        void start( OP& op )
        {
                if ( condition ) {
                        op.receiver.set_value();
                        return;
                }
                asrt_diag_client_record(
                    &pctx->assm.diag,
                    loc.file_name(),
                    static_cast< uint32_t >( loc.line() ),
                    nullptr,
                    +[]( void* p, enum asrt_status s ) {
                            auto& o = *static_cast< OP* >( p );
                            if ( s == ASRT_SUCCESS )
                                    o.receiver.set_error( ASRT_FAILURE );
                            else
                                    o.receiver.set_error( s );
                    },
                    &op );
        }
};

inline ecor::sender_from< _expect_ctx >
expect( utest& ctx, bool condition, std::source_location loc = std::source_location::current() )
{
        return { { &ctx, condition, loc } };
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
