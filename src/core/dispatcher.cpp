#include "./dispatcher.hpp"

#include "../cfg/handler.hpp"
#include "../cnv/utils.hpp"
#include "../lib/atom_visit.hpp"
#include "../lib/json_ser.hpp"
#include "../status.hpp"

#include <git.h>
#include <string_view>

namespace servio::core
{
using namespace avakar::literals;
using namespace std::string_view_literals;

namespace
{

void handle_set_mode(
    microseconds                          now,
    ctl::control&                         ctl,
    vari::vref< iface::mode_stmts const > inpt,
    iface::root_ser                       out )
{
        inpt.visit(
            [&]( iface::mode_disengaged_stmt const& ) {
                    ctl.disengage();
            },
            [&]( iface::mode_power_stmt const& kv ) {
                    ctl.switch_to_power_control( pwr( kv.power ) );
            },
            [&]( iface::mode_current_stmt const& kv ) {
                    ctl.switch_to_current_control( now, kv.current );
            },
            [&]( iface::mode_velocity_stmt const& kv ) {
                    ctl.switch_to_velocity_control( now, kv.velocity );
            },
            [&]( iface::mode_position_stmt const& kv ) {
                    ctl.switch_to_position_control( now, kv.position );
            } );

        std::move( out ).ok();
}

std::string_view get_mode( ctl::control const& ctl )
{
        switch ( ctl.get_mode() ) {
        case control_mode::DISENGAGED:
                return "disengaged";
        case control_mode::POWER:
                return "power";
        case control_mode::CURRENT:
                return "current";
        case control_mode::VELOCITY:
                return "velocity";
        case control_mode::POSITION:
                return "position";
        }
        // XXX: maybe something better?
        return "disengaged";
}

void handle_get_property(
    ctl::control const&          ctl,
    mtr::metrics const&          met,
    cnv::converter const&        conv,
    drv::get_pos_iface const&    pos_drv,
    drv::get_curr_iface const&   curr_drv,
    drv::vcc_iface const&        vcc_drv,
    drv::temp_iface const&       temp_drv,
    drv::motor_info_iface const& motor,
    iface::property              inpt,
    iface::root_ser              out )
{
        switch ( inpt ) {
        case iface::property::mode:
                std::move( out ).ok()( get_mode( ctl ) );
                break;
        case iface::property::current:
                std::move( out ).ok()( cnv::current( conv, curr_drv, motor ) );
                break;
        case iface::property::vcc:
                std::move( out ).ok()( conv.vcc.convert( vcc_drv.get_vcc() ) );
                break;
        case iface::property::temp:
                std::move( out ).ok()( conv.temp.convert( temp_drv.get_temperature() ) );
                break;
        case iface::property::position:
                std::move( out ).ok()( cnv::position( conv, pos_drv ) );
                break;
        case iface::property::velocity:
                std::move( out ).ok()( met.get_velocity() );
                break;
        default:
                std::move( out ).nok()( "unknown property" );
                break;
        }
}

void handle_message( dispatcher& dis, vari::vref< iface::stmts const > inpt, iface::root_ser out )
{
        inpt.visit(
            [&]( iface::mode_stmt const& m ) {
                    handle_set_mode( dis.now, dis.ctl, m.sub, std::move( out ) );
            },
            [&]( iface::prop_stmt const& p ) {
                    handle_get_property(
                        dis.ctl,
                        dis.met,
                        dis.conv,
                        dis.pos_drv,
                        dis.curr_drv,
                        dis.vcc_drv,
                        dis.temp_drv,
                        dis.motor,
                        p.name,
                        std::move( out ) );
            },
            [&]( iface::cfg_stmt const& cfg ) {
                    cfg::dispatcher cfg_disp{
                        .m     = dis.cfg_map,
                        .ctl   = dis.ctl,
                        .conv  = dis.conv,
                        .met   = dis.met,
                        .mon   = dis.mon,
                        .motor = dis.motor,
                        .pos   = dis.pos_drv,
                    };
                    cfg.sub.visit(
                        [&]( iface::cfg_set_stmt const& st ) {
                                auto opt_key = cfg::cmd_iface::on_cmd_set(
                                    cfg_disp.m, st.field, st.value.data, std::move( out ) );
                                if ( opt_key )
                                        cfg_disp.apply( *opt_key );
                        },
                        [&]( iface::cfg_get_stmt const& s ) {
                                cfg::cmd_iface::on_cmd_get( cfg_disp.m, s.field, std::move( out ) );
                        },
                        [&]( iface::cfg_list5_stmt const& st ) {
                                cfg::cmd_iface::on_cmd_list5< cfg::map >(
                                    st.offset, std::move( out ) );
                        },
                        [&]( iface::cfg_commit_stmt const& ) {
                                if ( dis.stor_drv.store_cfg( dis.cfg_map ) == status::SUCCESS )
                                        std::move( out ).ok();
                                else
                                        std::move( out ).nok();
                        },
                        [&]( iface::cfg_clear_stmt const& ) {
                                if ( dis.stor_drv.clear_cfg() == status::SUCCESS )
                                        std::move( out ).ok();
                                else
                                        std::move( out ).nok();
                        } );
            },
            [&]( iface::info_stmt const& ) {
                    json::array_ser  as = std::move( out ).ok();
                    json::object_ser obj{ as };
                    obj( "version", git::Describe() );
                    obj( "commit", git::CommitSHA1() );
            } );
}
}  // namespace

std::tuple< status, em::view< std::byte* > > handle_message(
    dispatcher&                  dis,
    em::view< std::byte const* > input_data,
    em::view< std::byte* >       output_buffer )
{
        // XXX: well, technically, em::view could've had data if it has pointer?
        std::string_view inpt{ (char const*) input_data.begin(), input_data.size() };

        json::jval_ser out{ output_buffer };
        using R = status;

        auto res = iface::parse( inpt ).visit(
            [&]( vari::vref< iface::stmt > s ) -> R {
                    handle_message( dis, s->sub, out );
                    return SUCCESS;
            },
            [&]( iface::invalid_stmt& st ) -> R {
                    iface::root_ser root{ out };
                    std::move( root ).nok()( "parse_error" )( to_str( st.st ) );
                    return FAILURE;
            } );
        return { res, std::move( out ) };
}

}  // namespace servio::core
