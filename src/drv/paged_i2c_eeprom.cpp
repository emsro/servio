
#include "./paged_i2c_eeprom.hpp"

#include "../base.hpp"
#include "../fw/util.hpp"
#include "../status.hpp"

#include <emlabcpp/experimental/cfg/handler.hpp>
#include <emlabcpp/experimental/cfg/page.hpp>
#include <span>

namespace em = emlabcpp;

namespace servio::drv
{

namespace
{

status write_data(
    I2C_HandleTypeDef*           hi2c,
    uint16_t                     dev_addr,
    uint16_t                     mem_addr,
    std::span< std::byte const > data )
{
        static constexpr uint32_t timeout = 200;

        auto volatile stat = HAL_I2C_Mem_Write(
            hi2c, dev_addr, mem_addr, 2, (uint8_t*) data.data(), (uint16_t) data.size(), timeout );
        if ( stat != HAL_OK )
                return status::error;
        return status::success;
}

status read_data(
    I2C_HandleTypeDef*           hi2c,
    uint16_t                     dev_addr,
    uint16_t                     mem_addr,
    std::span< std::byte const > data )
{
        static constexpr uint32_t timeout = 200;

        auto stat = HAL_I2C_Mem_Read(
            hi2c, dev_addr, mem_addr, 2, (uint8_t*) data.data(), (uint16_t) data.size(), timeout );
        if ( stat != HAL_OK )
                return status::error;
        return status::success;
}

status clear_range_impl(
    I2C_HandleTypeDef*     hi2c,
    clk_iface&             clk,
    uint16_t               dev_addr,
    uint16_t               addr,
    uint16_t               end_addr,
    std::span< std::byte > buffer )
{
        for ( ; addr < end_addr; addr += static_cast< uint16_t >( buffer.size() ) ) {
                auto stat = write_data( hi2c, dev_addr, static_cast< uint16_t >( addr ), buffer );
                if ( stat != status::success )
                        return status::error;
                wait_for( clk, 4_ms );
        }
        return status::success;
}

template < std::size_t BatchSize >
status clear_range(
    I2C_HandleTypeDef* hi2c,
    clk_iface&         clk,
    uint16_t           dev_addr,
    uint16_t           addr,
    uint16_t           end_addr )
{
        std::byte buffer[BatchSize];
        std::memset( buffer, 0, sizeof( buffer ) );
        return clear_range_impl(
            hi2c, clk, dev_addr, addr, end_addr, std::span< std::byte >( buffer ) );
}

cfg::iface* find_section( zll::ll_list< cfg::iface >& cfgs, uint32_t id )
{
        for ( cfg::iface& c : cfgs )
                if ( c.section == id >> 16U )
                        return &c;
        return nullptr;
}

void reset_seen( zll::ll_list< cfg::iface >& cfgs )
{
        for ( cfg::iface& c : cfgs )
                c.storage_reset_seen();
}

em::cfg::cache_res check_seen( zll::ll_list< cfg::iface >& cfgs, uint32_t id )
{
        cfg::iface* c = find_section( cfgs, id );
        if ( c && c->storage_mark_seen( id & 0xFFFFU ) )
                return em::cfg::cache_res::NOT_SEEN;
        return em::cfg::cache_res::SEEN;
}

struct update_iface : em::cfg::update_iface
{
        std::byte buffer[42];

        zll::ll_list< cfg::iface >& cfgs;
        I2C_HandleTypeDef*          i2c;
        clk_iface&                  clk;
        uint16_t                    dev_addr;
        uint32_t                    mem_size;
        uint32_t                    page_size;

        update_iface(
            zll::ll_list< cfg::iface >& cfgs,
            I2C_HandleTypeDef*          i2c,
            clk_iface&                  clk,
            uint16_t                    dev_addr,
            uint32_t                    mem_size,
            uint32_t                    page_size )
          : cfgs( cfgs )
          , i2c( i2c )
          , clk( clk )
          , dev_addr( dev_addr )
          , mem_size( mem_size )
          , page_size( page_size )
        {
        }

