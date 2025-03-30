#include "../../fw/util.hpp"
#include "./setup.hpp"

namespace servio::plt
{

status setup_i2c( I2C_HandleTypeDef& i2c, i2c_cfg cfg )
{
        i2c.Instance              = cfg.instance;
        i2c.Init.Timing           = 0x202029A8;
        i2c.Init.OwnAddress1      = 0;
        i2c.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
        i2c.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
        i2c.Init.OwnAddress2      = 0;
        i2c.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
        i2c.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
        i2c.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

        setup_gpio( cfg.sda );
        setup_gpio( cfg.scl );

        HAL_NVIC_SetPriority( cfg.ev_irq, cfg.ev_irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.ev_irq );

        HAL_NVIC_SetPriority( cfg.er_irq, cfg.er_irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.er_irq );

        if ( HAL_I2C_Init( &i2c ) != HAL_OK )
                fw::stop_exec();

        if ( HAL_I2CEx_ConfigAnalogFilter( &i2c, I2C_ANALOGFILTER_ENABLE ) != HAL_OK )
                fw::stop_exec();

        if ( HAL_I2CEx_ConfigDigitalFilter( &i2c, 0 ) != HAL_OK )
                fw::stop_exec();

        return SUCCESS;
}

}  // namespace servio::plt
