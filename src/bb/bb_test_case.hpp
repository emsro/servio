#include "scmdio/cli.hpp"

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/serial_port.hpp>
#include <chrono>
#include <filesystem>
#include <gtest/gtest.h>

#pragma once

namespace servio::bb
{

using test_signature =
    boost::asio::awaitable< void >( boost::asio::io_context&, scmdio::cobs_port& );

struct bb_test_case : ::testing::Test
{
        bb_test_case(
            scmdio::common_cli&             cli,
            std::function< test_signature > test,
            std::chrono::milliseconds       timeout )
          : cli( cli )
          , test( std::move( test ) )
          , timeout( timeout )
        {
        }

        void TestBody() final
        {
                cli.context.restart();
                std::exception_ptr excep_ptr;
                bool               finished = false;
                co_spawn(
                    cli.context,
                    [&]() -> boost::asio::awaitable< void > {
                            co_await test( cli.context, *cli.port_ptr );
                            finished = true;
                    },
                    [&]( std::exception_ptr ptr ) {
                            excep_ptr = std::move( ptr );
                    } );
                cli.context.run_for( timeout );

                if ( excep_ptr )
                        std::rethrow_exception( excep_ptr );
                EXPECT_TRUE( finished )
                    << "Test failed to finish in time, timeout: " << timeout.count() << "s";
        }

        scmdio::common_cli&             cli;
        std::function< test_signature > test;
        std::chrono::milliseconds       timeout;
};

inline void register_test(
    const std::string&              fixture_name,
    const std::string&              name,
    scmdio::common_cli&             cli,
    std::function< test_signature > test,
    std::chrono::milliseconds       timeout )
{
        ::testing::RegisterTest(
            fixture_name.c_str(),
            name.c_str(),
            nullptr,
            nullptr,
            __FILE__,
            __LINE__,
            [&cli, test = std::move( test ), timeout] {
                    return new bb_test_case( cli, test, timeout );
            } );
}

}  // namespace servio::bb
