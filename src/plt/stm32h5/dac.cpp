#include "setup.hpp"

namespace servio::plt
{
em::result setup_dac( DAC_HandleTypeDef& dac, const drv::pin_cfg& pcfg )
{
        DAC_ChannelConfTypeDef cfg{};

        dac.Instance = DAC1;  // TODO: << this should be in config
        if ( HAL_DAC_Init( &dac ) != HAL_OK )
                return em::ERROR;

        cfg.DAC_HighFrequency           = DAC_HIGH_FREQUENCY_INTERFACE_MODE_DISABLE;
        cfg.DAC_DMADoubleDataMode       = DISABLE;
        cfg.DAC_SignedFormat            = DISABLE;
        cfg.DAC_SampleAndHold           = DAC_SAMPLEANDHOLD_DISABLE;
        cfg.DAC_Trigger                 = DAC_TRIGGER_NONE;
        cfg.DAC_OutputBuffer            = DAC_OUTPUTBUFFER_ENABLE;
        cfg.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_EXTERNAL;
        cfg.DAC_UserTrimming            = DAC_TRIMMING_FACTORY;
        // XXX: channel should be configurable
        if ( HAL_DAC_ConfigChannel( &dac, &cfg, DAC_CHANNEL_1 ) != HAL_OK )
                return em::ERROR;

        GPIO_InitTypeDef ginit;
        ginit.Pin       = pcfg.pin;
        ginit.Mode      = GPIO_MODE_ANALOG;
        ginit.Pull      = GPIO_NOPULL;
        ginit.Speed     = GPIO_SPEED_FREQ_LOW;
        ginit.Alternate = pcfg.alternate;
        HAL_GPIO_Init( pcfg.port, &ginit );

        return em::SUCCESS;
}
}  // namespace servio::plt
