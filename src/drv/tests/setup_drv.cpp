#include "drv/tests/impl_tests.hpp"
#include "drv/tests/interface_tests.hpp"
#include "ftest/setup_tests.hpp"

namespace servio::ftest
{

void setup_tests(
    em::pmr::memory_resource& mem,
    em::testing::reactor&     reac,
    em::testing::collector&   coll,
    em::testing::parameters&,
    core::drivers& cdrv,
    core::core&,
    em::result& res )

{
        drv::tests::setup_interface_tests(
            mem,
            reac,
            coll,
            *cdrv.clock,
            *cdrv.comms,
            *cdrv.period,
            *cdrv.motor,
            *cdrv.vcc,
            *cdrv.temperature,
            *cdrv.position,
            res );
        drv::tests::setup_impl_tests( mem, reac, *cdrv.clock, res );
}

}  // namespace servio::ftest
