#include "converter.hpp"
#include "indication.hpp"

#pragma once

namespace fw
{

class monitor
{
public:
        monitor(
            microseconds                 now,
            const control&               ctl,
            const vcc_interface&         vcc_drv,
            const temperature_interface& temp_drv,
            indication&                  indi,
            const converter&             conv )
          : ctl_( ctl )
          , indi_( indi )
          , vcc_drv_( vcc_drv )
          , temp_drv_( temp_drv )
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

                // TODO: check that this works
                const float vcc = conv_.vcc.convert( vcc_drv_.get_vcc() );
                if ( vcc < min_vcc_ ) {
                        indi_.on_event( now, indication_event::VOLTAGE_LOW );
                }

                // TODO: check that this works
                const float temp = conv_.temp.convert( temp_drv_.get_temperature() );
                if ( temp > max_tmp_ ) {
                        indi_.on_event( now, indication_event::TEMPERATURE_HIGH );
                }

                if ( ctl_.get_mode() != control_mode::DISENGAGED ) {
                        indi_.on_event( now, indication_event::ENGAGED );
                } else {
                        indi_.on_event( now, indication_event::DISENGAGED );
                }
        }

private:
        const control&               ctl_;
        indication&                  indi_;
        const vcc_interface&         vcc_drv_;
        const temperature_interface& temp_drv_;
        const converter&             conv_;

        float min_vcc_ = 0.f;
        float max_tmp_ = 90.f;
};

}  // namespace fw
