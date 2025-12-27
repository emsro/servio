
#pragma once

#include "../cnv/converter.hpp"
#include "../core/drivers.hpp"
#include "../gov/governor_manager.hpp"
#include "../mon/monitor.hpp"
#include "../mtr/metrics.hpp"
#include "./def.hpp"
#include "./handler.hpp"

namespace servio::cfg
{

inline vari::vval< vari::vref< iface >, str_err >
find_iface( iface* root_handler, gov::governor_manager& gm, std::string_view gov )
{
        if ( gov == "" ) {
                if ( root_handler )
                        return vari::vref< iface >{ *root_handler };
                return "unknown governor"_err;
        }
        for ( vari::vref< gov::governor > g : gm.governors() )
                if ( g->name() == gov ) {
                        auto* c = g->get_cfg();
                        if ( c )
                                return vari::vref< iface >{ *c };
                        return "governor without config"_err;
                }
        return "unknown governor"_err;
}

struct resolve_governor_res
{
        std::string_view governor;
        std::string_view field;
};

inline opt< resolve_governor_res >
resolve_governor( std::string_view field, std::string_view governor )
{
        auto npos = field.find( '.' );
        if ( npos != std::string_view::npos ) {
                if ( !governor.empty() )
                        return {};
                return resolve_governor_res{
                    .governor = field.substr( 0, npos ), .field = field.substr( npos + 1 ) };
        } else {
                return resolve_governor_res{ .governor = governor, .field = field };
        }
}

struct root_handler
{
        map&                  m;
        cfg::handler< map >   m_handler{ m, em::member_function< &root_handler::apply >{ *this } };
        cnv::converter&       conv;
        mtr::metrics&         met;
        mon::monitor&         mon;
        drv::pwm_motor_iface& motor;
        drv::get_pos_iface&   pos;

        template < key Key, typename T >
        void set( T const& item )
        {
                m.ref_by_key< Key >() = item;
                this->apply( Key );
        }

        template < key Key >
        auto const& get() const
        {
                return m.ref_by_key< Key >();
        }

        void full_apply();
        void apply( key k );
};

struct dispatcher
{
        cfg::iface*            root_handler;
        gov::governor_manager& gov;

        // Dispatcher honors advanced syntax, there are two ways to set field of governor:
        // 1) Using dot syntax: "cfg set governor_name.field_name value"
        // 2) Using governor argument: "cfg set field_name value governor=governor_name"
        // @param field is either name of param or dot syntax with governor name
        // @param governor is name of explicit governor parameter or empty string

        opt_str_err on_cmd_set(
            std::string_view                        field,
            vari::vref< servio::cfg::base_t const > value,
            std::string_view                        governor )
        {
                auto r = resolve_governor( field, governor );
                if ( !r )
                        return "dot syntax collides with governor"_err;

                return find_iface( root_handler, gov, r->governor )
                    .visit(
                        [&]( vari::vref< iface > ifc ) -> opt_str_err {
                                return ifc->on_cmd_set( r->field, value );
                        },
                        [&]( str_err const& e ) -> opt_str_err {
                                return e;
                        } );
        }

        vari::vval< base_t, str_err >
        on_cmd_get( std::string_view field, std::string_view governor )
        {
                auto r = resolve_governor( field, governor );
                if ( !r )
                        return "dot syntax collides with governor"_err;

                using R = vari::vval< base_t, str_err >;
                return find_iface( root_handler, gov, r->governor )
                    .visit(
                        [&]( vari::vref< iface > ifc ) -> R {
                                return ifc->on_cmd_get( r->field );
                        },
                        [&]( str_err const& e ) -> R {
                                return e;
                        } );
        }

        vari::vval< std::string_view, str_err >
        on_cmd_list( int32_t offset, std::string_view governor )
        {
                using R = vari::vval< std::string_view, str_err >;
                return find_iface( root_handler, gov, governor )
                    .visit(
                        [&]( vari::vref< iface > ifc ) -> R {
                                return ifc->on_cmd_list( offset );
                        },
                        [&]( str_err const& e ) -> R {
                                return e;
                        } );
        }
};

}  // namespace servio::cfg
