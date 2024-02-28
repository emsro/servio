#include "drv/cobs_uart.hpp"

#include "base/status.hpp"
#include "emlabcpp/result.h"
#include "fw/util.hpp"

namespace servio::drv
{
base::status cobs_uart::get_status() const
{
        auto rx_status =
            rx_start_result_ == em::SUCCESS ? base::status::NOMINAL : base::status::INOPERABLE;

        base::status error_status = base::status::NOMINAL;
        if ( ( error_status_ | tolerable_errors_ ) != 0 )
                error_status = base::status::DEGRADED;
        else if ( error_status_ != 0 )
                error_status = base::status::INOPERABLE;

        return base::worst_of( { rx_status, error_status } );
}

base::com_res cobs_uart::load_message( em::view< std::byte* > data )
{
        if ( rx_sizes_.empty() )
                return { true, {} };
        if ( rx_sizes_.front() > data.size() )
                return { false, em::view< std::byte* >{} };
        const uint16_t size  = rx_sizes_.take_front();
        em::view       dview = em::view_n( data.begin(), size );

        for ( std::byte& b : dview )
                b = rx_buffer_.take_front();

        error_status_ = error_status_ & ( ~tolerable_errors_ );

        return { true, dview };
}

em::result cobs_uart::send( em::view< const std::byte* > data )
{
        // TODO: well, timeout would be better
        while ( !tx_done_ )
                asm( "nop" );

        auto [succ, used] = em::encode_cobs( data, tx_buffer_ );
        if ( !succ )
                return em::ERROR;
        // TODO: this might fial
        tx_buffer_[used.size()] = std::byte{ 0 };

        tx_done_ = false;

        // TODO: problematic cast
        if ( HAL_UART_Transmit_DMA(
                 uart_,
                 reinterpret_cast< uint8_t* >( tx_buffer_.begin() ),
                 static_cast< uint16_t >( used.size() + 1 ) ) != HAL_OK ) {
                return em::ERROR;
        }

        return em::SUCCESS;
}

}  // namespace servio::drv
