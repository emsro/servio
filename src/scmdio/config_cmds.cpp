#include "./field_util.hpp"
#include "./serial.hpp"

#include <boost/asio.hpp>
#include <emlabcpp/algorithm.h>
#include <fstream>
#include <iostream>

namespace em = emlabcpp;

namespace servio::scmdio
{
namespace
{
void print_configs( std::map< std::string, nlohmann::json > const& out )
{
        std::cout << em::joined( out, std::string{ "\n" }, [&]( auto const& cfg ) -> std::string {
                return std::format( "{}:\t{}", cfg.first, cfg.second );
        } ) << std::endl;
}

void print_configs_json( std::map< std::string, nlohmann::json > const& out )
{
        std::cout << "["
                  << em::joined(
                         em::view( out ),
                         std::string{ ",\n " },
                         [&]( auto const& cfg ) -> std::string {
                                 return std::format( "{{ {}:{} }}", cfg.first, cfg.second );
                         } )
                  << "]" << std::endl;
}
}  // namespace

awaitable< void > cfg_query_cmd( sptr< port_iface > port, bool json )
{
        auto out = co_await get_full_config( *port );

        if ( json )
                print_configs_json( out );
        else
                print_configs( out );
}

awaitable< void > cfg_commit_cmd( sptr< port_iface > port )
{
        std::string msg = std::format( "cfg commit" );

        auto reply = co_await exchg( *port, msg );
        // XXX: check the retcode
        std::ignore = reply;
}

awaitable< void > cfg_clear_cmd( sptr< port_iface > port )
{
        std::string msg   = std::format( "cfg clear" );
        auto        reply = co_await exchg( *port, msg );
        std::ignore       = reply;
}

awaitable< void > cfg_get_cmd( sptr< port_iface > port, std::string const& name, bool json )
{
        auto val = co_await get_config_field( *port, name );

        if ( json )
                std::cout << std::format( "[{{ {}:{} }}]", name, val ) << std::endl;
        else
                std::cout << std::format( "{},{}", name, val ) << std::endl;
}

awaitable< void >
cfg_set_cmd( sptr< port_iface > port, std::string const& name, nlohmann::json value )
{
        co_await set_config_field( *port, name, value );
}

awaitable< void > cfg_load_cmd( sptr< port_iface > port, std::filesystem::path const& cfg )
{
        std::ifstream fd( cfg );

        auto j = nlohmann::json::parse( fd );

        for ( auto&& [key, val] : j.items() )
                co_await cfg_set_cmd( port, key, val.dump() );
}

}  // namespace servio::scmdio
