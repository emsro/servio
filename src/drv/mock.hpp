#pragma once

#include "../status.hpp"
#include "interfaces.hpp"

#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

namespace servio::drv::mock
{
struct pwm_mot : drv::pwm_motor_iface
{
        void set_invert( bool ) override
        {
        }

        void set_power( pwr ) override
        {
        }

        void force_stop() override
        {
        }

        bool is_stopped() const override
        {
                return false;
        }

        int8_t get_direction() const override
        {
                return -1;
        };

        pwr get_power() const override
        {
                return 0_pwr;
        }
};

struct pos : drv::get_pos_iface
{
        uint32_t x;

        uint32_t get_position() const override
        {
                return x;
        }

        limits< uint32_t > get_position_range() const override
        {
                return { 0, 1024 };
        }
};

struct curr : drv::get_curr_iface
{
        uint32_t x;

        uint32_t get_current() const override
        {
                return x;
        }
};

struct vcc : drv::vcc_iface
{
        uint32_t x;

        uint32_t get_vcc() const override
        {
                return x;
        }
};

struct temp : drv::temp_iface
{
        int32_t x;

        int32_t get_temperature() const override
        {
                return x;
        }
};

struct stor : drv::storage_iface
{
        std::size_t                                    store_cnt = 0;
        std::size_t                                    clear_cnt = 0;
        std::map< uint32_t, std::vector< std::byte > > records;

        error_code store_cfg( zll::ll_list< cfg::iface >& cfgs ) override
        {
                store_cnt += 1;
                for ( cfg::iface& c : cfgs ) {
                        c.storage_reset_seen();
                        while ( auto const key = c.storage_take_unseen() ) {
                                std::byte  buffer[64];
                                auto const data = c.on_storage_save( *key, buffer );
                                if ( !data )
                                        return status::error;
                                records[cfg_storage_id( c.section, *key )].assign(
                                    data->begin(), data->end() );
                        }
                }
                return status::success;
        }

        error_code load_cfg( zll::ll_list< cfg::iface >& cfgs ) override
        {
                for ( cfg::iface& c : cfgs ) {
                        c.storage_reset_seen();
                        while ( auto const key = c.storage_take_unseen() ) {
                                auto const it = records.find( cfg_storage_id( c.section, *key ) );
                                if ( it == records.end() )
                                        continue;
                                if ( c.on_storage_load( *key, it->second ) != status::success )
                                        return status::error;
                        }
                }
                return status::success;
        }

        error_code clear_cfg() override
        {
                clear_cnt += 1;
                records.clear();
                return status::success;
        }
};

}  // namespace servio::drv::mock
