#include "control.hpp"
#include "drivers/acquisition.hpp"
#include "drivers/comms.hpp"
#include "fw/cfg_dispatcher.hpp"
#include "fw/conversion.hpp"
#include "fw/servio_pb.hpp"

#pragma once

namespace fw
{

struct dispatcher
{
        hbridge&        hb;
        acquisition&    acquis;
        control&        ctl;
        cfg_dispatcher& cfg_disp;
        converter&      conv;
        microseconds    now;

        ServioToHost handle_message( const HostToServio& msg )
        {
                switch ( msg.which_pld ) {
                case HostToServio_set_mode_tag:
                        return handle_message( msg.set_mode );
                case HostToServio_get_property_tag:
                        return handle_message( msg.get_property );
                case HostToServio_set_config_tag:
                        return handle_message( msg.set_config );
                case HostToServio_get_config_tag:
                        return handle_message( msg.get_config );
                default:
                        // TODO: well, this is aggresive
                        fw::stop_exec();
                }
                // TODO: add default switch case for an error
                return {};
        }

        ServioToHost handle_message( const HostToServio_SetMode& msg )
        {
                float val;
                switch ( msg.mode ) {
                case MODE_POWER:
                        val = em::map_range< float, float >(
                            msg.goal,
                            -1.0f,
                            1.0f,
                            std::numeric_limits< int16_t >::lowest(),
                            std::numeric_limits< int16_t >::max() );
                        ctl.switch_to_power_control( static_cast< int16_t >( val ) );
                        break;
                case MODE_CURRENT:
                        ctl.switch_to_current_control( now, msg.goal );
                        break;
                case MODE_VELOCITY:
                        ctl.switch_to_velocity_control( now, msg.goal );
                        break;
                case MODE_POSITION:
                        ctl.switch_to_position_control( now, msg.goal );
                        break;
                }
                // TODO: add default switch case for an error
                ServioToHost rep;
                rep.which_pld = ServioToHost_set_mode_tag;
                return rep;
        }

        ServioToHost handle_message( const HostToServio_GetProperty& msg )
        {
                float val;
                switch ( msg.property ) {
                case PROPERTY_CURRENT:
                        val = current( conv, acquis, hb );
                        break;
                case PROPERTY_VCC:
                        val = conv.vcc.convert( acquis.get_vcc() );
                        break;
                case PROPERTY_TEMP:
                        val = conv.temp.convert( acquis.get_temp() );
                        break;
                case PROPERTY_POSITION:
                        val = position( conv, acquis );
                        break;
                }
                // TODO: add default switch case for an error
                ServioToHost repl;
                repl.stat               = STATUS_SUCCESS;
                repl.get_property.value = val;
                repl.which_pld          = ServioToHost_get_property_tag;
                return repl;
        }

        ServioToHost handle_message( const Config& req );

        ServioToHost handle_message( const HostToServio_GetConfig& req );
};

}  // namespace fw
