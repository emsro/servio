#pragma once

namespace fw::drv
{

struct detailed_adc_channel
{
};

struct adc_channel
{
};

enum chan_ids
{
        CURRENT_CHANNEL,
        POSITION_CHANNEL,
        VCC_CHANNEL,
        TEMP_CHANNEL,
};

struct adc_config
{
        detailed_adc_channel< CURRENT_CHANNEL, 128 > current_channel;
        add_channel< POSITION_CHANNEL >              position;
        add_channel< VCC_CHANNEL >                   vcc;
        add_channel< TEMP_CHANNEL >                  temp;

        em::view< const chan_ids* > sequence;
};

template < typename Cfg = adc_config >
struct adc_pooler
{
};

}  // namespace fw::drv
