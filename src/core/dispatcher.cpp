#include "./dispatcher.hpp"

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
void handle_get_property(
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

void handle_gov_message(
    vari::vref< iface::govctl_stmts const > inpt,
    em::pmr::memory_resource&               mem,
    gov::governor_manager&                  gv,
    iface::root_ser                         out )
{
        inpt.visit(
            [&]( iface::govctl_activate_stmt const& a ) {
                    auto s = gv.activate( a.governor, mem );
                    if ( s == status::SUCCESS )
                            std::move( out ).ok();
                    else
                            std::move( out ).nok()( to_str( s ) );
            },
            [&]( iface::govctl_deactivate_stmt const& ) {
                    auto s = gv.deactivate();
                    if ( s == status::SUCCESS )
                            std::move( out ).ok();
                    else
                            std::move( out ).nok();
            },
            [&]( iface::govctl_active_stmt const& ) {
                    auto* gov = gv.active();
                    if ( !gov )
                            std::move( out ).ok();
                    else
                            std::move( out ).ok()( gov->name() );
            },
            [&]( iface::govctl_list_stmt const& st ) {
                    auto      i    = (uint32_t) st.index;
                    std::span govs = gv.governors();
                    if ( i < govs.size() )
                            std::move( out ).ok()( govs[i]->name() );
                    else
                            std::move( out ).ok();
            } );
}

void handle_message( dispatcher& dis, vari::vref< iface::stmts > inpt, iface::root_ser out )
{
        inpt.visit(
            [&]( iface::govctl_stmt const& g ) {
                    handle_gov_message( g.sub, dis.mem, dis.gov, std::move( out ) );
            },
            [&]( iface::gov_stmt& g ) {
                    auto cmd = g.parser.next_cmd();
                    if ( !cmd ) {
                            std::move( out ).nok()( "missing governor" );
                            return;
                    }
                    for ( auto gov : dis.gov.governors() ) {
                            if ( gov->name() == *cmd ) {
                                    std::ignore =
                                        gov->on_cmd( std::move( g.parser ), std::move( out ) );
                                    return;
                            }
                    }
            },
            [&]( iface::prop_stmt const& p ) {
                    handle_get_property(
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
                    cfg::root_handler rh{
                        .m     = dis.cfg_map,
                        .conv  = dis.conv,
                        .met   = dis.met,
                        .mon   = dis.mon,
                        .motor = dis.motor,
                        .pos   = dis.pos_drv,
                    };
                    cfg::dispatcher cfg_disp{
                        .root_handler = &rh.m_handler,
                        .gov          = dis.gov,
                    };
                    cfg.sub.visit(
                        [&]( iface::cfg_set_stmt const& st ) {
                                auto opt_err =
                                    cfg_disp.on_cmd_set( st.field, st.value.data, st.governor );
                                if ( opt_err )
                                        std::move( out ).nok()( opt_err.error );
                                else
                                        std::move( out ).ok();
                        },
                        [&]( iface::cfg_get_stmt const& st ) {
                                cfg_disp.on_cmd_get( st.field, st.governor )
                                    .visit(
                                        [&]( vari::vref< cfg::base_t > value ) {
                                                value.visit( std::move( out ).ok() );
                                        },
                                        [&]( str_err error ) {
                                                std::move( out ).nok()( error.error );
                                        } );
                        },
                        [&]( iface::cfg_list_stmt const& st ) {
                                cfg_disp.on_cmd_list( st.index, st.governor )
                                    .visit(
                                        [&]( std::string_view value ) {
                                                if ( value != "" )
                                                        std::move( out ).ok()( value );
                                                else
                                                        std::move( out ).ok();
                                        },
                                        [&]( str_err error ) {
                                                std::move( out ).nok()( error.error );
                                        } );
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

        parser::parser p{ inpt };
        auto           res = iface::parse( p ).visit(
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
