#include "./bflash.hpp"

#include "../base/base.hpp"

#define MAGIC_ENUM_RANGE_MAX 256

#include <magic_enum/magic_enum.hpp>

namespace servio::scmdio
{

using namespace servio::literals;

namespace
{
static constexpr std::byte ACK  = 0x79_b;
static constexpr std::byte NACK = 0x1F_b;
static constexpr std::byte INIT = 0x7F_b;

awaitable< void > init_comm( stream_iface& port )
{
        opt< std::byte > b;

        co_await port.write( INIT );
        b = co_await port.read();
        if ( b ) {
                if ( b != ACK )
                        log_error( "Failed to init device" );
                co_return;
        }
        co_await port.write( INIT );
        b = co_await port.read();
        // XXX: revisit exceptions?
        if ( b != NACK )
                log_error( "Response for init is not NACK as expected for double-init: {}", b );
}

awaitable< void > send( stream_iface& port, auto&& data, std::byte checksum = 0x00_b )
{
        std::byte b =
            checksum ^
            std::accumulate(
                data.begin(), data.end(), std::byte{ 0 }, [&]( std::byte a, std::byte b ) {
                        std::bitset< 8 > lh{ (unsigned long) a }, rh{ (unsigned long) b };
                        return ( std::byte )( lh ^ rh ).to_ulong();
                } );
        co_await port.write( data );
        co_await port.write( b );
}

enum cmd_e : uint8_t
{
        GET                 = 0x00,
        GET_VERSION         = 0x01,
        GET_ID              = 0x02,
        READ_MEMORY         = 0x11,
        GO                  = 0x21,
        WRITE_MEMORY        = 0x31,
        ERASE               = 0x43,
        EXTENDED_ERASE      = 0x44,
        SPECIAL             = 0x50,
        EXTENDED_SPECIAL    = 0x51,
        WRITE_PROTECTED     = 0x63,
        WRITE_UNPROTECTED   = 0x73,
        READOUT_PROTECTED   = 0x82,
        READOUT_UNPROTECTED = 0x92,
        GET_CHECKSUM        = 0xA1
};

awaitable< void > wait_for_ack( stream_iface& port )
{
        opt< std::byte > resp = co_await port.read();

        if ( resp != ACK )
                log_error( "Response is not ACK: {}", resp );
}

awaitable< void > cmd( stream_iface& port, cmd_e cmd )
{
        spdlog::debug( "Sending command: {}", magic_enum::enum_name( cmd ) );
        auto      b      = static_cast< std::byte >( cmd );
        std::byte data[] = { b, ~b };
        co_await port.write( data );
        co_await wait_for_ack( port );
}

struct get_data
{
        uint8_t                ver;
        std::vector< uint8_t > cmds;
};

awaitable< get_data > get( stream_iface& port )
{
        co_await cmd( port, GET );
        auto size_b = co_await port.read();
        assert( size_b );  // XXX: proper error
        auto size = (std::size_t) *size_b;

        std::vector< std::byte > vec( size + 1, 0_b );
        co_await port.read( vec );
        assert( vec.size() > 0 );
        get_data res{
            .ver  = (uint8_t) vec[0],
            .cmds = {},
        };
        for ( std::byte b : em::tail( vec ) )
                res.cmds.emplace_back( (uint8_t) b );

        co_await wait_for_ack( port );
        co_return res;
}

awaitable< uint16_t > get_id( stream_iface& port )
{
        co_await cmd( port, GET_ID );
        auto size_b = co_await port.read();
        assert( size_b );  // XXX: porper error
        auto                     size = (std::size_t) *size_b;
        std::vector< std::byte > vec( size + 1, 0_b );
        co_await port.read( vec );
        assert( vec.size() == 2 );
        co_await wait_for_ack( port );
        co_return ( (int) vec[0] << 8 ) + (int) vec[1];
}

static constexpr std::size_t mem_step = 252;

awaitable< void > read_memory( stream_iface& port, uint32_t addr, em::view< std::byte* > buff )
{
        co_await cmd( port, READ_MEMORY );

        co_await send( port, bflash_conv( addr ) );
        co_await wait_for_ack( port );

        assert( buff.size() <= 255 );
        auto size_b = static_cast< std::byte >( buff.size() - 1 );
        co_await port.write( size_b );
        co_await port.write( ~size_b );
        co_await wait_for_ack( port );

        co_await port.read( buff );
}

awaitable< void >
write_memory( stream_iface& port, uint32_t addr, em::view< std::byte const* > buff )
{
        co_await cmd( port, WRITE_MEMORY );

        spdlog::debug( "Sending address: {}", addr );
        co_await send( port, bflash_conv( addr ) );
        co_await wait_for_ack( port );

        if ( buff.size() % 4 != 0 || buff.size() > 255 )
                log_error( "Invalid buffer size: {}", buff.size() );
        auto size_b = static_cast< std::byte >( buff.size() - 1 );
        co_await port.write( size_b );
        co_await send( port, buff, size_b );
        co_await wait_for_ack( port );
}

struct chip_info
{
        em::min_max< uint32_t > flash;
};

static std::map< uint16_t, chip_info > const chips = {
    { 0x468, chip_info{ .flash = { 0x0800'0000, 0x0802'0000 } } } };

awaitable< chip_info const* > get_chip_id( stream_iface& port )
{
        uint16_t id   = co_await get_id( port );
        auto     iter = chips.find( id );
        if ( iter == chips.end() )
                log_error( "Unknown chip id:", id );
        spdlog::info( "chip id: {}", id );
        co_return &iter->second;
}

}  // namespace

awaitable< void > bflash_info( stream_iface& port, std::ostream& os )
{
        co_await init_comm( port );
        uint16_t id = co_await get_id( port );
        os << "id: 0x" << std::hex << +id << '\n';
        get_data d = co_await get( port );
        os << "version: " << std::dec << +d.ver << '\n';
        for ( uint8_t x : d.cmds )
                os << "cmd: " << magic_enum::enum_name( (cmd_e) x ) << '\n';
}

awaitable< void > bflash_download( stream_iface& port, std::ostream& os )
{
        co_await init_comm( port );
        chip_info const* ci = co_await get_chip_id( port );
        for ( uint32_t addr = ci->flash.min(); addr < ci->flash.max(); addr += mem_step ) {
                std::size_t s = std::min( mem_step, (std::size_t) ci->flash.max() - addr );
                std::vector< std::byte > tmp( s, 0x00_b );
                co_await read_memory( port, addr, tmp );
                os.write( (char*) tmp.data(), (long) tmp.size() );
        }
}

awaitable< void > bflash_flash( stream_iface& port, std::istream& is )
{
        co_await init_comm( port );
        chip_info const* ci = co_await get_chip_id( port );
        for ( uint32_t addr = ci->flash.min(); addr < ci->flash.max(); addr += mem_step ) {
                std::size_t s = std::min( mem_step, (std::size_t) ci->flash.max() - addr );
                std::vector< std::byte > tmp( s, 0x00_b );
                is.read( (char*) tmp.data(), (long) tmp.size() );
                tmp.resize( (unsigned long) is.gcount() );
                co_await write_memory( port, addr, tmp );
                if ( !is )
                        break;
        }
}

}  // namespace servio::scmdio
