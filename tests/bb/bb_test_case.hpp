#include "host/cli.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <filesystem>
#include <gtest/gtest.h>

#pragma once

namespace tests::bb
{

using test_signature = boost::asio::awaitable< void >( boost::asio::io_context&, host::cobs_port& );

struct bb_test_case : ::testing::Test
{
        bb_test_case( host::common_cli& cli, std::function< test_signature > test )
          : cli( cli )
          , test( std::move( test ) )
        {
        }

        void TestBody() final
        {
                cli.context.restart();
                std::exception_ptr excep_ptr;
                co_spawn(
                    cli.context, test( cli.context, *cli.port_ptr ), [&]( std::exception_ptr ptr ) {
                            excep_ptr = std::move( ptr );
                    } );
                cli.context.run();

                if ( excep_ptr ) {
                        std::rethrow_exception( excep_ptr );
                }
        }

        host::common_cli&               cli;
        std::function< test_signature > test;
};

inline void register_test(
    const std::string&              fixture_name,
    const std::string&              name,
    host::common_cli&               cli,
    std::function< test_signature > test )
{
        ::testing::RegisterTest(
            fixture_name.c_str(),
            name.c_str(),
            nullptr,
            nullptr,
            __FILE__,
            __LINE__,
            [&cli, test = std::move( test )] {
                    return new bb_test_case( cli, test );
            } );
}

}  // namespace tests::bb
