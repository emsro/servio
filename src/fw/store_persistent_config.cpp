#include "store_persistent_config.hpp"

#include "brd/brd.hpp"
#include "git.h"
#include "platform.hpp"
#include "util.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/defer.h>

namespace servio::fw
{

// XXX: this no longer has to be in fw/
bool persistent_config_writer::operator()( const cfg::map* cfg )
{
        const cfg::payload pld{
            .git_ver  = git::Describe(),
            .git_date = git::CommitDate(),
            .id       = pl.id + 1,
        };
        constexpr std::size_t buffer_n = cfg::map::registers_count * sizeof( cfg::keyval ) + 128;
        std::array< std::byte, buffer_n > buffer;
        auto [succ, used_buffer] = cfg::store( pld, cfg, buffer );

        if ( !succ )
                return false;

        if ( iface.store_page( used_buffer ) == em::ERROR )
                return false;
        pl = pld;
        return true;
}

}  // namespace servio::fw
