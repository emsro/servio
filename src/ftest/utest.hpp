#pragma once

#include "../base.hpp"
#include "../ctl/control.hpp"
#include "../status.hpp"

#include <emlabcpp/defer.h>
#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/coroutine.h>
#include <emlabcpp/experimental/testing/interface.h>
#include <emlabcpp/experimental/testing/parameters.h>
#include <emlabcpp/experimental/testing/reactor.h>

namespace em = emlabcpp;

namespace servio::ftest
{

namespace t = em::testing;

struct uctx
{
        t::collector& coll;
        t::node_id    met_id;  // TODO: this prevents pararell execution

        em::function_view< em::result( std::span< std::byte const > ) > record;

        uctx(
            t::collector&                                                   coll,
            em::function_view< em::result( std::span< std::byte const > ) > record )
          : coll( coll )
          , record( std::move( record ) )
        {
        }

        auto expect( bool expr, std::source_location sl = std::source_location::current() )
        {
                return t::expect( coll, expr, sl );
        }
};

struct utest_base : t::test_interface
{
        uctx& ctx;

        utest_base( uctx& ctx )
          : ctx( ctx )
        {
        }

        t::coroutine< void > setup( em::pmr::memory_resource& ) override
        {
                ctx.met_id =
                    co_await ctx.coll.set( "metrics", em::contiguous_container_type::ARRAY );
        }
};

t::coroutine< void > store_metric(
    em::pmr::memory_resource&,
    uctx&            ctx,
    std::string_view name,
    auto&&           value,
    std::string_view unit = "" )

{
        em::testing::node_id id =
            co_await ctx.coll.append( ctx.met_id, em::contiguous_container_type::OBJECT );
        ctx.coll.set( id, "name", name );
        ctx.coll.set( id, "unit", unit );
        ctx.coll.set( id, "value", value );
}

t::coroutine< void >
store_data( em::pmr::memory_resource&, uctx& ctx, std::string_view key, auto&& data )
{
        auto id = co_await ctx.coll.set( key, em::contiguous_container_type::ARRAY );
        for ( auto b : data )
                ctx.coll.append( id, b );
}

inline auto setup_poweroff( ctl::control& ctl )
{
        return em::defer{ [&] {
                ctl.switch_to_power_control( pwr{ 0 } );
        } };
}

template < typename T >
struct utest : utest_base

{
        T item;

        template < typename... Args >
        utest( uctx& ctx, Args&&... args )
          : utest_base( ctx )
          , item( std::forward< Args >( args )... )
        {
        }

        std::string_view get_name() const override
        {
                return item.name;
        }

        t::coroutine< void > run( em::pmr::memory_resource& mem ) override
        {
                return item.run( mem, ctx );
        }
};

template < typename T, typename... Args >
void setup_utest(
    em::pmr::memory_resource& mem,
    t::reactor&               r,
    uctx&                     ctx,
    status&                   res,
    Args&&... args )
{
        if ( res != SUCCESS )
                return;
        res = t::construct_test_unit< utest< T > >( mem, r, ctx, std::forward< Args >( args )... );
}

}  // namespace servio::ftest
