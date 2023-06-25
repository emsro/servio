#include "fw/dispatcher.hpp"

namespace fw
{
template < typename Cfg, typename UnaryFunction >
void map_cfg( uint32_t key, Cfg& cfg, UnaryFunction&& f )
{
        switch ( key ) {
        case Config_model_tag:
                f.template operator()< MODEL >( cfg.model );
                break;
        case Config_position_conv_lower_setpoint_value_tag:
                f.template operator()< POSITION_CONV_LOWER_SETPOINT_VALUE >(
                    cfg.position_conv_lower_setpoint_value );
                break;
        case Config_position_conv_lower_setpoint_angle_tag:
                f.template operator()< POSITION_CONV_LOWER_SETPOINT_ANGLE >(
                    cfg.position_conv_lower_setpoint_angle );
                break;
        case Config_position_conv_higher_setpoint_value_tag:
                f.template operator()< POSITION_CONV_HIGHER_SETPOINT_VALUE >(
                    cfg.position_conv_higher_setpoint_value );
                break;
        case Config_position_conv_higher_setpoint_angle_tag:
                f.template operator()< POSITION_CONV_HIGHER_SETPOINT_ANGLE >(
                    cfg.position_conv_higher_setpoint_angle );
                break;
        case Config_current_conv_scale_tag:
                f.template operator()< CURRENT_CONV_SCALE >( cfg.current_conv_scale );
                break;
        case Config_current_conv_offset_tag:
                f.template operator()< CURRENT_CONV_OFFSET >( cfg.current_conv_offset );
                break;
        case Config_temp_conv_scale_tag:
                f.template operator()< TEMP_CONV_SCALE >( cfg.temp_conv_scale );
                break;
        case Config_temp_conv_offset_tag:
                f.template operator()< TEMP_CONV_OFFSET >( cfg.temp_conv_offset );
                break;
        case Config_voltage_conv_scale_tag:
                f.template operator()< VOLTAGE_CONV_SCALE >( cfg.voltage_conv_scale );
                break;
        case Config_current_loop_p_tag:
                f.template operator()< CURRENT_LOOP_P >( cfg.current_loop_p );
                break;
        case Config_current_loop_i_tag:
                f.template operator()< CURRENT_LOOP_I >( cfg.current_loop_i );
                break;
        case Config_current_loop_d_tag:
                f.template operator()< CURRENT_LOOP_D >( cfg.current_loop_d );
                break;
        case Config_current_lim_min_tag:
                f.template operator()< CURRENT_LIM_MIN >( cfg.current_lim_min );
                break;
        case Config_current_lim_max_tag:
                f.template operator()< CURRENT_LIM_MAX >( cfg.current_lim_max );
                break;
        case Config_velocity_loop_p_tag:
                f.template operator()< VELOCITY_LOOP_P >( cfg.velocity_loop_p );
                break;
        case Config_velocity_loop_i_tag:
                f.template operator()< VELOCITY_LOOP_I >( cfg.velocity_loop_i );
                break;
        case Config_velocity_loop_d_tag:
                f.template operator()< VELOCITY_LOOP_D >( cfg.velocity_loop_d );
                break;
        case Config_velocity_lim_min_tag:
                f.template operator()< VELOCITY_LIM_MIN >( cfg.velocity_lim_min );
                break;
        case Config_velocity_lim_max_tag:
                f.template operator()< VELOCITY_LIM_MAX >( cfg.velocity_lim_max );
                break;
        case Config_position_loop_p_tag:
                f.template operator()< POSITION_LOOP_P >( cfg.position_loop_p );
                break;
        case Config_position_loop_i_tag:
                f.template operator()< POSITION_LOOP_I >( cfg.position_loop_i );
                break;
        case Config_position_loop_d_tag:
                f.template operator()< POSITION_LOOP_D >( cfg.position_loop_d );
                break;
        case Config_position_lim_min_tag:
                f.template operator()< POSITION_LIM_MIN >( cfg.position_lim_min );
                break;
        case Config_position_lim_max_tag:
                f.template operator()< POSITION_LIM_MAX >( cfg.position_lim_max );
                break;
        case Config_static_friction_scale_tag:
                f.template operator()< STATIC_FRICTION_SCALE >( cfg.static_friction_scale );
                break;
        case Config_static_friction_decay_tag:
                f.template operator()< STATIC_FRICTION_DECAY >( cfg.static_friction_decay );
                break;
        case Config_minimum_voltage_tag:
                f.template operator()< MINIMUM_VOLTAGE >( cfg.minimum_voltage );
                break;
        case Config_maximum_temperature_tag:
                f.template operator()< MAXIMUM_TEMPERATURE >( cfg.maximum_temperature );
                break;
        case Config_moving_detection_step_tag:
                f.template operator()< MOVING_DETECTION_STEP >( cfg.moving_detection_step );
                break;
        }
        // TODO: handle a missing key
}

ServioToHost dispatcher::handle_message( const HostToServio& msg )
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

ServioToHost dispatcher::handle_message( const HostToServio_SetMode& msg )
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

ServioToHost dispatcher::handle_message( const HostToServio_GetProperty& msg )
{
        float val = 0.f;
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

ServioToHost dispatcher::handle_message( const Config& req )
{
        map_cfg( req.which_pld, req, [&]< cfg_key K >( auto& val ) {
                cfg_disp.set< K >( val );
        } );
        ServioToHost msg;
        msg.which_pld = ServioToHost_set_config_tag;
        return msg;
}

ServioToHost dispatcher::handle_message( const HostToServio_GetConfig& req )
{
        ServioToHost msg;
        map_cfg( req.key, msg.get_config, [&]< cfg_key K, typename T >( T& val ) {
                if constexpr ( std::same_as< typename cfg_map::reg_value_type< K >, model_name > ) {
                        const model_name& n = cfg_disp.map.get_val< K >();
                        // TODO: well, dis is bug to happen
                        std::strncpy( val, n.data(), std::min( n.size(), sizeof( val ) ) );
                } else {
                        val = cfg_disp.map.get_val< K >();
                }
        } );
        msg.get_config.which_pld =
            static_cast< uint16_t >( req.key );  // TODO: well this is not ideal
        msg.which_pld = ServioToHost_get_config_tag;
        return msg;
}

}  // namespace fw
