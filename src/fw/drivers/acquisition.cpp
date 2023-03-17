#include "fw/drivers/acquisition.hpp"

#include "fw/util.hpp"

namespace fw
{

bool acquisition::setup( em::function_view< bool( handles& ) > setup_f )
{
        return setup_f( h_ );
}
void acquisition::dma_irq()
{
        HAL_DMA_IRQHandler( &h_.dma );
}

void acquisition::adc_irq()
{
        HAL_ADC_IRQHandler( &h_.adc );
}

void acquisition::adc_error_irq( ADC_HandleTypeDef* h )
{
        std::ignore = h;
        stop_exec();
}

void acquisition::adc_conv_cplt_irq( ADC_HandleTypeDef* h )
{
        if ( h != &h_.adc ) {
                return;
        }
        switch ( adc_state_ ) {
        case READ_POSITION:
                HAL_ADC_Stop_IT( &h_.adc );
                position_ = HAL_ADC_GetValue( &h_.adc );
                position_cb_->on_position( position_ );
                break;
        case READ_VCC:
                HAL_ADC_Stop_IT( &h_.adc );
                vcc_ = HAL_ADC_GetValue( &h_.adc );
                break;
        case READ_TEMP:
                HAL_ADC_Stop_IT( &h_.adc );
                temp_ = HAL_ADC_GetValue( &h_.adc );
                break;
        case READ_CURRENT:
                break;
        }
        switch_adc_channel( READ_CURRENT );
}

void acquisition::start()
{
        HAL_TIM_OC_Start( &h_.tim, h_.tim_channel );
}

void acquisition::set_current_callback( current_cb_interface& cb )
{
        current_cb_ = &cb;
}
current_cb_interface& acquisition::get_current_callback()
{
        return *current_cb_;
}

void acquisition::switch_adc_channel( adc_states state )
{
        ADC_ChannelConfTypeDef* chconf;
        switch ( state ) {
        case READ_POSITION:
                chconf = &h_.position_chconf;
                break;
        case READ_VCC:
                chconf = &h_.vcc_chconf;
                break;
        case READ_TEMP:
                chconf = &h_.temp_chconf;
                break;
        case READ_CURRENT:
                chconf = &h_.current_chconf;
                break;
        default:
                stop_exec();
                return;
        }
        HAL_StatusTypeDef res = HAL_ADC_ConfigChannel( &h_.adc, chconf );
        if ( res != HAL_OK ) {
                stop_exec();
        }
        adc_state_ = state;
}
void acquisition::start_simple_reading()
{
        HAL_StatusTypeDef res = HAL_ADC_Start_IT( &h_.adc );
        if ( res != HAL_OK ) {
                stop_exec();
        }
}

acquisition::adc_states acquisition::next_side_state( adc_states inpt )
{
        switch ( inpt ) {
        case READ_POSITION:
                return READ_VCC;
        case READ_VCC:
                // TODO: this skips the temp reading, turns out that for some reason READ_TEMP is
                // much slower
                //            return READ_TEMP;
                return READ_POSITION;
        case READ_TEMP:
                return READ_POSITION;
        case READ_CURRENT:
                stop_exec();
        }
        return READ_POSITION;
}

void acquisition::set_position_callback( position_cb_interface& cb )
{
        position_cb_ = &cb;
}
position_cb_interface& acquisition::get_position_callback()
{
        return *position_cb_;
}

void acquisition::period_elapsed_irq()
{
        HAL_StatusTypeDef res           = HAL_OK;
        std::size_t       next_buffer_i = ( current_sequence_i_ + 1 ) % current_sequences_.size();
        current_sequence& next_se       = current_sequences_[next_buffer_i];

        if ( period_i_ % 2 == 0 ) {
                res = HAL_ADC_Start_DMA(
                    &h_.adc,
                    reinterpret_cast< uint32_t* >( &next_se.buffer ),
                    current_sequence::buffer_size );
                if ( res != HAL_OK ) {
                        stop_exec();
                }
        } else {
                next_se.used = current_sequence::buffer_size - __HAL_DMA_GET_COUNTER( &h_.dma );
                // For some reason the Stop is necessary to call even thou the ADC should've
                // finished by now
                res = HAL_ADC_Stop_DMA( &h_.adc );
                if ( res != HAL_OK ) {
                        stop_exec();
                }
                if ( next_se.used == 0 ) {
                        stop_exec();
                }
                if ( next_se.used == current_sequence::buffer_size ) {
                        status_.buffer_was_full = true;
                }

                std::span readings( &next_se.buffer[0], next_se.used );

                current_ = em::avg( readings );
                current_cb_->on_current( current_, readings );
                current_sequence_i_ = next_buffer_i;

                side_state_ = next_side_state( side_state_ );
                switch_adc_channel( side_state_ );
                start_simple_reading();
        }

        period_i_ += 1;
}

em::view< const uint16_t* > acquisition::get_current_reading() const
{
        const current_sequence& se = current_sequences_[current_sequence_i_];
        return em::view_n( &se.buffer[0], se.used );
}

}  // namespace fw
