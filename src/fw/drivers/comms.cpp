#include "fw/drivers/comms.hpp"

#include "emlabcpp/experimental/cobs.h"
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

void comms::uart_irq()
{
        HAL_UART_IRQHandler( &h_.uart );
}

void comms::rx_cplt_irq( UART_HandleTypeDef* huart )
{
        if ( huart != &h_.uart ) {
                return;
        }

        ibuffer_.push_back( rx_byte_ );
        start();
}

std::tuple< bool, em::view< std::byte* > > comms::load_message( em::view< std::byte* > data )
{
        auto cobv = em::cobs_decode_view( em::view{ ibuffer_ } );

        if ( cobv.size() > data.size() ) {
                return { false, em::view< std::byte* >{} };
        }
        copy( cobv, data.begin() );
        return { true, em::view_n( data.begin(), cobv.size() ) };
}

void comms::start()
{
        // TODO: return value ignored
        HAL_UART_Receive_IT( &h_.uart, reinterpret_cast< uint8_t* >( &rx_byte_ ), 1 );
}

void comms::send( em::view< std::byte* > data )
{
        while ( !( HAL_UART_GetState( &h_.uart ) & HAL_UART_STATE_READY ) ) {
                asm( "nop" );
        }

        auto [succ, used] = em::encode_cobs( data, otmp_ );
        if ( !succ ) {
                return;
        }

        // TODO: problematic cast
        // TODO: return value ignored
        HAL_UART_Transmit_DMA(
            &h_.uart,
            reinterpret_cast< uint8_t* >( otmp_.begin() ),
            static_cast< uint16_t >( used.size() ) );
}

}  // namespace fw
