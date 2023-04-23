#include "fw/drivers/comms.hpp"

#include "fw/util.hpp"

namespace fw
{

bool comms::setup( em::function_view< bool( handles& ) > setup_f )
{
        return setup_f( h_ );
}

void comms::tx_dma_irq()
{
        HAL_DMA_IRQHandler( &h_.tx_dma );
}

void comms::uart_irq()
{
        HAL_UART_IRQHandler( &h_.uart );
}

void comms::rx_cplt_irq( UART_HandleTypeDef* huart )
{
        if ( huart != &h_.uart ) {
                return;
        }

        ep_.insert( em::view_n( &rx_byte_, 1 ) );
        start();
}

std::variant< std::monostate, master_to_servo_variant, em::protocol::error_record >
comms::get_message()
{
        using return_type =
            std::variant< std::monostate, master_to_servo_variant, em::protocol::error_record >;
        return em::match(
            ep_.get_value(),
            []( std::size_t ) -> return_type {
                    return std::monostate{};
            },
            []( master_to_servo_variant var ) -> return_type {
                    return var;
            },
            []( em::protocol::error_record err ) -> return_type {
                    return err;
            } );
}

void comms::start()
{
        // TODO: return value ignored
        HAL_UART_Receive_IT( &h_.uart, reinterpret_cast< uint8_t* >( &rx_byte_ ), 1 );
}

void comms::send( const servo_to_master_variant& var )
{
        while ( !( HAL_UART_GetState( &h_.uart ) & HAL_UART_STATE_READY ) ) {
                asm( "nop" );
        }
        omsg_ = ep_.serialize( var );
        static_assert( servo_to_master_message::capacity < std::numeric_limits< uint16_t >::max() );
        // TODO: problematic cast
        // TODO: return value ignored
        HAL_UART_Transmit_DMA(
            &h_.uart,
            reinterpret_cast< uint8_t* >( omsg_.begin() ),
            static_cast< uint16_t >( omsg_.size() ) );
}

}  // namespace fw
