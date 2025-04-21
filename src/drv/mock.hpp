#pragma once

#include "../status.hpp"
#include "interfaces.hpp"

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
        std::size_t store_cnt = 0;
        std::size_t clear_cnt = 0;

        status store_cfg( cfg::map const& ) override
        {
                store_cnt += 1;
                return SUCCESS;
        }

        status load_cfg( cfg::map& data ) override
        {
                std::ignore = data;
                return SUCCESS;
        }

        status clear_cfg() override
        {
                clear_cnt += 1;
                return SUCCESS;
        }
};

}  // namespace servio::drv::mock
