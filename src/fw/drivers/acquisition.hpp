#include "base.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/view.h>
#include <span>
#include <stm32g4xx_hal.h>

#pragma once

namespace em = emlabcpp;  // I hate this

namespace fw
{

struct acquisition_status
{
        bool start_failed    = false;
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

        acquisition();

        bool setup( em::function_view< bool( handles& ) > );

        void adc_irq();
        void adc_conv_cplt_irq( ADC_HandleTypeDef* h );
        void adc_error_irq( ADC_HandleTypeDef* h );
        void dma_irq();
        void period_elapsed_irq();

        void start();

        // Callback is called each time current measurement of one PWM pulse.
        // The callback gets average current during that pulse.
        // (Does not happen every pulse!)
        void                  set_current_callback( current_cb_interface& );
        current_cb_interface& get_current_callback();

        // Access last measured current reading
        em::view< const uint16_t* > get_current_reading() const;

        // Callback is called each time new position is read, the argument is the position
        void                   set_position_callback( position_cb_interface& );
        position_cb_interface& get_position_callback();

        uint32_t get_current() const
        {
                return current_;
        }
        uint32_t get_position() const
        {
                return position_;
        }
        uint32_t get_vcc() const
        {
                return vcc_;
        }
        uint32_t get_temp() const
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

        current_cb_interface*  current_cb_;
        position_cb_interface* position_cb_;

        uint32_t position_;
        uint32_t temp_;
        uint32_t vcc_;
        uint32_t current_;

        std::size_t                       period_i_           = 0;
        std::size_t                       current_sequence_i_ = 0;
        std::array< current_sequence, 2 > current_sequences_{};

        handles    h_{};
        adc_states adc_state_  = READ_CURRENT;
        adc_states side_state_ = READ_POSITION;
};

}  // namespace fw
