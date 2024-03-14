#pragma once

#include <emlabcpp/experimental/testing/coroutine.h>
#include <emlabcpp/experimental/testing/interface.h>
#include <emlabcpp/experimental/testing/reactor.h>

namespace em = emlabcpp;

namespace servio::drv::tests
{

namespace t = em::testing;

template < typename T >
struct utest : t::test_interface
{
        T item;

        template < typename... Args >
        utest( Args&&... args )
          : item( std::forward< Args >( args )... )
        {
        }

        std::string_view get_name() const override
        {
                return item.name;
        }

        t::coroutine< void > run( em::pmr::memory_resource& mem ) override
        {
                return item.run( mem );
        }
};

template < typename T, typename... Args >
void setup_utest( em::pmr::memory_resource& mem, t::reactor& reac, em::result& res, Args&&... args )
{
        if ( res != em::SUCCESS )
                return;
        res = t::construct_test_unit< utest< T > >( mem, reac, std::forward< Args >( args )... );
}

}  // namespace servio::drv::tests
