#pragma once

#include "./str_lib.hpp"

#include <avakar/atom.h>
#include <emlabcpp/experimental/string_buffer.h>
#include <string_view>
#include <vari/vopt.h>

namespace servio::parser
{
using avakar::atom;
using namespace avakar::literals;
namespace em = emlabcpp;

template < typename T >
using opt = std::optional< T >;

inline vari::vopt< std::string_view > parse_str( char const*& p, char const* e ) noexcept
{

        char const* pp = p;
        if ( *p != '"' )
                return {};
        while ( pp != e ) {
                pp++;
                if ( str_lib::bits::is_id_letter( *pp ) || *pp == ' ' )
                        continue;
                if ( *pp != '"' )
                        return {};
                std::string_view res{ p + 1, pp };
                pp++;
                p = pp;
                return res;
        }
        return {};
}

struct err
{
};

struct end
{
};

struct id
{
        std::string_view s;
};

struct str
{
        std::string_view s;
};

using tok = vari::typelist< id, str, float, int32_t, err >;

struct lexer
{
        char const* p;
        char const* e;

        lexer( std::string_view sv )
          : p( sv.data() )
          , e( sv.data() + sv.size() )
        {
        }

        vari::vval< tok, end > next()
        {
                while ( p != e && str_lib::bits::is_ws( *p ) )
                        p++;

                if ( p == e || *p == '\0' )
                        return end{};
                str_lib::s_to_nr_res r;
                if ( str_lib::s_to_nr( p, e, r ) ) {
                        if ( r.is_num )
                                return r.n;
                        return r.r;
                }
                if ( auto s = parse_str( p, e ) )
                        return str{ .s = *s };
                char const* pp = p;
                if ( str_lib::lex_letters( p, e ) )
                        return id{ .s = { pp, p } };
                return err{};
        }
};

struct parser
{
        opt< lexer > l;

        constexpr operator bool() noexcept
        {
                return !!l;
        }

        vari::vopt< std::string_view, float, int32_t > expr()
        {
                using R = vari::vopt< std::string_view, float, int32_t >;
                if ( l )
                        return l->next().visit(
                            [&]( id& x ) -> R {
                                    return x.s;
                            },
                            [&]( str& x ) -> R {
                                    return x.s;
                            },
                            [&]( float& x ) -> R {
                                    return x;
                            },
                            [&]( int32_t& x ) -> R {
                                    return x;
                            },
                            [&]( vari::vref< err, end > ) -> R {
                                    return {};
                            } );
                return {};
        }

        opt< std::string_view > exp( atom< "id"_a > )
        {
                using R = opt< std::string_view >;
                return l->next().visit(
                    [&]( id& sv ) -> R {
                            return sv.s;
                    },
                    [&]( vari::vref< str, float, int32_t, err, end > ) -> R {
                            return {};
                    } );
        }

        template < typename T >
        opt< T > exp_any()
        {
                if ( auto r = this->exp( "id"_a ) ) {
                        for ( T x : T::iota() )
                                if ( x.to_string() == *r )
                                        return x;
                }
                return {};
        }
};

inline opt< float > _real( parser& p )
{
        using R = opt< float >;
        return p.expr().visit(
            [&]( std::string_view& ) -> R {
                    return {};
            },
            [&]( float& x ) -> R {
                    return x;
            },
            [&]( int32_t& x ) -> R {
                    return (float) x;
            },
            [&]( vari::empty_t ) -> R {
                    return {};
            } );
}

inline opt< bool > _bool( parser& p )
{
        using R = opt< bool >;
        return p.expr().visit(
            [&]( std::string_view& s ) -> R {
                    if ( s == "true" )
                            return true;
                    if ( s == "false" )
                            return false;
                    return {};
            },
            [&]( float& ) -> R {
                    return {};
            },
            [&]( int32_t& x ) -> R {
                    return x != 0;
            },
            [&]( vari::empty_t ) -> R {
                    return {};
            } );
}

template < typename T >
struct parse_trait;

struct unit
{
        constexpr auto operator<=>( unit const& ) const noexcept = default;
};

template <>
struct parse_trait< unit >
{
        static vari::vopt< unit > parse( parser& ) noexcept
        {
                return unit{};
        }
};

template < avakar::atomish A >
struct parse_trait< A >
{
        static vari::vopt< A > parse( parser& p ) noexcept
        {
                if ( auto v = p.exp_any< A >() )
                        return *v;
                return {};
        }
};

template <>
struct parse_trait< bool >
{
        static vari::vopt< bool > parse( parser& p ) noexcept
        {
                if ( auto v = _bool( p ) )
                        return *v;
                return {};
        }
};

template <>
struct parse_trait< float >
{
        static vari::vopt< float > parse( parser& p ) noexcept
        {
                if ( auto v = _real( p ) )
                        return *v;
                return {};
        }
};

template < std::size_t N >
struct parse_trait< em::string_buffer< N > >
{
        static vari::vopt< em::string_buffer< N > > parse( parser& p ) noexcept
        {
                using R = vari::vopt< em::string_buffer< N > >;
                return p.expr().visit(
                    [&]( std::string_view& s ) -> R {
                            // note that this will cutoff the string if it's longer than N
                            return em::string_buffer< N >{ s };
                    },
                    [&]( float& ) -> R {
                            return {};
                    },
                    [&]( int32_t& ) -> R {
                            return {};
                    },
                    [&]( vari::empty_t ) -> R {
                            return {};
                    } );
        }
};

template <>
struct parse_trait< uint32_t >
{
        static vari::vopt< uint32_t > parse( parser& p ) noexcept
        {
                using R = vari::vopt< uint32_t >;
                return p.expr().visit(
                    [&]( std::string_view& ) -> R {
                            return {};
                    },
                    [&]( float& ) -> R {
                            return {};
                    },
                    [&]( int32_t& x ) -> R {
                            if ( x < 0 )
                                    return {};
                            return (uint32_t) x;
                    },
                    [&]( vari::empty_t ) -> R {
                            return {};
                    } );
        }
};

}  // namespace servio::parser
