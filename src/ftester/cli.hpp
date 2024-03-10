#pragma once

#include <CLI/CLI.hpp>
#include <filesystem>
#include <optional>

namespace servio::ftester
{

CLI::Option* ocfg_opt( CLI::App& app, std::optional< std::filesystem::path >& o_cfg );

CLI::Option*
firmware_opt( CLI::App& app, std::optional< std::filesystem::path >& fw, CLI::Option* cfg_opt );

}  // namespace servio::ftester
