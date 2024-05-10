#pragma once

#include "platform.hpp"

#include <optional>

namespace servio::drv
{

struct pin_cfg
{
        uint16_t      pin;
        GPIO_TypeDef* port;
        uint8_t       mode      = GPIO_MODE_OUTPUT_PP;
        uint8_t       alternate = 0x0;
        uint8_t       pull      = GPIO_NOPULL;
};

}  // namespace servio::drv
