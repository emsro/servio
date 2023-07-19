#include "fw/drv/cobs_uart.hpp"

#include "fw/util.hpp"

namespace fw::drv
{

bool cobs_uart::setup( em::function_view< bool( handles& ) > setup_f )
{
        return setup_f( h_ );
}

void cobs_uart::tx_dma_irq()
{
        HAL_DMA_IRQHandler( &h_.tx_dma );
}

void cobs_uart::uart_irq()
{
        HAL_UART_IRQHandler( &h_.uart );
}

void cobs_uart::rx_cplt_irq( UART_HandleTypeDef* huart )
{
        if ( huart != &h_.uart ) {
                return;
        }

        if ( rx_byte_ == std::byte{ 0 } ) {
                isizes_.push_back( current_size_ - 1 );
                current_size_ = 0;
        } else if ( current_size_ == 0 ) {
                cd_ = em::cobs_decoder{ rx_byte_ };
                current_size_ += 1;
        } else {
                current_size_ += 1;
                const std::byte b = cd_.iter( rx_byte_ );
                ibuffer_.push_back( b );
        }
        start();
}

void cobs_uart::tx_cplt_irq( UART_HandleTypeDef* huart )
{
        if ( huart != &h_.uart ) {
                return;
        }
        tx_done_ = true;
}

std::tuple< bool, em::view< std::byte* > > cobs_uart::load_message( em::view< std::byte* > data )
{
        if ( isizes_.empty() ) {
                return { true, {} };
        }
        if ( isizes_.front() > data.size() ) {
                return { false, em::view< std::byte* >{} };
        }
        const uint16_t size  = isizes_.take_front();
        em::view       dview = em::view_n( data.begin(), size );

        for ( std::byte& b : dview ) {
                b = ibuffer_.take_front();
        }

        return { true, dview };
}

void cobs_uart::start()
{
        // TODO: return value ignored
        hal_check{} << HAL_UART_Receive_IT(
            &h_.uart, reinterpret_cast< uint8_t* >( &rx_byte_ ), 1 );
}

bool cobs_uart::send( em::view< const std::byte* > data )
{
        // TODO: well, timeout would be better
        while ( !tx_done_ ) {
                asm( "nop" );
        }

        auto [succ, used] = em::encode_cobs( data, otmp_ );
        if ( !succ ) {
                return false;
        }
        // TODO: this might fial
        otmp_[used.size()] = std::byte{ 0 };

        tx_done_ = false;

        // TODO: problematic cast
        // TODO: return value ignored
        hal_check{} << HAL_UART_Transmit_DMA(
            &h_.uart,
            reinterpret_cast< uint8_t* >( otmp_.begin() ),
            static_cast< uint16_t >( used.size() + 1 ) );

        return true;
}

}  // namespace fw::drv
