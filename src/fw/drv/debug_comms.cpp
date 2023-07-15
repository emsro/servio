#include "fw/drv/debug_comms.hpp"

#include "fw/util.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/iterators/numeric.h>

namespace fw::drv
{

bool debug_comms::setup( em::function_view< bool( handles& ) > setup_f )
{
        return setup_f( h_ );
}

void debug_comms::tx_dma_irq()
{
        HAL_DMA_IRQHandler( &h_.tx_dma );
}

void debug_comms::uart_irq()
{
        HAL_UART_IRQHandler( &h_.uart );
}

void debug_comms::transmit( std::span< const std::byte > inpt )
{
        while ( ( HAL_UART_GetState( &h_.uart ) & HAL_UART_STATE_BUSY_TX ) ==
                HAL_UART_STATE_BUSY_TX ) {
                asm( "nop" );  // TODO: fix this properly
        }
        if ( inpt.size() > odata_buffer_.size() ) {
                stop_exec();
        }
        // TODO: size check!
        std::copy( inpt.begin(), inpt.end(), odata_buffer_.begin() );

        hal_check{} << HAL_UART_Transmit_DMA(
            &h_.uart,
            reinterpret_cast< uint8_t* >( odata_buffer_.begin() ),
            static_cast< uint16_t >( inpt.size() ) );
}

em::static_vector< std::byte, 32 > debug_comms::receive( std::size_t size )
{
        if ( idata_buffer_.size() < size ) {
                HAL_Delay( 10 );
        }
        size = std::min( idata_buffer_.size(), size );
        em::static_vector< std::byte, 32 > data;
        for ( std::size_t i = 0; i < size; i++ ) {
                data.push_back( idata_buffer_.take_front() );
        }
        return data;
}

void debug_comms::start()
{
        hal_check{} << HAL_UART_Receive_IT( &h_.uart, reinterpret_cast< uint8_t* >( &ibyte_ ), 1 );
}

void debug_comms::rx_cplt_irq( UART_HandleTypeDef* huart )
{
        if ( huart != &h_.uart ) {
                return;
        }

        idata_buffer_.push_back( ibyte_ );
        start();
}

}  // namespace fw::drv
