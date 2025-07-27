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
    json::jval_ser&                       out )
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

        json::array_ser{ out }( "OK" );
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
    json::jval_ser&              out )
{
        json::array_ser as{ out };

        switch ( inpt ) {
        case iface::property::mode:
                as( "OK" );
                as( get_mode( ctl ) );
                break;
        case iface::property::current:
                as( "OK" );
                as( cnv::current( conv, curr_drv, motor ) );
                break;
        case iface::property::vcc:
                as( "OK" );
                as( conv.vcc.convert( vcc_drv.get_vcc() ) );
                break;
        case iface::property::temp:
                as( "OK" );
                as( conv.temp.convert( temp_drv.get_temperature() ) );
                break;
        case iface::property::position:
                as( "OK" );
                as( cnv::position( conv, pos_drv ) );
                break;
        case iface::property::velocity:
                as( "OK" );
                as( met.get_velocity() );
                break;
        default:
                as( "NOK" );
                as( "unknown property" );
                break;
        }
}

void handle_message( dispatcher& dis, vari::vref< iface::stmts const > inpt, json::jval_ser& out )
{
        inpt.visit(
            [&]( iface::mode_stmt const& m ) {
                    handle_set_mode( dis.now, dis.ctl, m.sub, out );
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
                        out );
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
                    json::array_ser as{ out };
                    cfg.sub.visit(
                        [&]( iface::cfg_set_stmt const& st ) {
                                cfg::on_cmd_set( cfg_disp.m, st.field, st.value.data )
                                    .visit(
                                        [&]( cfg::key k ) {
                                                cfg_disp.apply( k );
                                                as( "OK" );
                                        },
                                        [&]( str_err const& e ) {
                                                as( "NOK" );
                                                as( e.error );
                                        } );
                        },
                        [&]( iface::cfg_get_stmt const& s ) {
                                auto res = cfg::on_cmd_get( cfg_disp.m, s.field );
                                res.visit(
                                    [&]( vari::vref< cfg::base_t > x ) {
                                            as( "OK" );
                                            x.visit( as );
                                    },
                                    [&]( str_err const& e ) {
                                            as( "NOK" );
                                            as( e.error );
                                    } );
                        },
                        [&]( iface::cfg_list5_stmt const& ) {
                                // XXX
                        },
                        [&]( iface::cfg_commit_stmt const& ) {
                                if ( dis.stor_drv.store_cfg( dis.cfg_map ) == status::SUCCESS )
                                        as( "OK" );
                                else
                                        as( "NOK" );
                        },
                        [&]( iface::cfg_clear_stmt const& ) {
                                if ( dis.stor_drv.clear_cfg() == status::SUCCESS )
                                        as( "OK" );
                                else
                                        as( "NOK" );
                        } );
            },
            [&]( iface::info_stmt const& ) {
                    json::array_ser as{ out };
                    as( "OK" );
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
                    json::array_ser as{ out };
                    as( "NOK" );
                    as( "parse error" );
                    as( to_str( st.st ) );
                    return FAILURE;
            } );
        return { res, std::move( out ) };
}

}  // namespace servio::core
