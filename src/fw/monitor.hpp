
#include "converter.hpp"
#include "fw/drivers/acquisition.hpp"
#include "indication.hpp"

#pragma once

namespace fw
{

class monitor
{
public:
        monitor(
            microseconds       now,
            const control&     ctl,
            const acquisition& acqui,
            indication&        indi,
            const converter&   conv )
          : ctl_( ctl )
          , acqui_( acqui )
          , indi_( indi )
          , conv_( conv )
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

        void tick( microseconds now )
        {
                indi_.on_event( now, indication_event::HEARTBEAT );

                float vcc = conv_.convert_vcc( acqui_.get_vcc() );
                if ( vcc < min_vcc_ ) {
                        indi_.on_event( now, indication_event::VOLTAGE_LOW );
                }

                float temp = conv_.convert_temp( acqui_.get_temp() );
                if ( temp > max_tmp_ ) {
                        indi_.on_event( now, indication_event::TEMPERATURE_HIGH );
                }

                if ( ctl_.is_engaged() ) {
                        indi_.on_event( now, indication_event::ENGAGED );
                } else {
                        indi_.on_event( now, indication_event::DISENGAGED );
                }
        }

private:
        const control&     ctl_;
        const acquisition& acqui_;
        indication&        indi_;
        const converter&   conv_;

        float min_vcc_ = 0.f;
        float max_tmp_ = 90.f;
};

}  // namespace fw
