
#include "./governor.hpp"

namespace servio::gov
{

static zll::ll_list< governor_factory > governor_factory_registry;

void register_factory( governor_factory& factory )
{
        governor_factory_registry.link_back( factory );
}

void for_each_factory( em::function_view< void( governor_factory& ) > fn )
{
        for ( governor_factory& factory : governor_factory_registry )
                fn( factory );
}

}  // namespace servio::gov
