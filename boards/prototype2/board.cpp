
#include "fw/board.hpp"

namespace brd
{

bool setup_adc( fw::acquisition::handles& );
bool setup_adc_timer( fw::acquisition::handles& );
bool setup_duart( fw::debug_comms::handles& );
bool setup_hbridge_timers( fw::hbridge::handles& );
bool setup_iuart( fw::comms::handles& );
bool setup_leds_gpio( fw::leds::handles& );
bool setup_leds_timer( fw::leds::handles& );
void setup_clock();
void setup_extra();

fw::acquisition ACQUISITION{};
fw::comms       COMMS{};
fw::debug_comms DEBUG_COMMS{};
fw::hbridge     HBRIDGE{};
fw::leds        LEDS;

}  // namespace brd

extern "C" {

void TIM1_UP_TIM16_IRQHandler()
{
        brd::HBRIDGE.timer_irq();
}

void USART2_IRQHandler( void )
{
        brd::COMMS.uart_irq();
}

void DMA1_Channel4_IRQHandler( void )
{
        brd::COMMS.tx_dma_irq();
}

void USART1_IRQHandler( void )
{
        brd::DEBUG_COMMS.uart_irq();
}

void DMA1_Channel2_IRQHandler( void )
{
        brd::DEBUG_COMMS.rx_dma_irq();
}

void DMA1_Channel5_IRQHandler( void )
{
        brd::DEBUG_COMMS.tx_dma_irq();
}

void DMA1_Channel1_IRQHandler()
{
        brd::ACQUISITION.dma_irq();
}
void ADC1_2_IRQHandler()
{
        brd::ACQUISITION.adc_irq();
}

void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* h )
{
        brd::ACQUISITION.adc_conv_cplt_irq( h );
}
void HAL_ADC_ErrorCallback( ADC_HandleTypeDef* h )
{
        brd::ACQUISITION.adc_error_irq( h );
}
}

namespace brd
{

void setup_board()
{
        fw::hal_check{} << HAL_Init();
        setup_clock();
}

fw::acquisition* setup_acquisition()
{
        auto acq_setup = []( fw::acquisition::handles& h ) {
                return setup_adc( h ) && setup_adc_timer( h );
        };
        if ( !ACQUISITION.setup( acq_setup ) ) {
                return nullptr;
        }
        return &ACQUISITION;
}

fw::hbridge* setup_hbridge()
{
        auto setup_f = []( fw::hbridge::handles& h ) {
                bool res = setup_hbridge_timers( h );

                fw::hal_check{} << HAL_TIM_RegisterCallback(
                    &h.timer, HAL_TIM_PERIOD_ELAPSED_CB_ID, []( TIM_HandleTypeDef* ) {
                            HBRIDGE.timer_period_irq();
                    } );

                return res;
        };
        if ( !HBRIDGE.setup( setup_f ) ) {
                return nullptr;
        }

        return &HBRIDGE;
}

fw::comms* setup_comms()
{
        auto comms_setup = []( fw::comms::handles& h ) {
                return setup_iuart( h );
        };
        if ( !COMMS.setup( comms_setup ) ) {
                return nullptr;
        }
        return &COMMS;
}

fw::debug_comms* setup_debug_comms()
{
        auto setup_f = []( fw::debug_comms::handles& h ) {
                bool res = setup_duart( h );
                fw::hal_check{} << HAL_UART_RegisterCallback(
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

fw::leds* setup_leds()
{
        auto setup_f = []( fw::leds::handles& h ) {
                return setup_leds_gpio( h ) && setup_leds_timer( h );
        };

        if ( !LEDS.setup( setup_f ) ) {
                return nullptr;
        }

        return &LEDS;
}

}  // namespace brd
