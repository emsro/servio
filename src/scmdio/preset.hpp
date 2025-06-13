#pragma once

#include <fstream>
#include <nlohmann/json.hpp>

namespace servio::scmdio
{

struct raw_preset_def
{
        nlohmann::json meta;
        nlohmann::json config;
};

raw_preset_def load_raw_preset( std::filesystem::path const& folder );

struct preset_def
{
        std::string    name;
        std::string    board;
        std::string    platform;
        nlohmann::json config;
};

preset_def load_preset( std::filesystem::path const& folder );

}  // namespace servio::scmdio
