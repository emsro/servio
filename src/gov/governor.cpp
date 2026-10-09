
#include "./governor.hpp"

#include <emlabcpp/experimental/function_view.hpp>
#include <zll.hpp>

namespace servio::gov
{

namespace
{
auto& get_registry()
{
        static zll::ll_list< governor_factory > governor_factory_registry;
        return governor_factory_registry;
}
}  // namespace

void register_factory( governor_factory& factory ) noexcept
{
        get_registry().link_back( factory );
}

void for_each_factory( em::function_view< void( governor_factory& ) > const& fn )
{
        for ( governor_factory& factory : get_registry() )
                fn( factory );
}

}  // namespace servio::gov
