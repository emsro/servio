#include "storage.hpp"

#include <emlabcpp/experimental/cfg/handler.h>
#include <emlabcpp/protocol/register_handler.h>

namespace servio::cfg
{

using handler = em::cfg::handler< payload, keyval, std::endian::native >;

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
        const page* find_cmp_page( em::view< const page* > pages, CmpFunction&& cmp_f )
        {
                const page* candidate    = nullptr;
                uint32_t    candidate_id = 0;
                for ( const page& p : pages ) {
                        EMLABCPP_DEBUG_LOG( "Checking page: ", p.begin(), " ", p.end() );
                        std::ignore = handler::load(
                            p,
                            [&]( const payload& pl ) -> bool {
                                    if ( candidate == nullptr || cmp_f( candidate_id, pl.id ) ) {
                                            candidate    = &p;
                                            candidate_id = pl.id;
                                    }
                                    return false;
                            },
                            [&]( const keyval& ) {},
                            chcksum_f );
                }
                return candidate;
        }
}  // namespace

const page* find_unused_page( em::view< const page* > pages )
{
        const page* iter = em::find_if( pages, [&]( const page& p ) {
                EMLABCPP_DEBUG_LOG( "Unused trying page: ", p.begin(), " ", p.end() );
                const em::cfg::load_result lr = handler::load(
                    p,
                    [&]( const payload& ) -> bool {
                            return false;
                    },
                    [&]( const keyval& ) {},
                    chcksum_f );
                return lr == em::cfg::load_result::DESERIALIZATION_ERROR;
        } );
        if ( iter == pages.end() ) {
                return nullptr;
        }
        EMLABCPP_DEBUG_LOG( "Got unused page: ", iter->begin(), " ", iter->end() );
        return &*iter;
}

const page* find_oldest_page( em::view< const page* > pages )
{
        return find_cmp_page( pages, [&]( uint32_t candidate_id, uint32_t page_id ) {
                EMLABCPP_DEBUG_LOG( "Ids: ", candidate_id, " ", page_id );
                return page_id < candidate_id;
        } );
}

const page* find_next_page( em::view< const page* > pages )
{
        const page* res = find_unused_page( pages );
        if ( res != nullptr ) {
                return res;
        }
        return find_oldest_page( pages );
}

const page* find_latest_page( em::view< const page* > pages )
{
        return find_cmp_page( pages, [&]( uint32_t candidate_id, uint32_t page_id ) {
                return page_id > candidate_id;
        } );
}

using reghandler = em::protocol::register_handler< map >;

std::tuple< bool, page > store( const payload& pl, const map* m, page buffer )
{
        return handler::store(
            buffer,
            pl,
            m != nullptr ? map::keys.size() : 0,
            [&]( std::size_t i ) -> keyval {
                    return keyval{
                        .key = map::keys[i], .msg = reghandler::select( *m, map::keys[i] ) };
            },
            chcksum_f );
}

bool load( page p, em::function_view< bool( const payload& ) > pl_cb, map& m )
{

        const em::cfg::load_result lr = handler::load(
            p,
            pl_cb,
            [&]( const keyval& kv ) -> bool {
                    const std::optional opt_err = reghandler::insert( m, kv.key, kv.msg );
                    return !opt_err.has_value();
            },
            chcksum_f );

        return lr == em::cfg::load_result::SUCCESS;
}

}  // namespace servio::cfg
