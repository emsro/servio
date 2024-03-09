
#pragma once

#include <emlabcpp/experimental/testing/parameters.h>

namespace em = emlabcpp;

namespace servio::drv::tests
{

inline auto is_powerless( em::testing::parameters& params )
{
        return params.get_value< bool >( 0, "powerless" );
}

}  // namespace servio::drv::tests
