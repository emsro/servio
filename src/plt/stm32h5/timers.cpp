#include "../../fw/util.hpp"
#include "setup.hpp"

#include <cstdlib>
#include <cstring>
#include <emlabcpp/result.h>
#include <initializer_list>
#include <limits>

namespace servio::plt
{

status setup_hbridge_timers( TIM_HandleTypeDef& tim, hb_timer_cfg cfg )
{

        tim.Instance               = cfg.timer_instance;
        tim.Init.Prescaler         = 0;
        tim.Init.CounterMode       = TIM_COUNTERMODE_UP;
        tim.Init.Period            = cfg.period;
        tim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
        tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

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

        setup_gpio( cfg.mc1_pin );
        setup_gpio( cfg.mc2_pin );

        if ( HAL_TIM_PWM_Init( &tim ) != HAL_OK )
                fw::stop_exec();

        if ( HAL_TIMEx_MasterConfigSynchronization( &tim, &mc ) != HAL_OK )
                fw::stop_exec();

        __HAL_TIM_ENABLE_IT( &tim, TIM_IT_UPDATE );

        if ( HAL_TIM_PWM_ConfigChannel( &tim, &oc_config, cfg.mc1_ch ) != HAL_OK )
                fw::stop_exec();

        if ( HAL_TIM_PWM_ConfigChannel( &tim, &oc_config, cfg.mc2_ch ) != HAL_OK )
                fw::stop_exec();

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

        if ( HAL_TIMEx_ConfigBreakDeadTime( &tim, &break_dead_time_cfg ) != HAL_OK )
                fw::stop_exec();

        HAL_NVIC_SetPriority( cfg.irq, cfg.irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.irq );

        return SUCCESS;
}

status setup_adc_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance )
{
        tim.Instance               = instance;
        tim.Init.Prescaler         = 0;
        tim.Init.CounterMode       = TIM_COUNTERMODE_UP;
        tim.Init.Period            = 256;
        tim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
        tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        TIM_MasterConfigTypeDef mc{};
        mc.MasterOutputTrigger = TIM_TRGO_UPDATE;
        mc.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;

        if ( HAL_TIM_OC_Init( &tim ) != HAL_OK )
                fw::stop_exec();

        if ( HAL_TIMEx_MasterConfigSynchronization( &tim, &mc ) != HAL_OK )
                fw::stop_exec();

        return SUCCESS;
}

status setup_clock_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance, IRQn_Type irq )
{
        tim.Instance               = instance;
        tim.Init.Prescaler         = __HAL_TIM_CALC_PSC( HAL_RCC_GetPCLK1Freq(), 1'000'000 );
        tim.Init.CounterMode       = TIM_COUNTERMODE_UP;
        tim.Init.Period            = 5'000;
        tim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
        tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        TIM_MasterConfigTypeDef mc{};
        mc.MasterOutputTrigger = TIM_TRGO_UPDATE;
        mc.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;

        __HAL_TIM_ENABLE_IT( &tim, TIM_IT_UPDATE );
        __HAL_TIM_UIFREMAP_ENABLE( &tim );

        if ( HAL_TIM_PWM_Init( &tim ) != HAL_OK )
                return ERROR;

        if ( HAL_TIMEx_MasterConfigSynchronization( &tim, &mc ) != HAL_OK )
                return ERROR;

        if ( HAL_TIM_Base_Start( &tim ) != HAL_OK )
                return ERROR;

        HAL_NVIC_SetPriority( irq, 0, 0 );
        HAL_NVIC_EnableIRQ( irq );

        return SUCCESS;
}

status setup_encoder_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance, uint32_t period )
{
        tim.Instance               = instance;
        tim.Init.Prescaler         = 0;
        tim.Init.CounterMode       = TIM_COUNTERMODE_UP;
        tim.Init.Period            = period;
        tim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
        tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        TIM_Encoder_InitTypeDef init;
        init.EncoderMode  = TIM_ENCODERMODE_TI12;
        init.IC1Polarity  = TIM_ICPOLARITY_RISING;
        init.IC1Selection = TIM_ICSELECTION_DIRECTTI;
        init.IC1Prescaler = TIM_ICPSC_DIV1;
        init.IC1Filter    = 0;
        init.IC2Polarity  = TIM_ICPOLARITY_RISING;
        init.IC2Selection = TIM_ICSELECTION_DIRECTTI;
        init.IC2Prescaler = TIM_ICPSC_DIV1;
        init.IC2Filter    = 0;

        if ( HAL_TIM_Encoder_Init( &tim, &init ) != HAL_OK )
                return ERROR;

        TIM_MasterConfigTypeDef mcfg;
        mcfg.MasterOutputTrigger = TIM_TRGO_RESET;
        mcfg.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
        if ( HAL_TIMEx_MasterConfigSynchronization( &tim, &mcfg ) != HAL_OK )
                return ERROR;

        return SUCCESS;
}

}  // namespace servio::plt
