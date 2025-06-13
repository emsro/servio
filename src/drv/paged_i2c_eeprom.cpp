
#include "./paged_i2c_eeprom.hpp"

#include "../base.hpp"
#include "../fw/util.hpp"
#include "../status.hpp"

#include <emlabcpp/experimental/cfg/handler.h>
#include <emlabcpp/experimental/cfg/page.h>
#include <span>

namespace em = emlabcpp;

namespace servio::drv
{

namespace
{
#pragma GCC push_options
#pragma GCC optimize( "O0" )

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
                return ERROR;
        return SUCCESS;
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
                return ERROR;
        return SUCCESS;
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
                if ( stat != SUCCESS )
                        return ERROR;
                wait_for( clk, 4_ms );
        }
        return SUCCESS;
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

#pragma GCC pop_options

struct update_iface : em::cfg::update_iface
{
        std::byte                                      buffer[42];
        em::static_vector< uint32_t, cfg::ids.size() > unseen_ids{ cfg::ids };

        cfg::map const&    cfg;
        I2C_HandleTypeDef* i2c;
        clk_iface&         clk;
        uint16_t           dev_addr;
        uint32_t           mem_size;
        uint32_t           page_size;

        update_iface(
            cfg::map const&    cfg,
            I2C_HandleTypeDef* i2c,
            clk_iface&         clk,
            uint16_t           dev_addr,
            uint32_t           mem_size,
            uint32_t           page_size )
          : cfg( cfg )
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

        em::result
        read( std::size_t addr, std::span< std::byte, em::cfg::cell_size > data ) override
        {
                return read_data( i2c, dev_addr, static_cast< uint16_t >( addr ), data );
        }

        em::result write( std::size_t addr, std::span< std::byte const > data ) override
        {
                return write_data( i2c, dev_addr, static_cast< uint16_t >( addr ), data );
        }

        em::cfg::cache_res check_key_cache( uint32_t id ) override
        {
                return em::cfg::key_check_unseen_container( unseen_ids, id );
        }

        bool value_changed( uint32_t id, std::span< std::byte const > data ) override
        {
                auto ptr = cfg.ref_by_id( id );
                if ( !ptr )  // XXX: maybe rethink?
                        return false;
                return ptr.vref().visit( [&]< typename T >( T const& val ) {
                        auto newval = em::cfg::get_val< T >( data );
                        if ( newval )
                                return val != *newval;
                        return false;
                } );
        }

        opt< std::span< std::byte const > >
        serialize_value( uint32_t id, std::span< std::byte > buffer ) override
        {
                opt< std::span< std::byte > > res;
                auto                          ptr = cfg.ref_by_id( id );
                if ( !ptr )
                        return res;
                ptr.vref().visit( [&]< typename T >( T const& val ) {
                        auto ran = em::cfg::store_val( val, buffer );
                        if ( ran )
                                res = *ran;
                } );
                return res;
        }

        em::result reset_keys() override
        {
                unseen_ids = cfg::ids;
                return em::result::SUCCESS;
        }

        opt< uint32_t > take_unseen_key() override
        {
                return em::cfg::pop_from_container( unseen_ids );
        }

        em::result clear_page( std::size_t addr ) override
        {
                uint16_t end_addr = static_cast< uint16_t >( addr + page_size );
                return clear_range< em::cfg::cell_size >(
                    i2c, clk, dev_addr, static_cast< uint16_t >( addr ), end_addr );
        }
};

}  // namespace

status i2c_eeprom::store_cfg( cfg::map const& m )
{
        em::static_vector< uint32_t, cfg::ids.size() > unseen_ids{ cfg::ids };

        update_iface lb{ m, i2c_, clk_, dev_addr, mem_size_, page_size_ };
        auto         res = em::cfg::update( mem_size_, page_size_, lb );
        return res;
}

namespace
{
struct load_iface : em::cfg::load_iface
{
        std::byte buffer[42];

        em::static_vector< uint32_t, cfg::ids.size() > unseen_ids{ cfg::ids };

        cfg::map&          cfg;
        I2C_HandleTypeDef* i2c;
        uint16_t           dev_addr;

        load_iface( cfg::map& cfg, I2C_HandleTypeDef* i2c, uint16_t dev_addr )
          : cfg( cfg )
          , i2c( i2c )
          , dev_addr( dev_addr )
        {
        }

        std::span< std::byte > get_buffer() override
        {
                return { buffer, sizeof( buffer ) };
        }

        em::result
        read( std::size_t addr, std::span< std::byte, em::cfg::cell_size > data ) override
        {
                return read_data( i2c, dev_addr, static_cast< uint16_t >( addr ), data );
        }

        em::cfg::cache_res check_key_cache( uint32_t id ) override
        {
                return em::cfg::key_check_unseen_container( unseen_ids, id );
        }

        em::result on_kval( uint32_t id, std::span< std::byte > data ) override
        {
                auto ptr = cfg.ref_by_id( id );
                if ( !ptr )
                        return em::result::ERROR;
                return ptr.vref().visit( [&]< typename T >( T& val ) -> em::result {
                        auto newval = em::cfg::get_val< T >( data );
                        if ( newval ) {
                                val = *newval;
                                return em::result::SUCCESS;
                        }
                        return em::result::ERROR;
                } );
        }
};
}  // namespace

status i2c_eeprom::load_cfg( cfg::map& m )
{
        load_iface liface{ m, i2c_, dev_addr };

        auto r = em::cfg::load( mem_size_, page_size_, liface );
        return r;
}

status i2c_eeprom::clear_cfg()
{
        static constexpr uint32_t batch = 64;
        return clear_range< batch >(
            i2c_, clk_, dev_addr, static_cast< uint16_t >( 0 ), mem_size_ );
}

}  // namespace servio::drv
