#include "./preset.hpp"

#include "./serial.hpp"

#include <spdlog/spdlog.h>

namespace servio::scmdio
{
namespace
{
nlohmann::json j_from_file( std::filesystem::path const& file )
{
        if ( !std::filesystem::exists( file ) )
                throw std::runtime_error( "File not found: " + file.string() );
        std::ifstream ifs{ file };
        return nlohmann::json::parse( ifs );
}
}  // namespace

raw_preset_def load_raw_preset( std::filesystem::path const& folder )
{
        nlohmann::json meta   = j_from_file( folder / "meta.json" );
        nlohmann::json config = nlohmann::json::object();
        if ( std::filesystem::exists( folder / "config.json" ) )
                config = j_from_file( folder / "config.json" );

        if ( meta.contains( "inherits" ) ) {
                raw_preset_def parent =
                    load_raw_preset( folder.parent_path() / meta["inherits"].get< std::string >() );
                parent.meta.merge_patch( std::move( meta ) );
                parent.config.merge_patch( std::move( config ) );
                return parent;
        }

        return {
            .meta   = std::move( meta ),
            .config = std::move( config ),
        };
}

preset_def load_preset( std::filesystem::path const& folder )
{
        raw_preset_def raw = load_raw_preset( folder );

        preset_def res{
            .name     = folder.filename().string(),
            .board    = raw.meta.at( "board" ),
            .platform = raw.meta.at( "platform" ),
            .config   = std::move( raw.config ),
        };

        return res;
}

awaitable< void > load_preset_cmd( port_iface& port, std::filesystem::path const& folder )
{
        spdlog::info( "Loading preset from {}", folder.string() );

        preset_def preset = load_preset( folder );
        spdlog::info( "Preset loaded: {}", preset.name );
        spdlog::info( "Board: {}", preset.board );
        spdlog::info( "Platform: {}", preset.platform );
        for ( auto& [k, v] : preset.config.items() ) {
                spdlog::info( "Config: {} = {}", k, v.dump() );
                co_await set_config_field( port, k, v );
        }
        spdlog::info( "Comming config" );
        co_await commit_config( port );
}

}  // namespace servio::scmdio
