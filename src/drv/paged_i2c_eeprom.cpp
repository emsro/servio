
#include "./paged_i2c_eeprom.hpp"

#include "../base.hpp"
#include "../fw/util.hpp"
#include "../status.hpp"

#include <emlabcpp/experimental/cfg/handler.h>
#include <emlabcpp/experimental/cfg/page.h>
#include <span>

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
        auto                      stat    = HAL_I2C_Mem_Write(
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

}  // namespace

status i2c_eeprom::store_cfg( cfg::map const& m )
{
        em::static_vector< uint32_t, cfg::ids.size() > unseen_ids{ cfg::ids };

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
            .check_key_cache_f = [&]( uint32_t id ) -> em::cfg::cache_res {
                    return em::cfg::key_check_unseen_container( unseen_ids, id );
            },
            .value_changed_f = [&]( uint32_t id, std::span< std::byte > data ) -> bool {
                    auto ptr = m.ref_by_id( id );
                    if ( !ptr )  // XXX: maybe rethink?
                            return false;
                    return ptr.vref().visit( [&]< typename T >( T const& val ) {
                            auto newval = em::cfg::get_val< T >( data );
                            if ( newval )
                                    return val != *newval;
                            return false;
                    } );
            },
            .serialize_key_f = [&]( uint32_t               id,
                                    std::span< std::byte > buffer ) -> opt< std::size_t > {
                    opt< std::size_t > res;
                    auto               ptr = m.ref_by_id( id );
                    if ( !ptr )
                            return res;
                    ptr.vref().visit( [&]< typename T >( T const& val ) {
                            auto ran = em::cfg::store_kval( id, val, buffer );
                            if ( ran )
                                    res = ran->size();
                    } );
                    return res;
            },
            .take_unseen_key_f =
                [&] {
                        return em::cfg::pop_from_container( unseen_ids );
                },
            .reset_keys_f = [&]() -> em::result {
                    unseen_ids = cfg::ids;
                    return em::result::SUCCESS;
            } };

        em::cfg::update_cbs_bind lb{ lu };
        auto                     res = em::cfg::update( mem_size_, page_size_, lb );
        return res;
}

status i2c_eeprom::load_cfg( cfg::map& m )
{
        std::byte tmp[42];

        em::static_vector< uint32_t, cfg::ids.size() > unseen_ids{ cfg::ids };

        em::cfg::load_cbs cbs = {
            .buffer = tmp,
            .read_f =
                [&]( std::size_t addr, std::span< std::byte, em::cfg::cell_size > data ) {
                        return read_data( i2c_, dev_addr_, static_cast< uint16_t >( addr ), data );
                },
            .check_key_cache_f = [&]( uint32_t id ) -> em::cfg::cache_res {
                    return em::cfg::key_check_unseen_container( unseen_ids, id );
            },
            .on_kval_f = [&]( uint32_t id, std::span< std::byte > data ) -> em::result {
                    auto ptr = m.ref_by_id( id );
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
            },
        };
        em::cfg::load_cbs_bind liface{ cbs };
        auto                   r = em::cfg::load( mem_size_, page_size_, liface );
        return r;
}

status i2c_eeprom::clear_cfg()
{
        // XXX: impl
        return ERROR;
}

}  // namespace servio::drv
