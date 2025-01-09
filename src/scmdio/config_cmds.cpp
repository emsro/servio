#include "./field_util.hpp"
#include "./serial.hpp"

#include <boost/asio.hpp>
#include <emlabcpp/algorithm.h>
#include <fstream>

namespace em = emlabcpp;

namespace servio::scmdio
{
namespace
{
void print_configs( std::vector< vari::vval< iface::cfg_vals > > const& out )
{
        std::cout << em::joined( out, std::string{ "\n" }, [&]( auto const& cfg ) {
                return cfg.visit( [&]( auto& c ) {
                        return std::format( "{{ {},{} }}", c.key.to_string(), c.value );
                } );
        } ) << std::endl;
}

void print_configs_json( std::vector< vari::vval< iface::cfg_vals > > const& out )
{
        std::cout << "["
                  << em::joined(
                         em::view( out ),
                         std::string{ ",\n " },
                         [&]( vari::vref< iface::cfg_vals const > cfg ) {
                                 return cfg.visit( [&]( auto const& c ) {
                                         return std::format(
                                             "{{ {}:{} }}", c.key.to_string(), c.value );
                                 } );
                         } )
                  << "]" << std::endl;
}
}  // namespace

awaitable< void > cfg_query_cmd( sptr< port_iface > port, bool json )
{
        std::vector< vari::vval< iface::cfg_vals > > out = co_await get_full_config( *port );

        if ( json )
                print_configs_json( out );
        else
                print_configs( out );
}

awaitable< void > cfg_commit_cmd( sptr< port_iface > port )
{
        std::string msg = std::format( "cfg cmt" );

        auto reply = co_await exchg( *port, msg );
        // XXX: check the retcode
        std::ignore = reply;
}

awaitable< void > cfg_clear_cmd( sptr< port_iface > port )
{
        std::string msg   = std::format( "cfg clr" );
        auto        reply = co_await exchg( *port, msg );
        std::ignore       = reply;
}

awaitable< void > cfg_get_cmd( sptr< port_iface > port, std::string const& name, bool json )
{
        auto field = iface::cfg_key::from_string( name );
        // XXX throw?
        if ( !field ) {
                std::cerr << "err" << std::endl;
                co_return;
        }

        auto val = co_await get_config_field( *port, *field );

        val.visit( [&]( auto& v ) {
                if ( json )
                        std::cout << std::format( "[{{ {}:{} }}]", name, v.value ) << std::endl;
                else
                        std::cout << std::format( "{},{}", name, v.value ) << std::endl;
        } );
}

awaitable< void > cfg_set_cmd( sptr< port_iface > port, std::string const& name, std::string value )
{

        auto v = kval_ser< iface::cfg_vals >::ser( name, value );
        if ( v )
                co_await set_config_field( *port, v.vref() );
        else
                throw "XXX";
}

awaitable< void > cfg_load_cmd( sptr< port_iface > port, std::filesystem::path const& cfg )
{
        std::ifstream fd( cfg );

        auto j = nlohmann::json::parse( fd );

        for ( auto&& [key, val] : j.items() )
                co_await cfg_set_cmd( port, key, val.dump() );
}

}  // namespace servio::scmdio
