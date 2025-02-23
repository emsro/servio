#include "./char_uart.hpp"

namespace servio::drv
{

em::result char_uart::send( send_data_t data, microseconds timeout )
{
        if ( !spin_with_timeout( clk_, tx_done_, timeout ) )
                return em::ERROR;

        uint16_t count = 0;
        for ( auto s : data )
                for ( std::byte b : s ) {
                        if ( count == tx_buffer_.size() )
                                return em::ERROR;
                        tx_buffer_[count++] = b;
                }

        tx_done_ = false;

        if ( HAL_UART_Transmit_DMA(
                 uart_, reinterpret_cast< uint8_t* >( tx_buffer_.data() ), count ) != HAL_OK ) {
                return em::ERROR;
        }

        return em::SUCCESS;
}

}  // namespace servio::drv
