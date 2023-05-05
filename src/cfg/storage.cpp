#include "storage.hpp"

#include <emlabcpp/experimental/cfg/handler.h>
#include <emlabcpp/protocol/register_handler.h>

namespace cfg
{

using handler = em::cfg::handler< payload, cfg_keyval, std::endian::native >;

namespace
{
        em::cfg::checksum chcksum_f( std::span< std::byte > )
        {
                return 0;
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

bool store( page p, const payload& pl, const cfg_map& m )
{
        using reghandler = em::protocol::register_handler< cfg_map >;

        static constexpr std::size_t N = cfg_map::registers_count;

        std::array keys = m.get_keys();

        std::array< std::byte, N * sizeof( cfg_keyval ) + 128 > buffer;

        if ( buffer.size() > p.size() ) {
                return false;
        }

        bool success = handler::store(
            buffer,
            pl,
            keys.size(),
            [&]( std::size_t i ) -> cfg_keyval {
                    return cfg_keyval{ .key = keys[i], .msg = reghandler::select( m, keys[i] ) };
            },
            chcksum_f );

        return success;
}

}  // namespace cfg
