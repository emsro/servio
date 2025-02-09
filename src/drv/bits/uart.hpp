
#pragma once

#include "../../plt/platform.hpp"
#include "../../sntr/sentry.hpp"

#include <emlabcpp/result.h>

namespace em = emlabcpp;

namespace servio::drv::bits
{

static constexpr uint32_t uart_common_tolerable_hal_errors =
    HAL_UART_ERROR_ORE | HAL_UART_ERROR_DMA | HAL_UART_ERROR_FE | HAL_UART_ERROR_NE;

inline em::result uart_start_it( UART_HandleTypeDef* h, std::byte& b )
{
        if ( h == nullptr )
                return em::ERROR;
        if ( HAL_UART_Receive_IT( h, reinterpret_cast< uint8_t* >( &b ), 1 ) != HAL_OK )
                return em::ERROR;
        return em::SUCCESS;
}

template < auto START_ERR >
inline void uart_rx_cplt_irq(
    UART_HandleTypeDef* h,
    UART_HandleTypeDef* local,
    auto&               drv,
    auto&               rx_buff,
    std::byte&          rx_byte,
    sntr::sentry&       sentry )
{
        if ( h != local )
                return;

        on_rx_cplt_irq( rx_buff, rx_byte );

        if ( drv.start() != em::SUCCESS )
                sentry.set_inoperable( START_ERR, "rx start" );
}

inline void uart_err_irq( UART_HandleTypeDef* h, UART_HandleTypeDef* local, sntr::sentry& sentry )
{

        if ( h != local )
                return;

        uint32_t error_status = HAL_UART_GetError( local );
        if ( ( error_status & ~uart_common_tolerable_hal_errors ) != 0 )
                sentry.set_inoperable_set( error_status << 1, "err irq" );
        else if ( error_status != 0 )
                sentry.set_degraded_set( error_status << 1, "err irq" );
}

}  // namespace servio::drv::bits
