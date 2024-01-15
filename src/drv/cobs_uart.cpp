#include "drv/cobs_uart.hpp"

#include "fw/util.hpp"

namespace servio::drv
{

base::com_res cobs_uart::load_message( em::view< std::byte* > data )
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

em::result cobs_uart::send( em::view< const std::byte* > data )
{
        // TODO: well, timeout would be better
        while ( !tx_done_ ) {
                asm( "nop" );
        }

        auto [succ, used] = em::encode_cobs( data, otmp_ );
        if ( !succ ) {
                return em::ERROR;
        }
        // TODO: this might fial
        otmp_[used.size()] = std::byte{ 0 };

        tx_done_ = false;

        // TODO: problematic cast
        if ( HAL_UART_Transmit_DMA(
                 uart_,
                 reinterpret_cast< uint8_t* >( otmp_.begin() ),
                 static_cast< uint16_t >( used.size() + 1 ) ) != HAL_OK ) {
                return em::ERROR;
        }

        return em::SUCCESS;
}

}  // namespace servio::drv
