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

void comms::uart_irq()
{
        HAL_UART_IRQHandler( &h_.uart );
}

void comms::rx_cplt_irq( UART_HandleTypeDef* huart )
{
        if ( huart != &h_.uart ) {
                return;
        }

        if ( rx_byte_ == std::byte{ 0 } ) {
                isizes_.push_back( current_size_ - 1 );
                current_size_ = 0;
        } else if ( current_size_ == 0 ) {
                current_size_ += 1;
                // TODO: add this as constructor to decoder
                cd_ = em::cobs_decoder{ static_cast< uint8_t >( rx_byte_ ) };
        } else {
                current_size_ += 1;
                ibuffer_.push_back( cd_.iter( rx_byte_ ) );
        }
        start();
}

std::tuple< bool, em::view< std::byte* > > comms::load_message( em::view< std::byte* > data )
{
        if ( isizes_.empty() ) {
                return { true, {} };
        }
        if ( isizes_.front() > data.size() ) {
                return { false, em::view< std::byte* >{} };
        }
        uint16_t size  = isizes_.take_front();
        em::view dview = em::view_n( data.begin(), size );

        for ( std::byte& b : dview ) {
                b = ibuffer_.take_front();
        }

        return { true, em::view_n( data.begin(), size ) };
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
        // TODO: this might fial
        otmp_[used.size()] = std::byte{ 0 };

        // TODO: problematic cast
        // TODO: return value ignored
        HAL_UART_Transmit_DMA(
            &h_.uart,
            reinterpret_cast< uint8_t* >( otmp_.begin() ),
            static_cast< uint16_t >( used.size() + 1 ) );
}

}  // namespace fw
