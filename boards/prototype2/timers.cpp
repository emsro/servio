#include "fw/board.hpp"

#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <limits>

namespace brd
{

// MC_1 is connected to PA8 on CH1
// MC_2 is connected to PA9 on CH2

bool setup_hbridge_timers( fw::hbridge::handles& h )
{

        h.timer.Instance               = TIM1;
        h.timer.Init.Prescaler         = 0;
        h.timer.Init.CounterMode       = TIM_COUNTERMODE_UP;
        h.timer.Init.Period            = std::numeric_limits< uint16_t >::max() / 4;
        h.timer.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
        h.timer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        h.mc1_channel = TIM_CHANNEL_1;
        h.mc2_channel = TIM_CHANNEL_2;

        TIM_MasterConfigTypeDef mc{};
        mc.MasterOutputTrigger  = TIM_TRGO_RESET;
        mc.MasterOutputTrigger2 = TIM_TRGO2_RESET;
        mc.MasterSlaveMode      = TIM_MASTERSLAVEMODE_DISABLE;

        TIM_OC_InitTypeDef oc_config{};

        oc_config.OCMode       = TIM_OCMODE_PWM1;
        oc_config.Pulse        = 0;
        oc_config.OCPolarity   = TIM_OCPOLARITY_HIGH;
        oc_config.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
        oc_config.OCFastMode   = TIM_OCFAST_DISABLE;
        oc_config.OCIdleState  = TIM_OCIDLESTATE_RESET;
        oc_config.OCNIdleState = TIM_OCNIDLESTATE_RESET;

        GPIO_InitTypeDef gpioa{};

        gpioa.Pin       = GPIO_PIN_8 | GPIO_PIN_9;
        gpioa.Mode      = GPIO_MODE_AF_PP;
        gpioa.Pull      = GPIO_NOPULL;
        gpioa.Speed     = GPIO_SPEED_FREQ_LOW;
        gpioa.Alternate = GPIO_AF6_TIM1;

        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        HAL_GPIO_Init( GPIOA, &gpioa );

        if ( HAL_TIM_PWM_Init( &h.timer ) != HAL_OK ) {
                fw::stop_exec();
        }

        if ( HAL_TIMEx_MasterConfigSynchronization( &h.timer, &mc ) != HAL_OK ) {
                fw::stop_exec();
        }

        __HAL_TIM_ENABLE_IT( &h.timer, TIM_IT_UPDATE );

        if ( HAL_TIM_PWM_ConfigChannel( &h.timer, &oc_config, h.mc1_channel ) != HAL_OK ) {
                fw::stop_exec();
        }

        if ( HAL_TIM_PWM_ConfigChannel( &h.timer, &oc_config, h.mc2_channel ) != HAL_OK ) {
                fw::stop_exec();
        }

        TIM_BreakDeadTimeConfigTypeDef break_dead_time_cfg{};

        break_dead_time_cfg.OffStateRunMode  = TIM_OSSR_DISABLE;
        break_dead_time_cfg.OffStateIDLEMode = TIM_OSSI_DISABLE;
        break_dead_time_cfg.LockLevel        = TIM_LOCKLEVEL_OFF;
        break_dead_time_cfg.DeadTime         = 0;
        break_dead_time_cfg.BreakState       = TIM_BREAK_DISABLE;
        break_dead_time_cfg.BreakPolarity    = TIM_BREAKPOLARITY_HIGH;
        break_dead_time_cfg.BreakFilter      = 0;
        break_dead_time_cfg.BreakAFMode      = TIM_BREAK_AFMODE_INPUT;
        break_dead_time_cfg.Break2State      = TIM_BREAK2_DISABLE;
        break_dead_time_cfg.Break2Polarity   = TIM_BREAK2POLARITY_HIGH;
        break_dead_time_cfg.Break2Filter     = 0;
        break_dead_time_cfg.Break2AFMode     = TIM_BREAK_AFMODE_INPUT;
        break_dead_time_cfg.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;

        if ( HAL_TIMEx_ConfigBreakDeadTime( &h.timer, &break_dead_time_cfg ) != HAL_OK ) {
                fw::stop_exec();
        }

        HAL_NVIC_SetPriority( TIM1_UP_TIM16_IRQn, 0, 0 );
        HAL_NVIC_EnableIRQ( TIM1_UP_TIM16_IRQn );

        return true;
}

bool setup_adc_timer( fw::acquisition::handles& h )
{
        h.tim.Instance               = TIM4;
        h.tim.Init.Prescaler         = 0;
        h.tim.Init.CounterMode       = TIM_COUNTERMODE_UP;
        h.tim.Init.Period            = 512;
        h.tim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
        h.tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        h.tim_channel = TIM_CHANNEL_1;

        TIM_MasterConfigTypeDef mc{};
        mc.MasterOutputTrigger = TIM_TRGO_UPDATE;
        mc.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;

        TIM_OC_InitTypeDef oc{};
        oc.OCMode     = TIM_OCMODE_TIMING;
        oc.Pulse      = h.tim.Init.Period / 2;
        oc.OCPolarity = TIM_OCPOLARITY_HIGH;
        oc.OCFastMode = TIM_OCFAST_DISABLE;

        __HAL_RCC_TIM4_CLK_ENABLE();

        if ( HAL_TIM_OC_Init( &h.tim ) != HAL_OK ) {
                fw::stop_exec();
        }

        if ( HAL_TIMEx_MasterConfigSynchronization( &h.tim, &mc ) != HAL_OK ) {
                fw::stop_exec();
        }

        if ( HAL_TIM_OC_ConfigChannel( &h.tim, &oc, h.tim_channel ) != HAL_OK ) {
                fw::stop_exec();
        }

        return true;
}

bool setup_leds_timer( fw::leds::handles& h )
{
        h.tim.Instance               = TIM3;
        h.tim.Init.Prescaler         = 0;
        h.tim.Init.CounterMode       = TIM_COUNTERMODE_UP;
        h.tim.Init.Period            = 65535;
        h.tim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
        h.tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        TIM_MasterConfigTypeDef smc;

        smc.MasterOutputTrigger = TIM_TRGO_RESET;
        smc.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;

        h.yellow_channel = TIM_CHANNEL_2;
        h.green_channel  = TIM_CHANNEL_3;

        TIM_OC_InitTypeDef chc;

        chc.OCMode     = TIM_OCMODE_PWM1;
        chc.Pulse      = 0;
        chc.OCPolarity = TIM_OCPOLARITY_HIGH;
        chc.OCFastMode = TIM_OCFAST_DISABLE;

        GPIO_InitTypeDef gp_cf;
        gp_cf.Pin       = GPIO_PIN_0 | GPIO_PIN_5;
        gp_cf.Mode      = GPIO_MODE_AF_PP;
        gp_cf.Pull      = GPIO_NOPULL;
        gp_cf.Speed     = GPIO_SPEED_FREQ_LOW;
        gp_cf.Alternate = GPIO_AF2_TIM3;

        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_TIM3_CLK_ENABLE();

        HAL_GPIO_Init( GPIOB, &gp_cf );

        if ( HAL_TIM_PWM_Init( &h.tim ) != HAL_OK ) {
                fw::stop_exec();
        }

        if ( HAL_TIMEx_MasterConfigSynchronization( &h.tim, &smc ) != HAL_OK ) {
                fw::stop_exec();
        }

        for ( uint32_t chan : { h.yellow_channel, h.green_channel } ) {
                if ( HAL_TIM_PWM_ConfigChannel( &h.tim, &chc, chan ) != HAL_OK ) {
                        fw::stop_exec();
                }
        }

        return true;
}

}  // namespace brd
