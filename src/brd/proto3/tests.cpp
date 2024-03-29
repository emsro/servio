#include "drv/tests/impl_tests.hpp"
#include "drv/tests/interface_tests.hpp"
#include "ftest/intg/ctl_test.hpp"
#include "ftest/intg/meas_test.hpp"
#include "ftest/setup_tests.hpp"

namespace servio::ftest
{

void setup_tests(
    em::pmr::memory_resource& mem,
    em::testing::reactor&     reac,
    em::testing::collector&   coll,
    em::testing::parameters&  params,
    core::drivers&            cdrv,
    core::core&               cor,
    em::result&               res )

{
        drv::tests::setup_iface_tests(
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
            *cdrv.current,
            res );
        drv::tests::setup_impl_tests( mem, reac, coll, *cdrv.clock, res );
        tests::setup_ctl_test(
            mem,
            reac,
            coll,
            params,
            *cdrv.clock,
            *cdrv.motor,
            *cdrv.current,
            *cdrv.position,
            cor,
            res );
        tests::setup_meas_tests(
            mem, reac, coll, params, *cdrv.clock, *cdrv.motor, *cdrv.current, cor, res );
}

}  // namespace servio::ftest
