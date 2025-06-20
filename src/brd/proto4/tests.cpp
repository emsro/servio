#include "../../drv/tests/impl_tests.hpp"
#include "../../drv/tests/interface_tests.hpp"
#include "../../ftest/intg/ctl_test.hpp"
#include "../../ftest/intg/meas_test.hpp"
#include "../../ftest/setup_tests.hpp"

namespace servio::ftest
{

void setup_tests(
    em::pmr::memory_resource& mem,
    t::reactor&               reac,
    uctx&                     ctx,
    t::parameters&            params,
    core::drivers&            cdrv,
    core::core&               cor,
    status&                   res )
{
        drv::tests::setup_iface_tests(
            mem,
            reac,
            ctx,
            cdrv.cfg->map,
            *cdrv.clock,
            *cdrv.comms,
            *cdrv.period,
            *cdrv.motor,
            *cdrv.vcc,
            *cdrv.temperature,
            *cdrv.position,
            *cdrv.current,
            *cdrv.storage,
            res );
        drv::tests::setup_impl_tests( mem, reac, ctx, *cdrv.clock, res );
        intg::setup_ctl_test(
            mem,
            reac,
            ctx,
            params,
            *cdrv.clock,
            *cdrv.motor,
            *cdrv.current,
            *cdrv.position,
            cor,
            res );
        intg::setup_meas_tests(
            mem, reac, ctx, params, *cdrv.clock, *cdrv.motor, *cdrv.current, cor, res );
}

}  // namespace servio::ftest
