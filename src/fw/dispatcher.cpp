#include "fw/dispatcher.hpp"

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
                    -1.0f,
                    1.0f,
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
    const control&     ctl,
    const converter&   conv,
    const acquisition& acquis,
    const hbridge&     hb,
    const GetProperty& msg )
{
        Property prop;
        prop.which_pld = static_cast< pb_size_t >( msg.field_id );
        switch ( msg.field_id ) {
        case Property_mode_tag:
                prop.mode = get_mode( ctl );
                break;
        case Property_current_tag:
                prop.current = current( conv, acquis, hb );
                break;
        case Property_vcc_tag:
                prop.vcc = conv.vcc.convert( acquis.get_vcc() );
                break;
        case Property_temp_tag:
                prop.temp = conv.temp.convert( acquis.get_temp() );
                break;
        case Property_position_tag:
                prop.position = position( conv, acquis );
                break;
        default:
                return error_msg( "got unknown property" );
        }
        ServioToHost repl;
        repl.get_property = prop;
        repl.which_pld    = ServioToHost_get_property_tag;
        return repl;
}

ServioToHost handle_set_config( cfg_dispatcher& cfg_disp, const Config& req )
{
        bool key_found = map_cfg( req.which_pld, req, [&]< cfg_key K >( auto& val ) {
                cfg_disp.set< K >( val );
        } );
        if ( !key_found ) {
                return error_msg( "unknown key" );
        }
        ServioToHost msg;
        msg.which_pld = ServioToHost_set_config_tag;
        return msg;
}

ServioToHost handle_get_config( const cfg_dispatcher& cfg_disp, const GetConfig& req )
{
        ServioToHost msg;
        bool         key_found =
            map_cfg( req.field_id, msg.get_config, [&]< cfg_key K, typename T >( T& val ) {
                    if constexpr ( K == MODEL ) {
                            const model_name& n = cfg_disp.map.get_val< K >();
                            copy_string_to( n.data(), n.size(), val );
                    } else {
                            val = cfg_disp.map.get_val< K >();
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

ServioToHost handle_commit_config( const cfg_dispatcher& cfg_disp, const auto& cfg_writer )
{

        bool succ = cfg_writer( &cfg_disp.map );
        if ( !succ ) {
                return error_msg( "commit failed" );
        }

        ServioToHost msg;
        msg.which_pld = ServioToHost_commit_config_tag;
        return msg;
}

ServioToHost handle_clear_config( const auto& cfg_writer )
{

        bool succ = cfg_writer( nullptr );
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
                    dis.ctl, dis.conv, dis.acquis, dis.hb, msg.get_property );
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

}  // namespace fw
