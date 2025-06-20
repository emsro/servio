#include "./preset.hpp"

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

        spdlog::info( "preset: {}", res.name );
        spdlog::info( "board: {}", res.board );
        spdlog::info( "preset config: {}", res.config.dump() );

        return res;
}

}  // namespace servio::scmdio
