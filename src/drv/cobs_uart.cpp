#include "./cobs_uart.hpp"

#include "../base/base.hpp"
#include "../fw/util.hpp"

#include <emlabcpp/result.h>

namespace servio::drv
{

com_res cobs_uart::recv( std::span< std::byte > data )
{
        auto [res, used] = bits::load_message( rx_, data );

        return { res, used };
}

em::result
cobs_uart::send( std::span< std::span< std::byte const > const > data, microseconds timeout )
{
        auto end = clk_.get_us() + timeout;
        while ( !tx_done_ )
                if ( clk_.get_us() > end )
                        return em::ERROR;

        em::cobs_encoder e( tx_buffer_ );
        for ( auto s : data )
                for ( std::byte b : s )
                        if ( !e.insert( b ) )
                                return em::ERROR;

        em::view< std::byte* > used = std::move( e ).commit();
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
