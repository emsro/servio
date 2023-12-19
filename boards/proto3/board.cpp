
#include "brd.hpp"
#include "cfg/default.hpp"
#include "core_drivers.hpp"
#include "platform.hpp"

#include <emlabcpp/result.h>

namespace em = emlabcpp;

namespace brd
{

em::result setup_board()
{
        if ( HAL_Init() != HAL_OK ) {
                return em::ERROR;
        }
        // TODO
        // setup_clk();

        return em::SUCCESS;
}

em::view< const page* > get_persistent_pages()
{
        return {};
}

core_drivers setup_core_drivers()
{
        return core_drivers{
            .start_cb = +[]( core_drivers& ) -> em::result {
                    return em::SUCCESS;
            },
        };
}

cfg::map get_default_config()
{
        return cfg::get_default_config();
}

}  // namespace brd
