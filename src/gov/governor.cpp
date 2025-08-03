
#include "./governor.hpp"

namespace servio::gov
{

static zll::ll_list< governor > governor_registry;

governor* find_governor( std::string_view id )
{
        if ( governor_registry.empty() )
                return nullptr;
        return find_if_node( governor_registry.front(), [&]( auto const& gov ) {
                return gov.name() == id;
        } );
}

void register_governor( governor& gov )
{
        governor_registry.link_back( gov );
}

}  // namespace servio::gov
