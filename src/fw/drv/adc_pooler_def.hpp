
#include "fw/drv/adc_pooler.hpp"

#pragma once

namespace servio::fw::drv
{

enum chan_ids
{
        CURRENT_CHANNEL,
        POSITION_CHANNEL,
        VCC_CHANNEL,
        TEMP_CHANNEL,
};

struct adc_set
{
        using id_type = chan_ids;

        fw::drv::detailed_adc_channel< CURRENT_CHANNEL, 128 >  current;
        fw::drv::adc_channel_with_callback< POSITION_CHANNEL > position;
        fw::drv::adc_channel< VCC_CHANNEL >                    vcc;
        fw::drv::adc_channel< TEMP_CHANNEL >                   temp;

        auto tie()
        {
                return std::tie( current, position, vcc, temp );
        }
};

fw::drv::adc_pooler< adc_set > ADC_POOLER{};

struct : period_cb_interface
{
        void on_period_irq() override
        {
                ADC_POOLER.on_period_irq();
        }
} ADC_PERIOD_CB;

struct : vcc_interface
{
        status get_status() const override
        {
                return ADC_POOLER->vcc.get_status();
        }

        uint32_t get_vcc() const override
        {
                return ADC_POOLER->vcc.last_value;
        }
} ADC_VCC;

struct : temperature_interface
{
        status get_status() const override
        {
                return ADC_POOLER->temp.get_status();
        }

        uint32_t get_temperature() const override
        {
                return ADC_POOLER->temp.last_value;
        }
} ADC_TEMPERATURE;

struct : position_interface
{
        status get_status() const override
        {
                return ADC_POOLER->position.get_status();
        }

        uint32_t get_position() const override
        {
                return ADC_POOLER->position.last_value;
        }

        void set_position_callback( position_cb_interface& cb ) override
        {
                ADC_POOLER->position.callback = &cb;
        }
        position_cb_interface& get_position_callback() const override
        {
                return *ADC_POOLER->position.callback;
        }
} ADC_POSITION;

struct : current_interface
{
        status get_status() const override
        {
                return ADC_POOLER->current.get_status();
        }

        void clear_status() override
        {
                ADC_POOLER->current.clear_status();
        }

        uint32_t get_current() const override
        {
                return ADC_POOLER->current.last_value;
        }

        void set_current_callback( current_cb_interface& cb ) override
        {
                ADC_POOLER->current.callback = &cb;
        }
        current_cb_interface& get_current_callback() const override
        {
                return *ADC_POOLER->current.callback;
        }
} ADC_CURRENT;

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


}  // namespace servio::fw::drv
