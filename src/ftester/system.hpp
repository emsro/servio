#pragma once

#include "ftester/handle_eptr.hpp"
#include "scmdio/async_cobs.hpp"

#include <emlabcpp/convert_view.h>
#include <emlabcpp/experimental/logging/eabi_logger.h>
#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/controller.h>
#include <emlabcpp/experimental/testing/gtest.h>
#include <emlabcpp/experimental/testing/json.h>
#include <emlabcpp/experimental/testing/parameters.h>
#include <emlabcpp/pmr/new_delete_resource.h>
#include <utility>

namespace em = emlabcpp;

namespace servio::ftester
{

using namespace std::chrono_literals;

inline em::eabi_logger COM_LOGGER{
    { em::set_stdout{ false }, em::set_stderr{ false }, em::DEBUG_LOGGER_COLORS } };
#define COM_LOG( chann, ... ) EMLABCPP_EABI_LOG_IMPL( COM_LOGGER, "com", chann, __VA_ARGS__ )

struct controller_interface : em::testing::controller_interface
{
        em::testing::collect_server& col_serv;
        std::stringstream            errss;

        controller_interface( em::testing::collect_server& col_serv )
          : col_serv( col_serv )
        {
        }

        void on_result( const em::testing::test_result& res ) final
        {
                if ( em::testing::is_problematic( res.status ) )
                        errss << em::testing::data_tree_to_json( col_serv.get_tree() ).dump( 4 )
                              << "\n";

                EMLABCPP_INFO_LOG( "Test finished" );
        }

        void on_error( const em::testing::error_variant& err ) final
        {
                errss << err;
        }
};

em::result send( auto& ctx, em::protocol::channel_type channel, const auto& data )
{
        COM_LOG( channel, "w: ", data );
        auto msg = em::protocol::serialize_multiplexed( channel, data );
        co_spawn( ctx.io_context, ctx.write( msg ), handle_eptr );

        return em::SUCCESS;
}

struct test_context
{
        boost::asio::io_context   io_context;
        servio::scmdio::cobs_port d_port;
        boost::asio::serial_port  c_port;

        em::testing::collect_server    col_serv;
        em::testing::parameters_server par_serv;

        test_context(
            const std::filesystem::path& d_dev,
            uint32_t                     d_baudrate,
            const std::filesystem::path& c_dev,
            uint32_t                     c_baudrate,
            const nlohmann::json&        input_json )
          : d_port( io_context, d_dev, d_baudrate )
          , c_port( io_context, c_dev )
          , col_serv(
                em::testing::collect_channel,
                em::pmr::new_delete_resource(),
                [this]( auto chann, const auto& data ) {
                        return send( *this, chann, data );
                } )
          , par_serv(
                em::testing::params_channel,
                em::testing::json_to_data_tree( em::pmr::new_delete_resource(), input_json )
                    .value(),
                [this]( auto chann, const auto& data ) {
                        return send( *this, chann, data );
                } )
        {
                c_port.set_option( boost::asio::serial_port_base::baud_rate( c_baudrate ) );
        }

        template < std::size_t N >
        boost::asio::awaitable< void > write( em::protocol::message< N > msg )
        {
                co_await d_port.async_write( msg );
        }
};

class test_system
{
public:
        test_system( test_context& ctx, em::testing::controller_interface& iface )
          : ctx_( ctx )
          , cont_(
                em::testing::core_channel,
                em::pmr::new_delete_resource(),
                iface,
                [this]( auto chann, const auto& data ) {
                        return send( this->ctx_, chann, data );
                } )
        {

                co_spawn( ctx.io_context, dread(), handle_eptr );
                co_spawn( ctx.io_context, cread(), handle_eptr );
        }

        std::string suite_name() const
        {
                return std::string{ cont_.suite_name() };
        }

        std::string suite_date() const
        {
                return std::string{ cont_.suite_date() };
        }

        const auto& get_tests() const
        {
                return cont_.get_tests();
        }

        bool wait_for_init()
        {
                while ( is_initializing() ) {
                        tick();
                        if ( system_failure_ ) {
                                EMLABCPP_ERROR_LOG( "Failed to initialize" );
                                return false;
                        }
                }
                return true;
        }

        bool is_initializing() const
        {
                return cont_.is_initializing();
        }

        const em::testing::data_tree& get_collected() const
        {
                return ctx_.col_serv.get_tree();
        }

        bool test_result() const
        {
                return !system_failure_;
        }

        void tick()
        {
                cont_.tick();

                ctx_.io_context.run_for( 5ms );
        }

        void clear()
        {
                ctx_.col_serv.clear();
        }

        void run_test( em::testing::test_id tid )
        {
                clear();
                cont_.start_test( tid );
                while ( cont_.is_test_running() && !system_failure_ )
                        tick();
        }

private:
        boost::asio::awaitable< void > dread()
        {

                std::vector< std::byte > buffer( 512, std::byte{ 0 } );

                while ( true ) {
                        em::view< std::byte* > data = co_await ctx_.d_port.async_read( buffer );

                        em::outcome out = em::protocol::extract_multiplexed(
                            data,
                            [&]( em::protocol::channel_type   chid,
                                 std::span< const std::byte > data ) {
                                    COM_LOG( chid, "r: ", em::protocol::message< 128 >{ data } );
                                    return em::protocol::multiplexed_dispatch(
                                        chid, data, cont_, ctx_.col_serv, ctx_.par_serv );
                            } );

                        system_failure_ |= out == em::ERROR;
                }
        }

        boost::asio::awaitable< void > cread()
        {
                std::vector< std::byte > buffer( 512, std::byte{ 0 } );
                while ( true ) {
                        std::size_t n = co_await ctx_.c_port.async_read_some(
                            boost::asio::buffer( buffer.data(), buffer.size() ),
                            boost::asio::use_awaitable );

                        std::cout << "replying: " << em::convert_view_n< int >( buffer.data(), n )
                                  << std::endl;

                        co_await boost::asio::async_write(
                            ctx_.c_port,
                            boost::asio::buffer( buffer.data(), n ),
                            boost::asio::use_awaitable );
                }
        }

        test_context&           ctx_;
        em::testing::controller cont_;
        bool                    system_failure_ = false;
};
}  // namespace servio::ftester
