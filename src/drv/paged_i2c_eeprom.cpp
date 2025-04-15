
#include "./paged_i2c_eeprom.hpp"

#include "../base.hpp"
#include "../fw/util.hpp"
#include "../status.hpp"

#include <emlabcpp/experimental/cfg2/handler.h>
#include <emlabcpp/experimental/cfg2/page.h>

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
        static constexpr std::size_t packet  = 32;
        static constexpr uint32_t    timeout = 200;

        // XXX maybe nonblocking next?
        while ( !data.empty() ) {
                auto n    = (uint16_t) std::min( data.size(), packet );
                auto stat = HAL_I2C_Mem_Write(
                    hi2c, dev_addr, mem_addr, 2, (uint8_t*) data.data(), n, timeout );
                if ( stat != HAL_OK )
                        return ERROR;
                data = data.subspan( 0, n );
        }
        return SUCCESS;
}

status read_data(
    I2C_HandleTypeDef*           hi2c,
    uint16_t                     dev_addr,
    uint16_t                     mem_addr,
    std::span< std::byte const > data )
{
        static constexpr std::size_t packet  = 32;
        static constexpr uint32_t    timeout = 200;

        // XXX maybe nonblocking next?
        while ( !data.empty() ) {
                auto n    = (uint16_t) std::min( data.size(), packet );
                auto stat = HAL_I2C_Mem_Read(
                    hi2c, dev_addr, mem_addr, 2, (uint8_t*) data.data(), n, timeout );
                if ( stat != HAL_OK )
                        return ERROR;
                data = data.subspan( 0, n );
        }
        return SUCCESS;
}

status locate_next(
    I2C_HandleTypeDef*  hi2c,
    uint16_t            dev_addr,
    uint16_t            mem_size,
    uint16_t            page_size,
    uint16_t&           page_addr,
    em::cfg::hdr_state& page_st )
{
        em::cfg::activ_page_sel psel;

        for ( uint16_t i = 0; i < mem_size / page_size; i++ ) {
                std::byte data[2] = { 0x00_b, 0x00_b };
                if ( read_data( hi2c, dev_addr, i * page_size, data ) != SUCCESS )
                        return ERROR;
                if ( psel.on_raw_hdr( i, std::span< std::byte, 2 >{ data, 2 } ) == ERROR ) {
                        page_addr = i * page_size;
                        page_st   = em::cfg::hdr_state::A;
                        return SUCCESS;
                }
        }

        page_addr = static_cast< uint16_t >( psel.idx * page_size );
        page_st   = psel.hdr_st ? *psel.hdr_st : em::cfg::hdr_state::A;
        return SUCCESS;
}

opt< uint16_t >
locate_current( I2C_HandleTypeDef* hi2c, uint16_t dev_addr, uint16_t mem_size, uint16_t page_size )
{
        em::cfg::activ_page_sel psel;

        for ( uint16_t i = 0; i < mem_size / page_size; i++ ) {
                std::byte data[2] = { 0x00_b, 0x00_b };
                if ( read_data( hi2c, dev_addr, i * page_size, data ) != SUCCESS )
                        return {};
                if ( psel.on_raw_hdr( i, std::span< std::byte, 2 >{ data, 2 } ) == ERROR )
                        continue;
        }
        if ( !psel.hdr_st )
                return {};
        return static_cast< uint16_t >( psel.idx * page_size );
}

}  // namespace

status i2c_eeprom::start()
{
        for ( uint16_t x = 0x00; x < 255; x++ ) {
                uint8_t data[1] = { 0x00 };
                auto    stat    = HAL_I2C_Mem_Read( i2c_, x, 0x00, 2, data, 1, 200 );
                if ( stat == HAL_OK )
                        fw::stop_exec();
        }
        fw::stop_exec();

        return SUCCESS;
}

status i2c_eeprom::store_cfg( cfg::map const& m )
{
        uint16_t page_addr = 0x00;
        if ( auto addr = locate_current( i2c_, dev_addr_, mem_size_, page_size_ ) )
                page_addr = *addr;

        em::static_vector< cfg::key, cfg::map::registers_count > unseen_keys{ cfg::map::keys };

        std::byte buffer[42];

        em::cfg::update_cbs lu{
            .buffer = buffer,
            .read_f = [&]( std::size_t                                addr,
                           std::span< std::byte, em::cfg::cell_size > data ) -> em::result {
                    return read_data( i2c_, dev_addr_, static_cast< uint16_t >( addr ), data );
            },
            .write_f = [&]( std::size_t addr, std::span< std::byte > data ) -> em::result {
                    return write_data( i2c_, dev_addr_, static_cast< uint16_t >( addr ), data );
            },
            .check_key_cache_f = [&]( uint32_t key ) -> em::cfg::cache_res {
                    return em::cfg::key_check_unseen_container( unseen_keys, key );
            },
            .value_changed_f = [&]( uint32_t key, std::span< std::byte > data ) -> bool {
                    return em::cfg::reg_map_value_changed( m, key, data );
            },
            .serialize_key_f = [&]( uint32_t               key,
                                    std::span< std::byte > buffer ) -> opt< std::size_t > {
                    return em::cfg::serialize_reg_map_key( m, key, buffer );
            },
            .take_unseen_key_f =
                [&] {
                        return em::cfg::pop_from_container( unseen_keys );
                },
        };

        // XXX: update begins after header!
        em::cfg::update_cbs_bind lb{ lu };
        em::cfg::update_result   res =
            em::cfg::update_stored_config( page_addr, page_addr + page_size_, lb );
        if ( res == em::cfg::update_result::ERROR )
                return ERROR;
        if ( res == em::cfg::update_result::FULL ) {
                em::cfg::hdr_state page_st;
                if ( locate_next( i2c_, dev_addr_, mem_size_, page_size_, page_addr, page_st ) !=
                     SUCCESS )
                        return ERROR;
                // XXX: update header?
                unseen_keys = cfg::map::keys;
                res = em::cfg::update_stored_config( page_addr, page_addr + page_size_, lb );
                if ( res != em::cfg::update_result::SUCCESS )
                        return ERROR;
        }

        return SUCCESS;
}

status i2c_eeprom::load_cfg( cfg::map& m )

{
        auto addr = locate_current( i2c_, dev_addr_, mem_size_, page_size_ );
        if ( !addr )
                return SUCCESS;
        std::byte tmp[42];

        em::static_vector< cfg::key, cfg::map::registers_count > unseen_keys{ cfg::map::keys };

        em::cfg::load_cbs cbs = {
            .buffer = tmp,
            .read_f =
                [&]( std::size_t addr, std::span< std::byte, em::cfg::cell_size > data ) {
                        return read_data( i2c_, dev_addr_, static_cast< uint16_t >( addr ), data );
                },
            .check_key_cache_f = [&]( uint32_t key ) -> em::cfg::cache_res {
                    return em::cfg::key_check_unseen_container( unseen_keys, key );
            },
            .on_kval_f =
                [&]( uint32_t key, std::span< std::byte > data ) {
                        return em::cfg::set_reg_map_key( m, key, data );
                },
        };
        // XXX: load begins after header
        em::cfg::load_cbs_bind liface{ cbs };
        em::result             r = em::cfg::load_stored_config( *addr, *addr + page_size_, liface );
        return r;
}

status i2c_eeprom::clear_cfg()
{
        // XXX: impl
        return ERROR;
}

}  // namespace servio::drv
