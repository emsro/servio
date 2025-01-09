#include "../scmdio/cli.hpp"

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
    boost::asio::awaitable< void >( boost::asio::io_context&, scmdio::port_iface& );

struct bb_test_case : ::testing::Test
{

        bb_test_case(
            boost::asio::io_context&              ctx,
            std::shared_ptr< scmdio::port_iface > port,
            std::function< test_signature >       test,
            std::chrono::milliseconds             timeout )
          : io_ctx( ctx )
          , port( port )
          , test( std::move( test ) )
          , timeout( timeout )
        {
        }

        boost::asio::io_context&              io_ctx;
        std::shared_ptr< scmdio::port_iface > port;
        std::function< test_signature >       test;
        std::chrono::milliseconds             timeout;

        void TestBody() final
        {
                io_ctx.restart();
                std::exception_ptr excep_ptr;
                bool               finished = false;
                co_spawn(
                    io_ctx,
                    [&]() -> boost::asio::awaitable< void > {
                            co_await this->test( io_ctx, *port );
                            finished = true;
                    },
                    [&]( std::exception_ptr ptr ) {
                            excep_ptr = std::move( ptr );
                    } );
                io_ctx.run_for( timeout );

                if ( excep_ptr )
                        std::rethrow_exception( excep_ptr );
                EXPECT_TRUE( finished )
                    << "Test failed to finish in time, timeout: " << timeout.count() << "s";
        }
};

inline void register_test(
    std::string const&                    fixture_name,
    std::string const&                    name,
    boost::asio::io_context&              io_ctx,
    std::shared_ptr< scmdio::port_iface > port,
    std::function< test_signature >       test,
    std::chrono::milliseconds             timeout )
{
        ::testing::RegisterTest(
            fixture_name.c_str(),
            name.c_str(),
            nullptr,
            nullptr,
            __FILE__,
            __LINE__,
            [&io_ctx, &port, test = std::move( test ), timeout] {
                    return new bb_test_case( io_ctx, port, std::move( test ), timeout );
            } );
}

}  // namespace servio::bb
