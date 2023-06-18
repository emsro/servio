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
                switch ( req.cfg.which_pld ) {
                case config_model_tag:
                        cfg_disp.set< MODEL >( req.cfg.model );
                        break;
                case config_position_conv_lower_setpoint_value_tag:
                        // TODO: add error check
                        cfg_disp.set< POSITION_CONV_LOWER_SETPOINT_VALUE >(
                            req.cfg.position_conv_lower_setpoint_value );
                        break;
                case config_position_conv_lower_setpoint_angle_tag:
                        cfg_disp.set< POSITION_CONV_LOWER_SETPOINT_ANGLE >(
                            req.cfg.position_conv_lower_setpoint_angle );
                        break;
                case config_position_conv_higher_setpoint_value_tag:
                        // TODO: add error check
                        cfg_disp.set< POSITION_CONV_HIGHER_SETPOINT_VALUE >(
                            req.cfg.position_conv_higher_setpoint_value );
                        break;
                case config_position_conv_higher_setpoint_angle_tag:
                        cfg_disp.set< POSITION_CONV_HIGHER_SETPOINT_ANGLE >(
                            req.cfg.position_conv_higher_setpoint_angle );
                        break;
                case config_current_conv_scale_tag:
                        cfg_disp.set< CURRENT_CONV_SCALE >( req.cfg.current_conv_scale );
                        break;
                case config_current_conv_offset_tag:
                        cfg_disp.set< CURRENT_CONV_OFFSET >( req.cfg.current_conv_offset );
                        break;
                case config_temp_conv_scale_tag:
                        cfg_disp.set< TEMP_CONV_SCALE >( req.cfg.temp_conv_scale );
                        break;
                case config_temp_conv_offset_tag:
                        cfg_disp.set< TEMP_CONV_OFFSET >( req.cfg.temp_conv_offset );
                        break;
                case config_voltage_conv_scale_tag:
                        cfg_disp.set< VOLTAGE_CONV_SCALE >( req.cfg.voltage_conv_scale );
                        break;
                case config_current_loop_p_tag:
                        cfg_disp.set< CURRENT_LOOP_P >( req.cfg.current_loop_p );
                        break;
                case config_current_loop_i_tag:
                        cfg_disp.set< CURRENT_LOOP_I >( req.cfg.current_loop_i );
                        break;
                case config_current_loop_d_tag:
                        cfg_disp.set< CURRENT_LOOP_D >( req.cfg.current_loop_d );
                        break;
                case config_current_lim_min_tag:
                        cfg_disp.set< CURRENT_LIM_MIN >( req.cfg.current_lim_min );
                        break;
                case config_current_lim_max_tag:
                        cfg_disp.set< CURRENT_LIM_MAX >( req.cfg.current_lim_max );
                        break;
                case config_velocity_loop_p_tag:
                        cfg_disp.set< VELOCITY_LOOP_P >( req.cfg.velocity_loop_p );
                        break;
                case config_velocity_loop_i_tag:
                        cfg_disp.set< VELOCITY_LOOP_I >( req.cfg.velocity_loop_i );
                        break;
                case config_velocity_loop_d_tag:
                        cfg_disp.set< VELOCITY_LOOP_D >( req.cfg.velocity_loop_d );
                        break;
                case config_velocity_lim_min_tag:
                        cfg_disp.set< VELOCITY_LIM_MIN >( req.cfg.velocity_lim_min );
                        break;
                case config_velocity_lim_max_tag:
                        cfg_disp.set< VELOCITY_LIM_MAX >( req.cfg.velocity_lim_max );
                        break;
                case config_position_loop_p_tag:
                        cfg_disp.set< POSITION_LOOP_P >( req.cfg.position_loop_p );
                        break;
                case config_position_loop_i_tag:
                        cfg_disp.set< POSITION_LOOP_I >( req.cfg.position_loop_i );
                        break;
                case config_position_loop_d_tag:
                        cfg_disp.set< POSITION_LOOP_D >( req.cfg.position_loop_d );
                        break;
                case config_position_lim_min_tag:
                        cfg_disp.set< POSITION_LIM_MIN >( req.cfg.position_lim_min );
                        break;
                case config_position_lim_max_tag:
                        cfg_disp.set< POSITION_LIM_MAX >( req.cfg.position_lim_max );
                        break;
                case config_static_friction_scale_tag:
                        cfg_disp.set< STATIC_FRICTION_SCALE >( req.cfg.static_friction_scale );
                        break;
                case config_static_friction_decay_tag:
                        cfg_disp.set< STATIC_FRICTION_DECAY >( req.cfg.static_friction_decay );
                        break;
                case config_minimum_voltage_tag:
                        cfg_disp.set< MINIMUM_VOLTAGE >( req.cfg.minimum_voltage );
                        break;
                case config_maximum_temperature_tag:
                        cfg_disp.set< MAXIMUM_TEMPERATURE >( req.cfg.maximum_temperature );
                        break;
                case config_moving_detection_step_tag:
                        cfg_disp.set< MOVING_DETECTION_STEP >( req.cfg.moving_detection_step );
                        break;
                }
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
