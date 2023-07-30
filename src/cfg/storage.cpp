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
                        const std::span< std::byte > tmp =
                            data.subspan( 0, std::min( n, data.size() ) );
                        for ( const std::size_t i : em::range( tmp.size() ) ) {
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
                        EMLABCPP_DEBUG_LOG( "Checking page: ", p.begin(), " ", p.end() );
                        std::ignore = handler::load(
                            p,
                            [&]( const payload& pl ) -> bool {
                                    if ( !candidate.has_value() || cmp_f( candidate_id, pl.id ) ) {
                                            candidate    = p;
                                            candidate_id = pl.id;
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
        const page* iter = em::find_if( pages, [&]( const page& p ) {
                EMLABCPP_DEBUG_LOG( "Unused trying page: ", p.begin(), " ", p.end() );
                const em::cfg::load_result lr = handler::load(
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
        EMLABCPP_DEBUG_LOG( "Got unused page: ", iter->begin(), " ", iter->end() );
        return *iter;
}

std::optional< page > find_oldest_page( em::view< const page* > pages )
{
        return find_cmp_page( pages, [&]( uint32_t candidate_id, uint32_t page_id ) {
                EMLABCPP_DEBUG_LOG( "Ids: ", candidate_id, " ", page_id );
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

        static constexpr std::size_t n = cfg_map::registers_count;

        constexpr std::size_t buffer_n =
            em::ceil_to( n * sizeof( cfg_keyval ) + 128, sizeof( uint64_t ) );
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

        constexpr std::size_t write_n = buffer.size() / sizeof( uint64_t );
        for ( const std::size_t i : em::range( write_n ) ) {
                const std::size_t j = write_n - 1 - i;

                uint64_t word;
                std::memcpy( &word, buffer.data() + j * sizeof( uint64_t ), sizeof( uint64_t ) );
                success = writer( j * sizeof( uint64_t ), word );

                if ( !success ) {
                        break;
                }
        }

        return success;
}

bool load( page p, em::function_view< bool( const payload& ) > pl_cb, cfg_map& m )
{

        const em::cfg::load_result lr = handler::load(
            p,
            pl_cb,
            [&]( const cfg_keyval& kv ) -> bool {
                    const std::optional opt_err = reghandler::insert( m, kv.key, kv.msg );
                    return !opt_err.has_value();
            },
            chcksum_f );

        return lr == em::cfg::load_result::SUCCESS;
}

}  // namespace cfg
