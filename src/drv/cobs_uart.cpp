#include "./cobs_uart.hpp"

#include "../base.hpp"
#include "../fw/util.hpp"

namespace servio::drv
{

error_code cobs_uart::send( send_get_data_f data, microseconds timeout )
{
        if ( !spin_with_timeout( clk_, tx_done_, timeout ) )
                return status::error;

        em::cobs_encoder             e( tx_buffer_ );
        std::span< std::byte const > d;
        do {
                d = data();
                for ( std::byte b : d )
                        if ( !e.insert( b ) )
                                return status::error;
        } while ( !d.empty() );

        em::view< std::byte* > used = std::move( e ).commit();
        if ( used.size() == tx_buffer_.size() )
                return status::error;
        tx_buffer_[used.size()] = std::byte{ 0 };

        tx_done_ = false;

        if ( HAL_UART_Transmit_DMA(
                 uart_,
                 reinterpret_cast< uint8_t* >( tx_buffer_.begin() ),
                 static_cast< uint16_t >( used.size() + 1 ) ) != HAL_OK ) {
                return status::error;
        }

        return status::success;
}

}  // namespace servio::drv
