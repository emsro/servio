#include "fw/drivers/comms.hpp"

#include "fw/util.hpp"

namespace fw
{

bool comms::setup( em::function_view< bool( handles& ) > setup_f )
{
    return setup_f( h_ );
}

void comms::tx_dma_irq()
{
    HAL_DMA_IRQHandler( &h_.tx_dma );
}

void comms::rx_dma_irq()
{
    HAL_DMA_IRQHandler( &h_.rx_dma );
}

void comms::uart_irq()
{
    HAL_UART_IRQHandler( &h_.uart );
}

void comms::rx_cplt_irq( UART_HandleTypeDef* huart )
{
    if ( huart != &h_.uart ) {
        return;
    }

    auto dview = em::view_n( idata_buffer_.begin(), huart->RxXferSize );

    iseq_.insert( dview );
    iseq_.get_message().match(
        [&]( em::protocol::sequencer_read_request next_read ) {
            // TODO: technically, the cast can create an error here
            HAL_UART_Receive_DMA(
                &h_.uart, idata_buffer_.begin(), static_cast< uint16_t >( *next_read ) );
        },
        [&]( auto msg ) {
            imsg_i_               = ( imsg_i_ + 1 ) % imsg_buffer_.size();
            imsg_buffer_[imsg_i_] = msg;
            received_counter_ += 1;
            if ( msg_callback_ ) {
                msg_callback_( imsg_buffer_[imsg_i_] );
            }
        } );
}

void comms::set_msg_callback( msg_callback c )
{
    msg_callback_ = std::move( c );
}

void comms::start_receiving()
{
    HAL_UART_Receive_DMA( &h_.uart, idata_buffer_.begin(), master_to_servo_sequencer::fixed_size );
}

void comms::send_msg( servo_to_master_message msg )
{
    if ( !( HAL_UART_GetState( &h_.uart ) & HAL_UART_STATE_READY ) ) {
        stop_exec();
    }
    omsg_ = msg;
    static_assert( servo_to_master_message::capacity < std::numeric_limits< uint16_t >::max() );
    // TODO: problematic cast
    HAL_UART_Transmit_DMA( &h_.uart, omsg_.begin(), static_cast< uint16_t >( omsg_.size() ) );
    sent_counter_ += 1;
}

void comms::transmit( servo_to_master_variant var )
{
    send_msg( servo_to_master_serialize( var ) );
}

}  // namespace fw
