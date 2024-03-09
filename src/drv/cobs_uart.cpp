#include "drv/cobs_uart.hpp"

#include "base/base.hpp"
#include "emlabcpp/result.h"
#include "fw/util.hpp"

namespace servio::drv
{

com_res cobs_uart::load_message( em::view< std::byte* > data )
{
        auto [res, used] = bits::load_message( rx_, data );

        return { res, used };
}

em::result cobs_uart::send( em::view< const std::byte* > data, base::microseconds timeout )
{
        auto end = clk_.get_us() + timeout;
        while ( !tx_done_ )
                if ( clk_.get_us() > end )
                        return em::ERROR;

        auto [succ, used] = em::encode_cobs( data, tx_buffer_ );
        if ( !succ )
                return em::ERROR;
        if ( used.size() == tx_buffer_.size() )
                return em::ERROR;
        tx_buffer_[used.size()] = std::byte{ 0 };

        tx_done_ = false;

        if ( HAL_UART_Transmit_DMA(
                 uart_,
                 reinterpret_cast< uint8_t* >( tx_buffer_.begin() ),
                 static_cast< uint16_t >( used.size() + 1 ) ) != HAL_OK ) {
                return em::ERROR;
        }

        return em::SUCCESS;
}

}  // namespace servio::drv
