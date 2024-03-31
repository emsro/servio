#include "ftest/bench/benchmarks.hpp"
#include "ftest/setup_tests.hpp"

namespace servio::ftest
{

void setup_tests(
    em::pmr::memory_resource& mem,
    t::reactor&               reac,
    uctx&                     ctx,
    t::parameters&,
    core::drivers& cdrv,
    core::core&    cor,
    em::result&    res )

{
        bench::setup_bench_tests(
            mem, reac, ctx, *cdrv.clock, *cdrv.position, *cdrv.current, *cdrv.period, cor, res );
}

}  // namespace servio::ftest
