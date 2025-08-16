#include "./dispatcher.hpp"

namespace servio::cfg
{

void dispatcher::apply( key key )
{
        switch ( key ) {
        case key::model:
        case key::id:
        case key::group_id:
                break;
        case key::encoder_mode:
                break;
        case key::quad_encoder_range:
                break;
        case key::position_low_angle:
        case key::position_high_angle:
                met.set_position_range( { m.position_low_angle, m.position_high_angle } );
                {
                        auto [min_val, max_val] = pos.get_position_range();
                        conv.set_position_cfg(
                            min_val, m.position_low_angle, max_val, m.position_high_angle );
                }
                break;
        case key::current_conv_scale:
        case key::current_conv_offset:
                conv.set_current_cfg( m.current_conv_scale, m.current_conv_offset );
                break;
        case key::temp_conv_scale:
        case key::temp_conv_offset:
                conv.set_temp_cfg( m.temp_conv_scale, m.temp_conv_offset );
                break;
        case key::voltage_conv_scale:
                conv.set_vcc_cfg( m.voltage_conv_scale );
                break;
        case key::invert_hbridge:
                motor.set_invert( m.invert_hbridge );
                break;
        case key::minimum_voltage:
                mon.set_minimum_voltage( m.minimum_voltage );
                break;
        case key::maximum_temperature:
                mon.set_maximum_temperature( m.maximum_temperature );
                break;
        case key::moving_detection_step:
                met.set_moving_step( m.moving_detection_step );
                break;
        default:
                break;
        }
}

void dispatcher::full_apply()
{
        for ( auto key : map::keys )
                dispatcher::apply( key );
}

}  // namespace servio::cfg
