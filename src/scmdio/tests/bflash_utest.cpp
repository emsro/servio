#include "../../base/base.hpp"
#include "../bflash.hpp"
#include "./util.hpp"
#include "spdlog/fmt/bin_to_hex.h"

#include <deque>
#include <emlabcpp/experimental/coro/owning_coroutine_handle.h>
#include <gtest/gtest.h>

namespace servio::scmdio
{

using namespace servio::literals;

template < typename T >
struct consumer
{
        struct promise_type;

        template < std::size_t N >
        struct get
        {
                constexpr bool await_ready() noexcept
                {
                        return false;
                }

                constexpr void await_suspend( std::coroutine_handle< promise_type > h )
                {
                        h.promise().buffer_ = buffer;
                }

                auto const& await_resume()
                {
                        return buffer;
                }

                std::array< T, N > buffer;
        };

        struct promise_type
        {
                consumer get_return_object()
                {
                        return consumer{ handle::from_promise( *this ) };
                }

                std::suspend_never initial_suspend()
                {
                        return {};
                }

                std::suspend_always final_suspend() noexcept
                {
                        return {};
                }

                void unhandled_exception()
                {
                        assert( false );
                }

                std::span< T > buffer_;
        };

        using handle        = std::coroutine_handle< promise_type >;
        using owning_handle = em::coro::owning_coroutine_handle< promise_type >;

        consumer( handle han )
          : h_( std::move( han ) )
        {
        }

        void eat( std::span< T const > data )
        {
                for ( T const& x : data )
                        eat( x );
        }

        void eat( T b )
        {
                assert( h_ );
                auto& buff = h_.promise().buffer_;
                assert( buff.size() != 0 );
                buff[0] = b;
                buff    = buff.subspan( 1 );
                if ( buff.size() == 0 )
                        h_();
        }

        [[nodiscard]] operator bool() const
        {
                return h_.done();
        }

private:
        owning_handle h_;
};

static constexpr std::byte ACK          = 0x79_b;
static constexpr std::byte NACK         = 0x1F_b;
static constexpr std::byte INIT         = 0x7F_b;
static constexpr std::byte GET          = 0x00_b;
static constexpr std::byte GET_ID       = 0x02_b;
static constexpr std::byte READ_MEMORY  = 0x11_b;
static constexpr std::byte WRITE_MEMORY = 0x31_b;

struct stm32_bootloader_mock : stream_iface
{
        using cons = consumer< std::byte >;

        static std::vector< std::byte > setup_mem( std::size_t k )
        {
                std::vector< std::byte > res;
                std::size_t              n = 1 << k;
                res.reserve( n );
                for ( std::size_t i = 0; i < n; ++i )
                        res.emplace_back( 0x00_b );
                return res;
        }

        std::vector< std::byte > memory = setup_mem( 24 );

        std::size_t mem_offset = 0x0800'0000;

        std::deque< std::byte > buffer;

        cons c = state_machine();

        auto get_data()
        {
                return std::array{
                    12_b,  // size
                    10_b,  // version
                    0x00_b,
                    0x01_b,
                    0x02_b,
                    0x11_b,
                    0x21_b,
                    0x31_b,
                    0x43_b,
                    0x63_b,
                    0x73_b,
                    0x82_b,
                    0x92_b,
                    0xA1_b };
        }

        auto get_id_data()
        {
                return std::array{ 1_b, 0x04_b, 0x68_b };
        }

        void reply_data( auto&& data )
        {
                for ( std::byte b : data )
                        buffer.emplace_back( b );
                buffer.emplace_back( ACK );
        }

        cons state_machine()
        {
                auto b = co_await cons::get< 1 >{};
                assert( b[0] == INIT );
                buffer.emplace_back( ACK );

                while ( true ) {
                        auto [b] = co_await cons::get< 1 >{};
                        if ( b == INIT ) {
                                buffer.emplace_back( ACK );
                                continue;
                        }
                        auto [chck] = co_await cons::get< 1 >{};
                        if ( b != ~chck )
                                log_error( "command check failed: {} {}", b, chck );

                        switch ( b ) {
                        case GET:
                                buffer.emplace_back( ACK );
                                reply_data( get_data() );
                                break;
                        case GET_ID:
                                buffer.emplace_back( ACK );
                                reply_data( get_id_data() );
                                break;
                        case READ_MEMORY: {
                                buffer.emplace_back( ACK );
                                auto     data = co_await cons::get< 4 >{};
                                uint32_t addr = conv_addr( data );
                                co_await cons::get< 1 >{};
                                // XXX: checksum check
                                buffer.emplace_back( ACK );
                                auto [size, schck] = co_await cons::get< 2 >{};
                                if ( size != ~schck )
                                        log_error( "chcksm mismatch: {} {}", size, schck );
                                buffer.emplace_back( ACK );
                                for ( std::size_t i = 0; i <= (std::size_t) size; i++ )
                                        buffer.emplace_back( memory.at( addr++ ) );
                                break;
                        }
                        case WRITE_MEMORY: {
                                buffer.emplace_back( ACK );
                                auto     data = co_await cons::get< 4 >{};
                                uint32_t addr = conv_addr( data );
                                co_await cons::get< 1 >{};
                                // XXX: checksum check
                                buffer.emplace_back( ACK );
                                auto [size] = co_await cons::get< 1 >{};
                                for ( std::size_t i = 0; i <= (std::size_t) size; i++ ) {
                                        auto [x]            = co_await cons::get< 1 >{};
                                        memory.at( addr++ ) = x;
                                }
                                co_await cons::get< 1 >{};
                                // XXX: checksum check
                                buffer.emplace_back( ACK );
                                break;
                        }
                        default:
                                buffer.emplace_back( NACK );
                        }
                }
        }

        awaitable< void > write( std::span< std::byte const > msg ) override
        {
                spdlog::debug( "writing: {}", msg );
                c.eat( msg );
                co_return;
        }

        uint32_t conv_addr( std::span< std::byte, 4 > data )
        {
                return static_cast< uint32_t >( bflash_conv( data ) - mem_offset );
        }

        awaitable< bool > read( std::span< std::byte > msg ) override
        {
                spdlog::debug( "reading buffer: {} {}", buffer.size(), msg.size() );
                assert( buffer.size() >= msg.size() );
                for ( std::byte& b : msg ) {
                        b = buffer.front();
                        buffer.pop_front();
                }
                spdlog::debug( "reading: {}", msg );
                co_return true;
        }
};

TEST( scmdio, info )
{
        boost::asio::io_context ctx;
        std::stringstream       ss;
        stm32_bootloader_mock   m;
        co_spawn( ctx, bflash_info( m, ss ), handle_eptr );

        ctx.run();

        std::string_view expected =
            R"(id: 0x468
version: 10
cmd: GET
cmd: GET_VERSION
cmd: GET_ID
cmd: READ_MEMORY
cmd: GO
cmd: WRITE_MEMORY
cmd: ERASE
cmd: WRITE_PROTECTED
cmd: WRITE_UNPROTECTED
cmd: READOUT_PROTECTED
cmd: READOUT_UNPROTECTED
cmd: GET_CHECKSUM
)";
        EXPECT_EQ( ss.str(), expected );
}

TEST( scmdio, reflash )
{
        spdlog::enable_backtrace( 32 );

        boost::asio::io_context ctx;
        std::stringstream       ss;
        stm32_bootloader_mock   m;
        co_spawn( ctx, bflash_download( m, ss ), handle_eptr );

        co_spawn( ctx, bflash_flash( m, ss ), handle_eptr );

        ctx.run();
}

}  // namespace servio::scmdio
