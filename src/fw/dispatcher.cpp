#include "fw/dispatcher.hpp"

#include "cnv/utils.hpp"
#include "fw/map_cfg.hpp"
#include "fw/servio_pb.hpp"

namespace fw
{

ServioToHost handle_set_mode( microseconds now, control& ctl, const Mode& msg )
{
        float val;
        switch ( msg.which_pld ) {
        case Mode_disengaged_tag:
                ctl.disengage();
                break;
        case Mode_power_tag:
                val = em::map_range< float, float >(
                    msg.power,
                    -1.0F,
                    1.0F,
                    std::numeric_limits< int16_t >::lowest(),
                    std::numeric_limits< int16_t >::max() );
                ctl.switch_to_power_control( static_cast< int16_t >( val ) );
                break;
        case Mode_current_tag:
                ctl.switch_to_current_control( now, msg.current );
                break;
        case Mode_velocity_tag:
                ctl.switch_to_velocity_control( now, msg.velocity );
                break;
        case Mode_position_tag:
                ctl.switch_to_position_control( now, msg.position );
                break;
        default:
                return error_msg( "got unknown mode" );
        }
        ServioToHost rep;
        rep.which_pld = ServioToHost_set_mode_tag;
        return rep;
}

Mode get_mode( const control& ctl )
{
        Mode res;
        switch ( ctl.get_mode() ) {
        case control_mode::DISENGAGED:
                res.which_pld = Mode_disengaged_tag;
                break;
        case control_mode::POWER:
                res.which_pld = Mode_power_tag;
                res.power     = ctl.get_power();
                break;
        case control_mode::CURRENT:
                res.which_pld = Mode_current_tag;
                res.current   = ctl.get_desired_current();
                break;
        case control_mode::VELOCITY:
                res.which_pld = Mode_velocity_tag;
                res.velocity  = ctl.get_desired_velocity();
                break;
        case control_mode::POSITION:
                res.which_pld = Mode_position_tag;
                res.position  = ctl.get_desired_position();
                break;
        }
        return res;
}

ServioToHost handle_get_property(
    const control&               ctl,
    const metrics&               met,
    const converter&             conv,
    const position_interface&    pos_drv,
    const current_interface&     curr_drv,
    const vcc_interface&         vcc_drv,
    const temperature_interface& temp_drv,
    const pwm_motor_interface&   motor,
    const GetProperty&           msg )
{
        Property prop;
        prop.which_pld = static_cast< pb_size_t >( msg.field_id );
        switch ( msg.field_id ) {
        case Property_mode_tag:
                prop.mode = get_mode( ctl );
                break;
        case Property_current_tag:
                prop.current = cnv::current( conv, curr_drv, motor );
                break;
        case Property_vcc_tag:
                prop.vcc = conv.vcc.convert( vcc_drv.get_vcc() );
                break;
        case Property_temp_tag:
                prop.temp = conv.temp.convert( temp_drv.get_temperature() );
                break;
        case Property_position_tag:
                prop.position = cnv::position( conv, pos_drv );
                break;
        case Property_velocity_tag:
                prop.velocity = met.get_velocity();
                break;
        default:
                return error_msg( "got unknown property" );
        }
        ServioToHost repl;
        repl.get_property = prop;
        repl.which_pld    = ServioToHost_get_property_tag;
        return repl;
}

ServioToHost handle_set_config( cfg::dispatcher& cfg_disp, const Config& req )
{
        const bool key_found = map_cfg( req.which_pld, req, [&]< cfg::key K >( auto& val ) {
                cfg_disp.set< K >( val );
        } );
        if ( !key_found ) {
                return error_msg( "unknown key" );
        }
        ServioToHost msg;
        msg.which_pld = ServioToHost_set_config_tag;
        return msg;
}

ServioToHost handle_get_config( const cfg::dispatcher& cfg_disp, const GetConfig& req )
{
        ServioToHost msg;
        const bool   key_found =
            map_cfg( req.field_id, msg.get_config, [&]< cfg::key K, typename T >( T& val ) {
                    if constexpr ( K == cfg::MODEL ) {
                            const cfg::model_name& n = cfg_disp.m.get_val< K >();
                            copy_string_to( n.data(), n.size(), val );
                    } else {
                            val = cfg_disp.m.get_val< K >();
                    }
            } );
        if ( !key_found ) {
                return error_msg( "unknown key" );
        }
        msg.get_config.which_pld =
            static_cast< uint16_t >( req.field_id );  // TODO: well this is not ideal
        msg.which_pld = ServioToHost_get_config_tag;
        return msg;
}

ServioToHost handle_commit_config( const cfg::dispatcher& cfg_disp, const auto& cfg_writer )
{

        const bool succ = cfg_writer( &cfg_disp.m );
        if ( !succ ) {
                return error_msg( "commit failed" );
        }

        ServioToHost msg;
        msg.which_pld = ServioToHost_commit_config_tag;
        return msg;
}

ServioToHost handle_clear_config( const auto& cfg_writer )
{

        const bool succ = cfg_writer( nullptr );
        if ( !succ ) {
                return error_msg( "clear failed" );
        }

        ServioToHost msg;
        msg.which_pld = ServioToHost_commit_config_tag;
        return msg;
}

ServioToHost handle_message( dispatcher& dis, const HostToServio& msg )
{
        switch ( msg.which_pld ) {
        case HostToServio_set_mode_tag:
                return handle_set_mode( dis.now, dis.ctl, msg.set_mode );
        case HostToServio_get_property_tag:
                return handle_get_property(
                    dis.ctl,
                    dis.met,
                    dis.conv,
                    dis.pos_drv,
                    dis.curr_drv,
                    dis.vcc_drv,
                    dis.temp_drv,
                    dis.motor,
                    msg.get_property );
        case HostToServio_set_config_tag:
                return handle_set_config( dis.cfg_disp, msg.set_config );
        case HostToServio_get_config_tag:
                return handle_get_config( dis.cfg_disp, msg.get_config );
        case HostToServio_commit_config_tag:
                return handle_commit_config( dis.cfg_disp, dis.cfg_writer );
        case HostToServio_clear_config_tag:
                return handle_clear_config( dis.cfg_writer );
        default:
                return error_msg( "unknown command" );
        }
        return {};
}

template < typename InputType, typename OutputType, typename Handle >
std::tuple< bool, em::view< std::byte* > > process_message(
    em::view< const std::byte* > input_data,
    em::view< std::byte* >       output_buffer,
    Handle&&                     h )
{
        InputType  inpt;
        const bool succ = decode( input_data, inpt );
        if ( !succ ) {
                return { false, {} };
        }

        std::optional< OutputType > opt_res = h( inpt );
        if ( !opt_res.has_value() ) {
                return { false, {} };
        }

        return encode( output_buffer, *opt_res );
}

std::tuple< bool, em::view< std::byte* > > handle_message(
    dispatcher&                  dis,
    em::view< const std::byte* > input_data,
    em::view< std::byte* >       output_buffer )
{
        return process_message< HostToServio, ServioToHost >(
            input_data, output_buffer, [&dis]( const HostToServio& msg ) {
                    return handle_message( dis, msg );
            } );
}

std::tuple< bool, em::view< std::byte* > > handle_message_packet(
    dispatcher&                  dis,
    em::view< const std::byte* > input_data,
    em::view< std::byte* >       output_buffer )
{
        return process_message< HostToServioPacket, ServioToHostPacket >(
            input_data,
            output_buffer,
            [&dis]( const HostToServioPacket& msg ) -> std::optional< ServioToHostPacket > {
                    // TODO: check whenver id equals to currently configured id, or group id
                    // equals to currently configured group id
                    if ( !msg.has_payload ) {
                            return std::nullopt;
                    }
                    ServioToHostPacket res;
                    res.id          = msg.id;
                    res.has_payload = true;
                    res.payload     = handle_message( dis, msg.payload );
                    return res;
            } );
}

}  // namespace fw
