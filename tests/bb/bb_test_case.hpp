#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <filesystem>
#include <gtest/gtest.h>

#pragma once

namespace tests::bb
{

using test_signature =
    boost::asio::awaitable< void >( boost::asio::io_context&, boost::asio::serial_port& );

struct bb_test_case : ::testing::Test
{
        bb_test_case(
            std::filesystem::path           device,
            unsigned                        baudrate,
            std::function< test_signature > test )
          : port( context, device )
          , test( test )
        {
                port.set_option( boost::asio::serial_port_base::baud_rate( baudrate ) );
        }

        void TestBody() final
        {
                std::exception_ptr excep_ptr;
                co_spawn(
                    port.get_executor(), test( context, port ), [&]( std::exception_ptr ptr ) {
                            excep_ptr = ptr;
                    } );
                context.run();

                if ( excep_ptr ) {
                        std::rethrow_exception( excep_ptr );
                }
        }

        boost::asio::io_context         context;
        boost::asio::serial_port        port;
        std::function< test_signature > test;
};

void register_test(
    const std::string&              fixture_name,
    const std::string&              name,
    std::filesystem::path           device,
    unsigned                        baudrate,
    std::function< test_signature > test )
{
        ::testing::RegisterTest(
            fixture_name.c_str(), name.c_str(), nullptr, nullptr, __FILE__, __LINE__, [=] {
                    return new bb_test_case( device, baudrate, test );
            } );
}

}  // namespace tests::bb
