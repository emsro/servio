#include "fw/board.hpp"
#include "setup.hpp"

#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <limits>

namespace brd
{

// MC_1 is connected to PA8 on CH1
// MC_2 is connected to PA9 on CH2

bool setup_hbridge_timers( fw::hbridge::handles& h, hb_timer_cfg cfg )
{

        h.timer.Instance               = cfg.timer_instance;
        h.timer.Init.Prescaler         = 0;
        h.timer.Init.CounterMode       = TIM_COUNTERMODE_UP;
        h.timer.Init.Period            = cfg.period;
        h.timer.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
        h.timer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        h.mc1_channel = cfg.mc1.channel;
        h.mc2_channel = cfg.mc2.channel;

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

        for ( const pinch_cfg& pc : { cfg.mc1, cfg.mc2 } ) {
                GPIO_InitTypeDef gpio_itd{};
                gpio_itd.Pin       = pc.pin;
                gpio_itd.Mode      = GPIO_MODE_AF_PP;
                gpio_itd.Pull      = GPIO_NOPULL;
                gpio_itd.Speed     = GPIO_SPEED_FREQ_LOW;
                gpio_itd.Alternate = pc.alternate;

                HAL_GPIO_Init( pc.port, &gpio_itd );
        }

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

        HAL_NVIC_SetPriority( cfg.irq, cfg.irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.irq );

        return true;
}

bool setup_adc_timer( fw::acquisition::handles& h, adc_timer_cfg cfg )
{
        h.tim.Instance               = cfg.timer_instance;
        h.tim.Init.Prescaler         = 0;
        h.tim.Init.CounterMode       = TIM_COUNTERMODE_UP;
        h.tim.Init.Period            = 256;
        h.tim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
        h.tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        h.tim_channel = cfg.channel;

        TIM_MasterConfigTypeDef mc{};
        mc.MasterOutputTrigger = TIM_TRGO_UPDATE;
        mc.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;

        TIM_OC_InitTypeDef oc{};
        oc.OCMode     = TIM_OCMODE_TIMING;
        oc.Pulse      = h.tim.Init.Period / 2;
        oc.OCPolarity = TIM_OCPOLARITY_HIGH;
        oc.OCFastMode = TIM_OCFAST_DISABLE;

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

bool setup_clock_timer( fw::clock::handles& h, clock_timer_cfg cfg )
{
        h.tim.Instance               = cfg.timer_instance;
        h.tim.Init.Prescaler         = __HAL_TIM_CALC_PSC( HAL_RCC_GetPCLK1Freq(), 1e6 );
        h.tim.Init.CounterMode       = TIM_COUNTERMODE_UP;
        h.tim.Init.Period            = std::numeric_limits< uint32_t >::max();
        h.tim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
        h.tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        h.tim_channel = cfg.channel;

        TIM_MasterConfigTypeDef mc{};
        mc.MasterOutputTrigger = TIM_TRGO_UPDATE;
        mc.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;

        TIM_OC_InitTypeDef oc{};
        oc.OCMode     = TIM_OCMODE_TIMING;
        oc.Pulse      = h.tim.Init.Period - 1;
        oc.OCPolarity = TIM_OCPOLARITY_HIGH;
        oc.OCFastMode = TIM_OCFAST_DISABLE;

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

bool setup_leds_timer( fw::leds::handles& h, leds_timer_cfg cfg )
{
        h.tim.Instance               = cfg.timer_instance;
        h.tim.Init.Prescaler         = 0;
        h.tim.Init.CounterMode       = TIM_COUNTERMODE_UP;
        h.tim.Init.Period            = 65535;
        h.tim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
        h.tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        TIM_MasterConfigTypeDef smc;

        smc.MasterOutputTrigger = TIM_TRGO_RESET;
        smc.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;

        h.yellow_channel = cfg.yellow.channel;
        h.green_channel  = cfg.green.channel;

        TIM_OC_InitTypeDef chc;

        chc.OCMode     = TIM_OCMODE_PWM1;
        chc.Pulse      = 0;
        chc.OCPolarity = TIM_OCPOLARITY_HIGH;
        chc.OCFastMode = TIM_OCFAST_DISABLE;

        for ( const pinch_cfg& pc : { cfg.yellow, cfg.green } ) {
                GPIO_InitTypeDef gpio_itd{};

                gpio_itd.Pin       = pc.pin;
                gpio_itd.Mode      = GPIO_MODE_AF_PP;
                gpio_itd.Pull      = GPIO_NOPULL;
                gpio_itd.Speed     = GPIO_SPEED_FREQ_LOW;
                gpio_itd.Alternate = pc.alternate;

                HAL_GPIO_Init( pc.port, &gpio_itd );
        }

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