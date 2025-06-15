#pragma once

#include <emlabcpp/experimental/multiplexer.h>
#include <filesystem>

namespace servio::ftester
{

namespace em = emlabcpp;

static constexpr em::protocol::channel_type rec_id = 4;

inline std::filesystem::path const& check_path( std::filesystem::path const& p )
{
        if ( !std::filesystem::exists( p ) )
                throw std::runtime_error( "Path " + p.string() + " does not exist" );
        return p;
}

}  // namespace servio::ftester
