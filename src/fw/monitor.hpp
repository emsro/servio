
#include "fw/drivers/acquisition.hpp"
#include "indication.hpp"

#pragma once

namespace fw
{

class monitor
{
public:
    monitor( std::chrono::milliseconds now, control& ctl, acquisition& acqui, indication& indi )
      : ctl_( ctl )
      , acqui_( acqui )
      , indi_( indi )
    {
        indi_.on_event( now, indication_event::BOOTING );
    }

    void set_minimum_voltage( float vcc )
    {
        min_vcc_ = vcc;
    }

    void set_maximum_temperature( float temp )
    {
        max_tmp_ = temp;
    }

    void tick( std::chrono::milliseconds now )
    {
        indi_.on_event( now, indication_event::HEARTBEAT );

        if ( acqui_.get_vcc() < min_vcc_ ) {
            indi_.on_event( now, indication_event::VOLTAGE_LOW );
        }

        if ( acqui_.get_temp() > max_tmp_ ) {
            indi_.on_event( now, indication_event::TEMPERATURE_HIGH );
        }

        if ( ctl_.is_engaged() ) {
            indi_.on_event( now, indication_event::ENGAGED );
        } else {
            indi_.on_event( now, indication_event::DISENGAGED );
        }
    }

private:
    control&     ctl_;
    acquisition& acqui_;
    indication&  indi_;

    float min_vcc_ = 0.f;
    float max_tmp_ = 90.f;
};

}  // namespace fw
