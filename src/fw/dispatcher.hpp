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

        void reply( servio_to_host& msg )
        {
                uint8_t      buffer[64];
                pb_ostream_t stream = pb_ostream_from_buffer( buffer, sizeof( buffer ) );
                pb_encode( &stream, servio_to_host_fields, &msg );

                comm.send(
                    em::view_n( reinterpret_cast< std::byte* >( buffer ), stream.bytes_written ) );
        }

        void reply_get_prop( float val )
        {
                servio_to_host msg;
                msg.get_prop.val = val;
                msg.which_pld    = servio_to_host_get_prop_tag;
                reply( msg );
        }

        void handle_message( const host_to_servio& msg )
        {
                switch ( msg.which_pld ) {
                case host_to_servio_swtch_tag:
                        handle_message( msg.swtch );
                        break;
                case host_to_servio_get_prop_tag:
                        handle_message( msg.get_prop );
                        break;
                case host_to_servio_set_cfg_tag:
                        handle_message( msg.set_cfg );
                        break;
                case host_to_servio_get_cfg_tag:
                        handle_message( msg.get_cfg );
                        break;
                }
                // TODO: add default switch case for an error
        }

        void handle_message( const host_to_servio_switch_to& msg )
        {
                switch ( msg.mod ) {
                case MODE_POWER:
                        ctl.switch_to_power_control( static_cast< int16_t >( msg.val ) );
                        break;
                case MODE_CURRENT:
                        ctl.switch_to_current_control( now, msg.val );
                        break;
                case MODE_VELOCITY:
                        ctl.switch_to_velocity_control( now, msg.val );
                        break;
                case MODE_POSITION:
                        ctl.switch_to_position_control( now, msg.val );
                        break;
                }
                // TODO: add default switch case for an error
        }

        void handle_message( const host_to_servio_get_property& msg )
        {
                float val;
                switch ( msg.prop ) {
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
                reply_get_prop( val );
        }

        void handle_message( const host_to_servio_set_config& req )
        {
                cfg_value_message msg( em::view_n(
                    reinterpret_cast< const std::byte* >( req.msg.bytes ), req.msg.size ) );
                cfg_disp.set( static_cast< cfg_key >( req.key ), msg );
        }

        void handle_message( const host_to_servio_get_config& req )
        {
                servio_to_host msg;
                msg.get_cfg = servio_to_host_get_config{
                    .key = req.key,
                    .msg = {},
                };
                em::protocol::message m = cfg_disp.get( static_cast< cfg_key >( req.key ) );
                // TODO: this is unsafe
                std::copy(
                    m.begin(), m.end(), reinterpret_cast< std::byte* >( msg.get_cfg.msg.bytes ) );
                msg.get_cfg.msg.size = static_cast< uint16_t >( m.size() );

                msg.which_pld = servio_to_host_get_cfg_tag;
                reply( msg );
        }
};

}  // namespace fw
