#include "./dispatcher.hpp"

#include "../cfg/storage.hpp"
#include "../cnv/utils.hpp"
#include "../lib/json_ser.hpp"
#include "../status.hpp"
#include "./map_cfg.hpp"

#include <git.h>

namespace servio::core
{
namespace
{

// XXX: this no longer has to be in fw/
bool store_persistent_config( drv::storage_iface& stor, cfg::payload& pl, cfg::map const* cfg )
{
        cfg::payload const pld{
            .git_ver  = git::Describe(),
            .git_date = git::CommitDate(),
            .id       = pl.id + 1,
        };
        constexpr std::size_t buffer_n = cfg::map::registers_count * sizeof( cfg::keyval ) + 128;
        std::array< std::byte, buffer_n > buffer;
        auto [succ, used_buffer] = cfg::store( pld, cfg, buffer );

        if ( !succ )
                return false;

        if ( stor.store_page( used_buffer ) == ERROR )
                return false;
        pl = pld;
        return true;
}

void handle_set_mode(
    microseconds                         now,
    ctl::control&                        ctl,
    vari::vref< iface::mode_vals const > inpt,
    json::jval_ser&                      out )
{
        inpt.visit(
            [&]( iface::kval< "disengaged"_a, void > const& ) {
                    ctl.disengage();
            },
            [&]( iface::kval< "power"_a, float > const& kv ) {
                    ctl.switch_to_power_control( pwr( kv.value ) );
            },
            [&]( iface::kval< "current"_a, float > const& kv ) {
                    ctl.switch_to_current_control( now, kv.value );
            },
            [&]( iface::kval< "velocity"_a, float > const& kv ) {
                    ctl.switch_to_velocity_control( now, kv.value );
            },
            [&]( iface::kval< "position"_a, float > const& kv ) {
                    ctl.switch_to_position_control( now, kv.value );
            } );

        json::array_ser{ out }( "OK" );
}

iface::mode_key get_mode( ctl::control const& ctl )
{
        switch ( ctl.get_mode() ) {
        case control_mode::DISENGAGED:
                return "disengaged"_a;
        case control_mode::POWER:
                return "power"_a;
        case control_mode::CURRENT:
                return "current"_a;
        case control_mode::VELOCITY:
                return "velocity"_a;
        case control_mode::POSITION:
                return "position"_a;
        }
        // XXX: maybe something better?
        return "disengaged"_a;
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
    iface::prop_key              inpt,
    json::jval_ser&              out )
{
        json::array_ser as{ out };
        as( "OK" );

        float x = 0;
        if ( inpt == "mode"_a ) {
                as( get_mode( ctl ).to_string() );
                return;
        } else if ( inpt == "current"_a )
                x = cnv::current( conv, curr_drv, motor );
        else if ( inpt == "vcc"_a )
                x = conv.vcc.convert( vcc_drv.get_vcc() );
        else if ( inpt == "temp"_a )
                x = conv.temp.convert( temp_drv.get_temperature() );
        else if ( inpt == "position"_a )
                x = cnv::position( conv, pos_drv );
        else if ( inpt == "velocity"_a )
                x = met.get_velocity();

        as( x );
}

void handle_set_config(
    cfg::dispatcher&                    cfg_disp,
    vari::vref< iface::cfg_vals const > v,
    json::jval_ser&                     out )
{
        json::array_ser as{ out };
        as( "OK" );

        v.visit( [&]< auto K, typename T >( iface::kval< K, T > const& kv ) {
                static constexpr auto cfg_k = iface_to_cfg[( (iface::cfg_key) K ).value()];

                // XXX: scaling issues /o\...
                if constexpr ( cfg_k == cfg::ENCODER_MODE ) {
                        if ( kv.value == "analog"_a )
                                cfg_disp.set< cfg_k >( cfg::ENC_MODE_ANALOG );
                        else if ( kv.value == "quad"_a )
                                cfg_disp.set< cfg_k >( cfg::ENC_MODE_QUAD );
                } else {
                        cfg_disp.set< cfg_k >( kv.value );
                }
        } );
}

void handle_get_config( cfg::dispatcher const& cfg_disp, iface::cfg_key ck, json::jval_ser& out )
{

        cfg::key cfg_k = iface_to_cfg[ck.value()];

        json::array_ser as{ out };
        as( "OK" );

        cfg_disp.m.with_register( cfg_k, [&]< typename R >( R& reg ) {
                if constexpr ( R::key == cfg::ENCODER_MODE )
                        switch ( reg.value ) {
                        case cfg::ENC_MODE_ANALOG:
                                as( "analog" );
                                break;
                        case cfg::ENC_MODE_QUAD:
                                as( "quad" );
                                break;
                        }
                else
                        as( reg.value );
        } );
}

void handle_message( dispatcher& dis, vari::vref< iface::stmt const > inpt, json::jval_ser& out )
{
        inpt.visit(
            [&]( iface::mode_stmt const& s ) {
                    handle_set_mode( dis.now, dis.ctl, s.arg, out );
            },
            [&]( iface::prop_stmt const& s ) {
                    handle_get_property(
                        dis.ctl,
                        dis.met,
                        dis.conv,
                        dis.pos_drv,
                        dis.curr_drv,
                        dis.vcc_drv,
                        dis.temp_drv,
                        dis.motor,
                        s.prop,
                        out );
            },
            [&]( vari::vref< iface::cfg_set_stmt const, iface::cfg_get_stmt const > s ) {
                    cfg::dispatcher cfg_disp{
                        .m     = dis.cfg_map,
                        .ctl   = dis.ctl,
                        .conv  = dis.conv,
                        .met   = dis.met,
                        .mon   = dis.mon,
                        .motor = dis.motor,
                        .pos   = dis.pos_drv,
                    };
                    s.visit(
                        [&]( iface::cfg_set_stmt const& st ) {
                                handle_set_config( cfg_disp, st.val, out );
                        },
                        [&]( iface::cfg_get_stmt const& s ) {
                                handle_get_config( cfg_disp, s.k, out );
                        } );
            },
            [&]( iface::cfg_commit_stmt const& ) {
                    json::array_ser as{ out };
                    if ( store_persistent_config( dis.stor_drv, dis.cfg_pl, &dis.cfg_map ) )
                            as( "OK" );
                    else
                            as( "NOK" );
            },
            [&]( iface::cfg_clear_stmt const& ) {
                    json::array_ser as{ out };
                    if ( store_persistent_config( dis.stor_drv, dis.cfg_pl, nullptr ) )
                            as( "OK" );
                    else
                            as( "NOK" );
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
                    handle_message( dis, s, out );
                    return SUCCESS;
            },
            [&]( iface::invalid_stmt& ) -> R {
                    json::array_ser as{ out };
                    as( "NOK" );
                    as( "parse error" );
                    return FAILURE;
            } );
        return { res, std::move( out ) };
}

}  // namespace servio::core
