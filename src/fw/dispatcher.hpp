#include "control.hpp"
#include "drivers/acquisition.hpp"
#include "drivers/comms.hpp"
#include "fw/cfg_dispatcher.hpp"
#include "fw/conversion.hpp"

#include <io.pb.h>
#include <pb_encode.h>

#pragma once

namespace fw
{

struct dispatcher
{
        comms&          comm;
        hbridge&        hb;
        acquisition&    acquis;
        control&        ctl;
        cfg_dispatcher& cfg_disp;
        converter&      conv;
        microseconds    now;

        void reply( const ServioToHost& msg )
        {
                uint8_t      buffer[64];
                pb_ostream_t stream = pb_ostream_from_buffer( buffer, sizeof( buffer ) );
                bool         res    = pb_encode( &stream, ServioToHost_fields, &msg );
                if ( !res ) {
                        // TODO: well this is aggresive
                        fw::stop_exec();
                }

                comm.send(
                    em::view_n( reinterpret_cast< std::byte* >( buffer ), stream.bytes_written ) );
        }

        void reply_get_property( float val )
        {
                ServioToHost msg;
                msg.stat               = STATUS_SUCCESS;
                msg.get_property.value = val;
                msg.which_pld          = ServioToHost_get_property_tag;
                reply( msg );
        }

        void handle_message( const HostToServio& msg )
        {
                switch ( msg.which_pld ) {
                case HostToServio_set_mode_tag:
                        handle_message( msg.set_mode );
                        break;
                case HostToServio_get_property_tag:
                        handle_message( msg.get_property );
                        break;
                case HostToServio_set_config_tag:
                        handle_message( msg.set_config );
                        break;
                case HostToServio_get_config_tag:
                        handle_message( msg.get_config );
                        break;
                default:
                        // TODO: well, this is aggresive
                        fw::stop_exec();
                }
                // TODO: add default switch case for an error
        }

        void handle_message( const HostToServio_SetMode& msg )
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
                reply( rep );
        }

        void handle_message( const HostToServio_GetProperty& msg )
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
                reply_get_property( val );
        }

        void handle_message( const Config& req );

        void handle_message( const HostToServio_GetConfig& req );
};

}  // namespace fw
