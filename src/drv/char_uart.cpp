#include "./char_uart.hpp"

namespace servio::drv
{

status char_uart::send( send_data_t data, microseconds timeout )
{
        if ( !spin_with_timeout( clk_, tx_done_, timeout ) )
                return ERROR;

        // XXX: check that char is NOT present

        uint16_t count = 0;
        for ( auto s : data )
                for ( std::byte b : s ) {
                        if ( count == tx_buffer_.size() )
                                return ERROR;
                        tx_buffer_[count++] = b;
                }
        if ( count == tx_buffer_.size() )
                return ERROR;
        tx_buffer_[count++] = std::byte{ delim };

        tx_done_ = false;

        if ( HAL_UART_Transmit_DMA(
                 uart_, reinterpret_cast< uint8_t* >( tx_buffer_.data() ), count ) != HAL_OK ) {
                return ERROR;
        }

        return SUCCESS;
}

}  // namespace servio::drv
