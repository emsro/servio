#include "fw/dispatcher.hpp"

#include "cnv/utils.hpp"
#include "emlabcpp/outcome.h"
#include "emlabcpp/result.h"
#include "fw/map_cfg.hpp"
#include "fw/servio_pb.hpp"

namespace servio::fw
{

ServioToHost handle_set_mode( microseconds now, ctl::control& ctl, const Mode& msg )
{
        switch ( msg.which_pld ) {
        case Mode_disengaged_tag:
                ctl.disengage();
                break;
        case Mode_power_tag:
                ctl.switch_to_power_control( pwr( msg.power ) );
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

Mode get_mode( const ctl::control& ctl )
{
        Mode res;
        switch ( ctl.get_mode() ) {
        case control_mode::DISENGAGED:
                res.which_pld = Mode_disengaged_tag;
                break;
        case control_mode::POWER:
                res.which_pld = Mode_power_tag;
                res.power     = *ctl.get_power();
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
    const ctl::control&         ctl,
    const mtr::metrics&         met,
    const cnv::converter&       conv,
    const drv::pos_iface&       pos_drv,
    const drv::curr_iface&      curr_drv,
    const drv::vcc_iface&       vcc_drv,
    const drv::temp_iface&      temp_drv,
    const drv::pwm_motor_iface& motor,
    const GetProperty&          msg )
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
                // XXX: scaling issues /o\...
                if constexpr ( K == cfg::ENCODER_MODE )
                        cfg_disp.set< K >( static_cast< cfg::encoder_mode >( val ) );
                else
                        cfg_disp.set< K >( val );
        } );
        if ( !key_found )
                return error_msg( "unknown key" );
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
                            // TODO scaling issues right here /o\...
                    } else if constexpr ( K == cfg::ENCODER_MODE ) {
                            val = static_cast< EncoderMode >( cfg_disp.m.get_val< K >() );
                    } else {
                            val = cfg_disp.m.get_val< K >();
                    }
            } );
        if ( !key_found )
                return error_msg( "unknown key" );
        msg.get_config.which_pld =
            static_cast< uint16_t >( req.field_id );  // TODO: well this is not ideal
        msg.which_pld = ServioToHost_get_config_tag;
        return msg;
}

ServioToHost handle_commit_config( const cfg::dispatcher& cfg_disp, const auto& cfg_writer )
{

        const bool succ = cfg_writer( &cfg_disp.m );
        if ( !succ )
                return error_msg( "commit failed" );

        ServioToHost msg;
        msg.which_pld = ServioToHost_commit_config_tag;
        return msg;
}

ServioToHost handle_clear_config( const auto& cfg_writer )
{

        const bool succ = cfg_writer( nullptr );
        if ( !succ )
                return error_msg( "clear failed" );

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
std::tuple< em::outcome, em::view< std::byte* > > process_message(
    em::view< const std::byte* > input_data,
    em::view< std::byte* >       output_buffer,
    Handle&&                     h )
{
        InputType  inpt;
        const bool succ = decode( input_data, inpt );
        if ( !succ )
                return { em::FAILURE, {} };

        OutputType output;

        em::outcome res = h( inpt, output );
        if ( res != em::SUCCESS )
                return { res, {} };

        return encode( output_buffer, output );
}

std::tuple< em::outcome, em::view< std::byte* > > handle_message(
    dispatcher&                  dis,
    em::view< const std::byte* > input_data,
    em::view< std::byte* >       output_buffer )
{
        return process_message< HostToServio, ServioToHost >(
            input_data, output_buffer, [&dis]( const HostToServio& inpt, ServioToHost& out ) {
                    out = handle_message( dis, inpt );
                    return em::SUCCESS;
            } );
}

std::tuple< em::outcome, em::view< std::byte* > > handle_message_packet(
    dispatcher&                  dis,
    em::view< const std::byte* > input_data,
    em::view< std::byte* >       output_buffer )
{
        return process_message< HostToServioPacket, ServioToHostPacket >(
            input_data,
            output_buffer,
            [&dis]( const HostToServioPacket& inpt, ServioToHostPacket& out ) -> em::outcome {
                    // TODO: check whenver id equals to currently configured id, or group id
                    // equals to currently configured group id
                    if ( !inpt.has_payload )
                            return em::ERROR;
                    out.id          = inpt.id;
                    out.has_payload = true;
                    out.payload     = handle_message( dis, inpt.payload );
                    return em::SUCCESS;
            } );
}

}  // namespace servio::fw