        std::span< std::byte > get_buffer() override
        {
                return { buffer, sizeof( buffer ) };
        }

        error_code
        read( std::size_t addr, std::span< std::byte, em::cfg::cell_size > data ) override
        {
                return read_data( i2c, dev_addr, static_cast< uint16_t >( addr ), data );
        }

        error_code write( std::size_t addr, std::span< std::byte const > data ) override
        {
                auto res = write_data( i2c, dev_addr, static_cast< uint16_t >( addr ), data );
                // XXX: read datasheet properly
                wait_for( clk, 4_ms );
                return res;
        }

        em::cfg::cache_res check_key_cache( uint32_t id ) override
        {
                return check_seen( cfgs, id );
        }

        bool value_changed( uint32_t id, std::span< std::byte const > data ) override
        {
                cfg::iface* c = find_section( cfgs, id );
                if ( !c )
                        return false;
                return c->on_storage_change_check( id & 0xFFFFU, data );
        }

        opt< std::span< std::byte const > >
        serialize_value( uint32_t id, std::span< std::byte > buffer ) override
        {
                cfg::iface* c = find_section( cfgs, id );
                if ( !c )
                        return {};
                return c->on_storage_save( id & 0xFFFFU, buffer );
        }

        error_code reset_keys() override
        {
                reset_seen( cfgs );
                return status::success;
        }

        opt< uint32_t > take_unseen_key() override
        {
                for ( cfg::iface& c : cfgs )
                        if ( auto const k = c.storage_take_unseen() )
                                return cfg_storage_id( c.section, *k );
                return {};
        }

        error_code clear_page( std::size_t addr ) override
        {
                uint16_t end_addr = static_cast< uint16_t >( addr + page_size );
                return clear_range< em::cfg::cell_size >(
                    i2c, clk, dev_addr, static_cast< uint16_t >( addr ), end_addr );
        }
};

}  // namespace

error_code i2c_eeprom::store_cfg( zll::ll_list< cfg::iface >& cfgs )
{
        reset_seen( cfgs );
        update_iface lb{ cfgs, i2c_, clk_, dev_addr, mem_size_, page_size_ };
        auto         res = em::cfg::update( mem_size_, page_size_, lb );
        return res == em::cfg::status::SUCCESS ? status::success : status::error;
}

namespace
{
struct load_iface : em::cfg::load_iface
{
        std::byte buffer[42];

        zll::ll_list< cfg::iface >& cfgs;
        I2C_HandleTypeDef*          i2c;
        uint16_t                    dev_addr;

        load_iface( zll::ll_list< cfg::iface >& cfgs, I2C_HandleTypeDef* i2c, uint16_t dev_addr )
          : cfgs( cfgs )
          , i2c( i2c )
          , dev_addr( dev_addr )
        {
        }

        std::span< std::byte > get_buffer() override
        {
                return { buffer, sizeof( buffer ) };
        }

        error_code
        read( std::size_t addr, std::span< std::byte, em::cfg::cell_size > data ) override
        {
                return read_data( i2c, dev_addr, static_cast< uint16_t >( addr ), data );
        }

        em::cfg::cache_res check_key_cache( uint32_t id ) override
        {
                return check_seen( cfgs, id );
        }

        error_code on_kval( uint32_t id, std::span< std::byte > data ) override
        {
                cfg::iface* c = find_section( cfgs, id );
                if ( !c )
                        return status::error;
                return c->on_storage_load( id & 0xFFFFU, data );
        }
};
}  // namespace

error_code i2c_eeprom::load_cfg( zll::ll_list< cfg::iface >& cfgs )
{
        reset_seen( cfgs );
        load_iface liface{ cfgs, i2c_, dev_addr };

        auto r = em::cfg::load( mem_size_, page_size_, liface );
        return r == em::cfg::status::SUCCESS ? status::success : status::error;
}

error_code i2c_eeprom::clear_cfg()
{
        static constexpr uint32_t batch = 64;
        return clear_range< batch >(
            i2c_, clk_, dev_addr, static_cast< uint16_t >( 0 ), mem_size_ );
}

}  // namespace servio::drv
