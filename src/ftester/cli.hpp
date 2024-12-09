#pragma once

#include <CLI/CLI.hpp>
#include <filesystem>
#include <optional>

namespace servio::ftester
{

template < typename T >
using opt = std::optional< T >;

CLI::Option* ocfg_opt( CLI::App& app, opt< std::filesystem::path >& o_cfg );

CLI::Option* firmware_opt( CLI::App& app, opt< std::filesystem::path >& fw, CLI::Option* cfg_opt );

}  // namespace servio::ftester
