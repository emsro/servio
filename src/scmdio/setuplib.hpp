#pragma once

#include <fstream>
#include <nlohmann/json.hpp>

namespace servio::scmdio
{

struct raw_setup_def
{
        nlohmann::json meta;
        nlohmann::json config;
};

raw_setup_def load_raw_setup( std::filesystem::path const& folder );

struct setup_def
{
        std::string    name;
        std::string    board;
        std::string    platform;
        nlohmann::json config;
};

setup_def load_setup( std::filesystem::path const& folder );

}  // namespace servio::scmdio
