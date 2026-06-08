#include "./flash.hpp"

#include "./dfu_flash.hpp"
#include "./serial.hpp"

#include <fstream>

namespace servio::scmdio
{

namespace
{

sptr< serial_stream >
make_bootloader_stream( io_context& io_ctx, std::filesystem::path const& path, unsigned baudrate )
{
        using spb = boost::asio::serial_port_base;
        auto ss   = std::make_shared< serial_stream >( io_ctx, path, baudrate );
        ss->port.set_option( spb::parity( spb::parity::even ) );
        ss->port.set_option( spb::character_size( 8 ) );
        ss->port.set_option( spb::stop_bits( spb::stop_bits::one ) );
        return ss;
}

}  // namespace

awaitable< void > flash_firmware(
    io_context&                  io_ctx,
    std::filesystem::path const& port_path,
    std::filesystem::path const& file_path,
    unsigned                     baudrate )
{
        auto ss = make_bootloader_stream( io_ctx, port_path, baudrate );

        if ( !co_await dfu_try_init( *ss ) )
                log_error(
                    "Bootloader not detected — hold BOOT0 high, reset the device, then retry" );

        std::ifstream f{ file_path, std::ios::binary };
        if ( !f )
                log_error( "Failed to open file: {}", file_path.string() );

        spdlog::info( "Uploading firmware..." );
        co_await dfu_upload_raw( *ss, f );

        spdlog::info( "Starting firmware..." );
        co_await dfu_reset( *ss );
}

}  // namespace servio::scmdio
