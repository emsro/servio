#include "emlabcpp/result.h"
#include "fw/util.hpp"
#include "platform.hpp"
#include "setup.hpp"

#include <cassert>
#include <cstdlib>

namespace servio::plt
{

namespace em = emlabcpp;

em::result setup_clk()
{

        RCC_OscInitTypeDef       RCC_OscInitStruct = {};
        RCC_ClkInitTypeDef       RCC_ClkInitStruct = {};
        RCC_PeriphCLKInitTypeDef PeriphClkInit     = {};

        __HAL_PWR_VOLTAGESCALING_CONFIG( PWR_REGULATOR_VOLTAGE_SCALE0 );

        while ( !__HAL_PWR_GET_FLAG( PWR_FLAG_VOSRDY ) )
                asm( "nop" );

        RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
        RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
        RCC_OscInitStruct.HSIDiv              = RCC_HSI_DIV2;
        RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
        RCC_OscInitStruct.LSIState            = RCC_LSI_ON;
        RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource       = RCC_PLL1_SOURCE_HSI;
        RCC_OscInitStruct.PLL.PLLM            = 2;
        RCC_OscInitStruct.PLL.PLLN            = 30;
        RCC_OscInitStruct.PLL.PLLP            = 2;
        RCC_OscInitStruct.PLL.PLLQ            = 2;
        RCC_OscInitStruct.PLL.PLLR            = 2;
        RCC_OscInitStruct.PLL.PLLRGE          = RCC_PLL1_VCIRANGE_3;
        RCC_OscInitStruct.PLL.PLLVCOSEL       = RCC_PLL1_VCORANGE_WIDE;
        RCC_OscInitStruct.PLL.PLLFRACN        = 0;
        if ( HAL_RCC_OscConfig( &RCC_OscInitStruct ) != HAL_OK )
                fw::stop_exec();

        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                      RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
                                      RCC_CLOCKTYPE_PCLK3;
        RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
        RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

        if ( HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_5 ) != HAL_OK )
                return em::ERROR;
        /** Initializes the peripherals clocks
         */
        PeriphClkInit.PeriphClockSelection =
            RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_ADCDAC | RCC_PERIPHCLK_DAC;
        PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
        PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
        PeriphClkInit.AdcDacClockSelection = RCC_ADCDACCLKSOURCE_HSI;
        if ( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInit ) != HAL_OK )
                return em::ERROR;

        TEMP_CALIB_COEFFS.cal1 = *TEMPSENSOR_CAL1_ADDR;
        TEMP_CALIB_COEFFS.cal2 = *TEMPSENSOR_CAL2_ADDR;

        if ( HAL_ICACHE_Enable() != HAL_OK )
                return em::ERROR;

        return em::SUCCESS;
}

}  // namespace servio::plt
