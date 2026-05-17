#include "../../ftest/bench/benchmarks.hpp"
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
        bench::setup_bench_tests(
            ctx, assm, *cdrv.clock, *cdrv.position, *cdrv.current, *cdrv.period, cor, res );
}

}  // namespace servio::ftest
