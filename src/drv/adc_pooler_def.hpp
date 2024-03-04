
#include "base/drv_interfaces.hpp"
#include "drv/adc_pooler.hpp"

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

template < auto& CentralSentry >
struct adc_set
{
        using id_type = chan_ids;

        drv::detailed_adc_channel< CURRENT_CHANNEL, 128 >  current{ { "current", CentralSentry } };
        drv::adc_channel_with_callback< POSITION_CHANNEL > position{ "position", CentralSentry };
        drv::adc_channel< VCC_CHANNEL >                    vcc{ { "vcc", CentralSentry } };
        drv::adc_channel< TEMP_CHANNEL >                   temp{ { "temp", CentralSentry } };

        [[gnu::flatten]] auto tie()
        {
                return std::tie( current, position, vcc, temp );
        }
};

template < auto& AdcPooler >
struct adc_pooler_period_cb : base::period_cb_interface
{
        void on_period_irq() override
        {
                AdcPooler.on_period_irq();
        }
};

template < auto& AdcPooler >
struct adc_pooler_vcc : base::vcc_interface
{
        uint32_t get_vcc() const override
        {
                return AdcPooler->vcc.last_value;
        }
};

template < auto& AdcPooler >
struct adc_pooler_temperature : base::temperature_interface
{
        uint32_t get_temperature() const override
        {
                return AdcPooler->temp.last_value;
        }
};

template < auto& AdcPooler >
struct adc_pooler_position : base::position_interface
{
        uint32_t get_position() const override
        {
                return AdcPooler->position.last_value;
        }

        void set_position_callback( base::position_cb_interface& cb ) override
        {
                AdcPooler->position.callback = &cb;
        }

        base::position_cb_interface& get_position_callback() const override
        {
                return *AdcPooler->position.callback;
        }
};

template < auto& AdcPooler >
struct adc_pooler_current : base::current_interface
{
        uint32_t get_current() const override
        {
                return AdcPooler->current.last_value;
        }

        void set_current_callback( base::current_cb_interface& cb ) override
        {
                AdcPooler->current.callback = &cb;
        }

        base::current_cb_interface& get_current_callback() const override
        {
                return *AdcPooler->current.callback;
        }
};

auto ADC_SEQUENCE = std::array{
    CURRENT_CHANNEL,
    POSITION_CHANNEL,
    CURRENT_CHANNEL,
    VCC_CHANNEL,
    CURRENT_CHANNEL,
    POSITION_CHANNEL,
    CURRENT_CHANNEL,
    TEMP_CHANNEL,
};

}  // namespace servio::drv
