#include "fw/util.hpp"

namespace fw::drv
{

template < std::size_t N >
acquisition< N >::acquisition()
{
        static empty_value_cb        EMPTY_VALUE_CB;
        static empty_adc_detailed_cb EMPTY_ADC_DETAILED_CB;

        detailed_cb_ = &EMPTY_ADC_DETAILED_CB;
        for ( value_cb_interface*& ptr : brief_cbs_ ) {
                ptr = &EMPTY_VALUE_CB;
        }
}

template < std::size_t N >
bool acquisition< N >::setup( em::function_view< bool( handles& ) > setup_f )
{
        return setup_f( h_ );
}

template < std::size_t N >
void acquisition< N >::adc_irq()
{
        HAL_ADC_IRQHandler( &h_.adc );
}

template < std::size_t N >
void acquisition< N >::adc_error_irq( ADC_HandleTypeDef* h )
{
        std::ignore = h;
        // TODO: well this was aggresive, convert to a flag
        // stop_exec();
}

template < std::size_t N >
void acquisition< N >::adc_conv_cplt_irq( ADC_HandleTypeDef* h )
{
        if ( h != &h_.adc ) {
                return;
        }
        // TODO: call callbacks properly

        if ( channel_index_ != detailed_chid ) {
                std::ignore           = HAL_ADC_Stop_IT( &h_.adc );
                vals_[channel_index_] = HAL_ADC_GetValue( &h_.adc );
                brief_cbs_[channel_index_]->on_value_irq( vals_[channel_index_] );
        }

        switch_channel( detailed_chid );
}

template < std::size_t N >
void acquisition< N >::dma_irq()
{
        HAL_DMA_IRQHandler( &h_.dma );
}

template < std::size_t N >
void acquisition< N >::on_period_irq()
{
        HAL_StatusTypeDef res           = HAL_OK;
        const std::size_t next_buffer_i = ( detailed_sequence_i_ + 1 ) % detailed_sequences_.size();
        detailed_sequence& next_se      = detailed_sequences_[next_buffer_i];

        if ( period_i_ % 2 == 0 ) {
                res = HAL_ADC_Start_DMA(
                    &h_.adc,
                    reinterpret_cast< uint32_t* >( &next_se.buffer ),
                    detailed_sequence::buffer_size );
                if ( res == HAL_OK ) {
                        period_i_ += 1;
                } else {
                        status_.hal_start_failed = true;
                }
        } else {
                next_se.used = detailed_sequence::buffer_size - __HAL_DMA_GET_COUNTER( &h_.dma );
                // For some reason the Stop is necessary to call even thou the ADC should've
                // finished by now
                res = HAL_ADC_Stop_DMA( &h_.adc );
                if ( res != HAL_OK ) {
                        stop_exec();
                }
                if ( next_se.used == 0 ) {
                        status_.empty_buffer = true;
                }
                if ( next_se.used == detailed_sequence::buffer_size ) {
                        status_.buffer_was_full = true;
                }

                const std::span readings( &next_se.buffer[0], next_se.used );

                vals_[detailed_chid] = em::avg( readings );
                detailed_cb_->on_value_irq( vals_[detailed_chid], readings );
                detailed_sequence_i_ = next_buffer_i;

                brief_index_ = ( brief_index_ + 1 ) % h_.chconfs.size();
                brief_index_ = brief_index_ == 0 ? 1 : brief_index_;
                switch_channel( brief_index_ );
                start_brief_reading();
                period_i_ += 1;
        }
}

template < std::size_t N >
void acquisition< N >::start()
{
        std::ignore = HAL_TIM_OC_Start( &h_.tim, h_.tim_channel );
}

template < std::size_t N >
void acquisition< N >::set_detailed_callback( adc_detailed_cb_interface& cb )
{
        detailed_cb_ = &cb;
}

template < std::size_t N >
adc_detailed_cb_interface& acquisition< N >::get_detailed_callback() const
{
        return *detailed_cb_;
}

template < std::size_t N >
void acquisition< N >::set_brief_callback( std::size_t i, value_cb_interface& cb )
{
        brief_cbs_[i] = &cb;
}

template < std::size_t N >
value_cb_interface& acquisition< N >::get_brief_callback( std::size_t i ) const
{
        return *brief_cbs_[i];
}

template < std::size_t N >
uint32_t acquisition< N >::get_val( std::size_t i ) const
{
        return vals_[i];
}

template < std::size_t N >
const acquisition_status& acquisition< N >::status() const
{
        return status_;
}

template < std::size_t N >
acquisition_status& acquisition< N >::status()
{
        return status_;
}

template < std::size_t N >
void acquisition< N >::switch_channel( std::size_t chid )
{
        const HAL_StatusTypeDef res = HAL_ADC_ConfigChannel( &h_.adc, &h_.chconfs[chid] );
        if ( res != HAL_OK ) {
                stop_exec();
        }
        channel_index_ = chid;
}

template < std::size_t N >
void acquisition< N >::start_brief_reading()
{
        const HAL_StatusTypeDef res = HAL_ADC_Start_IT( &h_.adc );
        if ( res != HAL_OK ) {
                stop_exec();
        }
}

}  // namespace fw::drv
