#include "fw/dispatcher.hpp"

#include "fw/map_cfg.hpp"

namespace fw
{

ServioToHost handle_message( microseconds now, control& ctl, const HostToServio_SetMode& msg )
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

ServioToHost handle_message(
    const converter&                conv,
    const acquisition&              acquis,
    const hbridge&                  hb,
    const HostToServio_GetProperty& msg )
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
        repl.status             = STATUS_SUCCESS;
        repl.get_property.value = val;
        repl.which_pld          = ServioToHost_get_property_tag;
        return repl;
}

ServioToHost handle_message( cfg_dispatcher& cfg_disp, const Config& req )
{
        map_cfg( req.which_pld, req, [&]< cfg_key K >( auto& val ) {
                cfg_disp.set< K >( val );
        } );
        ServioToHost msg;
        msg.which_pld = ServioToHost_set_config_tag;
        return msg;
}

ServioToHost handle_message( const cfg_dispatcher& cfg_disp, const HostToServio_GetConfig& req )
{
        ServioToHost msg;
        map_cfg( req.key, msg.get_config, [&]< cfg_key K, typename T >( T& val ) {
                if constexpr ( std::same_as< typename cfg_map::reg_value_type< K >, model_name > ) {
                        std::memset( val, '\0', sizeof( val ) );
                        const model_name& n = cfg_disp.map.get_val< K >();
                        // TODO: well, dis is bug to happen
                        std::strncpy( val, n.data(), std::min( n.size() + 1, sizeof( val ) - 1 ) );
                } else {
                        val = cfg_disp.map.get_val< K >();
                }
        } );
        msg.get_config.which_pld =
            static_cast< uint16_t >( req.key );  // TODO: well this is not ideal
        msg.which_pld = ServioToHost_get_config_tag;
        return msg;
}

ServioToHost handle_message(
    const cfg_dispatcher& cfg_disp,
    const auto&           cfg_writer,
    const HostToServio_CommitConfig& )
{
        std::ignore = cfg_disp;

        bool succ = cfg_writer( cfg_disp.map );
        if ( !succ ) {
                // TODO: well, this is aggresive
                fw::stop_exec();
        }

        ServioToHost msg;
        msg.which_pld = ServioToHost_commit_config_tag;
        return msg;
}

ServioToHost handle_message( dispatcher& dis, const HostToServio& msg )
{
        switch ( msg.which_pld ) {
        case HostToServio_set_mode_tag:
                return handle_message( dis.now, dis.ctl, msg.set_mode );
        case HostToServio_get_property_tag:
                return handle_message( dis.conv, dis.acquis, dis.hb, msg.get_property );
        case HostToServio_set_config_tag:
                return handle_message( dis.cfg_disp, msg.set_config );
        case HostToServio_get_config_tag:
                return handle_message( dis.cfg_disp, msg.get_config );
        case HostToServio_commit_config_tag:
                return handle_message( dis.cfg_disp, dis.cfg_writer, msg.commit_config );
        default:
                // TODO: well, this is aggresive
                fw::stop_exec();
        }
        // TODO: add default switch case for an error
        return {};
}

}  // namespace fw
