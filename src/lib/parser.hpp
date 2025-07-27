#pragma once

#include "./str_lib.hpp"

#include <emlabcpp/experimental/string_buffer.h>
#include <string_view>
#include <vari/vopt.h>

namespace servio::parser
{
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
                if ( *pp != '"' )
                        continue;
                std::string_view res{ p + 1, pp };
                pp++;
                p = pp;
                return res;
        }
        return {};
}

enum class op_t : uint8_t
{
        ASSIGN = '='
};

inline vari::vopt< op_t > parse_op( char const*& p, char const* e ) noexcept
{
        if ( p == e )
                return {};
        if ( *p == '=' ) {
                p++;
                return op_t::ASSIGN;
        }
        return {};
}

struct err
{
};

struct end
{
};

struct id_t
{
        std::string_view s;
};

using string = em::string_buffer< 32 >;

using tok = vari::typelist< op_t, id_t, string, float, int32_t, bool, err >;

struct lexer
{
        char const* b;
        char const* p;
        char const* e;

        lexer( std::string_view sv )
          : b( sv.data() )
          , p( sv.data() )
          , e( sv.data() + sv.size() )
        {
        }

        bool clear_ws()
        {
                while ( p != e && str_lib::bits::is_ws( *p ) )
                        p++;
                return p == e || *p == '\0';
        }

        vari::vval< tok, end > next()
        {
                if ( clear_ws() )
                        return end{};

                str_lib::s_to_nr_res r;
                if ( str_lib::s_to_nr( p, e, r ) ) {
                        if ( r.is_num )
                                return r.n;
                        return r.r;
                }
                if ( auto op = parse_op( p, e ) )
                        return op_t{ *op };
                if ( auto s = parse_str( p, e ) )
                        return string{ *s };
                char const* pp = p;
                if ( !str_lib::lex_letters( p, e ) )
                        return err{};
                std::string_view id{ pp, p };
                if ( id == "true" )
                        return true;
                else if ( id == "false" )
                        return false;
                return id_t{ .s = id };
        }
};

using expr_t = vari::typelist< string, float, int32_t, bool >;

struct parser
{
        lexer l;

        std::size_t loc()
        {
                return static_cast< std::size_t >( l.p - l.b );
        }

        bool ended()
        {
                return l.clear_ws();
        }

        vari::vopt< string, float, int32_t, id_t, op_t, bool > next()
        {
                using R = vari::vopt< string, float, int32_t, id_t, op_t, bool >;
                return l.next().visit(
                    [&]( id_t& x ) -> R {
                            return x;
                    },
                    [&]( string& x ) -> R {
                            return x;
                    },
                    [&]( bool& x ) -> R {
                            return x;
                    },
                    [&]( float& x ) -> R {
                            return x;
                    },
                    [&]( int32_t& x ) -> R {
                            return x;
                    },
                    [&]( op_t& op ) -> R {
                            return op;
                    },
                    [&]( vari::vref< err, end > ) -> R {
                            return {};
                    } );
        }

        vari::vopt< expr_t > expr()
        {
                using R = vari::vopt< expr_t >;
                return l.next().visit(
                    [&]( bool& x ) -> R {
                            return x;
                    },
                    [&]( id_t& ) -> R {
                            return {};
                    },
                    [&]( string& x ) -> R {
                            return x;
                    },
                    [&]( float& x ) -> R {
                            return x;
                    },
                    [&]( int32_t& x ) -> R {
                            return x;
                    },
                    [&]( vari::vref< op_t, err, end > ) -> R {
                            return {};
                    } );
        }

        opt< op_t > op()
        {
                using R = opt< op_t >;
                return l.next().visit(
                    [&]( op_t& op ) -> R {
                            return op;
                    },
                    [&]( vari::vref< id_t, string, float, int32_t, bool, err, end > ) -> R {
                            return {};
                    } );
        }

        opt< std::string_view > id()
        {
                using R = opt< std::string_view >;
                return l.next().visit(
                    [&]( id_t& sv ) -> R {
                            return sv.s;
                    },
                    [&]( vari::vref< op_t, string, float, int32_t, bool, err, end > ) -> R {
                            return {};
                    } );
        }
};

inline bool
load_value( vari::vref< string, float, int32_t, bool > tgt, vari::vref< expr_t const > val )
{

        return tgt.visit(
            [&]( bool& b ) {
                    return val.visit( [&]< typename T >( T const& x ) {
                            if constexpr ( std::same_as< T, bool > ) {
                                    b = x;
                                    return true;
                            }
                            return false;
                    } );
            },
            [&]( string& s ) {
                    return val.visit(
                        [&]( string const& sv ) {
                                s = sv;
                                return true;
                        },
                        [&]( vari::vref< float const, int32_t const, bool const > ) {
                                return false;
                        } );
            },
            [&]( float& f ) {
                    return val.visit( [&]< typename T >( T const& x ) {
                            if constexpr ( std::same_as< T, float > || std::same_as< T, int32_t > ) {
                                    f = (float) x;
                                    return true;
                            }
                            return false;
                    } );
            },
            [&]( int32_t& i ) {
                    return val.visit( [&]< typename T >( T const& x ) {
                            if constexpr ( std::same_as< T, int32_t > ) {
                                    i = x;
                                    return true;
                            }
                            return false;
                    } );
            } );
};

struct expr_tok
{
        vari::vval< expr_t > data = int32_t{ 0 };

        friend constexpr auto operator<=>( expr_tok const&, expr_tok const& ) noexcept = default;
};

}  // namespace servio::parser
