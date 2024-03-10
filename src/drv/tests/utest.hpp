#pragma once

#include <emlabcpp/experimental/testing/coroutine.h>
#include <emlabcpp/experimental/testing/interface.h>
#include <emlabcpp/experimental/testing/reactor.h>

namespace em = emlabcpp;

namespace servio::drv::tests
{

template < typename T >
struct utest : em::testing::test_interface
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

        em::testing::test_coroutine run( em::pmr::memory_resource& mem, em::testing::record& rec )
        {
                return item.run( mem, rec );
        }
};

template < typename T, typename... Args >
void setup_utest(
    em::pmr::memory_resource& mem,
    em::testing::reactor&     reac,
    em::result&               res,
    Args&&... args )
{
        if ( res != em::SUCCESS )
                return;
        res = em::testing::construct_test_unit< utest< T > >(
            mem, reac, std::forward< Args >( args )... );
}

}  // namespace servio::drv::tests
