#include "./cobs_uart.hpp"

#include "../base.hpp"
#include "../fw/util.hpp"

#include <emlabcpp/result.h>

namespace servio::drv
{

status cobs_uart::send( send_data_t data, microseconds timeout )
{
        if ( !spin_with_timeout( clk_, tx_done_, timeout ) )
                return ERROR;

        em::cobs_encoder e( tx_buffer_ );
        for ( auto s : data )
                for ( std::byte b : s )
                        if ( !e.insert( b ) )
                                return ERROR;

        em::view< std::byte* > used = std::move( e ).commit();
        if ( used.size() == tx_buffer_.size() )
                return ERROR;
        tx_buffer_[used.size()] = std::byte{ 0 };

        tx_done_ = false;

        if ( HAL_UART_Transmit_DMA(
                 uart_,
                 reinterpret_cast< uint8_t* >( tx_buffer_.begin() ),
                 static_cast< uint16_t >( used.size() + 1 ) ) != HAL_OK ) {
                return ERROR;
        }

        return SUCCESS;
}

}  // namespace servio::drv
