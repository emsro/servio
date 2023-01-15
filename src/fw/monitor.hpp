
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

    void tick( std::chrono::milliseconds now )
    {
        indi_.on_event( now, indication_event::HEARTBEAT );

        // TODO: hardcoded constant /o....
        if ( acqui_.get_vcc() < 6.f ) {
            indi_.on_event( now, indication_event::VOLTAGE_LOW );
        }

        // TODO: constant /o..
        if ( acqui_.get_temp() > 70.f ) {
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
};

}  // namespace fw
