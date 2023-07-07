#include "storage.hpp"

#include <emlabcpp/experimental/cfg/handler.h>
#include <emlabcpp/protocol/register_handler.h>

namespace cfg
{

using handler = em::cfg::handler< payload, cfg_keyval, std::endian::native >;

namespace
{
        em::cfg::checksum chcksum_f( std::span< std::byte > data )
        {
                static constexpr std::size_t n = sizeof( em::cfg::checksum );

                em::cfg::checksum          res;
                std::array< std::byte, n > buffer{};
                std::fill( buffer.begin(), buffer.end(), std::byte{ 0xAA } );

                while ( !data.empty() ) {
                        std::span< std::byte > tmp = data.subspan( 0, std::min( n, data.size() ) );
                        for ( std::size_t i : em::range( tmp.size() ) ) {
                                buffer[i] ^= tmp[i];
                        }
                        data = data.subspan( tmp.size() );
                }
                std::memcpy( &res, buffer.data(), n );
                return res;
        }

        template < typename CmpFunction >
        std::optional< page > find_cmp_page( em::view< const page* > pages, CmpFunction&& cmp_f )
        {
                std::optional< page > candidate    = std::nullopt;
                uint32_t              candidate_id = 0;
                for ( const page& p : pages ) {
                        std::ignore = handler::load(
                            p,
                            [&]( const payload& pl ) -> bool {
                                    if ( !candidate.has_value() || cmp_f( candidate_id, pl.id ) ) {
                                            candidate = p;
                                    }
                                    return false;
                            },
                            [&]( const cfg_keyval& ) {},
                            chcksum_f );
                }
                return candidate;
        }
}  // namespace

std::optional< page > find_unused_page( em::view< const page* > pages )
{
        auto iter = em::find_if( pages, [&]( const page& p ) {
                em::cfg::load_result lr = handler::load(
                    p,
                    [&]( const payload& ) -> bool {
                            return false;
                    },
                    [&]( const cfg_keyval& ) {},
                    chcksum_f );
                return lr == em::cfg::load_result::DESERIALIZATION_ERROR;
        } );
        if ( iter == pages.end() ) {
                return std::nullopt;
        }
        return *iter;
}

std::optional< page > find_oldest_page( em::view< const page* > pages )
{
        return find_cmp_page( pages, [&]( uint32_t candidate_id, uint32_t page_id ) {
                return page_id < candidate_id;
        } );
}

std::optional< page > find_next_page( em::view< const page* > pages )
{
        std::optional< page > opt_res = find_unused_page( pages );
        if ( opt_res.has_value() ) {
                return *opt_res;
        }
        return find_oldest_page( pages );
}

std::optional< page > find_latest_page( em::view< const page* > pages )
{
        return find_cmp_page( pages, [&]( uint32_t candidate_id, uint32_t page_id ) {
                return page_id > candidate_id;
        } );
}

using reghandler = em::protocol::register_handler< cfg_map >;

bool store(
    const payload&                                     pl,
    const cfg_map*                                     m,
    em::function_view< bool( std::size_t, uint64_t ) > writer )
{

        static constexpr std::size_t N = cfg_map::registers_count;

        constexpr std::size_t buffer_n =
            em::ceil_to( N * sizeof( cfg_keyval ) + 128, sizeof( uint64_t ) );
        std::array< std::byte, buffer_n > buffer;

        bool success = handler::store(
            buffer,
            pl,
            m != nullptr ? cfg_map::keys.size() : 0,
            [&]( std::size_t i ) -> cfg_keyval {
                    return cfg_keyval{
                        .key = cfg_map::keys[i],
                        .msg = reghandler::select( *m, cfg_map::keys[i] ) };
            },
            chcksum_f );

        if ( !success ) {
                return false;
        }
        for ( std::size_t i : em::range( buffer.size() / sizeof( uint64_t ) ) ) {
                const std::size_t j = ( buffer.size() / sizeof( uint64_t ) ) - 1 - i;

                uint64_t word;
                std::memcpy( &word, buffer.data() + j * sizeof( uint64_t ), sizeof( uint64_t ) );
                success |= writer( j * sizeof( uint64_t ), word );
        }

        return success;
}

bool load( page p, em::function_view< bool( const payload& ) > pl_cb, cfg_map& m )
{

        const em::cfg::load_result lr = handler::load(
            p,
            pl_cb,
            [&]( const cfg_keyval& kv ) -> bool {
                    std::optional opt_err = reghandler::insert( m, kv.key, kv.msg );
                    return !opt_err.has_value();
            },
            chcksum_f );

        return lr == em::cfg::load_result::SUCCESS;
}

}  // namespace cfg
