
#include "fw/board.hpp"

namespace fw
{

bool setup_iuart( comms::handles& );
bool setup_adc( acquisition::handles& );
void setup_clock();
bool setup_hbridge_timers( hbridge::handles& );
bool setup_adc_timer( acquisition::handles& );
bool setup_duart( debug_comms::handles& );
void setup_extra();

debug_comms DEBUG_COMMS{};
acquisition ACQUISITION{};
hbridge     HBRIDGE{};
comms       COMMS{};

}  // namespace fw

extern "C" {

void TIM1_UP_TIM16_IRQHandler()
{
    fw::HBRIDGE.timer_irq();
}

void USART2_IRQHandler( void )
{
    fw::COMMS.uart_irq();
}

void DMA1_Channel3_IRQHandler( void )
{
    fw::COMMS.rx_dma_irq();
}

void DMA1_Channel4_IRQHandler( void )
{
    fw::COMMS.tx_dma_irq();
}

void USART1_IRQHandler( void )
{
    fw::DEBUG_COMMS.uart_irq();
}

void DMA1_Channel2_IRQHandler( void )
{
    fw::DEBUG_COMMS.rx_dma_irq();
}

void DMA1_Channel5_IRQHandler( void )
{
    fw::DEBUG_COMMS.tx_dma_irq();
}

void DMA1_Channel1_IRQHandler()
{
    fw::ACQUISITION.dma_irq();
}
void ADC1_2_IRQHandler()
{
    fw::ACQUISITION.adc_irq();
}

void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* h )
{
    fw::ACQUISITION.adc_conv_cplt_irq( h );
}
void HAL_ADC_ErrorCallback( ADC_HandleTypeDef* h )
{
    fw::ACQUISITION.adc_error_irq( h );
}
}

namespace fw
{

void setup_board()
{
    hal_check{} << HAL_Init();
    setup_clock();
}

acquisition* setup_acquisition( const config& cfg )
{
    auto acq_setup = []( acquisition::handles& h ) {
        return setup_adc( h ) && setup_adc_timer( h );
    };
    if ( !ACQUISITION.setup( acq_setup, cfg ) ) {
        return nullptr;
    }
    return &ACQUISITION;
}

hbridge* setup_hbridge( const config& cfg )
{
    auto setup_f = []( hbridge::handles& h ) {
        bool res = setup_hbridge_timers( h );

        hal_check{} << HAL_TIM_RegisterCallback(
            &h.timer, HAL_TIM_PERIOD_ELAPSED_CB_ID, []( TIM_HandleTypeDef* ) {
                HBRIDGE.timer_period_irq();
            } );

        return res;
    };
    if ( !HBRIDGE.setup( setup_f, cfg ) ) {
        return nullptr;
    }

    return &HBRIDGE;
}
comms* setup_comms()
{
    auto comms_setup = []( comms::handles& h ) {
        return setup_iuart( h );
    };
    if ( !COMMS.setup( comms_setup ) ) {
        return nullptr;
    }
    return &COMMS;
}
debug_comms* setup_debug_comms()
{
    auto setup_f = []( debug_comms::handles& h ) {
        bool res = setup_duart( h );
        hal_check{} << HAL_UART_RegisterCallback(
            &h.uart, HAL_UART_RX_COMPLETE_CB_ID, []( UART_HandleTypeDef* huart ) {
                DEBUG_COMMS.rx_cplt_irq( huart );
            } );
        return res;
    };

    if ( !DEBUG_COMMS.setup( setup_f ) ) {
        return nullptr;
    }

    return &DEBUG_COMMS;
}

}  // namespace fw
