
#include "./def.h"
#include "./util.h"

namespace servio::iface
{
namespace
{

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

using expr = vari::typelist< std::string_view, float, int32_t >;

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

opt< float > _real( parser& p )
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

opt< bool > _bool( parser& p )
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
struct kvals_parse;

template < auto K, typename T >
struct kval_parse;

template < typename... Ts >
struct kvals_parse< vari::typelist< Ts... > >
{
        using kvals = vari::typelist< Ts... >;

        static vari::vopt< kvals > parse( parser& p ) noexcept
        {
                auto id = p.exp( "id"_a );

                vari::vopt< kvals > res;
                auto                f = [&]< typename KV >() {
                        if ( *id != KV::key.to_string() )
                                return false;
                        if ( auto r = kval_parse< KV::key, typename KV::value_type >::parse( p ) )
                                res = r;
                        return true;
                };
                ( f.template operator()< Ts >() || ... );
                return res;
        }
};

template < auto K >
struct kval_parse< K, void >
{
        static vari::vopt< kval< K, void > > parse( parser& ) noexcept
        {
                return kval< K, void >{};
        }
};

template < auto K, avakar::atomish A >
struct kval_parse< K, A >
{
        static vari::vopt< kval< K, A > > parse( parser& p ) noexcept
        {
                if ( auto v = p.exp_any< A >() )
                        return kval< K, A >{ .value = *v };
                return {};
        }
};

template < auto K >
struct kval_parse< K, bool >
{
        static vari::vopt< kval< K, bool > > parse( parser& p ) noexcept
        {
                if ( auto v = _bool( p ) )
                        return kval< K, bool >{ .value = *v };
                return {};
        }
};

template < auto K >
struct kval_parse< K, float >
{
        static vari::vopt< kval< K, float > > parse( parser& p ) noexcept
        {
                if ( auto v = _real( p ) )
                        return kval< K, float >{ .value = *v };
                return {};
        }
};

template < auto K, std::size_t N >
struct kval_parse< K, em::string_buffer< N > >
{
        static vari::vopt< kval< K, em::string_buffer< N > > > parse( parser& p ) noexcept
        {
                using R = vari::vopt< kval< K, em::string_buffer< N > > >;
                return p.expr().visit(
                    [&]( std::string_view& s ) -> R {
                            // note that this will cutoff the string if it's longer than N
                            return kval< K, em::string_buffer< N > >{ .value = s };
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

template < auto K >
struct kval_parse< K, uint32_t >
{
        static vari::vopt< kval< K, uint32_t > > parse( parser& p ) noexcept
        {
                using R = vari::vopt< kval< K, uint32_t > >;
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
                            return kval< K, uint32_t >{
                                .value = (uint32_t) x,
                            };
                    },
                    [&]( vari::empty_t ) -> R {
                            return {};
                    } );
        }
};

opt< mode_stmt > _mode_stmt( parser& p )
{
        if ( auto r = kvals_parse< mode_vals >::parse( p ) )
                return mode_stmt{ .arg = std::move( r ).vval() };
        return {};
}

vari::vopt< cfg_vals > _cfg_vals( parser& p )
{
        return kvals_parse< cfg_vals >::parse( p );
}

vari::vval< stmt, invalid_stmt > _parse( parser& p )
{
        // XXX, missing tests:
        // - no overflow after parsed message
        auto id = p.exp( "id"_a );
        if ( !id ) {
                return invalid_stmt{};
        } else if ( *id == "mode" ) {
                if ( auto r = _mode_stmt( p ) )
                        return *r;
                return invalid_stmt{};
        } else if ( *id == "prop" ) {
                auto pr = p.exp_any< prop_key >();
                if ( !pr )
                        return invalid_stmt{};
                return prop_stmt{
                    .prop = *pr,
                };
        } else if ( *id == "cfg" ) {
                auto sub = p.exp( "id"_a );
                if ( !sub )
                        return invalid_stmt{};
                if ( *sub == "set" ) {
                        if ( auto r = _cfg_vals( p ) )
                                return cfg_set_stmt{ .val = std::move( r ).vval() };
                } else if ( *sub == "get" ) {
                        auto m = p.exp_any< cfg_key >();
                        if ( !m )
                                return invalid_stmt{};
                        return cfg_get_stmt{
                            .k = *m,
                        };
                } else if ( *sub == "commit" ) {
                        return cfg_commit_stmt{};
                } else if ( *sub == "clear" ) {
                        return cfg_clear_stmt{};
                }
        } else if ( *id == "info" ) {
                return info_stmt{};
        }
        return invalid_stmt{};
}

}  // namespace

vari::vval< stmt, invalid_stmt > parse( std::string_view inpt )
{
        lexer  l{ inpt };
        parser p{ std::move( l ) };

        using R = vari::vval< stmt, invalid_stmt >;
        return _parse( p ).visit(
            [&]( invalid_stmt& is ) -> R {
                    return is;
            },
            [&]( vari::vval< stmt > st ) -> R {
                    if ( !p.l )
                            return st;
                    return p.l->next().visit(
                        [&]( end& ) -> R {
                                return st;
                        },
                        [&]( vari::vref< tok > ) -> R {
                                return invalid_stmt{};
                        } );
            } );
}

}  // namespace servio::iface
