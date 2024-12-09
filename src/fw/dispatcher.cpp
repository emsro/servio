#include "fw/dispatcher.hpp"

#include "cnv/utils.hpp"
#include "emlabcpp/outcome.h"
#include "emlabcpp/result.h"
#include "fw/map_cfg.hpp"

namespace servio::fw
{
namespace
{
std::string_view
handle_set_mode( microseconds now, ctl::control& ctl, vari::vref< iface::mode_vals const > inpt )
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
        return "ok";
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

std::string_view fmt_arithm( em::view< char* > buff, auto&& x )
{
        // XXX: check ec
        // auto [ptr, ec] = std::to_chars( buff.begin(), buff.end(), x );
        return {};
}

std::string_view handle_get_property(
    ctl::control const&         ctl,
    mtr::metrics const&         met,
    cnv::converter const&       conv,
    drv::pos_iface const&       pos_drv,
    drv::curr_iface const&      curr_drv,
    drv::vcc_iface const&       vcc_drv,
    drv::temp_iface const&      temp_drv,
    drv::pwm_motor_iface const& motor,
    iface::prop_key             inpt,
    em::view< char* >           buffer )
{
        float x = 0;
        if ( inpt == "mode"_a )
                return get_mode( ctl ).to_string();
        else if ( inpt == "current"_a )
                x = cnv::current( conv, curr_drv, motor );
        else if ( inpt == "vcc"_a )
                x = conv.vcc.convert( vcc_drv.get_vcc() );
        else if ( inpt == "temp"_a )
                x = conv.temp.convert( temp_drv.get_temperature() );
        else if ( inpt == "position"_a )
                x = cnv::position( conv, pos_drv );
        else if ( inpt == "velocity"_a )
                x = met.get_velocity();

        return fmt_arithm( buffer, x );
}

std::string_view
handle_set_config( cfg::dispatcher& cfg_disp, vari::vref< iface::cfg_vals const > v )
{
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
        return "ok";
}

std::string_view
handle_get_config( cfg::dispatcher const& cfg_disp, iface::cfg_key ck, em::view< char* > buffer )
{
        std::string_view res;

        cfg::key cfg_k = iface_to_cfg[ck.value()];

        cfg_disp.m.with_register( cfg_k, [&]< typename R >( R& reg ) {
                // XXX: bounds check for cfg::MODEL? or ANYTHING?
                if constexpr ( R::key == cfg::MODEL )
                        res = copy_string_to( reg.value, buffer );
                else if constexpr ( R::key == cfg::ENCODER_MODE )
                        switch ( cfg_disp.m.get_val< R::key >() ) {
                        case cfg::ENC_MODE_ANALOG:
                                res = copy_string_to( "analog", buffer );
                                break;
                        case cfg::ENC_MODE_QUAD:
                                res = copy_string_to( "quad", buffer );
                                break;
                        }
                else if constexpr ( R::key == cfg::INVERT_HBRIDGE )
                        if ( cfg_disp.m.get_val< cfg::INVERT_HBRIDGE >() )
                                res = copy_string_to( "true", buffer );
                        else
                                res = copy_string_to( "false", buffer );
                else
                        res = fmt_arithm( buffer, cfg_disp.m.get_val< R::key >() );
        } );
        return res;
}

std::string_view handle_commit_config( cfg::dispatcher const& cfg_disp, auto const& cfg_writer )
{
        bool const succ = cfg_writer( &cfg_disp.m );
        if ( !succ )
                return "failed";
        return "ok";
}

std::string_view handle_clear_config( auto const& cfg_writer )
{
        bool const succ = cfg_writer( nullptr );
        if ( !succ )
                return "failed";
        return "ok";
}

std::string_view
handle_message( dispatcher& dis, vari::vref< iface::stmt const > inpt, em::view< char* > buffer )
{
        return inpt.visit(
            [&]( iface::mode_stmt const& s ) {
                    return handle_set_mode( dis.now, dis.ctl, s.arg );
            },
            [&]( iface::prop_stmt const& s ) {
                    return handle_get_property(
                        dis.ctl,
                        dis.met,
                        dis.conv,
                        dis.pos_drv,
                        dis.curr_drv,
                        dis.vcc_drv,
                        dis.temp_drv,
                        dis.motor,
                        s.prop,
                        buffer );
            },
            [&]( iface::cfg_set_stmt const& s ) {
                    return handle_set_config( dis.cfg_disp, s.val );
            },
            [&]( iface::cfg_get_stmt const& s ) {
                    return handle_get_config( dis.cfg_disp, s.k, buffer );
            },
            [&]( iface::cfg_commit_stmt const& ) {
                    return handle_commit_config( dis.cfg_disp, dis.cfg_writer );
            },
            [&]( iface::cfg_clear_stmt const& ) {
                    return handle_clear_config( dis.cfg_writer );
            } );
}
}  // namespace

std::tuple< em::outcome, em::view< std::byte* > > handle_message(
    dispatcher&                  dis,
    em::view< std::byte const* > input_data,
    em::view< std::byte* >       output_buffer )
{
        // XXX: well, technically, em::view could've had data if it has pointer?
        std::string_view inpt{ (char const*) input_data.begin(), input_data.size() };

        using R = std::tuple< em::outcome, em::view< std::byte* > >;
        return iface::parse( inpt ).visit(
            [&]( vari::vref< iface::stmt > s ) -> R {
                    std::string_view res = handle_message(
                        dis, s, em::view_n( (char*) output_buffer.begin(), output_buffer.size() ) );
                    // XXX: how to detect failure?
                    return { em::SUCCESS, em::view_n( (std::byte*) res.data(), res.size() ) };
            },
            [&]( iface::invalid_stmt& ) -> R {
                    // XXX: proapgate error?
                    return { em::FAILURE, {} };
            } );
}

}  // namespace servio::fw
