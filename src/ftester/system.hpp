#pragma once

#include "../scmdio/async_cobs.hpp"
#include "./handle_eptr.hpp"
#include "./recorder.hpp"

#include <emlabcpp/convert_view.h>
#include <emlabcpp/experimental/logging/eabi_logger.h>
#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/controller.h>
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
        std::variant< std::monostate, em::testing::error_variant, em::testing::test_result > res =
            std::monostate{};

        controller_interface( em::testing::collect_server& col_serv )
          : col_serv( col_serv )
        {
        }

        void on_result( em::testing::test_result const& r ) final
        {
                res = r;
        }

        void on_error( em::testing::error_variant const& err ) final
        {
                res = err;
        }
};

struct test_system
{

        test_system(
            std::filesystem::path const& d_dev,
            uint32_t                     d_baudrate,
            std::filesystem::path const& c_dev,
            uint32_t                     c_baudrate,
            nlohmann::json const&        input_json )
          : d_port_( io_context_, d_dev, d_baudrate )
          , c_port_( io_context_, c_dev )
          , col_serv_(
                em::testing::collect_channel,
                em::pmr::new_delete_resource(),
                [this]( auto chann, auto const& data ) {
                        return send( chann, data );
                } )
          , par_serv_(
                em::testing::params_channel,
                em::testing::json_to_data_tree( em::pmr::new_delete_resource(), input_json )
                    .value(),
                [this]( auto chann, auto const& data ) {
                        return send( chann, data );
                } )
          , rec_( rec_id )
          , col_iface_( col_serv_ )
          , cont_(
                em::testing::core_channel,
                em::pmr::new_delete_resource(),
                col_iface_,
                [this]( auto chann, auto const& data ) {
                        return send( chann, data );
                } )
        {
                c_port_.set_option( boost::asio::serial_port_base::baud_rate( c_baudrate ) );

                co_spawn( io_context_, dread(), handle_eptr );
                co_spawn( io_context_, cread(), handle_eptr );
        }

        std::string suite_name() const
        {
                return std::string{ cont_.suite_name() };
        }

        std::string suite_date() const
        {
                return std::string{ cont_.suite_date() };
        }

        auto const& get_tests() const
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

        em::testing::data_tree const& get_collected() const
        {
                return col_serv_.get_tree();
        }

        auto const& get_records() const
        {
                return rec_.get_buffer();
        }

        void tick()
        {
                cont_.tick();

                io_context_.run_for( 1ms );
        }

        std::variant< em::testing::test_result, em::testing::error_variant >
        run_test( em::testing::test_id tid )
        {
                clear();
                cont_.start_test( tid );
                while ( cont_.is_test_running() && !system_failure_ )
                        tick();

                if ( auto* ptr = std::get_if< em::testing::test_result >( &col_iface_.res ) )
                        return *ptr;
                if ( auto* ptr = std::get_if< em::testing::error_variant >( &col_iface_.res ) )
                        return *ptr;
                return {};
        }

private:
        void clear()
        {
                col_serv_.clear();
                rec_.clear();
                col_iface_.res = std::monostate{};
        }

        em::result send( em::protocol::channel_type channel, auto const& data )
        {
                COM_LOG( channel, "w: ", data );
                auto msg = em::protocol::serialize_multiplexed( channel, data );
                co_spawn( io_context_, write( msg ), handle_eptr );

                return em::SUCCESS;
        }

        template < std::size_t N >
        boost::asio::awaitable< void > write( em::protocol::message< N > msg )
        {
                co_await d_port_.async_write( msg );
        }

        boost::asio::awaitable< void > dread()
        {

                std::vector< std::byte > buffer( 1024, std::byte{ 0 } );

                while ( true ) {
                        em::view< std::byte* > data = co_await d_port_.async_read( buffer );

                        em::outcome out = em::protocol::extract_multiplexed(
                            data,
                            [&]( em::protocol::channel_type   chid,
                                 std::span< std::byte const > data ) {
                                    COM_LOG( chid, "r: ", em::protocol::msg_format{ data } );
                                    return em::protocol::multiplexed_dispatch(
                                        chid, data, cont_, col_serv_, par_serv_, rec_ );
                            } );

                        system_failure_ |= out == em::ERROR;
                }
        }

        boost::asio::awaitable< void > cread()
        {
                std::vector< std::byte > buffer( 8, std::byte{ 0 } );
                while ( true ) {
                        std::size_t n = co_await c_port_.async_read_some(
                            boost::asio::buffer( buffer.data(), buffer.size() ),
                            boost::asio::use_awaitable );

                        co_await boost::asio::async_write(
                            c_port_,
                            boost::asio::buffer( buffer.data(), n ),
                            boost::asio::use_awaitable );
                }
        }

private:
        boost::asio::io_context   io_context_;
        servio::scmdio::cobs_port d_port_;
        boost::asio::serial_port  c_port_;

        em::testing::collect_server    col_serv_;
        em::testing::parameters_server par_serv_;
        recorder                       rec_;

        controller_interface col_iface_;

        em::testing::controller cont_;
        bool                    system_failure_ = false;
};
}  // namespace servio::ftester
