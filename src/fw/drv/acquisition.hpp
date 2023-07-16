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

class acquisition : public period_cb_interface
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
        void on_period_irq() override;

        void start();

        void                       set_detailed_callback( adc_detailed_cb_interface& );
        adc_detailed_cb_interface& get_detailed_callback() const;

        void                set_brief_callback( std::size_t i, value_cb_interface& );
        value_cb_interface& get_brief_callback( std::size_t i ) const;

        uint32_t get_val( std::size_t i ) const
        {
                return vals_[i];
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

        adc_detailed_cb_interface* detailed_cb_;
        // note: brief_cbs_[0] is never used as that is detailed one
        std::array< value_cb_interface*, chan_n > brief_cbs_;
        std::array< uint32_t, chan_n >            vals_;

        std::size_t                        period_i_            = 0;
        std::size_t                        detailed_sequence_i_ = 0;
        std::array< detailed_sequence, 2 > detailed_sequences_{};

        handles     h_{};
        std::size_t channel_index_ = 0;
        std::size_t brief_index_   = 1;
};

}  // namespace fw::drv
