
#include "drv/adc_pooler.hpp"
#include "drv/interfaces.hpp"

#pragma once

namespace servio::drv
{

enum chan_ids
{
        CURRENT_CHANNEL  = 0x1,
        POSITION_CHANNEL = 0x2,
        VCC_CHANNEL      = 0x3,
        TEMP_CHANNEL     = 0x4,
};

template < auto& AdcPooler >
struct adc_pooler_period_cb : period_cb_iface
{
        void on_period_irq() override
        {
                AdcPooler.on_period_irq();
        }
};

template < auto& AdcPooler >
struct adc_pooler_vcc : vcc_iface
{
        uint32_t get_vcc() const override
        {
                return AdcPooler->vcc.last_value;
        }
};

template < auto& AdcPooler >
struct adc_pooler_temperature : temp_iface
{
        int32_t get_temperature() const override
        {
                return static_cast< int32_t >( AdcPooler->temp.last_value );
        }
};

template < auto& AdcPooler >
struct adc_pooler_position : pos_iface
{
        uint32_t get_position() const override
        {
                return AdcPooler->position.last_value;
        }

        void set_position_callback( position_cb_iface& cb ) override
        {
                AdcPooler->position.callback = &cb;
        }

        position_cb_iface& get_position_callback() const override
        {
                return *AdcPooler->position.callback;
        }
};

template < auto& AdcPooler >
struct adc_pooler_current : curr_iface
{
        uint32_t get_current() const override
        {
                return AdcPooler->current.last_value;
        }

        void set_current_callback( current_cb_iface& cb ) override
        {
                AdcPooler->current.callback = &cb;
        }

        current_cb_iface& get_current_callback() const override
        {
                return *AdcPooler->current.callback;
        }
};

chan_ids ADC_FULL_SEQUENCE[] = {
    CURRENT_CHANNEL,
    POSITION_CHANNEL,
    CURRENT_CHANNEL,
    VCC_CHANNEL,
    CURRENT_CHANNEL,
    POSITION_CHANNEL,
    CURRENT_CHANNEL,
    TEMP_CHANNEL,
};

chan_ids ADC_CURR_POS_VCC_SEQUENCE[] = {
    CURRENT_CHANNEL,
    POSITION_CHANNEL,
    CURRENT_CHANNEL,
    VCC_CHANNEL,
};

chan_ids ADC_CURR_VCC_SEQUENCE[] = {
    CURRENT_CHANNEL,
    VCC_CHANNEL,
};

}  // namespace servio::drv
