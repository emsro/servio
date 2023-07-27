#include "base.hpp"
#include "drv_interfaces.hpp"
#include "fw/drv/callbacks.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/static_vector.h>
#include <emlabcpp/view.h>
#include <span>
#include <stm32g4xx_hal.h>

#pragma once

namespace em = emlabcpp;  // TODO: I hate this

namespace fw::drv
{

struct acquisition_status
{
        bool hal_start_failed = false;
        bool buffer_was_full  = false;
        bool empty_buffer     = false;
};

/// TODO: rename this to better name /o\...
///
/// Pooler to get values from ADC, works with multiple channels in following way:
/// - first channel (id=0) is detailed
/// - other channels are brief
///
/// The detailed channel is scanned via DMA and ADC, the scan starts on one each
/// `on_period_irq`, scans multiple values and ends on another call to `on_period_irq`. The
/// brief channels are always scanned after the detailed one (between the ending on_period
/// call and the next one), however, for brief channels we take only one measurement.
///
/// The idea for servio use case is: one period we do detailed measurements of current and
/// second period we just measure one of the other channels, to give CPU time for other
/// stuff
template < std::size_t N >
class acquisition : public period_cb_interface
{
        static_assert( N > 0, "N has to be bigger than 0" );

public:
        static constexpr std::size_t chan_n        = N;
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

        uint32_t get_val( std::size_t i ) const;

        const acquisition_status& status() const;
        acquisition_status&       status();

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

#include "acquisition_inl.hpp"
