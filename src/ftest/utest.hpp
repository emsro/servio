#pragma once

#include "base/base.hpp"
#include "ctl/control.hpp"

#include <emlabcpp/defer.h>
#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/coroutine.h>
#include <emlabcpp/experimental/testing/interface.h>
#include <emlabcpp/experimental/testing/reactor.h>

namespace em = emlabcpp;

namespace servio::ftest
{

namespace t = em::testing;
using namespace base::literals;

struct collector
{
        t::collector& coll;
        t::node_id    met_id;

        collector( t::collector& coll )
          : coll( coll )
        {
        }

        operator t::collector&()
        {
                return coll;
        }

        t::collector* operator->()
        {
                return &coll;
        }
};

struct utest_base : t::test_interface
{
        collector coll;

        utest_base( t::collector& coll )
          : coll( coll )
        {
        }

        t::coroutine< void > setup( em::pmr::memory_resource& ) override
        {
                coll.met_id = co_await coll->set( "metrics", em::contiguous_container_type::ARRAY );
        }
};

t::coroutine< void > store_metric(
    em::pmr::memory_resource&,
    collector&       coll,
    std::string_view name,
    auto&&           value,
    std::string_view unit = "" )

{
        em::testing::node_id id =
            co_await coll->append( coll.met_id, em::contiguous_container_type::OBJECT );
        coll->set( id, "name", name );
        coll->set( id, "unit", unit );
        coll->set( id, "value", value );
}

t::coroutine< void >
store_data( em::pmr::memory_resource&, collector& coll, std::string_view key, auto&& data )
{
        auto id = co_await coll->set( key, em::contiguous_container_type::ARRAY );
        for ( auto b : data )
                coll->append( id, b );
}

inline auto setup_poweroff( ctl::control& ctl )
{
        return em::defer{ [&] {
                ctl.switch_to_power_control( 0 );
        } };
}

template < typename T >
struct utest : utest_base

{
        T item;

        template < typename... Args >
        utest( t::collector& coll, Args&&... args )
          : utest_base( coll )
          , item( std::forward< Args >( args )... )
        {
        }

        std::string_view get_name() const override
        {
                return item.name;
        }

        t::coroutine< void > run( em::pmr::memory_resource& mem ) override
        {
                return item.run( mem, coll );
        }
};

template < typename T, typename... Args >
void setup_utest(
    em::pmr::memory_resource& mem,
    t::reactor&               reac,
    t::collector&             coll,
    em::result&               res,
    Args&&... args )
{
        if ( res != em::SUCCESS )
                return;
        res = t::construct_test_unit< utest< T > >(
            mem, reac, coll, std::forward< Args >( args )... );
}

}  // namespace servio::ftest
