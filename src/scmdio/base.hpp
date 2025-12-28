

#include <cstdint>
//
#include <avakar/atom.h>
#include <boost/asio.hpp>
#include <emlabcpp/experimental/string_buffer.h>
#include <emlabcpp/view.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#pragma once

namespace servio::scmdio
{
using boost::asio::awaitable;
using boost::asio::detached;
using boost::asio::io_context;
using boost::asio::use_awaitable;

template < typename T >
using sptr = std::shared_ptr< T >;

template < typename T >
using opt = std::optional< T >;

struct servio_exception : std::runtime_error
{
        using std::runtime_error::runtime_error;
};

template < typename... Args >
[[noreturn]] inline void log_error( spdlog::format_string_t< Args... > fmt, Args&&... args )
{
        spdlog::error( fmt, (Args&&) args... );
        throw servio_exception{ "Critical error occured" };
}

inline std::filesystem::path const& check_path( std::filesystem::path const& p )
{
        if ( !std::filesystem::exists( p ) )
                throw std::runtime_error( "Path " + p.string() + " does not exist" );
        return p;
}

}  // namespace servio::scmdio

template < avakar::atom_literal A >
struct std::formatter< A, char >
{
        template < typename ParseContext >
        constexpr ParseContext::iterator parse( ParseContext& ctx )
        {
                return ctx.begin();
        }

        template < class FmtContext >
        FmtContext::iterator format( A const&, FmtContext& ctx ) const
        {
                return std::formatter< std::string_view, char >{}.format( A::to_string(), ctx );
        }
};

template < auto... As >
struct std::formatter< avakar::atom< As... >, char >
{
        template < typename ParseContext >
        constexpr ParseContext::iterator parse( ParseContext& ctx )
        {
                return ctx.begin();
        }

        template < class FmtContext >
        FmtContext::iterator format( avakar::atom< As... > const& s, FmtContext& ctx ) const
        {
                return std::formatter< std::string_view, char >{}.format( s.to_string(), ctx );
        }
};

template < typename T >
struct std::formatter< std::optional< T >, char >
{
        template < typename ParseContext >
        constexpr ParseContext::iterator parse( ParseContext& ctx )
        {
                return ctx.begin();
        }

        template < class FmtContext >
        FmtContext::iterator format( std::optional< T > const& s, FmtContext& ctx ) const
        {
                if ( s )
                        return std::formatter< T, char >{}.format( *s, ctx );
                else
                        return std::formatter< std::string_view, char >{}.format( "none", ctx );
        }
};

template <>
struct std::formatter< std::byte, char >
{
        template < typename ParseContext >
        constexpr ParseContext::iterator parse( ParseContext& ctx )
        {
                return ctx.begin();
        }

        template < class FmtContext >
        FmtContext::iterator format( std::byte const& s, FmtContext& ctx ) const
        {
                return std::formatter< int, char >{}.format( (int) s, ctx );
        }
};

template < typename T >
struct std::formatter< std::span< T >, char >
{
        template < typename ParseContext >
        constexpr ParseContext::iterator parse( ParseContext& ctx )
        {
                return ctx.begin();
        }

        template < class FmtContext >
        FmtContext::iterator format( std::span< T > const& v, FmtContext& ctx ) const
        {
                auto&& out = ctx.out();
                format_to( out, "[" );
                if ( !v.empty() )
                        format_to( out, "{}", v[0] );
                for ( std::size_t i = 1; i < v.size(); ++i )
                        format_to( out, ", {}", v[i] );
                return format_to( out, "]" );
        }
};

// XXX: duplication from span much
template < typename T >
struct std::formatter< emlabcpp::view< T >, char >
{
        template < typename ParseContext >
        constexpr ParseContext::iterator parse( ParseContext& ctx )
        {
                return ctx.begin();
        }

        template < class FmtContext >
        FmtContext::iterator format( emlabcpp::view< T > const& v, FmtContext& ctx ) const
        {
                auto&& out = ctx.out();
                format_to( out, "[" );
                if ( !v.empty() )
                        format_to( out, "{}", v[0] );
                for ( std::size_t i = 1; i < v.size(); ++i )
                        format_to( out, ", {}", v[i] );
                return format_to( out, "]" );
        }
};

template < std::size_t N >
struct std::formatter< emlabcpp::string_buffer< N >, char >
{
        template < typename ParseContext >
        constexpr ParseContext::iterator parse( ParseContext& ctx )
        {
                return ctx.begin();
        }

        template < class FmtContext >
        FmtContext::iterator format( emlabcpp::string_buffer< N > const& v, FmtContext& ctx ) const
        {
                return std::formatter< std::string_view, char >{}.format( v, ctx );
        }
};

template <>
struct std::formatter< nlohmann::json, char >
{
        bool pretty = false;

        template < class ParseContext >
        constexpr ParseContext::iterator parse( ParseContext& ctx )
        {
                auto it = ctx.begin();
                if ( it == ctx.end() )
                        return it;

                if ( *it == '#' )  // Use `#` to indicate pretty formatting
                {
                        pretty = true;
                        ++it;
                }
                if ( it != ctx.end() && *it != '}' )
                        throw std::format_error( "Invalid format args for nlohmann::json." );

                return it;
        }

        template < class FmtContext >
        FmtContext::iterator format( nlohmann::json const& j, FmtContext& ctx ) const
        {
                std::ostringstream out;
                if ( pretty )
                        out << std::setw( 4 ) << j;
                else
                        out << j;  // Compact JSON

                return std::ranges::copy( out.str(), ctx.out() ).out;
        }
};
