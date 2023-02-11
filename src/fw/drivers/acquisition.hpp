#include "conversion/current.hpp"
#include "conversion/position.hpp"
#include "conversion/temperature.hpp"
#include "conversion/voltage.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/static_function.h>
#include <emlabcpp/view.h>
#include <span>
#include <stm32g4xx_hal.h>

#pragma once

namespace em = emlabcpp;  // I hate this

namespace fw
{

struct acquisition_status
{
        bool buffer_was_full = false;
};

class acquisition
{
        enum adc_states
        {
                READ_POSITION,
                READ_VCC,
                READ_TEMP,
                READ_CURRENT
        };

public:
        struct handles
        {
                ADC_HandleTypeDef      adc;
                DMA_HandleTypeDef      dma;
                TIM_HandleTypeDef      tim;
                uint32_t               tim_channel;
                ADC_ChannelConfTypeDef current_chconf;
                ADC_ChannelConfTypeDef position_chconf;
                ADC_ChannelConfTypeDef vcc_chconf;
                ADC_ChannelConfTypeDef temp_chconf;
        };

        // seqeunce of current measurements from adc
        struct current_sequence
        {
                static constexpr std::size_t buffer_size = 128;
                alignas( uint32_t ) uint16_t buffer[buffer_size];
                std::size_t used;
        };

        using current_callback  = em::static_function< void( float ), 16 >;
        using position_callback = em::static_function< void( float ), 16 >;

        acquisition() = default;

        bool setup( em::function_view< bool( handles& ) > );

        void adc_irq();
        void adc_conv_cplt_irq( ADC_HandleTypeDef* h );
        void adc_error_irq( ADC_HandleTypeDef* h );
        void dma_irq();
        void period_elapsed_irq( int8_t power_direction );

        void start();

        void set_position_cfg(
            uint16_t low_value,
            float    low_angle,
            uint16_t high_value,
            float    high_angle );
        void set_current_cfg( float scale, float offset );
        void set_temp_cfg( float scale, float offset );
        void set_vcc_cfg( float scale );

        // Callback is called each time current measurement of one PWM pulse.
        // The callback gets average current during that pulse.
        // (Does not happen every pulse!)
        void                    set_current_callback( current_callback );
        const current_callback& get_current_callback();

        // Access last measured current reading
        em::view< const uint16_t* > get_current_reading() const;

        // Callback is called each time new position is read, the argument is the position
        void                     set_position_callback( position_callback );
        const position_callback& get_position_callback();

        float get_current() const
        {
                return current_;
        }
        float get_position() const
        {
                return position_;
        }
        float get_vcc() const
        {
                return vcc_;
        }
        float get_temp() const
        {
                return vcc_;
        }

        const acquisition_status& get_status() const
        {
                return status_;
        }

        void clear_status()
        {
                status_ = acquisition_status{};
        }

private:
        void       switch_adc_channel( adc_states );
        adc_states next_side_state( adc_states );
        void       start_simple_reading();

        acquisition_status status_;

        current_callback  current_cb_;
        position_callback position_cb_;

        float position_;
        float temp_;
        float vcc_;
        float current_;

        std::size_t                       period_i_           = 0;
        std::size_t                       current_sequence_i_ = 0;
        std::array< current_sequence, 2 > current_sequences_{};

        handles    h_{};
        adc_states adc_state_  = READ_CURRENT;
        adc_states side_state_ = READ_POSITION;

        position_converter    pos_conver_;
        current_converter     curr_conver_;
        temperature_converter temp_conver_;
        voltage_converter     volt_conver_;
};

}  // namespace fw
