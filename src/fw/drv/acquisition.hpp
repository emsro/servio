#include "base.hpp"
#include "fw/drv/callbacks.hpp"
#include "fw/drv/interfaces.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/static_vector.h>
#include <emlabcpp/view.h>
#include <span>
#include <stm32g4xx_hal.h>

#pragma once

namespace em = emlabcpp;  // I hate this

namespace fw::drv
{

struct acquisition_status
{
        bool start_failed    = false;
        bool buffer_was_full = false;
};

class acquisition : public position_interface,
                    public current_interface,
                    public vcc_interface,
                    public temperature_interface,
                    public period_cb_interface
{
public:
        static constexpr std::size_t chan_n        = 4;
        static constexpr std::size_t detailed_chid = 0;

        struct handles
        {
                ADC_HandleTypeDef                                   adc;
                DMA_HandleTypeDef                                   dma;
                TIM_HandleTypeDef                                   tim;
                uint32_t                                            tim_channel;
                em::static_vector< ADC_ChannelConfTypeDef, chan_n > chconfs;
        };

        struct detailed_sequence
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
        void on_period_irq();

        void start();

        // Callback is called each time current measurement of one PWM pulse.
        // The callback gets average current during that pulse.
        // (Does not happen every pulse!)
        void                  set_current_callback( current_cb_interface& ) override;
        current_cb_interface& get_current_callback() const override;

        // Callback is called each time new position is read, the argument is the position
        void                   set_position_callback( position_cb_interface& ) override;
        position_cb_interface& get_position_callback() const override;

        uint32_t get_current() const override
        {
                return vals_[0];
        }
        uint32_t get_position() const override
        {
                return vals_[1];
        }
        uint32_t get_vcc() const override
        {
                return vals_[2];
        }
        uint32_t get_temperature() const override
        {
                return vals_[3];
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
        void switch_channel( std::size_t i );
        void start_brief_reading();

        acquisition_status status_;

        current_cb_interface*  current_cb_;
        position_cb_interface* position_cb_;

        std::array< uint32_t, chan_n > vals_;

        std::size_t                        period_i_            = 0;
        std::size_t                        detailed_sequence_i_ = 0;
        std::array< detailed_sequence, 2 > detailed_sequences_{};

        handles     h_{};
        std::size_t channel_index_ = 0;
        std::size_t brief_index_   = 1;
};

}  // namespace fw::drv
