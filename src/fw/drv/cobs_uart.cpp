#include "fw/drv/cobs_uart.hpp"

#include "fw/util.hpp"

namespace fw::drv
{

bool cobs_uart::setup( em::function_view< bool( handles& ) > setup_f )
{
        return setup_f( h_ );
}

com_res cobs_uart::load_message( em::view< std::byte* > data )
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
        std::ignore = HAL_UART_Transmit_DMA(
            &h_.uart,
            reinterpret_cast< uint8_t* >( otmp_.begin() ),
            static_cast< uint16_t >( used.size() + 1 ) );

        return true;
}

}  // namespace fw::drv
