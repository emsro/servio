#include "./setuplib.hpp"

namespace servio::scmdio
{
namespace
{
nlohmann::json j_from_file( std::filesystem::path const& file )
{
        assert( std::filesystem::exists( file ) );
        std::ifstream ifs{ file };
        return nlohmann::json::parse( ifs );
}
}  // namespace

raw_setup_def load_raw_setup( std::filesystem::path const& folder )
{
        nlohmann::json meta   = j_from_file( folder / "meta.json" );
        nlohmann::json config = nlohmann::json::object();
        if ( std::filesystem::exists( folder / "config.json" ) )
                config = j_from_file( folder / "config.json" );

        if ( meta.contains( "inherits" ) ) {
                raw_setup_def parent =
                    load_raw_setup( folder.parent_path() / meta["inherits"].get< std::string >() );
                parent.meta.merge_patch( std::move( meta ) );
                parent.config.merge_patch( std::move( config ) );
                return parent;
        }

        return {
            .meta   = std::move( meta ),
            .config = std::move( config ),
        };
}

setup_def load_setup( std::filesystem::path const& folder )
{
        raw_setup_def raw = load_raw_setup( folder );

        setup_def res;

        return {
            .name     = folder.filename().string(),
            .board    = raw.meta.at( "board" ),
            .platform = raw.meta.at( "platform" ),
            .config   = std::move( raw.config ),
        };
}

}  // namespace servio::scmdio
