#include "../../drv/tests/impl_tests.hpp"
#include "../../drv/tests/interface_tests.hpp"
#include "../../ftest/intg/ctl_test.hpp"
#include "../../ftest/intg/meas_test.hpp"
#include "../../ftest/setup_tests.hpp"

namespace servio::ftest
{

void setup_tests(
    asrt::task_ctx& ctx,
    asrt_reac_assm& assm,
    core::drivers&  cdrv,
    core::core&     cor,
    status&         res )
{
        drv::tests::setup_iface_tests(
            ctx,
            assm,
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
        drv::tests::setup_impl_tests( ctx, assm, *cdrv.clock, res );
        intg::setup_ctl_test(
            ctx, assm, *cdrv.clock, *cdrv.motor, *cdrv.current, *cdrv.position, cor, res );
        intg::setup_meas_tests( ctx, assm, *cdrv.clock, *cdrv.motor, *cdrv.current, cor, res );
}

}  // namespace servio::ftest
