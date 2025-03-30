#include "setup.hpp"

namespace servio::plt
{

status setup_dts( DTS_HandleTypeDef& h, DTS_TypeDef* inst )
{
        h.Instance           = inst;
        h.Init.QuickMeasure  = DTS_QUICKMEAS_DISABLE;
        h.Init.RefClock      = DTS_REFCLKSEL_PCLK;
        h.Init.TriggerInput  = DTS_TRIGGER_HW_NONE;
        h.Init.SamplingTime  = DTS_SMP_TIME_5_CYCLE;
        h.Init.Divider       = 0;
        h.Init.HighThreshold = 0x0;
        h.Init.LowThreshold  = 0x0;
        if ( HAL_DTS_Init( &h ) != HAL_OK )
                return ERROR;

        return SUCCESS;
}

}  // namespace servio::plt
