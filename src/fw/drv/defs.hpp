#pragma once

#include "platform.hpp"

namespace fw::drv
{

struct pin_cfg
{
        uint16_t      pin;
        GPIO_TypeDef* port;
        uint8_t       alternate = 0x0;
};

struct pinch_cfg
{
        uint32_t      channel;
        uint32_t      pin;
        GPIO_TypeDef* port;
        uint8_t       alternate = 0x0;
};

}  // namespace fw::drv
